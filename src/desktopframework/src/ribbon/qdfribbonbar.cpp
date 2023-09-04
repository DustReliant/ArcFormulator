#include "private/qdfribbon_p.h"
#include "private/qdfribbonbar_p.h"
#include "private/qdfribbonbutton_p.h"
#include "private/qdfribbonpage_p.h"
#include "qdfribbon_def.h"
#include "style/qdfofficeframehelper.h"
#include "style/qdfofficeframehelper_win.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QEvent>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPainter>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyleOption>
#include <QTranslator>
#include <QWidgetAction>
#include <qglobal.h>
#include <ribbon/qdfofficepopupmenu.h>
#include <ribbon/qdfribbonbackstageview.h>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribboncustomizemanager.h>
#include <ribbon/qdfribbongallery.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfribbonsystempopupbar.h>
#include <ribbon/qdfribbontooltip.h>
#include <ribbon/qdfstylehelpers.h>

#ifdef Q_OS_WIN
    #include <WinUser.h>
#endif

QDF_USE_NAMESPACE


const QString strCustomizeQAToolBar = QString("customizeQAToolBar");
const QString strCustomizeRibbonBar = QString("customizeRibbonBar");


QdfLogotypeLabel::QdfLogotypeLabel(QWidget *parent) : QLabel(parent), m_alignment(Qt::AlignRight) {}

void QdfLogotypeLabel::paintEvent(QPaintEvent *event)
{
    bool visible = true;
    if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
    {
        visible = ribbonBar->isMaximized();
    }

    if (!visible)
    {
        return;
    }

    QPainter p(this);
    QdfStyleOptionRibbon opt;
    opt.init(this);

    QRect rect = parentWidget()->geometry();
    if (m_alignment == Qt::AlignRight)
    {
        QPoint pnt = mapFromParent(rect.topLeft());
        opt.rect.setLeft(pnt.x());
        opt.rect.setWidth(rect.width());
    }
    else
    {
        opt.rect.setWidth(rect.width());
    }

    style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonGroups, &opt, &p, this);

    QLabel::paintEvent(event);
}


QdfRibbonBarPrivate::QdfRibbonBarPrivate()
{
    m_tabBar = nullptr;
    m_quickAccessBar = nullptr;
    m_controlSystemButton = nullptr;
    m_currentIndexPage = -1;
    m_rowItemHeight = 22;
    m_rowItemCount = 3;
    m_heightTabs = -1;
    m_frameHelper = nullptr;
    m_showTitleAlways = true;
    m_ribbonBarVisible = true;
    m_minimizationEnabled = true;
    m_keyTipsComplement = true;
    m_keyTipsEnabled = true;
    m_keyTipsShowing = false;
    m_minimized = false;
    m_titleGroupsVisible = true;
    m_quickAccessBarPosition = QdfRibbonBar::QATopPosition;
    m_tabBarPosition = QdfRibbonBar::TabBarTopPosition;
    m_hoveredSubControl = QStyle::SC_None;
    m_activeSubControl = QStyle::SC_None;
    m_countKey = 0;
    m_customizeManager = nullptr;
    m_customizeDialog = nullptr;
}

QdfRibbonBarPrivate::~QdfRibbonBarPrivate() { removeAllContextHeaders(); }

void QdfRibbonBarPrivate::init()
{
    QDF_Q(QdfRibbonBar);
    q->ensurePolished();
    q->setNativeMenuBar(false);
    QdfRibbonStyle *ribbonStyle = qobject_cast<QdfRibbonStyle *>(q->style());
    if (ribbonStyle)
    {
        q->setFont(ribbonStyle->font(q));
    }
    else
    {
        q->setFont(QApplication::font("Qdf::QdfRibbonBar"));
    }

    q->setAttribute(Qt::WA_Hover, true);
    q->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    m_tabBar = new QdfRibbonTabBar(q);
    m_tabBar->show();
    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)), Qt::DirectConnection);

    m_quickAccessBar = new QdfRibbonQuickAccessBar(q);
    m_quickAccessBar->setObjectName("qdf_Ribbon_Quick_Access_Bar");
    m_quickAccessBar->setVisible(false);

    m_logotypeLabel = new QdfLogotypeLabel(q);
    m_logotypeLabel->setVisible(false);
}

void QdfRibbonBarPrivate::calcHeightItem()
{
    QDF_Q(QdfRibbonBar);
    {
        QFont fontRegular = q->font();
#ifdef Q_OS_WIN
        QFontMetrics fm(fontRegular);
        int baseHeight = fm.overlinePos();
        if (baseHeight <= 12)
        {
            baseHeight = 11;
        }
        m_rowItemHeight = qMax(22, baseHeight * 195 / (int) 96 - 3);
        if (m_rowItemHeight > 32)
        {
            m_rowItemHeight = 32;
        }
#else
        m_rowItemHeight = 22;
#endif
        QLineEdit ed;
        ed.setAttribute(Qt::WA_MacSmallSize);
        ed.setFont(fontRegular);
        QSize sizeHint = ed.sizeHint();
        m_rowItemHeight = qMax(sizeHint.height(), m_rowItemHeight);

        QComboBox box1;
        box1.setAttribute(Qt::WA_MacSmallSize);
        box1.setFont(fontRegular);
        sizeHint = box1.sizeHint();
        m_rowItemHeight = qMax(sizeHint.height(), m_rowItemHeight);

        QComboBox box2;
        box2.setAttribute(Qt::WA_MacSmallSize);
        box2.setEditable(true);// Affects on MacOSX widget
        box2.setFont(fontRegular);
        sizeHint = box2.sizeHint();
        m_rowItemHeight = qMax(sizeHint.height(), m_rowItemHeight);

        QSpinBox box3;
        box3.setAttribute(Qt::WA_MacSmallSize);
        box3.setFont(fontRegular);
        sizeHint = box3.sizeHint();
        m_rowItemHeight = qMax(sizeHint.height(), m_rowItemHeight);

        QTabBar tab;
        tab.setFont(fontRegular);
        tab.setAttribute(Qt::WA_MacSmallSize);
        tab.insertTab(0, QString("TEXT"));

        bool isDPIAware = true;
        QdfRibbonStyle *ribbonStyle = qobject_cast<QdfRibbonStyle *>(q->style());
        if (ribbonStyle)
        {
            isDPIAware = ribbonStyle->isDPIAware();
        }

        int hframe = qMax(tab.sizeHint().height(), (isDPIAware ? int(QdfDrawHelpers::dpiScaled(24.)) : 24));
        int heightTab = qMax(22, hframe + 1);
        m_heightTabs = heightTab > 22 ? heightTab : 23;
    }
}

void QdfRibbonBarPrivate::layoutRibbon()
{
    QDF_Q(QdfRibbonBar);
    /* In MacOSX the font has been selected in QdfRibbonStyle::polish(), so we have non valid reference to the tabbar. */
    if (m_tabBar == nullptr || m_quickAccessBar == nullptr)
    {
        return;
    }

    QWidget *parent = q->parentWidget();
    bool updates = parent ? parent->updatesEnabled() : true;
    if (parent && updates)
    {
        parent->setUpdatesEnabled(false);
    }

    calcHeightItem();

    bool saveRibbonBarVisible = m_ribbonBarVisible;
    m_ribbonBarVisible = true;

    QStyleOption opt;
    opt.init(q);
    const int vmargin = q->style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, q);
    int heightGroup = maxGroupHeight();
    const int tabsHeight = m_tabBar->sizeHint().height();
    const int pageAreaHeight = heightGroup;

    int quickAccessHeight = m_quickAccessBar->sizeHint().height();

    bool frameThemeEnabled = q->isFrameThemeEnabled();
    bool showTitle = frameThemeEnabled || m_showTitleAlways;

    int frameBorder = 4;

    QRect rcTabBar(QPoint(2, 0), QSize(opt.rect.width() - 2, tabsHeight));
    QRect rcPageArea(QPoint(0, tabsHeight), QSize(opt.rect.width(), pageAreaHeight + vmargin * 2 + 2 + 1));

    if (q->tabBarPosition() == QdfRibbonBar::TabBarBottomPosition)
    {
        rcTabBar.translate(0, rcPageArea.height() - 2);
        rcPageArea.translate(0, -tabsHeight);
    }

    m_rcTitle = QRect();
    m_rcHeader = QRect();
    m_rcTitleText = QRect();

    if (showTitle)
    {
        frameBorder = m_frameHelper ? m_frameHelper->frameBorder() : 0;
        int ribbonTopBorder = topBorder();
        int titleOffset = ribbonTopBorder - frameBorder;

        m_rcTitle = QRect(QPoint(-frameBorder, titleOffset),
                          QPoint(opt.rect.width() + frameBorder, q->titleBarHeight() + ribbonTopBorder));

        rcTabBar.translate(0, m_rcTitle.height() + titleOffset);
        rcPageArea.translate(0, m_rcTitle.height() + titleOffset);

        m_rcHeader = QRect(0, 0, opt.rect.width(), m_rcTitle.bottom());
        m_rcTitleText = QRect(0, 0, opt.rect.width(), m_rcTitle.bottom());

        if (!m_frameHelper)
        {
            m_rcTitleText = QRect();
        }
    }

    if (m_controlSystemButton && m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
    {
        m_controlSystemButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }

    int corner = quickAccessHeight + tabsHeight - 3;

    if (!m_controlSystemButton || m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle)
    {
        QdfStyleOptionTitleBar titleBarOpt;
        titleBarOptions(titleBarOpt);

        QSize szIcon =
                m_frameHelper ? m_frameHelper->sizeSystemIcon(titleBarOpt.icon, titleBarOpt.rect) : QSize(16, 16);
        corner = 5 + (!titleBarOpt.icon.isNull() ? szIcon.width() : 0);
    }

    QSize szBar = m_quickAccessBar->sizeHint();
    if (m_quickAccessBar->isVisible())
    {
        if (m_quickAccessBarPosition == QdfRibbonBar::QATopPosition)
        {
            m_rcQuickAccess = QRect(QPoint(5 + corner, m_rcTitle.top() + frameBorder), szBar);
        }
        else if (m_quickAccessBarPosition == QdfRibbonBar::QABottomPosition)
        {
            if (m_minimized)
            {
                m_rcQuickAccess = QRect(QPoint(0, m_rcTabBar.bottom()),
                                        QPoint(rcPageArea.right(), m_rcTabBar.bottom() + quickAccessHeight));
            }
            else if (m_tabBarPosition == QdfRibbonBar::TabBarTopPosition)
            {
                m_rcQuickAccess = QRect(QPoint(0, rcPageArea.bottom()),
                                        QPoint(rcPageArea.right(), rcPageArea.bottom() + quickAccessHeight));
            }
            else if (m_tabBarPosition == QdfRibbonBar::TabBarBottomPosition)
            {
                m_rcQuickAccess = QRect(QPoint(0, rcTabBar.bottom() + 2),
                                        QPoint(rcPageArea.right(), rcTabBar.bottom() + quickAccessHeight + 2));
            }
        }
    }

    QPoint posAccessBar = m_rcQuickAccess.topLeft();
    m_quickAccessBar->move(posAccessBar);
    m_quickAccessBar->resize(m_rcQuickAccess.size());

    int left = 2;
    int right = opt.rect.width() - 2;
    if (m_controlSystemButton)
    {
        QSize szControl(corner + 1, corner + 1);
        int top = m_rcTitle.top() + frameBorder;
        left = 0;
        int nGap = 0;

        if (q->tabBarPosition() == QdfRibbonBar::TabBarBottomPosition &&
            m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonFollowStyle)
        {
            top = rcPageArea.bottom() - m_controlSystemButton->sizeHint().height();
        }
        else if (m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle)
        {
            szControl = m_controlSystemButton->sizeHint();
            szControl.setHeight(tabsHeight);

            top = rcTabBar.top() + (tabsHeight - szControl.height()) * 0.5;
            nGap = 3;
        }
        else if (!showTitle)
        {
            szControl = m_controlSystemButton->sizeHint();
            top = rcTabBar.top() + (tabsHeight - szControl.height()) * 0.5;
        }
        else
        {
            if (top == 0)
            {
                top += frameBorder / 2;
            }
            szControl -= QSize(2, 2);
        }

        int add = q->tabBarPosition() == QdfRibbonBar::TabBarBottomPosition ? 2 : 0;
        szControl.setHeight(szControl.height() - add);
        QRect rcControl(QPoint(left + 1, top + add), szControl);
        left += szControl.width() + nGap;
        m_controlSystemButton->raise();
        m_controlSystemButton->setGeometry(rcControl);
    }

    bool quickAccessNearTabs =
            m_quickAccessBar->isVisible() && m_quickAccessBarPosition == QdfRibbonBar::QATopPosition && !showTitle;

    if (quickAccessNearTabs)
    {
        m_rcQuickAccess.setLeft(left);
        m_rcQuickAccess.setTop(m_rcTabBar.top());
        m_rcQuickAccess.setBottom(m_rcTabBar.bottom());

        m_quickAccessBar->move(m_rcQuickAccess.topLeft());

        left += szBar.width();
    }

    if (left + 6 > right)
    {
        m_rcTabBar = QRect(QPoint(0, rcTabBar.top()), QPoint(0, rcTabBar.bottom() + 1));
    }
    else
    {
        m_rcTabBar = QRect(QPoint(left, rcTabBar.top()), QPoint(right, rcTabBar.bottom() + 1));
    }

    // It's needed to apply a width of the logo if exists.
    QRect rectLogotype = m_logotypeLabel->isVisible() ? m_logotypeLabel->geometry() : QRect();
    if (!rectLogotype.isNull() && m_logotypeLabel->alignmentLogotype() == Qt::AlignLeft)
    {
        m_rcTabBar.setLeft(qMax(m_rcTabBar.left(), rectLogotype.width()));
    }

    m_tabBar->setGeometry(m_rcTabBar);
    m_tabBar->layoutWidgets();

    m_rcPageArea = rcPageArea;

    if (m_minimized)
    {
        m_rcPageArea.setHeight(0);
    }

    for (QList<QdfRibbonPage *>::iterator it = m_listPages.begin(); it != m_listPages.end(); ++it)
    {
        QdfRibbonPage *page = *(it);
        page->setGroupsHeight(heightGroup);

        if (m_logotypeLabel->isVisible())
        {
            if (m_logotypeLabel->alignmentLogotype() == Qt::AlignRight)
            {
                m_rcPageArea.adjust(0, 0, -4, 0);
            }
            else
            {
                m_rcPageArea.adjust(2, 0, 0, 0);
            }
        }
        page->setGeometry(m_rcPageArea);
    }

    int index = m_tabBar->currentIndex();
    if (validIndex(index))
    {
        QdfRibbonPage *page = m_listPages.at(index);
        page->updateLayout();
    }

    layoutTitleButton();

    if (q->isBackstageVisible())
    {
        setVisibleBackstage(true);
    }

    if (m_frameHelper)
    {
        m_frameHelper->setHeaderRect(m_rcHeader);
    }

    m_ribbonBarVisible = saveRibbonBarVisible;

    bool ribbonBarVisible = true;
    if (q->isTitleBarVisible())
    {
        ribbonBarVisible = m_rcHeader.width() > 3;

        if (m_quickAccessBarPosition == QdfRibbonBar::QABottomPosition)
        {
            if (m_controlSystemButton && m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                const int wd = m_controlSystemButton->sizeHint().width();
                ribbonBarVisible = m_rcHeader.width() - (wd * 2) > 3;
            }
            else
            {
                const int left = m_rcHeader.left();
                ribbonBarVisible = m_rcHeader.width() - left * 2 > 3;
            }
        }
    }
    else
    {
        ribbonBarVisible = opt.rect.width() >= 250;
    }

    if (m_ribbonBarVisible != ribbonBarVisible && q->QMenuBar::isVisible())
    {
        m_ribbonBarVisible = ribbonBarVisible;
        if (m_frameHelper)
        {
            m_frameHelper->postRecalcFrameLayout();
        }
    }

    if (!m_ribbonBarVisible && m_frameHelper && m_frameHelper->canHideTitle())
    {
        m_rcTitleText.setLeft(m_rcQuickAccess.left());
        m_rcTitleText.setRight(m_rcHeader.right());
        m_quickAccessBar->setGeometry(QRect());
    }

    if (showTitle && m_ribbonBarVisible)
    {
        layoutContextHeaders();
    }
    else
    {
        removeAllContextHeaders();
    }

    if (parent && updates)
    {
        parent->setUpdatesEnabled(updates);
    }

    if (m_logotypeLabel->isVisible())
    {
        int index = m_tabBar->currentIndex();
        if (validIndex(index))
        {
            if (QdfRibbonPage *page = m_listPages.at(index))
            {
                QRect rectLogotype;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                int width = m_logotypeLabel->pixmap()->width();
                int height = m_logotypeLabel->pixmap()->height();
#else
                int width = m_logotypeLabel->pixmap(Qt::ReturnByValue).width();
                int height = m_logotypeLabel->pixmap(Qt::ReturnByValue).height();
#endif
                QRect rcPage = page->geometry();

                if (height < rcPage.height())
                {
                    rectLogotype.setTop(rcPage.top());
                }
                else
                {
                    rectLogotype.setTop(0);
                    height = rcPage.height();
                }

                if (m_logotypeLabel->alignmentLogotype() == Qt::AlignRight)
                {
                    rectLogotype.setLeft(rcPage.right() - width + 4);
                }

                rectLogotype.setWidth(width);
                rectLogotype.setHeight(rcPage.height());

                m_logotypeLabel->setGeometry(rectLogotype);
                m_logotypeLabel->raise();
            }
        }
    }
}

void QdfRibbonBarPrivate::layoutTitleButton()
{
    QDF_Q(QdfRibbonBar);
    if (!q->isFrameThemeEnabled())
    {
        return;
    }

    QdfStyleOptionTitleBar titleBarOpt;
    titleBarOptions(titleBarOpt);

    QRect rcQuickAccess = m_rcQuickAccess;
    if (m_ribbonBarVisible)
    {
        if (m_quickAccessBarPosition != QdfRibbonBar::QATopPosition || !m_quickAccessBar->isVisible())
        {
            if (!m_controlSystemButton)
            {
                m_rcHeader.setLeft(m_rcHeader.left() + titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
            }
            else
            {
                if (m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonFollowStyle)
                {
                    m_rcHeader.setLeft(m_rcHeader.left() + m_controlSystemButton->sizeHint().width() - 5);
                }
                else
                {
                    m_rcHeader.setLeft(m_rcHeader.left() + titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
                }
            }

            if (m_controlSystemButton)
            {
                rcQuickAccess = m_controlSystemButton->rect();
            }
            else
            {
                rcQuickAccess = QRect();
            }
        }
        else
        {
            rcQuickAccess.setRight(rcQuickAccess.left() +
                                   (m_quickAccessBar->isVisible() ? m_quickAccessBar->width() + 2 : 12));
            m_rcHeader.setLeft(rcQuickAccess.right());
        }
    }
    else
    {
        int corner = 2 + (!titleBarOpt.icon.isNull() ? titleBarOpt.icon.actualSize(QSize(16, 16)).width() : 0);
        m_rcHeader.setLeft(corner);
    }

    bool dwmEnabled = m_frameHelper->isDwmEnabled();

    int frameBorder = m_frameHelper && dwmEnabled ? m_frameHelper->frameBorder() : 0;
    m_rcHeader.adjust(0, frameBorder, 0, 0);

    const bool buttonClose = titleBarOpt.titleBarFlags & Qt::WindowSystemMenuHint;

    const bool buttonMax =
            titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint && !(titleBarOpt.titleBarState & Qt::WindowMaximized);

    const bool buttonMin =
            titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint && !(titleBarOpt.titleBarState & Qt::WindowMinimized);

    const bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint) &&
                                (titleBarOpt.titleBarState & Qt::WindowMinimized)) ||
                               ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint) &&
                                (titleBarOpt.titleBarState & Qt::WindowMaximized)));

    QRect rcButtons = m_rcHeader;
    m_actCloseButton = addTitleButton(QStyle::SC_TitleBarCloseButton, !dwmEnabled && buttonClose, rcButtons);
    m_actNormalButton = addTitleButton(QStyle::SC_TitleBarNormalButton, !dwmEnabled && buttonNormal, rcButtons);
    m_actMaxButton = addTitleButton(QStyle::SC_TitleBarMaxButton, !dwmEnabled && buttonMax, rcButtons);
    m_actMinButton = addTitleButton(QStyle::SC_TitleBarMinButton, !dwmEnabled && buttonMin, rcButtons);
    m_rcHeader = rcButtons;

#ifdef Q_OS_WIN
    if (dwmEnabled)
    {
        int width = (::GetSystemMetrics(SM_CXSIZE) + ::GetSystemMetrics(SM_CXBORDER) + 8) * 3;
        m_rcHeader.setRight(m_rcHeader.right() - width);
    }
#endif// Q_OS_WIN
    m_rcTitleText = m_rcHeader;

    // m_rcTitleText = QRect(m_rcTitleText.left() + (m_rcHeader.right() - m_rcHeader.right()), m_rcHeader.top(),
    //     m_rcHeader.right() - (m_rcHeader.right() - m_rcHeader.right()), m_rcHeader.bottom() - m_rcHeader.top());

    QString strTitle = q->parentWidget() ? q->parentWidget()->windowTitle() : QString();

    const QFont font = QApplication::font("QMdiSubWindowTitleBar");
    const QFontMetrics fm(font);

    QRect br(fm.boundingRect(strTitle));

    int captionLength = br.width();

    if (dwmEnabled)
    {
        captionLength += 2 * 10;// glowSize = 10 for Windows7 (Vista)
    }

    captionLength = qMin(m_rcTitleText.width(), captionLength);

    QRect rcTitleText(m_rcTitleText);
    rcTitleText.setLeft(rcTitleText.left() + (rcTitleText.width() - captionLength) * 0.5);
    if (rcTitleText.left() < m_rcHeader.left())
    {
        rcTitleText.setLeft(m_rcHeader.left());
    }

    rcTitleText.setRight(rcTitleText.left() + captionLength);

    if (rcQuickAccess.intersects(rcTitleText))
    {
        captionLength = qMin(m_rcHeader.width(), captionLength);
        rcTitleText.setRight(rcTitleText.left() + captionLength);
    }
    else if (!rcQuickAccess.isValid() || !m_quickAccessBar->isVisible())
    {
        captionLength = qMin(m_rcHeader.width(), captionLength);
        if (dwmEnabled)
        {
            rcTitleText.setLeft(m_rcHeader.left() + 10);
        }
        else
        {
            rcTitleText.setLeft(m_rcHeader.left() + (m_rcHeader.width() - captionLength) * 0.5);
        }
    }

    if (rcTitleText.right() > m_rcHeader.right())
    {
        rcTitleText.setRight(m_rcHeader.right());
    }

    m_rcTitleText = rcTitleText;

    if (m_controlSystemButton &&
        (bool) q->style()->styleHint((QStyle::StyleHint) QdfRibbonStyle::SH_RibbonBackstageHideTabs))
    {
        if (QdfRibbonBackstageView *backstage = qobject_cast<QdfRibbonBackstageView *>(m_controlSystemButton->backstage()))
        {
            if (backstage->isVisible())
            {
                m_rcTitleText.setLeft(backstage->menuGeometry().width());
                m_rcTitleText.setRight(m_rcHeader.right());
            }
        }
    }
}

bool QdfRibbonBarPrivate::isExistAssociativeTabWithPage(const QdfRibbonTab *tab)
{
    bool find = false;
    for (int i = 0, count = m_listPages.size(); count > i && !find; i++)
    {
        find = m_listPages.at(i)->associativeTab() == tab;
    }
    return find;
}

void QdfRibbonBarPrivate::layoutContextHeaders()
{
    QDF_Q(QdfRibbonBar);
    removeAllContextHeaders();

    if (!(q->isFrameThemeEnabled() || m_showTitleAlways))
    {
        return;
    }

    QdfContextHeader *prevContextHeader = nullptr;

    int count = m_tabBar->getTabCount();
    if (count == 0)
    {
        return;
    }

    if (m_quickAccessBarPosition == QdfRibbonBar::QATopPosition && m_quickAccessBar->isVisible() && !m_frameHelper)
    {
        m_rcHeader.setLeft(m_rcQuickAccess.right() +
                           (m_quickAccessBar ? m_quickAccessBar->sizeHint().width() + 2 : 12));
    }

    QRect rectTitleText = m_rcTitleText.isEmpty() ? m_rcHeader : m_rcTitleText;

    for (int i = 0; i < count; ++i)
    {
        QdfRibbonTab *tab = m_tabBar->getTab(i);

        if (!tab->isVisible())
        {
            continue;
        }

        if (tab->contextColor() == QdfRibbonPage::ContextColorNone || tab->contextTextTab().isEmpty())
        {
            prevContextHeader = nullptr;
            continue;
        }

        QRect rect = tab->geometry();
        QPoint topLeft = m_tabBar->mapToParent(rect.topLeft());
        QPoint bottomRight = m_tabBar->mapToParent(rect.bottomRight());

        if (prevContextHeader && !prevContextHeader->strGroupName.isEmpty())
        {
            prevContextHeader->rcRect.setRight(bottomRight.x());
            prevContextHeader->lastTab = tab;
            tab->setContextHeader(prevContextHeader);
        }
        else
        {
            QdfContextHeader *header = new QdfContextHeader(tab);
            header->rcRect =
                    QRect(QPoint(topLeft.x(), rectTitleText.top()), QPoint(bottomRight.x(), rectTitleText.bottom() + 1));
            m_listContextHeaders.append(header);
            prevContextHeader = header;
        }
    }

    count = m_listContextHeaders.count();
    if (count == 0)
    {
        return;
    }

    if (!reduceContextHeaders())
    {
        removeAllContextHeaders();
        return;
    }

    QRect rcHeaders(QPoint(m_listContextHeaders.at(0)->rcRect.left(), m_rcTitle.top() + 6),
                    QPoint(m_listContextHeaders.at(count - 1)->rcRect.right(), m_rcTitle.bottom()));

    int captionLength = m_rcTitleText.width();
    QRect rcCaptionText(m_rcTitleText);

    if (rcCaptionText.intersects(rcHeaders))
    {
        if (rcCaptionText.center().x() > rcHeaders.center().x())
        {
            if (m_rcHeader.right() - rcHeaders.right() < captionLength)
            {
                m_rcTitleText.setLeft(rcHeaders.right());
                m_rcTitleText.setRight(m_rcHeader.right());
            }
            else
            {
                m_rcTitleText.setLeft(rcHeaders.right());
                m_rcTitleText.setRight(rcHeaders.right() + captionLength);
            }
        }
        else
        {
            if (rcHeaders.left() - m_rcHeader.left() < captionLength)
            {
                m_rcTitleText.setLeft(m_rcHeader.left());
                m_rcTitleText.setRight(rcHeaders.left());
            }
            else
            {
                m_rcTitleText.setLeft(rcHeaders.left() - captionLength);
                m_rcTitleText.setRight(rcHeaders.left());
            }
        }
    }
}

void QdfRibbonBarPrivate::layoutCorner()
{
    QDF_Q(QdfRibbonBar);
    if (QWidget *rightWidget = q->cornerWidget(Qt::TopRightCorner))
    {
        int hmargin = q->style()->pixelMetric(QStyle::PM_MenuBarHMargin, 0, q);

        if (rightWidget && rightWidget->isVisible())
        {
            QRect rcTab = m_tabBar->geometry();
            QRect rc = rightWidget->geometry();
            QPoint pos(q->width() - rc.width() - hmargin + 1, rcTab.top() + (rcTab.height() - rc.height()) / 2);
            QRect vRect = QStyle::visualRect(q->layoutDirection(), q->rect(), QRect(pos, rc.size()));
            rightWidget->setGeometry(vRect);
        }
    }
}

bool QdfRibbonBarPrivate::reduceContextHeaders()
{
    int left = m_rcHeader.left();
    int right = m_rcHeader.right();

    int count = m_listContextHeaders.count();

    for (int i = 0; i < count; i++)
    {
        QdfContextHeader *header = m_listContextHeaders.at(i);
        if (header->rcRect.left() < left)
        {
            header->rcRect.setLeft(left);
            if (header->rcRect.width() < 40)
            {
                header->rcRect.setRight(left + 40);
            }

            left = header->rcRect.right();
        }
        else
        {
            break;
        }
    }
    for (int i = count - 1; i >= 0; i--)
    {
        QdfContextHeader *header = m_listContextHeaders.at(i);
        if (header->rcRect.right() > right)
        {
            header->rcRect.setRight(right);
            if (header->rcRect.width() < 40)
            {
                header->rcRect.setLeft(right - 40);
            }

            right = header->rcRect.left();

            if (right < left)
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }
    return true;
}

void QdfRibbonBarPrivate::removeAllContextHeaders()
{
    for (int i = 0; i < m_listContextHeaders.count(); i++)
    {
        delete m_listContextHeaders.at(i);
    }
    m_listContextHeaders.clear();
}

void QdfRibbonBarPrivate::updateMinimizedModePage(QdfRibbonPage *page)
{
    QDF_Q(QdfRibbonBar);
    if (m_minimized)
    {
        page->setPageVisible(false);
        page->setWindowFlags(Qt::Popup);
        page->setFocusPolicy(Qt::NoFocus);
        page->setFocusProxy(q);
        page->setRibbonMinimized(true);
        page->setUpdatesEnabled(true);
    }
    else
    {
        Qt::WindowFlags flags = page->windowFlags();
        if (flags & Qt::Popup)
        {
            flags &= ~Qt::Popup;
            page->setWindowFlags(flags);
            page->setRibbonMinimized(false);
        }
    }
}

void QdfRibbonBarPrivate::swapVisiblePages(int index)
{
    QDF_Q(QdfRibbonBar)
    if (validIndex(index))
    {
        if (m_currentIndexPage != -1 && m_currentIndexPage != index && q->parentWidget())
        {
            q->parentWidget()->setFocus();
        }

        for (int i = 0, count = m_listPages.size(); count > i; i++)
        {
            m_listPages.at(i)->setPageVisible(index == i);
        }

        m_currentIndexPage = index;
    }
}

void QdfRibbonBarPrivate::setVisibleBackstage(bool visible)
{
    QDF_Q(QdfRibbonBar);
    if ((bool) q->style()->styleHint((QStyle::StyleHint) QdfRibbonStyle::SH_RibbonBackstageHideTabs))
    {
        if (visible)
        {
            if (m_tabBar && m_tabBar->isVisible())
            {
                m_visibleWidgets << m_tabBar;
                m_tabBar->setVisible(false);
            }

            if (m_quickAccessBar && m_quickAccessBar->isVisible())
            {
                m_visibleWidgets << m_quickAccessBar;
                m_quickAccessBar->setVisible(false);
            }

            if (m_controlSystemButton && m_controlSystemButton->isVisible())
            {
                m_visibleWidgets << m_controlSystemButton;
                m_controlSystemButton->setVisible(false);
            }
        }
        else
        {
            for (int i = 0; i < m_visibleWidgets.count(); i++)
            {
                m_visibleWidgets.at(i)->setVisible(true);
            }
            m_visibleWidgets.clear();
            layoutRibbon();
        }
    }
}

QAction *QdfRibbonBarPrivate::createSystemButton(const QIcon &icon, const QString &text)
{
    QDF_Q(QdfRibbonBar);
    QAction *actionSystemPopupBar = nullptr;
    if (icon.isNull())
    {
        QIcon iconLogo;
        QIcon icon32(":/res/logo.png");
        iconLogo.addPixmap(icon32.pixmap(QSize(32, 32), QIcon::Disabled));
        actionSystemPopupBar = new QAction(iconLogo, text, q);
    }
    else
    {
        actionSystemPopupBar = new QAction(icon, text, q);
    }

    m_controlSystemButton = new QdfRibbonSystemButton(q);
    m_controlSystemButton->show();
    m_controlSystemButton->setAutoRaise(true);
    m_controlSystemButton->setFocusPolicy(Qt::NoFocus);
    m_controlSystemButton->setDefaultAction(actionSystemPopupBar);

    m_controlSystemButton->setPopupMode(QToolButton::InstantPopup);
    m_controlSystemButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    actionSystemPopupBar->setIconText(actionSystemPopupBar->text());

    return actionSystemPopupBar;
}

QdfRibbonPage *QdfRibbonBarPrivate::insertPage(int indTab, int index)
{
    QDF_Q(QdfRibbonBar);
    QdfRibbonPage *page = new QdfRibbonPage(q);
    page->hide();
    insertPage(indTab, index, page);

    return page;
}

void QdfRibbonBarPrivate::insertPage(int indTab, int index, QdfRibbonPage *page)
{
    QDF_Q(QdfRibbonBar);
    Q_ASSERT(page != nullptr);

    bool layoutPage = index == 0 && m_listPages.size() == 0;
    if (!validIndex(index))
    {
        index = m_listPages.count();
        m_listPages.append(page);
    }
    else
    {
        m_listPages.insert(index, page);
    }

    if (QdfRibbonTab *tab = m_tabBar->getTab(indTab))
    {
        page->setAssociativeTab(tab);
    }

    if (!m_minimized)
    {
        layoutRibbon();

        if (QWidget *parent = q->parentWidget())
        {
            if (QLayout *layout = parent->layout())
            {
                layout->invalidate();
            }
        }

        int currentIndex = m_tabBar->currentIndex();
        if (currentIndex != -1 && m_currentIndexPage != currentIndex && currentIndex < m_listPages.count())
        {
            swapVisiblePages(currentIndex);
            m_currentIndexPage = currentIndex;
        }
        if (layoutPage)
        {
            swapVisiblePages(index);
        }
    }
}

void QdfRibbonBarPrivate::removePage(int index, bool deletePage)
{
    if (validIndex(index))
    {
        removeAllContextHeaders();
        QdfRibbonPage *page = m_listPages[index];
        m_listPages.removeAt(index);
        if (deletePage)
        {
            delete page;
        }
        else
        {
            updateMinimizedModePage(page);
            page->setAssociativeTab(nullptr);
            page->setParent(nullptr);
        }
    }
    m_currentIndexPage = -1;
}

int QdfRibbonBarPrivate::indexPage(QdfRibbonPage *page) const { return m_listPages.indexOf(page); }

int QdfRibbonBarPrivate::heightTabs() const { return m_heightTabs; }

int QdfRibbonBarPrivate::topBorder() const
{
    return m_frameHelper && m_frameHelper->isDwmEnabled() ? m_frameHelper->frameBorder() : 0;
}

void QdfRibbonBarPrivate::currentChanged(int index)
{
    QDF_Q(QdfRibbonBar);

    if (validIndex(index))
    {
        if (!m_minimized)
        {
            bool updates = q->updatesEnabled();
            if (updates)
            {
                q->setUpdatesEnabled(false);
            }

            emit q->currentPageIndexChanged(index);
            emit q->currentPageChanged(m_listPages[index]);
            swapVisiblePages(index);

            QdfRibbonPage *page = q->getPage(index);
            page->updateLayout();

            if (updates)
            {
                q->setUpdatesEnabled(updates);
            }
        }
        else
        {
            if (QdfRibbonPage *page = q->getPage(index))
            {
                emit q->currentPageIndexChanged(index);
                emit q->currentPageChanged(m_listPages[index]);

                page->setGeometry(QRect(QPoint(0, 0), QSize(1, 1)));

                bool updates = page->updatesEnabled();
                if (updates)
                {
                    page->setUpdatesEnabled(false);
                }

                QWidget *activePopup = qobject_cast<QdfRibbonPage *>(QApplication::activePopupWidget());

                QStyleOption opt;
                opt.init(page);

                const int vmargin = q->style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, q);
                int heightGroup = maxGroupHeight();
                int height = heightGroup + vmargin * 2 + vmargin + 1;
                page->setGroupsHeight(heightGroup);

                QPoint pnt(0, 0);
                QRect rect = q->rect();
                rect.setTop(m_rcTabBar.top());
                rect.setBottom(m_rcTabBar.bottom());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                if (q->parentWidget() && q->parentWidget()->windowState() & Qt::WindowMaximized)
                {
                    rect.translate(0, topBorder());
                }
#endif// QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

                QRect screen = QApplication::desktop()->availableGeometry(q);
                int h = height;
                if (q->mapToGlobal(QPoint(0, rect.bottom())).y() + h <= screen.height())
                {
                    pnt = q->mapToGlobal(rect.bottomLeft());
                }
                else
                {
                    pnt = q->mapToGlobal(rect.topLeft() - QPoint(0, h));
                }

                page->popup();

                QRect rc(pnt, QSize(q->width(), height));
                page->setGeometry(rc);

                if (updates)
                {
                    page->setUpdatesEnabled(updates);
                }

                if (activePopup)
                {
                    activePopup->close();
                }
            }
        }
    }
}

void QdfRibbonBarPrivate::titleBarOptions(QdfStyleOptionTitleBar &titleBarOptions) const
{
    QDF_Q(const QdfRibbonBar);
    QWidget *widget = q->parentWidget();
    if (!widget)
    {
        return;
    }

    titleBarOptions.initFrom(widget);

    titleBarOptions.frameHelper = m_frameHelper;

    if (m_activeSubControl != QStyle::SC_None)
    {
        if (m_hoveredSubControl == m_activeSubControl)
        {
            titleBarOptions.state |= QStyle::State_Sunken;
            titleBarOptions.activeSubControls = m_activeSubControl;
        }
    }
    else if (q->style()->styleHint(QStyle::SH_TitleBar_AutoRaise, 0, widget) &&
             m_hoveredSubControl != QStyle::SC_None && m_hoveredSubControl != QStyle::SC_TitleBarLabel)
    {
        titleBarOptions.state |= QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = m_hoveredSubControl;
    }
    else
    {
        titleBarOptions.state &= ~QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = QStyle::SC_None;
    }

    titleBarOptions.subControls = QStyle::SC_All;
    titleBarOptions.titleBarFlags = widget->windowFlags();
    titleBarOptions.titleBarState = widget->windowState();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (titleBarOptions.frameHelper && titleBarOptions.frameHelper->isMaximize() &&
        !(titleBarOptions.titleBarState & Qt::WindowMaximized))
    {
        titleBarOptions.titleBarState |= Qt::WindowMaximized;
    }
#endif

    if (titleBarOptions.titleBarFlags & Qt::WindowTitleHint)
    {
        QIcon icon = q->parentWidget() ? q->parentWidget()->windowIcon() : QIcon();
        QSize s = icon.actualSize(QSize(64, 64));
        titleBarOptions.icon = icon.pixmap(s);
    }

    if (m_frameHelper && titleBarOptions.icon.isNull())
    {
        titleBarOptions.icon = m_frameHelper->getFrameSmallIcon();
    }

    if (m_controlSystemButton && m_controlSystemButton->isVisible())
    {
        titleBarOptions.drawIcon =
                !m_controlSystemButton || m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle;
    }

    int border = q->style()->styleHint(QStyle::SH_TitleBar_NoBorder, &titleBarOptions, widget) ? 4 : 0;

    int paintHeight = q->titleBarHeight();
    paintHeight -= q->isMinimized() ? 2 * border : border;
    titleBarOptions.rect = m_rcTitle;

    QdfRibbonBarPrivate *that = const_cast<QdfRibbonBarPrivate *>(this);
    that->m_windowTitle = q->parentWidget() ? q->parentWidget()->windowTitle() : QString();

    if (!m_windowTitle.isEmpty())
    {
        titleBarOptions.text = m_windowTitle;
        const QFont font = QApplication::font("QMdiSubWindowTitleBar");
        titleBarOptions.fontMetrics = QFontMetrics(font);
        int width = m_rcHeader.width();
        titleBarOptions.text = titleBarOptions.fontMetrics.elidedText(m_windowTitle, Qt::ElideRight, width);
    }

    titleBarOptions.rcTitleText = m_rcTitleText;
    int visCount = (m_quickAccessBar != nullptr) ? m_quickAccessBar->visibleCount() : int(0);
    titleBarOptions.quickAccessVisibleCount = visCount;
    titleBarOptions.quickAccessOnTop = m_quickAccessBarPosition == QdfRibbonBar::QATopPosition;
    QSize szAccessBar = (m_quickAccessBar != nullptr) ? m_quickAccessBar->size() : QSize();
    titleBarOptions.quickAccessVisible =
            (m_quickAccessBar != nullptr) && m_quickAccessBar->isVisible() && !szAccessBar.isNull();
    titleBarOptions.rcQuickAccess = m_rcQuickAccess;
    titleBarOptions.existSysButton = m_controlSystemButton;
    titleBarOptions.normalSysButton =
            m_controlSystemButton ? (m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle) : true;
}

QStyle::SubControl QdfRibbonBarPrivate::getSubControl(const QPoint &pos) const
{
    QDF_Q(const QdfRibbonBar);
    QdfStyleOptionTitleBar opt;
    titleBarOptions(opt);
    return q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, pos, q->parentWidget());
}

QdfRibbonTitleButton *QdfRibbonBarPrivate::findTitleButton(QStyle::SubControl subControl) const
{
    QDF_Q(const QdfRibbonBar);
    QList<QdfRibbonTitleButton *> buttons_list = q->findChildren<QdfRibbonTitleButton *>();
    foreach (QdfRibbonTitleButton *but, buttons_list)
    {
        if (but->subControl() == subControl)
        {
            return but;
        }
    }
    return nullptr;
}

QAction *QdfRibbonBarPrivate::addTitleButton(QStyle::SubControl subControl, bool add, QRect &rcControl)
{
    QDF_Q(QdfRibbonBar);

    QdfRibbonTitleButton *but = findTitleButton(subControl);

    QAction *act = but ? but->defaultAction() : nullptr;

    if (!but && add)
    {
        act = new QAction(q);
        but = new QdfRibbonTitleButton(q, subControl);
        but->show();
        but->setDefaultAction(act);
        connect(act, SIGNAL(triggered()), this, SLOT(processClickedSubControl()));
        act->setToolTip(m_frameHelper->getSystemMenuString(subControl));
    }
    else if (but && !add)
    {
        act = but->defaultAction();
        but->setVisible(false);
        but->setParent(nullptr);
        but->deleteLater();
        but = nullptr;
        act->deleteLater();
        act = nullptr;
    }

    if (add)
    {
        QRect rcButton(rcControl.right() - rcControl.height(), rcControl.top(),
                       rcControl.right() - (rcControl.right() - rcControl.height()), rcControl.bottom());
        rcButton.adjust(0, 0, -1, 0);
        if (m_frameHelper)
        {
            int iCaptionWidth, iCaptionHeight;
            if (m_frameHelper->clientMetrics(iCaptionWidth, iCaptionHeight))
            {
                int sysBtnEdge = qMin(iCaptionHeight, rcControl.height() - 2);
                rcButton.setTopLeft(QPoint(rcControl.right() - iCaptionWidth, rcControl.top()));
                rcButton.setWidth(iCaptionWidth);
                rcButton.setHeight(sysBtnEdge);
            }
        }

        but->setGeometry(rcButton);
        rcControl.setWidth(rcControl.width() - rcButton.width());
    }
    return act;
}

QdfContextHeader *QdfRibbonBarPrivate::hitTestContextHeaders(const QPoint &point) const
{
    for (int i = 0, count = m_listContextHeaders.count(); i < count; i++)
    {
        QdfContextHeader *header = m_listContextHeaders.at(i);
        if (header->rcRect.contains(point))
        {
            return header;
        }
    }
    return nullptr;
}

QMenu *QdfRibbonBarPrivate::createContextMenu()
{
    QDF_Q(QdfRibbonBar)
    if (q->isBackstageVisible())
    {
        return nullptr;
    }

    QMenu *popup = new QMenu(q);

    QAction *action = nullptr;
    if (q->isQuickAccessVisible())
    {
        if (QdfRibbonQuickAccessBar *quickAccessBar = q->quickAccessBar())
        {
            QList<QAction *> actions = quickAccessBar->actions();
            if (actions.size() > 0)
            {
                action = popup->addAction(QdfRibbonBar::tr_compatible(RibbonCustomizeQuickAccessToolBarDotString));
                action->setObjectName(strCustomizeQAToolBar);
                connect(action, SIGNAL(triggered()), this, SLOT(toggledCustomizeBar()));

                action = popup->addAction(q->quickAccessBarPosition() == QdfRibbonBar::QATopPosition
                                                  ? QdfRibbonBar::tr_compatible(RibbonShowQuickAccessToolBarBelowString)
                                                  : QdfRibbonBar::tr_compatible(RibbonShowQuickAccessToolBarAboveString));
                action->setObjectName(strCustomizeRibbonBar);
                connect(action, SIGNAL(triggered()), this, SLOT(toggledQuickAccessBarPos()));
            }
        }
        popup->addSeparator();
    }

    action = popup->addAction(QdfRibbonBar::tr_compatible(RibbonCustomizeActionString));
    action->setObjectName(strCustomizeRibbonBar);
    connect(action, SIGNAL(triggered()), this, SLOT(toggledCustomizeBar()));

    if (q->isMinimizationEnabled())
    {
        action = popup->addAction(QdfRibbonBar::tr_compatible(RibbonMinimizeActionString));
        action->setCheckable(true);
        action->setChecked(q->isMinimized());
        connect(action, SIGNAL(triggered()), this, SLOT(toggledMinimized()));
    }
    return popup;
}

static QString findAccel(const QString &text)
{
    if (text.isEmpty())
    {
        return text;
    }

    int fa = 0;
    QChar ac;
    while ((fa = text.indexOf(QLatin1Char('&'), fa)) != -1)
    {
        ++fa;
        if (fa < text.length())
        {
            // ignore "&&"
            if (text.at(fa) == QLatin1Char('&'))
            {
                ++fa;
                continue;
            }
            else
            {
                ac = text.at(fa);
                break;
            }
        }
    }
    if (ac.isNull())
    {
        return QString();
    }
    return (QString) ac.toUpper();
}

void QdfRibbonBarPrivate::showKeyTips(QWidget *w)
{
    hideKeyTips();
    m_countKey = 0;

    if (qobject_cast<QdfRibbonBar *>(w))
    {
        createKeyTips();
    }
    else if (qobject_cast<QdfRibbonPage *>(w))
    {
        createGroupKeyTips();
    }
    else if (QMenu *menu = qobject_cast<QMenu *>(w))
    {
        createPopupKeyTips(menu);
    }

    int count = m_keyTips.count();
    if (count == 0)
    {
        return;
    }

    for (int i = count - 1; i >= 0; i--)
    {
        QdfRibbonKeyTip *keyTip = m_keyTips.at(i);
        keyTip->setExplicit(!keyTip->getStringTip().isEmpty());

        if (keyTip->isExplicit())
        {
            continue;
        }

        QString strCaption = keyTip->getCaption();
        if (strCaption.isEmpty())
        {
            keyTip->close();
            keyTip->deleteLater();
            m_keyTips.removeAt(i);
            continue;
        }

        strCaption = strCaption.toUpper();
        keyTip->setCaption(strCaption);

        if (keyTip->getPrefix().isEmpty())
        {
            QString key = ::findAccel(strCaption);
            if (key.isEmpty())
            {
                if (!strCaption.isEmpty())
                {
                    keyTip->setStringTip(QChar(strCaption[0]));
                }
            }
            else
            {
                keyTip->setStringTip(key);
            }
        }
        else
        {
            keyTip->setStringTip(keyTip->getPrefix());
        }
    }

    count = m_keyTips.count();
    for (int i = 0; i < count; i++)
    {
        QdfRibbonKeyTip *keyTip = m_keyTips.at(i);

        QString strTip = keyTip->getStringTip();

        QList<QdfRibbonKeyTip *> list;
        list.append(keyTip);

        QString strUsed("& ");
        for (int j = i + 1; j < count; j++)
        {
            QdfRibbonKeyTip *keyTipWidget = m_keyTips.at(j);
            ;
            if (keyTipWidget->getStringTip()[0] == strTip[0])
            {
                list.append(keyTipWidget);
                if (keyTipWidget->getStringTip().length() == 2)
                {
                    strUsed += keyTipWidget->getStringTip()[1];
                }
            }
        }

        if (list.count() < 2)
        {
            continue;
        }

        int k = 1;
        for (int j = 0; j < list.count(); j++)
        {
            QdfRibbonKeyTip *keyTipWidget = list.at(j);
            if (keyTipWidget->getStringTip().length() > 1)
            {
                continue;
            }

            QChar chAccel(0);
            QString strCaption = keyTipWidget->getCaption();
            for (int n = 0; n < strCaption.length() - 1; n++)
            {
                if ((strCaption[n] == ' ') && (strUsed.indexOf(strCaption[n + 1]) == -1))
                {
                    chAccel = strCaption[n + 1];
                    strUsed += chAccel;
                    break;
                }
            }

            if (chAccel.isNull())
            {
                int start = strCaption[0] == QLatin1Char('&') ? 1 : 0;
                if (strCaption[start] == keyTipWidget->getStringTip()[0])
                {
                    start++;
                }

                for (int n = start; n < strCaption.length(); n++)
                {
                    if (strUsed.indexOf(strCaption[n]) == -1)
                    {
                        chAccel = keyTipWidget->getCaption()[n];
                        strUsed += chAccel;
                        break;
                    }
                }
            }
            if (chAccel.isNull())
            {
                QString str = QString("%1").arg(k);
                chAccel = str[0];
                k++;
            }
            keyTipWidget->setStringTip(keyTipWidget->getStringTip() + chAccel);
        }
    }

    calcPositionKeyTips();

    if (m_keyTips.count() > 0)
    {
        qApp->installEventFilter(this);
    }

    QDF_Q(QdfRibbonBar);
    q->setFocus();
}

bool QdfRibbonBarPrivate::hideKeyTips()
{
    bool res = false;
    if (m_keyTips.count() > 0)
    {
        qApp->removeEventFilter(this);
        res = true;
    }
    destroyKeyTips();
    m_countKey = 0;
    return res;
}

void QdfRibbonBarPrivate::createKeyTips()
{
    QDF_Q(QdfRibbonBar);
    m_levels.push(q);

    QList<QToolButton *> list = m_quickAccessBar->findChildren<QToolButton *>();
    for (int i = 0, count = list.count(); i < count; i++)
    {
        QToolButton *button = list.at(i);
        if (!button->isVisible() || button->objectName() == QLatin1String("QdfRibbonQuickAccessButton"))
        {
            continue;
        }

        int k = 1;
        QString strCaption;
        if (QAction *act = button->defaultAction())
        {
            strCaption = act->property(_qdf_KeyTip).toString();
            if (strCaption.isEmpty())
            {
                QList<QAction *> listActs = m_quickAccessBar->actions();
                int index = listActs.indexOf(act);
                if (index != -1)
                {
                    k = index;
                }
            }
        }

        if (strCaption.isEmpty())
        {
            if (k < 10)
            {
                strCaption = QString("%1").arg(k);
            }
            else if (k < 19)
            {
                strCaption = QString("%1").arg(k - 9);
            }
            else
            {
                break;
            }
        }

        m_keyTips.append(
                new QdfRibbonKeyTip(q, button, strCaption,
                                    button->mapToGlobal(QPoint(button->rect().center().x(), button->rect().bottom() - 11)),
                                    QdfRibbonKeyTip::AlignTipCenter | QdfRibbonKeyTip::AlignTipTop, button->isEnabled()));
    }

    if (m_tabBar && m_tabBar->isVisible())
    {
        for (int i = 0, count = m_tabBar->getTabCount(); i < count; i++)
        {
            QdfRibbonTab *tab = m_tabBar->getTab(i);

            bool propKeyTip = true;
            QString strCaption = tab->defaultAction()->property(_qdf_KeyTip).toString();

            if (strCaption.isEmpty())
            {
                strCaption = tab->textTab();
                propKeyTip = false;
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
            {
                strCaption = QString();
            }

            if (!strCaption.isEmpty() && tab->isEnabled() && tab->isVisible())
            {
                QPoint pt(tab->rect().center().x(), tab->rect().bottom() - 9);
                QdfRibbonKeyTip *keyTip = new QdfRibbonKeyTip(q, tab, strCaption, tab->mapToGlobal(pt),
                                                              QdfRibbonKeyTip::AlignTipCenter | QdfRibbonKeyTip::AlignTipTop, true);
                m_keyTips.append(keyTip);
            }
        }
    }

    if (m_controlSystemButton)
    {
        QPoint pt(m_controlSystemButton->rect().center().x(), m_controlSystemButton->rect().center().y());

        QString strCaption = m_controlSystemButton->defaultAction()->property(_qdf_KeyTip).toString();
        if (strCaption.isEmpty())
        {
            strCaption = m_controlSystemButton->text();
        }

        m_keyTips.append(new QdfRibbonKeyTip(q, m_controlSystemButton, strCaption, m_controlSystemButton->mapToGlobal(pt),
                                             QdfRibbonKeyTip::AlignTipCenter | QdfRibbonKeyTip::AlignTipVCenter,
                                             m_controlSystemButton->isEnabled()));
    }
}

void QdfRibbonBarPrivate::createGroupKeyTips()
{
    int index = m_tabBar->currentIndex();
    if (!validIndex(index))
    {
        return;
    }

    QdfRibbonPage *page = m_listPages.at(index);
    m_levels.push(page);
    for (int i = 0, count = page->groupCount(); i < count; i++)
    {
        QdfRibbonGroup *group = page->getGroup(i);
        bool visible = group->isVisible();

        QList<QWidget *> list = group->findChildren<QWidget *>();
        for (int j = 0; j < list.count(); j++)
        {
            QWidget *widget = list.at(j);
            createWidgetKeyTips(group, widget, QString(),
                                page->rect().translated(page->mapToGlobal(page->rect().topLeft())), visible);
        }
    }
}

static QString getTextAction(const QdfRibbonGroup &group, QWidget &widget, bool &propKeyTip)
{
    QString strCaption;
    foreach (QAction *action, group.actions())
    {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action))
        {
            if (&widget == widgetAction->defaultWidget())
            {
                strCaption = widgetAction->property(_qdf_KeyTip).toString();
                if (strCaption.isEmpty())
                {
                    strCaption = widgetAction->text();
                    propKeyTip = false;
                }
                break;
            }
        }
    }
    return strCaption;
}

void QdfRibbonBarPrivate::createWidgetKeyTips(QdfRibbonGroup *group, QWidget *widget, const QString &prefix,
                                              const QRect &rcGroups, bool visible)
{
    QDF_Q(QdfRibbonBar);

    if (!widget)
    {
        return;
    }

    bool propKeyTip = true;
    QString strCaption;
    if (QAbstractButton *button = qobject_cast<QAbstractButton *>(widget))
    {
        strCaption = ::getTextAction(*group, *button, propKeyTip);
        if (strCaption.isEmpty())
        {
            if (QToolButton *toolButton = qobject_cast<QToolButton *>(button))
            {
                if (QAction *action = toolButton->defaultAction())
                {
                    strCaption = action->property(_qdf_KeyTip).toString();
                    if (strCaption.isEmpty())
                    {
                        strCaption = action->text();
                        propKeyTip = false;
                    }
                }

                if (strCaption.isEmpty())
                {
                    if (QdfRibbonGallery *ribGallery = qobject_cast<QdfRibbonGallery *>(toolButton->parentWidget()))
                    {
                        if (toolButton->property(_qdf_PopupButtonGallery).toBool())
                        {
                            if (QdfOfficePopupMenu *menu = ribGallery->popupMenu())
                            {
                                if (QAction *act = menu->defaultAction())
                                {
                                    strCaption = act->text();
                                }
                            }
                            if (strCaption.isEmpty())
                            {
                                strCaption = ::getTextAction(*group, *toolButton->parentWidget(), propKeyTip);
                            }
                        }
                    }
                    else if (strCaption.isEmpty())
                    {
                        strCaption = group->title();
                    }
                }
            }
            else
            {
                strCaption = ::getTextAction(*group, *widget, propKeyTip);
                if (strCaption.isEmpty())
                {
                    strCaption = button->text();
                }
            }
        }
    }

    bool textWidget = true;
    if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(widget))
    {
        textWidget = !gallery->popupMenu();
    }

    if (strCaption.isEmpty() && textWidget)
    {
        strCaption = ::getTextAction(*group, *widget, propKeyTip);
    }

    if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
    {
        strCaption = QString();
    }

    if (strCaption.isEmpty())
    {
        return;
    }

    QRect rc = widget->rect();
    rc.translate(widget->mapToGlobal(rc.topLeft()));

    if (rc.isEmpty())
    {
        return;
    }

    QPoint pt(rc.left() + 21, rc.bottom() - 4);
    uint align = QdfRibbonKeyTip::AlignTipTop;

    QStyleOption opt;
    opt.init(q);

    QRect rcGroup = group->rect();
    rcGroup.translate(group->mapToGlobal(rcGroup.topLeft()));
    const int heightCaptionGroup =
            q->style()->pixelMetric((QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonHeightCaptionGroup, &opt, group);
    QRect rcCaption(QPoint(rcGroup.left(), rcGroup.bottom() - heightCaptionGroup + 2),
                    QPoint(rcGroup.right(), rcGroup.bottom()));
    rcGroup.setBottom(rcCaption.top());

    if (rc.intersects(rcCaption))
    {
        pt = QPoint(rc.center().x(), rcGroups.bottom() - 6);
        align = QdfRibbonKeyTip::AlignTipTop | QdfRibbonKeyTip::AlignTipCenter;
    }
    else if ((Qt::ToolButtonStyle) widget->property("toolButtonStyle").toInt() == Qt::ToolButtonTextUnderIcon &&
             rc.height() > rcGroup.height() * 0.5)
    {
        pt = QPoint(rc.center().x(), rcGroup.bottom() - 2);
        align = QdfRibbonKeyTip::AlignTipCenter | QdfRibbonKeyTip::AlignTipVCenter;
    }
    else if (qobject_cast<QdfRibbonGallery *>(widget))
    {
        pt = QPoint(rc.right(), rcGroup.bottom() - 2);
        align = QdfRibbonKeyTip::AlignTipCenter | QdfRibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() < rcGroup.top() + rcGroup.height() * 1 / 3)
    {
        ///
        pt = QPoint(rc.left() + 11, rcGroup.top() + 2);
        align = QdfRibbonKeyTip::AlignTipLeft | QdfRibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() > rcGroup.top() + rcGroup.height() * 2 / 3)
    {
        pt = QPoint(rc.left() + 11, rcGroup.bottom() - 2);
        align = QdfRibbonKeyTip::AlignTipLeft | QdfRibbonKeyTip::AlignTipVCenter;
    }
    else
    {
        pt = QPoint(rc.left() + 11, rcGroup.center().y() - 1);
        align = QdfRibbonKeyTip::AlignTipLeft | QdfRibbonKeyTip::AlignTipVCenter;
    }
    if (qobject_cast<QCheckBox *>(widget) || qobject_cast<QRadioButton *>(widget))
    {
        pt.setX(rc.left() + 6);
    }

    QdfRibbonKeyTip *keyTip = new QdfRibbonKeyTip(q, widget, strCaption, pt, align, widget->isEnabled());

    if (!prefix.isEmpty())
    {
        keyTip->setPrefix(prefix);
    }

    QRect rcIntersect;
    rcIntersect = rcGroups.intersected(rc);
    keyTip->setVisibleTip(visible && (rcIntersect == rc) && (widget->isVisible()));
    m_keyTips.append(keyTip);
}

bool QdfRibbonBarPrivate::createPopupKeyTips(QMenu *levalMenu)
{
    QDF_Q(QdfRibbonBar);
    QMenu *menu = levalMenu ? levalMenu : qobject_cast<QMenu *>(QApplication::activePopupWidget());
    if (menu)
    {
        m_levels.push(menu);

        QList<QAction *> list = menu->actions();
        for (int i = 0; i < list.count(); i++)
        {
            QAction *action = list.at(i);

            if (action->text().count(QString(_qdf_PopupLable)) != 0)
            {
                continue;
            }

            bool propKeyTip = true;
            QString strCaption = action->property(_qdf_KeyTip).toString();

            if (strCaption.isEmpty())
            {
                strCaption = action->text();
                propKeyTip = false;
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
            {
                strCaption = QString();
            }

            if (!action->isVisible() || strCaption.isEmpty())
            {
                continue;
            }

            QRect rect = menu->actionGeometry(action);
            QPoint pt(rect.left() + 12, rect.bottom() - 11);
            QdfRibbonKeyTip *keyTip = new QdfRibbonKeyTip(q, menu, strCaption, menu->mapToGlobal(pt),
                                                          QdfRibbonKeyTip::AlignTipTop, action->isEnabled(), action);

            m_keyTips.append(keyTip);
        }

        QList<QWidget *> listWidget = menu->findChildren<QWidget *>();
        for (int i = 0; i < listWidget.count(); i++)
        {
            QWidget *widget = listWidget.at(i);

            bool propKeyTip = true;
            QString strCaption;

            if (QToolButton *toolButton = qobject_cast<QToolButton *>(widget))
            {
                if (QAction *action = toolButton->defaultAction())
                {
                    strCaption = action->property(_qdf_KeyTip).toString();
                    if (strCaption.isEmpty())
                    {
                        strCaption = action->text();
                        propKeyTip = false;
                    }
                }
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
            {
                strCaption = QString();
            }

            if (!widget->isVisible() || strCaption.isEmpty())
            {
                continue;
            }

            QPoint pt(widget->rect().left() + 12, widget->rect().bottom() - 11);
            QdfRibbonKeyTip *keyTip = new QdfRibbonKeyTip(q, widget, strCaption, widget->mapToGlobal(pt),
                                                          QdfRibbonKeyTip::AlignTipTop, widget->isEnabled());
            m_keyTips.append(keyTip);
        }
        return true;
    }
    return false;
}

void QdfRibbonBarPrivate::destroyKeyTips()
{
    for (int i = 0; i < m_keyTips.count(); i++)
    {
        QdfRibbonKeyTip *keyTip = m_keyTips.at(i);
        keyTip->close();
        keyTip->deleteLater();
    }
    m_keyTips.clear();
}

void QdfRibbonBarPrivate::calcPositionKeyTips()
{
    for (int i = 0, count = m_keyTips.count(); i < count; i++)
    {
        QdfRibbonKeyTip *keyTip = m_keyTips.at(i);
        if (!keyTip->isVisibleTip())
        {
            continue;
        }

        QSize sz(0, 0);
        QFontMetrics fontMetrics(keyTip->font());
        QRect rect = fontMetrics.boundingRect(keyTip->getStringTip());
        sz = QSize(qMax(rect.width() + 6, 16), qMax(rect.height() + 2, 15));

        QPoint pt = keyTip->posTip();
        uint flagAlign = keyTip->getAlign();

        QRect rc(QPoint(pt.x() - ((flagAlign & QdfRibbonKeyTip::AlignTipCenter)  ? sz.width() * 0.5
                                  : (flagAlign & QdfRibbonKeyTip::AlignTipRight) ? sz.width()
                                                                                 : 0),
                        pt.y() - (flagAlign & QdfRibbonKeyTip::AlignTipVCenter  ? sz.height() * 0.5
                                  : flagAlign & QdfRibbonKeyTip::AlignTipBottom ? sz.height()
                                                                                : 0)),
                 sz);

        keyTip->setGeometry(rc);
        keyTip->show();
    }
}

int QdfRibbonBarPrivate::rowItemHeight() const { return m_rowItemHeight; }

int QdfRibbonBarPrivate::rowItemCount() const { return m_rowItemCount; }

int QdfRibbonBarPrivate::maxGroupHeight() const
{
    QDF_Q(const QdfRibbonBar);
    int ret = rowItemHeight() * rowItemCount();
    QStyleOption opt;
    opt.init(q);
    ret += q->style()->pixelMetric((QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonHeightCaptionGroup, &opt, nullptr) + 4;
    for (int i = 0, count = m_listPages.count(); count > i; i++)
    {
        QdfRibbonPage *page = m_listPages[i];
        for (int g = 0, countG = page->groupCount(); countG > g; g++)
        {
            if (QdfRibbonGroup *group = page->getGroup(g))
            {
                ret = qMax(ret, group->sizeHint().height());
            }
        }
    }
    return ret;
}

bool QdfRibbonBarPrivate::pressTipCharEvent(const QKeyEvent *key)
{
    QDF_Q(QdfRibbonBar);
    QString str = key->text().toUpper();
    if (str.isEmpty())
    {
        return false;
    }
    for (int i = 0; i < m_keyTips.count(); i++)
    {
        QdfRibbonKeyTip *keyTip = m_keyTips.at(i);
        int length = keyTip->getStringTip().length();
        if (length > m_countKey)
        {
            if (keyTip->getStringTip()[m_countKey] == str[0])
            {
                if (length - 1 > m_countKey)
                {
                    m_countKey++;

                    QString str = keyTip->getStringTip().left(m_countKey);
                    for (int j = m_keyTips.count() - 1; j >= 0; j--)
                    {
                        keyTip = m_keyTips.at(j);
                        if (keyTip->getStringTip().left(m_countKey) != str)
                        {
                            m_keyTips.removeAt(j);
                            keyTip->hide();
                            keyTip->deleteLater();
                        }
                    }
                }
                else
                {
                    if (QAbstractButton *absButton = qobject_cast<QAbstractButton *>(keyTip->getOwner()))
                    {
                        if (keyTip->isEnabledTip() && keyTip->isVisibleTip())
                        {
                            hideKeyTips();

                            m_keyTipsShowing = false;
                            emit q->keyTipsShowed(false);

                            bool clearLevel = true;
                            bool showMenu = false;
                            if (QToolButton *button = qobject_cast<QToolButton *>(absButton))
                            {
                                if (button->popupMode() == QToolButton::InstantPopup)
                                {
                                    clearLevel = false;
                                }
                                else if (button->popupMode() == QToolButton::MenuButtonPopup)
                                {
                                    button->showMenu();
                                    clearLevel = false;
                                    showMenu = true;
                                }
                            }
                            if (clearLevel)
                            {
                                m_levels.clear();
                            }

                            if (!showMenu)
                            {
                                absButton->animateClick(0);
                            }
                        }
                        return false;
                    }
                    else if (qobject_cast<QMenu *>(keyTip->getOwner()) && keyTip->isEnabledTip() &&
                             keyTip->isVisibleTip())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit q->keyTipsShowed(false);

                        if (QAction *action = keyTip->getAction())
                        {
                            if (qobject_cast<QdfRibbonPageSystemPopup *>(action->menu()))
                            {
                                if (QMenu *activPopup =
                                            qobject_cast<QdfRibbonSystemPopupBar *>(QApplication::activePopupWidget()))
                                {
                                    activPopup->setActiveAction(action);
                                }
                                return false;
                            }
                            else
                            {
                                m_levels.clear();
                                action->trigger();
                                QWidget *owner = keyTip->getOwner();
                                owner->close();

                                if (qobject_cast<QdfRibbonPageSystemPopup *>(owner))
                                {
                                    if (QWidget *activPopup =
                                                qobject_cast<QdfRibbonSystemPopupBar *>(QApplication::activePopupWidget()))
                                    {
                                        activPopup->close();
                                    }
                                }
                                return false;
                            }
                        }
                    }
                    else if (qobject_cast<QdfRibbonTab *>(keyTip->getOwner()))
                    {
                        QdfKeyTipEvent kte(keyTip);
                        QApplication::sendEvent(q, &kte);
                        return false;
                    }
                    else if (keyTip->getOwner() && keyTip->isEnabledTip() && keyTip->isVisibleTip())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit q->keyTipsShowed(false);

                        m_levels.clear();
                        keyTip->getOwner()->setFocus();
                    }
                }
                return false;
            }
        }
    }
    return false;
}

bool QdfRibbonBarPrivate::eventFilter(QObject *obj, QEvent *event)
{
    QDF_Q(QdfRibbonBar)
    switch (event->type())
    {
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::NonClientAreaMouseButtonPress:
        case QEvent::NonClientAreaMouseButtonRelease:
        case QEvent::NonClientAreaMouseButtonDblClick:
            {
                m_keyTipsShowing = false;
                emit q->keyTipsShowed(false);

                hideKeyTips();
                m_levels.clear();
            }
            break;
        case QEvent::Show:
            if (qobject_cast<QdfRibbonPageSystemPopup *>(obj))
            {
                hideKeyTips();
                m_levels.clear();
            }
            break;
        case QEvent::KeyPress:
            if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event))
            {
                if (keyEvent->key() != Qt::Key_Escape)
                {
                    if ((qobject_cast<QMainWindow *>(obj) || (m_minimized && qobject_cast<QdfRibbonPage *>(obj))))
                    {
                        pressTipCharEvent(keyEvent);
                    }
                    else if (qobject_cast<QMenu *>(obj))
                    {
                        return !pressTipCharEvent(keyEvent);
                    }
                }
                else
                {
                    if (m_levels.size() > 1 && m_levels.pop())
                    {
                        showKeyTips(m_levels.pop());
                    }
                    else if (m_levels.size() > 0 && m_levels.pop())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit q->keyTipsShowed(false);
                    }
                }
            }
            break;
        case QEvent::Close:
            if (m_minimized)
            {
                if (m_levels.size() > 1 && m_levels.pop())
                {
                    showKeyTips(m_levels.pop());
                }
            }
            break;

        default:
            break;
    }
    return false;
}

void QdfRibbonBarPrivate::processClickedSubControl()
{
    QDF_Q(QdfRibbonBar);
    if (QAction *action = qobject_cast<QAction *>(sender()))
    {
        QWidget *widget = q->parentWidget();
        if (widget == nullptr)
        {
            return;
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (m_frameHelper)
        {
            if (m_actCloseButton == action)
            {
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarCloseButton);
            }
            else if (m_actNormalButton == action)
            {
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarNormalButton);
            }
            else if (m_actMaxButton == action)
            {
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarMaxButton);
            }
            else if (m_actMinButton == action)
            {
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarMinButton);
            }
        }
#else
        if (m_actCloseButton == action)
        {
            widget->close();
        }
        else if (m_actNormalButton == action)
        {
            widget->showNormal();
        }
        else if (m_actMaxButton == action)
        {
            widget->showMaximized();
        }
        else if (m_actMinButton == action)
        {
            widget->showMinimized();
        }
#endif
    }
}

void QdfRibbonBarPrivate::createCustomizeDialog()
{
    QDF_Q(QdfRibbonBar)

    if (m_customizeDialog != nullptr)
    {
        return;
    }

    m_customizeDialog = new QdfRibbonCustomizeDialog(q->parentWidget());
    QdfRibbonQuickAccessBarCustomizePage *quickAccessBarCustomizePage = new QdfRibbonQuickAccessBarCustomizePage(q);
    m_customizeDialog->addPage(quickAccessBarCustomizePage);
    QdfRibbonBarCustomizePage *ribbonBarCustomizePage = new QdfRibbonBarCustomizePage(q);
    m_customizeDialog->addPage(ribbonBarCustomizePage);
}

void QdfRibbonBarPrivate::toggledCustomizeBar()
{
    QDF_Q(QdfRibbonBar)
    q->showCustomizeDialog();
}

void QdfRibbonBarPrivate::toggledQuickAccessBarPos()
{
    QDF_Q(QdfRibbonBar)
    q->setQuickAccessBarPosition(q->quickAccessBarPosition() == QdfRibbonBar::QATopPosition ? QdfRibbonBar::QABottomPosition
                                                                                            : QdfRibbonBar::QATopPosition);
}

void QdfRibbonBarPrivate::toggledMinimized()
{
    QDF_Q(QdfRibbonBar)
    q->setMinimized(!q->isMinimized());
}

QdfRibbonBar::QdfRibbonBar(QWidget *parent) : QMenuBar(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonBar);
    QDF_D(QdfRibbonBar);
    d->init();
}

QdfRibbonBar::~QdfRibbonBar() { QDF_FINI_PRIVATE(); }

bool QdfRibbonBar::isVisible() const
{
    QDF_D(const QdfRibbonBar);
    return QMenuBar::isVisible() && d->m_ribbonBarVisible;
}

QdfRibbonPage *QdfRibbonBar::addPage(const QString &text) { return insertPage(-1, text); }

void QdfRibbonBar::addPage(QdfRibbonPage *page) { insertPage(-1, page); }

QdfRibbonPage *QdfRibbonBar::insertPage(int index, const QString &text)
{
    QDF_D(QdfRibbonBar);
    int indTab = d->m_tabBar->insertTab(index, text);

    QdfRibbonPage *page = d->insertPage(indTab, index);
    Q_ASSERT(page != nullptr);
    page->setTitle(text);
    d->updateMinimizedModePage(page);
    return page;
}

void QdfRibbonBar::insertPage(int index, QdfRibbonPage *page)
{
    Q_ASSERT(page != nullptr);

    QDF_D(QdfRibbonBar);
    page->setParent(this);
    int indTab = d->m_tabBar->insertTab(index, page->title());
    page->hide();
    d->insertPage(indTab, index, page);
    d->updateMinimizedModePage(page);
}

void QdfRibbonBar::movePage(QdfRibbonPage *page, int newIndex)
{
    QDF_D(QdfRibbonBar);
    movePage(d->indexPage(page), newIndex);
}

void QdfRibbonBar::movePage(int index, int newIndex)
{
    QDF_D(QdfRibbonBar);
    if (d->validIndex(index) && d->validIndex(newIndex))
    {
        d->m_tabBar->moveTab(index, newIndex);
        d->m_listPages.move(index, newIndex);
        d->layoutRibbon();
    }
}

void QdfRibbonBar::removePage(QdfRibbonPage *page)
{
    QDF_D(QdfRibbonBar);
    int index = d->indexPage(page);
    if (index != -1)
    {
        removePage(index);
    }
}

void QdfRibbonBar::removePage(int index)
{
    QDF_D(QdfRibbonBar);
    d->removePage(index);
    d->m_tabBar->removeTab(index);
    d->layoutRibbon();
}

void QdfRibbonBar::detachPage(QdfRibbonPage *page)
{
    QDF_D(QdfRibbonBar);
    int index = d->indexPage(page);
    if (index != -1)
    {
        detachPage(index);
    }
}

void QdfRibbonBar::detachPage(int index)
{
    QDF_D(QdfRibbonBar);
    d->removePage(index, false);
    d->m_tabBar->removeTab(index);
    d->layoutRibbon();
}

void QdfRibbonBar::clearPages()
{
    for (int i = getPageCount() - 1; i >= 0; i--)
    {
        removePage(i);
    }
}

bool QdfRibbonBar::isKeyTipsShowing() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_keyTipsShowing;
}

bool QdfRibbonBar::keyTipsEnabled() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_keyTipsEnabled;
}

void QdfRibbonBar::setKeyTipsEnable(bool enable)
{
    QDF_D(QdfRibbonBar);
    d->m_keyTipsEnabled = enable;
}

bool QdfRibbonBar::isKeyTipsComplement() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_keyTipsComplement;
}

void QdfRibbonBar::setKeyTipsComplement(bool complement)
{
    QDF_D(QdfRibbonBar);
    d->m_keyTipsComplement = complement;
}

void QdfRibbonBar::setKeyTip(QAction *action, const QString &keyTip)
{
    if (!action)
    {
        Q_ASSERT(false);
        return;
    }
    action->setProperty(_qdf_KeyTip, keyTip);
}

void QdfRibbonBar::setLogoPixmap(Qt::AlignmentFlag alignment, const QPixmap &logotype)
{
    QDF_D(QdfRibbonBar);
    if (alignment == Qt::AlignRight || alignment == Qt::AlignLeft)
    {
        if (logotype.isNull())
        {
            Q_ASSERT(false);
            return;
        }
        d->m_logotypeLabel->setPixmap(logotype);
        d->m_logotypeLabel->setAlignmentLogotype(alignment);
        d->m_logotypeLabel->setVisible(!logotype.isNull());

        int index = currentPageIndex();
        if (index != -1)
        {
            if (QdfRibbonPage *page = getPage(index))
            {
                page->qdf_d()->updateLayout();
            }
        }
        d->layoutRibbon();
        return;
    }
    Q_ASSERT(false);
}

QPixmap QdfRibbonBar::logoPixmap(Qt::AlignmentFlag &alignment) const
{
    QDF_D(const QdfRibbonBar);

    alignment = d->m_logotypeLabel->alignmentLogotype();
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    return d->m_logotypeLabel->isVisible() ? *d->m_logotypeLabel->pixmap() : QPixmap();
#else
    return d->m_logotypeLabel->isVisible() ? d->m_logotypeLabel->pixmap(Qt::ReturnByValue) : QPixmap();
#endif
}

void QdfRibbonBar::setTitleBackground(const QPixmap &px)
{
    QDF_D(QdfRibbonBar);
    d->m_pixTitleBackground = px;
    update();
}

const QPixmap &QdfRibbonBar::titleBackground() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_pixTitleBackground;
}

QdfRibbonQuickAccessBar *QdfRibbonBar::quickAccessBar() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_quickAccessBar;
}

void QdfRibbonBar::showQuickAccess(bool show)
{
    QDF_D(QdfRibbonBar);
    d->m_quickAccessBar->setVisible(show);
}

bool QdfRibbonBar::isQuickAccessVisible() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_quickAccessBar && d->m_quickAccessBar->isVisible();
}

void QdfRibbonBar::setQuickAccessBarPosition(QdfRibbonBar::QuickAccessBarPosition position)
{
    QDF_D(QdfRibbonBar);
    if (d->m_quickAccessBarPosition != position)
    {
        d->m_quickAccessBarPosition = position;
        d->layoutCorner();
        d->layoutRibbon();
        updateGeometry();
    }
}

QdfRibbonBar::QuickAccessBarPosition QdfRibbonBar::quickAccessBarPosition() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_quickAccessBarPosition;
}

void QdfRibbonBar::setTabBarPosition(QdfRibbonBar::TabBarPosition position)
{
    QDF_D(QdfRibbonBar);
    if (d->m_tabBarPosition != position)
    {
        d->m_tabBarPosition = position;
        d->layoutCorner();
        d->layoutRibbon();
        updateGeometry();
    }
}

QdfRibbonBar::TabBarPosition QdfRibbonBar::tabBarPosition() const
{
    QDF_D(const QdfRibbonBar);
    QdfRibbonBar::TabBarPosition pos = d->m_tabBarPosition;
    if (pos == QdfRibbonBar::TabBarBottomPosition && isMinimized())
    {
        pos = QdfRibbonBar::TabBarTopPosition;
    }
    return pos;
}

void QdfRibbonBar::setMinimizedFlag(bool flag)
{
    QDF_D(QdfRibbonBar);
    if (d->m_minimized != flag)
    {
        d->m_minimized = flag;

        if (!d->m_minimized && (d->m_tabBar && d->m_tabBar->getTabCount() > 0 && d->m_tabBar->currentIndex() == -1))
        {
            setCurrentPageIndex(0);
        }

        if (d->m_minimized)
        {
            for (int i = 0, count = d->m_listPages.size(); count > i; i++)
            {
                d->updateMinimizedModePage(d->m_listPages.at(i));
            }
        }
        else
        {
            for (int i = 0, count = d->m_listPages.size(); count > i; i++)
            {
                d->updateMinimizedModePage(d->m_listPages.at(i));
            }
            d->currentChanged(currentPageIndex());
        }

        d->m_tabBar->refresh();

        d->layoutRibbon();
        adjustSize();

        emit minimizationChanged(d->m_minimized);
    }
}

#ifdef Q_OS_WIN
void QdfRibbonBar::updateWindowTitle()
{
    QDF_D(QdfRibbonBar);
    if (d->m_frameHelper)
    {
        d->layoutRibbon();
    }
}
#endif// Q_OS_WIN

void QdfRibbonBar::minimize()
{
    setMinimizedFlag(true);
}

bool QdfRibbonBar::isMinimized() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_minimized;
}

void QdfRibbonBar::setMinimized(bool flag) { setMinimizedFlag(flag); }

void QdfRibbonBar::maximize() { setMinimizedFlag(false); }

bool QdfRibbonBar::isMaximized() const { return !isMinimized(); }

void QdfRibbonBar::setMaximized(bool flag) { setMinimizedFlag(!flag); }

void QdfRibbonBar::setMinimizationEnabled(bool enabled)
{
    QDF_D(QdfRibbonBar);
    d->m_minimizationEnabled = enabled;
}

bool QdfRibbonBar::isMinimizationEnabled() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_minimizationEnabled;
}

QdfRibbonCustomizeManager *QdfRibbonBar::customizeManager()
{
    QDF_D(QdfRibbonBar);
    if (d->m_customizeManager == nullptr)
    {
        d->m_customizeManager = new QdfRibbonCustomizeManager(this);
    }
    return d->m_customizeManager;
}

QdfRibbonCustomizeDialog *QdfRibbonBar::customizeDialog()
{
    QDF_D(QdfRibbonBar);
    d->createCustomizeDialog();
    return d->m_customizeDialog;
}

void QdfRibbonBar::showCustomizeDialog()
{
    QDF_D(QdfRibbonBar)
    QdfRibbonCustomizeDialog *dialog = customizeDialog();
    Q_ASSERT(dialog != nullptr);
    if (QAction *action = qobject_cast<QAction *>(d->sender()))
    {
        if (action->objectName() == strCustomizeQAToolBar)
        {
            QdfRibbonQuickAccessBarCustomizePage *page = nullptr;
            for (int i = 0; i < dialog->pageCount(); ++i)
            {
                page = qobject_cast<QdfRibbonQuickAccessBarCustomizePage *>(dialog->pageByIndex(i));
                if (page != nullptr)
                {
                    dialog->setCurrentPage(page);
                    break;
                }
            }
        }
        else if (action->objectName() == strCustomizeRibbonBar)
        {
            QdfRibbonBarCustomizePage *page = nullptr;
            for (int i = 0; i < dialog->pageCount(); ++i)
            {
                page = qobject_cast<QdfRibbonBarCustomizePage *>(dialog->pageByIndex(i));
                if (page != nullptr)
                {
                    dialog->setCurrentPage(page);
                    break;
                }
            }
        }
    }
    dialog->exec();
}

void QdfRibbonBar::setCurrentPageIndex(int index)
{
    QDF_D(QdfRibbonBar);
    d->m_tabBar->setCurrentIndex(index);
    updateLayout();
}

int QdfRibbonBar::currentPageIndex() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_tabBar->currentIndex();
}

const QList<QdfRibbonPage *> &QdfRibbonBar::pages() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_listPages;
}

QdfRibbonPage *QdfRibbonBar::getPage(int index) const
{
    QDF_D(const QdfRibbonBar);
    if (d->validIndex(index))
    {
        return d->m_listPages[index];
    }
    return nullptr;
}

int QdfRibbonBar::getPageCount() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_listPages.count();
}

QMenu *QdfRibbonBar::addMenu(const QString &text)
{
    QDF_D(QdfRibbonBar);
    return d->m_tabBar->addMenu(text);
}

QAction *QdfRibbonBar::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu)
{
    QDF_D(QdfRibbonBar);
    QAction *action = d->m_tabBar->addAction(icon, text, style, menu);
    QMenuBar::addAction(action);
    return action;
}

QAction *QdfRibbonBar::addAction(QAction *action, Qt::ToolButtonStyle style)
{
    QDF_D(QdfRibbonBar);
    QMenuBar::addAction(action);
    return d->m_tabBar->addAction(action, style);
}

QAction *QdfRibbonBar::addSystemButton(const QString &text) { return addSystemButton(QIcon(), text); }

QAction *QdfRibbonBar::addSystemButton(const QIcon &icon, const QString &text)
{
    QDF_D(QdfRibbonBar);
    QAction *actionSystemPopupBar =
            d->m_controlSystemButton ? d->m_controlSystemButton->defaultAction() : d->createSystemButton(icon, text);

    if (QMenu *menu = actionSystemPopupBar->menu())
    {
        delete menu;
    }

    actionSystemPopupBar->setMenu(new QdfRibbonSystemPopupBar(this));

    return actionSystemPopupBar;
}

QdfRibbonSystemButton *QdfRibbonBar::getSystemButton() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_controlSystemButton;
}

bool QdfRibbonBar::isBackstageVisible() const
{
    QDF_D(const QdfRibbonBar);
    if (d->m_controlSystemButton)
    {
        if (QdfRibbonBackstageView *backstage = qobject_cast<QdfRibbonBackstageView *>(d->m_controlSystemButton->backstage()))
        {
            return backstage->isVisible();
        }
    }
    return false;
}

void QdfRibbonBar::setFrameThemeEnabled(bool enable)
{
#ifdef Q_OS_WIN
    QDF_D(QdfRibbonBar);
    if (!enable)
    {
        if (d->m_frameHelper)
        {
            delete d->m_frameHelper;
            d->m_frameHelper = nullptr;
        }

        d->addTitleButton(QStyle::SC_TitleBarCloseButton, false, d->m_rcHeader);
        d->addTitleButton(QStyle::SC_TitleBarNormalButton, false, d->m_rcHeader);
        d->addTitleButton(QStyle::SC_TitleBarMaxButton, false, d->m_rcHeader);
        d->addTitleButton(QStyle::SC_TitleBarMinButton, false, d->m_rcHeader);
        adjustSize();
    }
    else if (!d->m_frameHelper)
    {
        d->m_frameHelper = new QdfOfficeFrameHelperWin(this);
        d->m_frameHelper->enableOfficeFrame(this->parentWidget());
        d->m_frameHelper->setContextHeader(&d->m_listContextHeaders);
    }
    d->layoutCorner();
    d->layoutRibbon();
#else // Q_OS_WIN
    Q_UNUSED(enable)
#endif// Q_OS_WIN
}

bool QdfRibbonBar::isFrameThemeEnabled() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_frameHelper;
}

int QdfRibbonBar::backstageViewTop() const
{
    QDF_D(const QdfRibbonBar);
    return titleBarHeight() + d->topBorder() + tabBarHeight(true);
}

int QdfRibbonBar::tabBarHeight(bool isVisible) const
{
    QDF_D(const QdfRibbonBar);
    return (d->m_tabBar && (d->m_tabBar->isVisible() || isVisible)) ? d->m_rcTabBar.height() : 2;
}

bool QdfRibbonBar::isTitleBarVisible() const
{
    QDF_D(const QdfRibbonBar);
    return isFrameThemeEnabled() || d->m_showTitleAlways;
}

void QdfRibbonBar::setTitleBarVisible(bool visible)
{
    QDF_D(QdfRibbonBar);
    if (d->m_showTitleAlways != visible)
    {
        d->m_showTitleAlways = visible;
        d->layoutRibbon();
    }
}

bool QdfRibbonBar::isTitleGroupsVisible() const
{
    QDF_D(const QdfRibbonBar);
    return d->m_titleGroupsVisible;
}

void QdfRibbonBar::setTitleGroupsVisible(bool visible)
{
    QDF_D(QdfRibbonBar);
    if (d->m_titleGroupsVisible != visible)
    {
        d->m_titleGroupsVisible = visible;
        for (int i = 0, count = d->m_listPages.count(); count > i; ++i)
        {
            QdfRibbonPage *page = d->m_listPages[i];
            for (int j = 0, size = page->groupCount(); size > j; ++j)
            {
                if (QdfRibbonGroup *group = page->getGroup(j))
                {
                    group->setProperty(_qdf_TitleGroupsVisible, d->m_titleGroupsVisible);
                }
            }
        }
        d->layoutCorner();
        d->layoutRibbon();
        updateGeometry();
    }
}

int QdfRibbonBar::titleBarHeight() const
{
    QDF_D(const QdfRibbonBar);
    if (!isFrameThemeEnabled() && !d->m_showTitleAlways)
    {
        return 0;
    }

    int height = style()->pixelMetric(QStyle::PM_TitleBarHeight, 0);
    if (d->m_tabBar == nullptr)
    {
        return height;
    }

    int quickAccessHeight = quickAccessBar() ? quickAccessBar()->height() : height;
    quickAccessHeight = qMax(quickAccessHeight, d->m_tabBar->height() - 2);

    if (height >= quickAccessHeight)
    {
        return height;
    }

    return quickAccessHeight;
}

int QdfRibbonBar::rowItemHeight() const
{
    QDF_D(const QdfRibbonBar);
    return d->rowItemHeight();
}

int QdfRibbonBar::rowItemCount() const
{
    QDF_D(const QdfRibbonBar);
    return d->rowItemCount();
}

void QdfRibbonBar::updateLayout()
{
    QDF_D(QdfRibbonBar);
    d->layoutRibbon();
}

void Qdf::qdf_set_font_to_ribbon_children(QWidget *parent, const QFont &font)
{
    QObjectList list = parent->children();
    for (QObjectList::iterator it = list.begin(); it != list.end(); ++it)
    {
        QWidget *widget = qobject_cast<QWidget *>(*it);
        if (widget == nullptr)
        {
            continue;
        }

        QMenu *menu = qobject_cast<QMenu *>(widget);
        QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(widget);
        QdfRibbonGroup *group = qobject_cast<QdfRibbonGroup *>(widget);
        if (menu != nullptr)
        {
            menu->setFont(QApplication::font(menu));
        }
        else if (page != nullptr)
        {
            page->setFont(font);
        }
        else if (group != nullptr)
        {
            group->setFont(font);
        }
        else
        {
            widget->setFont(font);
            qdf_set_font_to_ribbon_children(widget, font);
        }
    }
}

bool QdfRibbonBar::event(QEvent *event)
{
    QDF_D(QdfRibbonBar);

    bool result = QMenuBar::event(event);

    switch (event->type())
    {
        case QEvent::ChildAdded:
            {
                QChildEvent *e = static_cast<QChildEvent *>(event);
                if (e->added())
                {
                    QWidget *widget = qobject_cast<QWidget *>(e->child());
                    if (widget != nullptr)
                    {
                        if (qobject_cast<QdfRibbonPage *>(widget) != nullptr)
                        {
                            qobject_cast<QdfRibbonPage *>(widget)->setFont(font());
                        }
                        else
                        {
                            widget->setFont(font());
                            qdf_set_font_to_ribbon_children(widget, font());
                        }
                    }
                }
            }
            break;
        case QEvent::StyleChange:
        case QEvent::FontChange:
            {
                qdf_set_font_to_ribbon_children(this, font());
                d->layoutCorner();
                d->layoutRibbon();
            }
            break;
        case QEvent::WindowTitleChange:
            {
                d->m_windowTitle = parentWidget() ? parentWidget()->windowTitle() : QString();
                d->layoutRibbon();
            }
            break;
        case QEvent::WindowIconChange:
            if (d->m_frameHelper)
            {
                d->m_frameHelper->resetWindowIcon();
            }
            break;
        case QEvent::ActionRemoved:
        case QEvent::ActionChanged:
            QApplication::sendEvent(d->m_tabBar, event);
            break;
        case QEvent::Show:
        case QEvent::Hide:
            {
                bool show = event->type() == QEvent::Show;
                if (isFrameThemeEnabled() && show != d->m_ribbonBarVisible)
                {
                    d->m_ribbonBarVisible = show;
                    d->m_frameHelper->recalcFrameLayout();
                }
                if (show)
                {
                    d->layoutRibbon();
                }
            }
            break;
        default:
            break;
    }

    if (event->type() == QdfKeyTipEvent::eventNumber())
    {
        QdfKeyTipEvent *keyTipEvent = static_cast<QdfKeyTipEvent *>(event);
        if (QdfRibbonTab *tab = qobject_cast<QdfRibbonTab *>(keyTipEvent->getKeyTip()->getOwner()))
        {
            int index = d->m_tabBar->getIndex(tab);
            if (index != -1)
            {
                setCurrentPageIndex(index);
                d->showKeyTips(getPage(index));
            }
        }
    }
    else if (event->type() == QdfShowKeyTipEvent::eventNumber())
    {
        QdfShowKeyTipEvent *showKeyTipEvent = static_cast<QdfShowKeyTipEvent *>(event);
        if (QWidget *widget = showKeyTipEvent->buddy())
        {
            if (d->m_levels.indexOf(widget) == -1)
            {
                d->m_keyTipsShowing = true;
                emit keyTipsShowed(true);

                d->showKeyTips(widget);
            }
        }
    }
    else if (event->type() == QdfHideKeyTipEvent::eventNumber())
    {
        d->hideKeyTips();
        d->m_levels.clear();
    }
    else if (event->type() == QdfMinimizedEvent::eventNumber())
    {
        setMinimizedFlag(!isMinimized());
    }

    return result;
}

bool QdfRibbonBar::eventFilter(QObject *object, QEvent *event)
{
    QDF_D(QdfRibbonBar);
    const QEvent::Type type = event->type();

    if (object == cornerWidget(Qt::TopRightCorner) || object == cornerWidget(Qt::TopLeftCorner))
    {
        switch (event->type())
        {
            case QEvent::ShowToParent:
            case QEvent::HideToParent:
                d->layoutCorner();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                return true;
#else
                break;
#endif
            default:
                break;
        }
    }

    if (type == QEvent::Resize)
    {
        if (object == parentWidget() && !isVisible())
        {
            d->layoutRibbon();
            return true;
        }
    }

    return QMenuBar::eventFilter(object, event);
}

void QdfRibbonBar::paintEvent(QPaintEvent *event)
{
    QDF_D(QdfRibbonBar);

    QWidget *leftCorner = cornerWidget(Qt::TopLeftCorner);
    if (leftCorner && leftCorner->isVisible())
    {
        leftCorner->setVisible(false);
    }

    // draw ribbon
    QPainter p(this);
    QRect rectEvent = event->rect();
    p.setClipRect(rectEvent);

    QdfStyleOptionRibbon opt;
    opt.init(this);
    opt.frameHelper = d->m_frameHelper;
    opt.rectTabBar = d->m_rcTabBar;
    opt.minimized = d->m_minimized;
    opt.isBackstageVisible = isBackstageVisible();
    opt.pixTitleBackground = d->m_pixTitleBackground;
    opt.titleBarHeight = titleBarHeight();
    opt.airRegion = event->region();
    opt.tabBarPosition = d->m_tabBarPosition;

    p.setBackgroundMode(Qt::TransparentMode);

    QStyle *ribbonStyle = style();
    if (rectEvent.intersects(opt.rect))
    {
        ribbonStyle->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonBar, &opt, &p, this);
    }

    if ((isFrameThemeEnabled() || d->m_showTitleAlways) && d->m_rcTitle.isValid())
    {
        QdfStyleOptionTitleBar titleBarOptions;
        d->titleBarOptions(titleBarOptions);
        titleBarOptions.frameHelper = d->m_frameHelper;
        titleBarOptions.rectTabBar = d->m_rcTabBar;
        titleBarOptions.rcRibbonClient = rect();
        titleBarOptions.listContextHeaders = &d->m_listContextHeaders;
        titleBarOptions.airRegion = event->region();
        titleBarOptions.isBackstageVisible = isBackstageVisible();

        if (rectEvent.intersects(titleBarOptions.rect))
        {
            QFont saveFont = p.font();
            const QFont font = QApplication::font("QMdiSubWindowTitleBar");
            p.setFont(font);
            ribbonStyle->drawComplexControl(QStyle::CC_TitleBar, &titleBarOptions, &p, this);
            p.setFont(saveFont);
        }
    }

    if (!d->m_minimized && getPageCount() > 0 && rectEvent.intersects(opt.rect))
    {
        opt.rect = d->m_rcPageArea;
        if (d->m_logotypeLabel->isVisible())
        {
            QRect rectPage = opt.rect;
            QRect rect = d->m_logotypeLabel->geometry();
            if (d->m_logotypeLabel->alignmentLogotype() == Qt::AlignRight)
            {
                rectPage.adjust(0, 0, 4, 0);
                rectPage.setRight(rectPage.right() - rect.width());
                p.setClipRect(rectPage);
            }
            else
            {
                rectPage.adjust(-2, 0, 0, 0);
                rectPage.setLeft(rect.width());
                p.setClipRect(rectPage);
            }
        }
        ribbonStyle->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonGroups, &opt, &p, this);
    }
}

void QdfRibbonBar::changeEvent(QEvent *event) { return QMenuBar::changeEvent(event); }

void QdfRibbonBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    QDF_D(QdfRibbonBar);
    d->layoutCorner();
    d->layoutRibbon();
}

void QdfRibbonBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    QMenuBar::mouseDoubleClickEvent(event);

    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    QDF_D(QdfRibbonBar);
    if (isFrameThemeEnabled() && isTitleBarVisible())
    {
        if (d->hitTestContextHeaders(event->pos()))
        {
            QWidget *widget = parentWidget();
            if (widget == nullptr)
            {
                return;
            }

            QdfStyleOptionTitleBar titleBarOpt;
            d->titleBarOptions(titleBarOpt);

            bool buttonMax = titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint &&
                             !(titleBarOpt.titleBarState & Qt::WindowMaximized);

            bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint) &&
                                  (titleBarOpt.titleBarState & Qt::WindowMinimized)) ||
                                 ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint) &&
                                  (titleBarOpt.titleBarState & Qt::WindowMaximized)));

            if (buttonMax)
            {
                widget->showMaximized();
            }
            else if (buttonNormal)
            {
                widget->showNormal();
            }

            QSize s = widget->size();
            if (s.isValid())
            {
                widget->resize(s);
            }

            return;
        }
    }
    if (d->m_minimizationEnabled && d->m_tabBar->getTab(d->m_tabBar->currentIndex()) == childAt(event->pos()))
    {
        QApplication::postEvent(this, new QdfMinimizedEvent());
    }
}

void QdfRibbonBar::mousePressEvent(QMouseEvent *event)
{
    // block the function call QMenuBar
    QWidget::mousePressEvent(event);

    QDF_D(QdfRibbonBar);
    if (event->button() == Qt::LeftButton)
    {
        if (QdfContextHeader *header = d->hitTestContextHeaders(event->pos()))
        {
            for (int i = 0, count = d->m_tabBar->getTabCount(); count > i; i++)
            {
                if (QdfRibbonTab *tab = d->m_tabBar->getTab(i))
                {
                    if (tab == header->firstTab)
                    {
                        setCurrentPageIndex(i);
                        break;
                    }
                }
            }
        }
    }
}

void QdfRibbonBar::wheelEvent(QWheelEvent *event)
{
    QDF_D(QdfRibbonBar);
    if (QApplication::activePopupWidget())
    {
        return;
    }

    if (!isMaximized())
    {
        return;
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QPoint pos = event->pos();
#else
    QPoint pos = event->position().toPoint();
#endif

    if (!geometry().contains(pos))
    {
        return;
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    d->m_tabBar->currentNextTab(event->delta() < 0);
#else
    d->m_tabBar->currentNextTab(event->angleDelta().y() < 0);
#endif
}

void QdfRibbonBar::contextMenuEvent(QContextMenuEvent *event)
{
    QDF_D(QdfRibbonBar)
    if (QMenu *menu = d->createContextMenu())
    {
        menu->setAttribute(Qt::WA_DeleteOnClose);

        event->accept();

        emit showRibbonContextMenu(menu, event);

        if (event->isAccepted())
        {
            menu->exec(event->globalPos());
        }
    }
}

int QdfRibbonBar::heightForWidth(int) const
{
    QDF_D(const QdfRibbonBar);

    const int tabsHeight = d->m_rcTabBar.height();
    const int pageAreaHeight = d->m_rcPageArea.height();
    const int ribbonTopBorder =
            d->m_frameHelper && d->m_frameHelper->isDwmEnabled() ? d->m_frameHelper->frameBorder() : 0;
    const int heightTitle = d->m_frameHelper ? titleBarHeight() : d->m_rcTitle.height();

    if (!d->m_ribbonBarVisible && d->m_frameHelper && d->m_frameHelper->canHideTitle())
    {
        return heightTitle;
    }

    int height = tabsHeight + pageAreaHeight + heightTitle + ribbonTopBorder;

    if (d->m_quickAccessBarPosition != QdfRibbonBar::QATopPosition)
    {
        height += d->m_rcQuickAccess.height();
    }

    if (d->m_minimized)
    {
        height += 3;
    }

    if (d->m_ribbonBarVisible)
    {
        return height;
    }

    return 0;
}

QSize QdfRibbonBar::sizeHint() const
{
    return QSize(rect().width(), heightForWidth(0)).expandedTo(QApplication::globalStrut());
}

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool QdfRibbonBar::nativeEvent(const QByteArray &eventType, void *message, long *result)
    #else
bool QdfRibbonBar::winEvent(MSG *message, long *result)
    #endif
{
    QDF_D(QdfRibbonBar);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d->m_frameHelper)
    {
        MSG *msg = static_cast<MSG *>(message);
        if (d->m_frameHelper->winEvent(msg, result))
        {
            return true;
        }
    }

    return QMenuBar::nativeEvent(eventType, message, result);
    #else
    if (d->m_frameHelper && d->m_frameHelper->winEvent(message, result))
    {
        return true;
    }

    return QMenuBar::winEvent(message, result);
    #endif
}
#endif// Q_OS_WIN

bool QdfRibbonBar::loadTranslation(const QString &country)
{
    static bool loaded = false;
    if (loaded)
    {
        return true;
    }

    const QString directory = QLatin1String(":/translations/ribbon");

    QTranslator *translator = new QTranslator(qApp);
    if (country.isEmpty())
    {
        foreach (const QLocale locale, QLocale().uiLanguages())
        {
            if (translator->load(locale, QLatin1String(""), QLatin1String(""), directory))
            {
                qApp->installTranslator(translator);
                loaded = true;
                return true;
            }
        }
    }
    else
    {
        if (translator->load(country, directory))
        {
            qApp->installTranslator(translator);
            loaded = true;
            return true;
        }
    }

    delete translator;
    return false;
}

QString QdfRibbonBar::tr_compatible(const char *s, const char *c)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCoreApplication::translate("Qdf::QdfRibbonBar", s, c);
#else
    return QCoreApplication::translate("Qdf::QdfRibbonBar", s, c, QCoreApplication::CodecForTr);
#endif
}