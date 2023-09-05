#include "private/qdfribbon_p.h"
#include "private/qdfribbonbutton_p.h"
#include "private/qdfribboncontrols_p.h"
#include "private/qdfribbongroup_p.h"
#include "private/qdfribbonpage_p.h"
#include "qdfribbon_def.h"
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QToolBar>
#include <ribbon/qdfribbonbuttoncontrols.h>
#include <ribbon/qdfribbongallery.h>
#include <ribbon/qdfribbongallerycontrol.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>


QDF_USE_NAMESPACE

#define ADDITIVE_WIDTH 20

static void setSizeDefinitionByStyle(QdfRibbonButtonControl *control, Qt::ToolButtonStyle style)
{
    if (Qt::ToolButtonTextUnderIcon == style)
    {
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(QdfRibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    }
    else if (Qt::ToolButtonTextBesideIcon == style)
    {
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)
                ->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
    }
    else if (Qt::ToolButtonIconOnly == style)
    {
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)
                ->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)
                ->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setLabelVisible(false);
    }
}


QdfRibbonButtonAction::QdfRibbonButtonAction(QObject *parent, const QIcon &icon, const QString &text,
                                             Qt::ToolButtonStyle style, QMenu *menu,
                                             QToolButton::ToolButtonPopupMode mode)
    : QWidgetAction(parent), m_style(style), m_mode(mode)
{
    setIcon(icon);
    setText(text);
    setMenu(menu);
}

QdfRibbonButtonAction::~QdfRibbonButtonAction()
{
}

QWidget *QdfRibbonButtonAction::createWidget(QWidget *parent)
{
    if (QToolBar *toolBar = qobject_cast<QToolBar *>(parent))
    {
        QdfRibbonButton *button = new QdfRibbonButton(parent);

        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIconSize(toolBar->iconSize());
        button->setToolButtonStyle(toolBar->toolButtonStyle());
        QObject::connect(toolBar, SIGNAL(iconSizeChanged(QSize)), button, SLOT(setIconSize(QSize)));
        QObject::connect(toolBar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)), button,
                         SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
        button->setDefaultAction(this);
        QObject::connect(button, SIGNAL(triggered(QAction *)), toolBar,
                         SIGNAL(actionTriggered(QAction *)));
        return button;
    }
    else if (QdfRibbonGroup *ribbonGroup = qobject_cast<QdfRibbonGroup *>(parent))
    {
        QdfRibbonButtonControl *control = new QdfRibbonButtonControl(ribbonGroup);
        ::setSizeDefinitionByStyle(control, m_style);
        control->setDefaultAction(this);
        control->setToolButtonStyle(m_style);

        if (menu())
        {
            control->setPopupMode(m_mode);
            control->setMenu(menu());
            menu()->setDefaultAction(this);
        }

        return control;
    }
    return nullptr;
}

QdfRibbonGroupPrivate::QdfRibbonGroupPrivate()
{
    m_dirty = false;
    m_butOptionVisible = false;
    m_optionButton = nullptr;
    m_actOption = nullptr;
    m_groupPopup = nullptr;
    m_toolBar = nullptr;
    m_groupScrollPos = 0;
    m_pageScrollPos = 0;
    m_font = nullptr;
    m_currentSize = QdfRibbonControlSizeDefinition::GroupLarge;
    m_titleElideMode = Qt::ElideRight;
    m_controlsAlignment = Qt::AlignHCenter;
}

QdfRibbonGroupPrivate::~QdfRibbonGroupPrivate()
{
    if (m_groupPopup)
    {
        delete m_groupPopup;
    }
    m_groupPopup = nullptr;

    if (m_font)
    {
        delete m_font;
    }
    m_font = nullptr;
}

void QdfRibbonGroupPrivate::init()
{
    QDF_Q(QdfRibbonGroup);
    q->setMouseTracking(true);
    q->setProperty(_qdf_TitleGroupsVisible, true);

    m_buttonScrollGroupLeft = new QdfRibbonGroupScroll(q, true);
    m_buttonScrollGroupLeft->setVisible(false);

    m_buttonScrollGroupRight = new QdfRibbonGroupScroll(q, false);
    m_buttonScrollGroupRight->setVisible(false);

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(pressed()), this, SLOT(pressLeftScrollButton()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(pressed()), this, SLOT(pressRightScrollButton()));

    m_optionButton = new QdfRibbonGroupOption(q);
    m_actOption = new QAction(q);
    m_actOption->setObjectName("__qtc_Action_Option");
    m_optionButton->setDefaultAction(m_actOption);
    QObject::connect(m_optionButton, SIGNAL(triggered(QAction *)), q,
                     SIGNAL(actionTriggered(QAction *)));
    m_optionButton->hide();
}

void QdfRibbonGroupPrivate::clear(bool deleteControls)
{
    QDF_Q(QdfRibbonGroup);
    if (q->isControlsGrouping())
    {
        m_toolBar->clear();
        return;
    }

    QList<QAction *> actions = q->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        q->removeAction(actions.at(i));
    }

    while (m_controls.size() > 0)
    {
        QdfRibbonControl *control = m_controls[0];
        if (QdfRibbonWidgetControl *widgetcontrol = qobject_cast<QdfRibbonWidgetControl *>(control))
        {
            widgetcontrol->qdf_d()->clearWidget();
        }

        q->removeControl(control);

        if (deleteControls)
        {
            delete control;
        }
    }
}

void QdfRibbonGroupPrivate::enableGroupScroll(bool scrollLeft, bool scrollRight)
{
    QDF_Q(QdfRibbonGroup);

    Q_ASSERT(m_buttonScrollGroupLeft);
    Q_ASSERT(m_buttonScrollGroupRight);

    if (!m_buttonScrollGroupRight || !m_buttonScrollGroupLeft)
    {
        return;
    }

    const QdfRibbonBar *rb = q->ribbonBar();
    Q_ASSERT(rb != NULL);

    const int heightGroup = rb->rowItemHeight() * rb->rowItemCount();

    QRect rcGroup(q->geometry());
    rcGroup.setHeight(heightGroup + 4);

    if (scrollLeft)
    {
        m_buttonScrollGroupLeft->raise();
        m_buttonScrollGroupLeft->setVisible(true);
        m_buttonScrollGroupLeft->setGeometry(QRect(QPoint(rcGroup.left() - 1, rcGroup.top()),
                                                   QPoint(rcGroup.left() + 12, rcGroup.bottom())));
    }
    else
    {
        m_buttonScrollGroupLeft->setVisible(false);
    }

    if (scrollRight)
    {
        m_buttonScrollGroupRight->raise();
        m_buttonScrollGroupRight->setVisible(true);
        m_buttonScrollGroupRight->setGeometry(QRect(QPoint(rcGroup.right() - 12, rcGroup.top()),
                                                    QPoint(rcGroup.right() + 1, rcGroup.bottom())));
    }
    else
    {
        m_buttonScrollGroupRight->setVisible(false);
    }
}

void QdfRibbonGroupPrivate::showGroupScroll()
{
    QDF_Q(QdfRibbonGroup);
    if (q->isReduced() && q->isVisible())
    {
        QRect screen = QApplication::desktop()->availableGeometry(q);
        int totalWidth = q->layout()->minimumSize().width();
        int groupLength = screen.width();
        int scrollPos = m_groupScrollPos;

        if (totalWidth > groupLength)
        {
            if (scrollPos > totalWidth - groupLength)
            {
                scrollPos = totalWidth - groupLength;
            }
        }
        else
        {
            scrollPos = 0;
        }

        if (scrollPos < 0)
        {
            scrollPos = 0;
        }

        m_groupScrollPos = scrollPos;
        enableGroupScroll(scrollPos > 0, totalWidth - groupLength - scrollPos > 0);
    }
}

void QdfRibbonGroupPrivate::initStyleOption(QStyleOptionGroupBox &opt) const
{
    QDF_Q(const QdfRibbonGroup)
    opt.init(q);
    opt.text = m_title;
    if (m_font)
    {
        opt.fontMetrics = QFontMetrics(*m_font);
    }
    opt.lineWidth = q->style()->pixelMetric(
            (QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonHeightCaptionGroup, &opt, q);
    opt.textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

int QdfRibbonGroupPrivate::calcMinimumWidth(QdfRibbonControlSizeDefinition::GroupSize size) const
{
    QDF_Q(const QdfRibbonGroup)

    QStyleOptionToolButton opt;
    opt.init(q);// opt.text = m_title;
    int width = q->style()->pixelMetric((QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonReducedGroupWidth,
                                        &opt, q);

    if (size == QdfRibbonControlSizeDefinition::GroupSmall)
    {
        width += 1;
    }
    else if (size == QdfRibbonControlSizeDefinition::GroupMedium)
    {
        width += 2;
    }
    else if (size == QdfRibbonControlSizeDefinition::GroupLarge)
    {
        width += 3;
    }
    return width;
}

int QdfRibbonGroupPrivate::arrangeRowContent(QList<QdfRibbonControl *> &row, int leftOffset,
                                             int rowItemHeight, int rowItemCount, QWidget *parent) const
{
    QDF_Q(const QdfRibbonGroup)
    static int margin = 4;

    int max = 0;
    int rowHeight = rowItemHeight;
    int topOffset = 0;

    if (q->contentAlignment() & Qt::AlignVCenter)
    {
        topOffset = (rowItemHeight * (rowItemCount - row.size())) * 0.5;
    }
    else if (q->contentAlignment() & Qt::AlignBottom)
    {
        topOffset = rowItemHeight * (rowItemCount - row.size());
    }

    for (QList<QdfRibbonControl *>::const_iterator it = row.constBegin(); it != row.constEnd(); ++it)
    {
        max = qMax((*it)->sizeHint().width(), max);
    }

    if (parent == nullptr)
    {
        row.clear();
        return max;
    }

    topOffset += margin / 2;
    leftOffset += margin;

    for (QList<QdfRibbonControl *>::const_iterator it = row.constBegin(); it != row.constEnd(); ++it)
    {
        QSize size = (*it)->sizeHint();
        size.rheight() = qMax(rowHeight, size.height());
        const bool stretchable = (*it)->sizeDefinition((*it)->currentSize())->isStretchable();
        QRect r = QRect(leftOffset, topOffset, max, rowHeight);
        size.rwidth() = stretchable ? max : size.width();

        QRect controlRect = QStyle::alignedRect(q->layoutDirection(), q->contentAlignment(), size, r);
        (*it)->setGeometry(controlRect);
        topOffset += rowHeight;
    }
    row.clear();
    return max;
}

void QdfRibbonGroupPrivate::reposition(const QRect &rect)
{
    QDF_Q(QdfRibbonGroup)
    QSize sz(rect.width(), q->sizeHint().height());
    q->setGeometry(QRect(rect.topLeft(), /*p.sizeHint()*/ sz));
    if (q->isReduced())
    {
        if (m_groupPopup == nullptr)
        {
            q->setUpdatesEnabled(true);
            m_groupPopup = new QdfRibbonGroupPopup(q);
            m_groupPopup->setFocusPolicy(Qt::NoFocus);
            QObject::connect(m_groupPopup, SIGNAL(aboutToHide()), q, SLOT(update()));
        }
    }
    else
    {
        if (m_groupPopup)
        {
            QObject::disconnect(m_groupPopup, SIGNAL(aboutToHide()), q, SLOT(update()));
            if (m_groupPopup)
            {
                delete m_groupPopup;
            }
            m_groupPopup = nullptr;
        }

#ifdef QRIBBON_DESIGNER
        if ((p.parentWidget()->windowFlags() & Qt::Popup))
        {
            q->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
#endif// QRIBBON_DESIGNER
#ifdef QRIBBON_DESIGNER
        QSize sz = rect.size();
        if (!sz.isEmpty() && !(p.parentWidget()->windowFlags() & Qt::Popup))
        {
            q->setFixedSize(sz);
        }
#endif// QRIBBON_DESIGNER
    }
}

void QdfRibbonGroupPrivate::updateOptionButtonLayout()
{
    QDF_Q(QdfRibbonGroup);
    if (m_optionButton && m_butOptionVisible)
    {
        QStyleOption opt;
        opt.init(q);
        int width = q->style()->pixelMetric(
                (QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonHeightCaptionGroup, &opt, q);
        opt.rect.setWidth(q->width());

        QRect rc = opt.rect;
        QSize sz(qMax(width - 3, m_optionButton->width()), qMax(width - 3, m_optionButton->height()));
        QRect rectButOption(QPoint(rc.right() - sz.width() - 2, rc.bottom() - sz.height() - 1), sz);
        m_optionButton->setGeometry(rectButOption);
        m_optionButton->setVisible(!q->isReduced() && q->property(_qdf_TitleGroupsVisible).toBool());
    }
}

QSize QdfRibbonGroupPrivate::updateControlsLayout(QWidget *parent, int leftOffset) const
{
    QDF_Q(const QdfRibbonGroup);

    const QdfRibbonBar *rb = q->ribbonBar();
    if (rb == NULL)
    {
        return QSize();
    }

    static int margin = 4;
    const int maxRows = rb->rowItemCount();// Get max row count from QdfRibbonBar here.
    const int rowItemHeight = rb->rowItemHeight();

    QList<QdfRibbonControl *> rowControls;
    QList<QdfRibbonControl *> controls = m_controls;

#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    qSort(controls.begin(), controls.end(), QdfRibbonGroupPrivate::visualIndexLessThan);
#else
    std::sort(controls.begin(), controls.end(), QdfRibbonGroupPrivate::visualIndexLessThan);
#endif

    for (QList<QdfRibbonControl *>::const_iterator it = controls.constBegin(); it != controls.constEnd();
         ++it)
    {
        QdfRibbonControl *ribbonControl = *it;
        QdfRibbonColumnBreakControl *sepatator =
                qobject_cast<QdfRibbonColumnBreakControl *>(ribbonControl);
        bool visible = ribbonControl->sizeDefinition(ribbonControl->currentSize())->isVisible();
        if (ribbonControl->defaultAction() != nullptr)
        {
            visible = visible && ribbonControl->defaultAction()->isVisible();
        }
        if (parent != nullptr)
        {
            ribbonControl->setParent(parent);
            if (sepatator == nullptr)
            {
                ribbonControl->setVisible(visible);
            }
            else
            {
                ribbonControl->setVisible(
                        visible && sepatator->sizeDefinition(sepatator->currentSize())->showSeparator());
            }
        }

        if (!visible)
        {
            continue;
        }

        ribbonControl->updateLayout();
        QSize sizeHint = ribbonControl->sizeHint();
        if (sepatator != nullptr || sizeHint.height() > rowItemHeight)
        {
            if (rowControls.size() > 0)
            {
                leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);
            }

            if (sepatator != nullptr)
            {
                if (!sepatator->sizeDefinition(sepatator->currentSize())->showSeparator())
                {
                    sizeHint = QSize(0, 0);
                }
            }

            if (parent != nullptr)
            {
                int maxHeight = sizeHint.height();
                if (maxHeight > rowItemHeight * maxRows)
                {
                    maxHeight = rowItemHeight * maxRows;
                }
                else
                {
                    maxHeight = qMax(rowItemHeight, sizeHint.height());
                }

                ribbonControl->setGeometry(
                        QRect(leftOffset + margin, margin / 2, sizeHint.width(), maxHeight));
            }
            leftOffset += sizeHint.width();
        }
        else
        {
            rowControls.append(ribbonControl);
            if (rowControls.size() >= maxRows)
            {
                leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);
            }
        }
    }

    if (rowControls.size() > 0)
    {
        leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);
    }

    leftOffset += (margin * 2 + 1);

    int minWidth = calcMinimumWidth(q->currentSize());
    if (minWidth > leftOffset)
    {
        leftOffset = minWidth;
    }

    return QSize(leftOffset, rowItemHeight * maxRows);
}

void QdfRibbonGroupPrivate::adjustCurrentSize(bool expand)
{
    for (QList<QdfRibbonControl *>::const_iterator it = m_controls.constBegin();
         it != m_controls.constEnd(); ++it)
    {
        (*it)->adjustCurrentSize(expand);
    }
}

void QdfRibbonGroupPrivate::updateLayoutParent(bool first)
{
    QDF_Q(const QdfRibbonGroup);
    if (QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(q->parentWidget()))
    {
        if (first)
        {
            page->updateLayout();
        }
        else
        {
            page->updateGeometry();
        }
    }
}

QSize QdfRibbonGroupPrivate::sizeHint() const
{
    QDF_Q(const QdfRibbonGroup);

    const QdfRibbonBar *rb = q->ribbonBar();
    if (rb == NULL)
    {
        return QSize();
    }

    QStyleOptionGroupBox opt;
    initStyleOption(opt);

    QSize size = updateControlsLayout(nullptr);

    const int heightGroup = qMax(rb->rowItemHeight() * rb->rowItemCount(), size.height());
    const int heightCaptionGroup = opt.lineWidth;

    QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, q->title());
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    textSize.rwidth() += opt.fontMetrics.width(QLatin1Char(' ')) * 4;
#else
    textSize.rwidth() += opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 4;
#endif

    if (m_optionButton->isVisible())
    {
        textSize.rwidth() += m_optionButton->sizeHint().width();
    }

    if (q->minimumWidth() == 0)
    {
        size.rwidth() = qMax(textSize.width(), size.width());
    }
    else
    {
        size.rwidth() = qMax(q->minimumWidth(), size.width());
    }

    //    else if (p.titleElideMode() == Qt::ElideNone)
    //        size.rwidth() = qMax(p.minimumWidth(), size.width());

    if (q->isMinimized() && q->isVisible())
    {
        QRect screen = QApplication::desktop()->availableGeometry(q);
        size.rwidth() = qMin(screen.width(), size.width());
    }

    size.setHeight(heightGroup + heightCaptionGroup + 2 * 2);
    return size;
}

void QdfRibbonGroupPrivate::setReductionSize(QdfRibbonControlSizeDefinition::GroupSize size)
{
    if (m_currentSize == size)
    {
        return;
    }

    m_currentSize = size;

    for (QList<QdfRibbonControl *>::const_iterator it = m_controls.constBegin();
         it != m_controls.constEnd(); ++it)
    {
        (*it)->sizeChanged(size);
    }
}

void QdfRibbonGroupPrivate::reduce()
{
    Q_ASSERT(canReduce());
    setReductionSize((QdfRibbonControlSizeDefinition::GroupSize)(m_currentSize + 1));
}

void QdfRibbonGroupPrivate::expand()
{
    Q_ASSERT(canExpand());
    setReductionSize((QdfRibbonControlSizeDefinition::GroupSize)(m_currentSize - 1));
}

bool QdfRibbonGroupPrivate::canReduce()
{
    return m_currentSize < QdfRibbonControlSizeDefinition::GroupPopup;
}

bool QdfRibbonGroupPrivate::canExpand()
{
    return m_currentSize > QdfRibbonControlSizeDefinition::GroupLarge;
}

static void listPageWidth(int totalWidth, int realWidth, QList<int> &pagesWidth)
{
    if (totalWidth > realWidth)
    {
        pagesWidth << realWidth;
        listPageWidth(totalWidth - realWidth, realWidth, pagesWidth);
    }
    else
    {
        pagesWidth << totalWidth;
    }
    return;
}

void QdfRibbonGroupPrivate::pressLeftScrollButton()
{
    QDF_Q(QdfRibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(q->layout()->sizeHint().width(), q->geometry().width(), pagesWidth);

    if (0 < m_pageScrollPos)
    {
        m_pageScrollPos--;
    }

    m_groupScrollPos -= pagesWidth[m_pageScrollPos];
    showGroupScroll();
    q->layout()->update();
}

void QdfRibbonGroupPrivate::pressRightScrollButton()
{
    QDF_Q(QdfRibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(q->layout()->sizeHint().width(), q->geometry().width(), pagesWidth);

    m_groupScrollPos += pagesWidth[m_pageScrollPos];
    showGroupScroll();
    q->layout()->update();

    if (pagesWidth.size() - 1 > m_pageScrollPos)
    {
        m_pageScrollPos++;
    }
}

void QdfRibbonGroupPrivate::hidePopupMode(QAction *action)
{
    Q_UNUSED(action);
    QDF_Q(QdfRibbonGroup)
    q->setVisible(false);
}

bool QdfRibbonGroupPrivate::visualIndexLessThan(QdfRibbonControl *first, QdfRibbonControl *second)
{
    int v1 = first->sizeDefinition(first->currentSize())->visualIndex();
    if (v1 == -1)
    {
        QdfRibbonGroup *group = first->parentGroup();
        v1 = group->qdf_d()->m_controls.indexOf(first);
    }

    int v2 = second->sizeDefinition(second->currentSize())->visualIndex();
    if (v2 == -1)
    {
        QdfRibbonGroup *group = second->parentGroup();
        v2 = group->qdf_d()->m_controls.indexOf(second);
    }
    return v1 < v2;
}

QdfRibbonBar *QdfRibbonGroup::ribbonBar() const
{
    if (QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(parentWidget()))
    {
        return qobject_cast<QdfRibbonBar *>(page->parentWidget());
    }
    return nullptr;
}

QdfRibbonGroup::QdfRibbonGroup(QdfRibbonPage *page, const QString &title) : QWidget(page)
{
    QDF_INIT_PRIVATE(QdfRibbonGroup);
    QDF_D(QdfRibbonGroup);
    d->init();
    setTitle(title);
}

QdfRibbonGroup::QdfRibbonGroup(QWidget *parent) : QWidget(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonGroup);
    QDF_D(QdfRibbonGroup);
    d->init();
}

QdfRibbonGroup::~QdfRibbonGroup()
{
    QDF_D(QdfRibbonGroup);
    d->clear(true);
    QDF_FINI_PRIVATE();
}

bool QdfRibbonGroup::isReduced() const
{
    QDF_D(const QdfRibbonGroup);
    if (d->m_controls.size() == 1)
    {
        if (!d->m_controls.at(0)
                     ->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)
                     ->isStretchable())
        {
            return false;
        }
    }
    return d->m_currentSize == QdfRibbonControlSizeDefinition::GroupPopup;
}

const QString &QdfRibbonGroup::title() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_title;
}

void QdfRibbonGroup::setTitle(const QString &title)
{
    QDF_D(QdfRibbonGroup);

    if (d->m_title == title)
    {
        return;
    }

    d->m_title = title;
    adjustSize();
    emit titleChanged(d->m_title);
}

const QFont &QdfRibbonGroup::titleFont() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_font != nullptr ? *d->m_font : font();
}

void QdfRibbonGroup::setTitleFont(const QFont &font)
{
    QDF_D(QdfRibbonGroup);

    if (d->m_font && *d->m_font == font)
    {
        return;
    }
    if (d->m_font)
    {
        delete d->m_font;
    }
    d->m_font = nullptr;
    d->m_font = new QFont(font);

    if (QdfRibbonBar *ribBar = ribbonBar())
    {
        ribBar->updateLayout();
        if (ribBar->parentWidget())
        {
            if (QLayout *layout = ribBar->parentWidget()->layout())
            {
                layout->update();
            }
        }
    }
    emit titleFontChanged(*d->m_font);
}

const QColor &QdfRibbonGroup::titleColor() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_color;
}

void QdfRibbonGroup::setTitleColor(const QColor &color)
{
    QDF_D(QdfRibbonGroup);

    if (d->m_color == color)
    {
        return;
    }
    d->m_color = color;
    update();
}

const QIcon &QdfRibbonGroup::icon() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_icon;
}

void QdfRibbonGroup::setIcon(const QIcon &icon)
{
    QDF_D(QdfRibbonGroup);
    d->m_icon = icon;
    adjustSize();
}

bool QdfRibbonGroup::isOptionButtonVisible() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_optionButton->isVisible();
}

void QdfRibbonGroup::setOptionButtonVisible(bool visible)
{
    QDF_D(QdfRibbonGroup);
    d->m_butOptionVisible = visible;
    d->m_optionButton->setVisible(visible);
    d->updateOptionButtonLayout();
    adjustSize();
}

QAction *QdfRibbonGroup::optionButtonAction() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_actOption;
}

void QdfRibbonGroup::setOptionButtonAction(QAction *action)
{
    QDF_D(QdfRibbonGroup);
    if (d->m_actOption != NULL)
    {
        delete d->m_actOption;
    }

    d->m_actOption = action;
    d->m_optionButton->setDefaultAction(d->m_actOption);
}

Qt::Alignment QdfRibbonGroup::contentAlignment() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_contentAlignment;
}

void QdfRibbonGroup::setContentAlignment(Qt::Alignment alignment)
{
    QDF_D(QdfRibbonGroup);
    d->m_contentAlignment = alignment;
}

Qt::Alignment QdfRibbonGroup::controlsAlignment() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_controlsAlignment;
}

void QdfRibbonGroup::setControlsAlignment(Qt::Alignment alignment)
{
    QDF_D(QdfRibbonGroup);
    if (d->m_controlsAlignment != alignment)
    {
        d->m_controlsAlignment = alignment;
        if (parentWidget())
        {
            int controlsLeftOffset = 0;
            d->updateControlsLayout(parentWidget(), controlsLeftOffset);
        }
    }
}

int QdfRibbonGroup::controlCount() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_controls.size();
}

void QdfRibbonGroup::setControlsCentering(bool controlsCentering)
{
    QDF_D(QdfRibbonGroup);
    if (controlsCentering)
    {
        d->m_contentAlignment = Qt::AlignVCenter;
    }
    else
    {
        d->m_contentAlignment = Qt::Alignment();
    }
}

bool QdfRibbonGroup::isControlsCentering() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_contentAlignment.testFlag(Qt::AlignVCenter);
}

void QdfRibbonGroup::setControlsGrouping(bool controlsGrouping)
{
    QDF_D(QdfRibbonGroup);
    if (controlsGrouping)
    {
        if (isControlsGrouping())
        {
            return;
        }
        d->m_toolBar = new QdfRibbonToolBarControl(this);
        addControl(d->m_toolBar);
    }
    else
    {
        if (d->m_toolBar != nullptr)
        {
            removeControl(d->m_toolBar);
        }
        if (d->m_toolBar)
        {
            delete d->m_toolBar;
        }
        d->m_toolBar = nullptr;
    }
}

bool QdfRibbonGroup::isControlsGrouping() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_toolBar != nullptr;
}

QdfRibbonControl *QdfRibbonGroup::controlByIndex(int index) const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_controls[index];
}

QdfRibbonControl *QdfRibbonGroup::controlByAction(QAction *action) const
{
    QDF_D(const QdfRibbonGroup);
    Q_ASSERT(action != nullptr);
    for (QList<QdfRibbonControl *>::const_iterator it = d->m_controls.constBegin();
         it != d->m_controls.constEnd(); ++it)
    {
        if ((*it)->defaultAction() == action)
        {
            return *it;
        }
    }
    return nullptr;
}

QdfRibbonWidgetControl *QdfRibbonGroup::controlByWidget(QWidget *widget) const
{
    QDF_D(const QdfRibbonGroup);
    Q_ASSERT(widget != nullptr);
    for (QList<QdfRibbonControl *>::const_iterator it = d->m_controls.constBegin();
         it != d->m_controls.constEnd(); ++it)
    {
        if (QdfRibbonWidgetControl *widgetControl = qobject_cast<QdfRibbonWidgetControl *>((*it)))
        {
            if (widgetControl->contentWidget() == widget)
            {
                return widgetControl;
            }
        }
    }
    return nullptr;
}

QdfRibbonControlSizeDefinition::GroupSize QdfRibbonGroup::currentSize() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_currentSize;
}

void QdfRibbonGroup::addControl(QdfRibbonControl *control)
{
    QDF_D(QdfRibbonGroup);
    if (d->m_controls.indexOf(control) != -1)
    {
        return;// Already was added.
    }

    control->qdf_d()->m_parentGroup = this;

    d->m_controls.append(control);
    control->setFont(font());
    control->sizeChanged(currentSize());

    QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(parentWidget());
    if (page != nullptr)
    {
        page->qdf_d()->updateLayout();
    }
}

void QdfRibbonGroup::removeControl(QdfRibbonControl *control)
{
    QDF_D(QdfRibbonGroup);
    if (d->m_controls.removeAll(control) > 0)
    {
        control->qdf_d()->m_parentGroup = nullptr;
        control->setParent(nullptr);

        QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(parentWidget());
        if (page != nullptr)
        {
            page->qdf_d()->updateLayout();
        }
    }
}

QAction *QdfRibbonGroup::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style,
                                   QMenu *menu, QToolButton::ToolButtonPopupMode mode)
{
    QDF_D(const QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addAction(icon, text, style, menu, mode);
    }

    QdfRibbonButtonAction *action = new QdfRibbonButtonAction(this, icon, text, style, menu, mode);
    addAction(action);
    return action;
}

QAction *QdfRibbonGroup::addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                   QToolButton::ToolButtonPopupMode mode)
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addAction(action, style, menu, mode);
    }

    addAction(action);

    QdfRibbonButtonControl *control = qobject_cast<QdfRibbonButtonControl *>(controlByAction(action));
    Q_ASSERT(control != nullptr);

    control->setToolButtonStyle(style);
    if (menu || action->menu())
    {
        control->setPopupMode(mode);
    }
    if (menu)
    {
        control->setMenu(menu);
    }

    return action;
}

QAction *QdfRibbonGroup::insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style,
                                      QMenu *menu, QToolButton::ToolButtonPopupMode mode)
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->insertAction(before, action, style, menu, mode);
    }

    insertAction(before, action);

    QdfRibbonButtonControl *control = qobject_cast<QdfRibbonButtonControl *>(controlByAction(action));
    Q_ASSERT(control != nullptr);

    control->setToolButtonStyle(style);
    if (menu || action->menu())
    {
        control->setPopupMode(mode);
    }
    if (menu)
    {
        control->setMenu(menu);
    }

    return action;
}

QAction *QdfRibbonGroup::addWidget(QWidget *widget)
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addWidget(widget);
    }

    return addWidget(QIcon(), QString(""), widget);
}

QAction *QdfRibbonGroup::addWidget(const QIcon &icon, const QString &text, QWidget *widget)
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addWidget(icon, text, widget);
    }

    QWidgetAction *action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);

    QdfRibbonWidgetControl *control = new QdfRibbonWidgetControl(this, false);
    control->setContentWidget(widget);
    control->setDefaultAction(action);
    action->setDefaultWidget(control);
    addAction(action);

    return action;
}

QAction *QdfRibbonGroup::addWidget(const QIcon &icon, const QString &text, bool stretch, QWidget *widget)
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addWidget(icon, text, widget);
    }

    QAction *action = addWidget(icon, text, widget);
    if (stretch)
    {
        QdfRibbonControl *control = controlByAction(action);
        Q_ASSERT(control != nullptr);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setStretchable(true);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setStretchable(true);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setStretchable(true);
        control->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setStretchable(true);
    }
    return action;
}

QMenu *QdfRibbonGroup::addMenu(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style)
{
    QDF_D(QdfRibbonGroup);

    QMenu *menu = new QMenu(text, this);
    if (isControlsGrouping())
    {
        d->m_toolBar->addMenu(icon, text, menu, QToolButton::InstantPopup);
    }
    else
    {
        QdfRibbonButtonAction *action =
                new QdfRibbonButtonAction(this, icon, text, style, menu, QToolButton::InstantPopup);
        addAction(action);
    }
    return menu;
}

QAction *QdfRibbonGroup::addSeparator()
{
    QDF_D(QdfRibbonGroup);

    if (isControlsGrouping())
    {
        return d->m_toolBar->addSeparator();
    }

    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

void QdfRibbonGroup::remove(QWidget *widget)
{
    if (isControlsGrouping())
    {
        Q_ASSERT(false);
        // d->m_toolBar->removeWidget(widget);
        return;
    }

    if (!widget)
    {
        return;
    }

    if (QdfRibbonWidgetControl *widgetcontrol = controlByWidget(widget))
    {
        widgetcontrol->qdf_d()->clearWidget();
    }

    QList<QAction *> actions = this->actions();
    for (int i = 0; i < actions.size(); i++)
    {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(actions.at(i)))
        {
            if (widgetAction->defaultWidget() == widget)
            {
                removeAction(actions.at(i));
                return;
            }
        }
    }
}

void QdfRibbonGroup::clear()
{
    QDF_D(QdfRibbonGroup);
    d->clear(false);
}

Qt::TextElideMode QdfRibbonGroup::titleElideMode() const
{
    QDF_D(const QdfRibbonGroup);
    return d->m_titleElideMode;
}

void QdfRibbonGroup::setTitleElideMode(Qt::TextElideMode mode)
{
    QDF_D(QdfRibbonGroup);
    if (d->m_titleElideMode != mode)
    {
        d->m_titleElideMode = mode;
        d->updateLayoutParent(true);
    }
}

QSize QdfRibbonGroup::sizeHint() const
{
    QDF_D(const QdfRibbonGroup);
    QSize size = d->sizeHint();

    if (isReduced())
    {
        QStyleOptionToolButton opt;
        opt.init(this);
        opt.text = d->m_title;
        return QSize(
                style()->pixelMetric((QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonReducedGroupWidth,
                                     &opt, this),
                size.height());
    }
    else
    {
        return size.expandedTo(QApplication::globalStrut());
    }
}

void QdfRibbonGroup::currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (isVisible())
    {
        bool doReleased = true;
        QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
        if (comboBox)
        {
            QAbstractItemView *itemView = comboBox->view();
            if (itemView && !itemView->isVisible())
            {
                doReleased = false;
            }
        }
        if (doReleased)
        {
            emit released();
        }
    }
}

void qdf_set_font_to_group_children(QdfRibbonGroup *group, const QFont &font)
{
    for (int i = 0, count = group->controlCount(); count > i; ++i)
    {
        if (QdfRibbonControl *control = group->controlByIndex(i))
        {
            control->setFont(font);
            qdf_set_font_to_ribbon_children(control, font);
        }
    }
}

bool QdfRibbonGroup::event(QEvent *event)
{
    QDF_D(QdfRibbonGroup);

    if (d->m_dirty && event->type() == QEvent::LayoutRequest)
    {
        d->updateLayoutParent(true);
        d->m_dirty = false;
    }

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
                        widget->setFont(font());
                        qdf_set_font_to_ribbon_children(widget, font());
                    }
                }
            }
            break;
        case QEvent::StyleChange:
        case QEvent::FontChange:
            qdf_set_font_to_group_children(this, font());
            break;
        default:
            break;
    }

    if (isMinimized())
    {
        if (event->type() == QEvent::Show)
        {
            d->showGroupScroll();
        }
        else if (event->type() == QEvent::Hide)
        {
            d->m_groupScrollPos = 0;
            d->m_pageScrollPos = 0;
            layout()->update();
            if (windowFlags() & Qt::Popup)
            {
                emit hidePopup();
            }
        }
    }
    else
    {
        switch (event->type())
        {
            case QEvent::KeyPress:
                {
                    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
                    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
                    {
                        if (QWidget *focus = focusWidget())
                        {
                            if (qobject_cast<QComboBox *>(focus) || qobject_cast<QLineEdit *>(focus))
                            {
                                emit released();
                            }
                        }
                    }
                }
                break;
            case QEvent::Show:
            case QEvent::Hide:
                updateGeometry();
                break;
            case QEvent::ParentChange:
                break;
            default:
                break;
        }
    }
    return QWidget::event(event);
}

void QdfRibbonGroup::paintEvent(QPaintEvent *event)
{
    QDF_D(QdfRibbonGroup);

    QPainter p(this);
    if (isReduced())
    {
        QStyleOptionToolButton opt;
        opt.init(this);
        opt.text = d->m_title;
        opt.icon = d->m_icon;
        opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));

        if (d->m_groupPopup && d->m_groupPopup->isVisible())
        {
            opt.features |= QStyleOptionToolButton::HasMenu;
        }

        QRect rcGroup = opt.rect;
        rcGroup.setBottom(rcGroup.bottom() - 1);
        opt.rect = rcGroup;
        style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_ReducedGroup, &opt, &p, this);
    }
    else
    {
        QRect rectEvent = event->rect();
        p.setClipRect(rectEvent);

        QStyle *ribbonStyle = style();

        if (windowFlags() & Qt::Popup)
        {
            QdfStyleOptionRibbon opt;
            opt.init(this);
            style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonGroups, &opt, &p,
                                 this);
        }

        QStyleOptionGroupBox opt;
        d->initStyleOption(opt);
        if (d->m_font != nullptr)
        {
            opt.fontMetrics = QFontMetrics(*d->m_font);
        }

        if (rectEvent.intersects(opt.rect))
        {
            ribbonStyle->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_Group, &opt, &p, this);
        }

        QFont saveFont = p.font();
        if (d->m_font != nullptr)
        {
            p.setFont(*d->m_font);
        }

        if (d->m_color.isValid())
        {
            opt.palette.setColor(QPalette::WindowText, d->m_color);
        }

        int flags = opt.textAlignment;
        flags |= Qt::TextHideMnemonic;

        QRect rcEntryCaption = opt.rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - opt.lineWidth);

        int indent = 0;
        if (d->m_optionButton->isVisible())
        {
            indent = d->m_optionButton->width();
        }

        rcEntryCaption.adjust(-2, -1, -indent, -2);
        opt.text = p.fontMetrics().elidedText(opt.text, titleElideMode(), rcEntryCaption.width());
        ribbonStyle->drawItemText(&p, rcEntryCaption, flags, opt.palette,
                                  opt.state & QStyle::State_Enabled, opt.text, QPalette::WindowText);

        p.setFont(saveFont);
    }
}

void QdfRibbonGroup::actionEvent(QActionEvent *event)
{
    QDF_D(QdfRibbonGroup);

    QAction *action = event->action();
    QdfRibbonControl *control = controlByAction(action);

    switch (event->type())
    {
        case QEvent::ActionAdded:
            {
                if (control == nullptr)
                {
                    if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action))
                    {
                        QWidget *widget = widgetAction->requestWidget(this);
                        if (widget != nullptr)
                        {
                            control = qobject_cast<QdfRibbonControl *>(widget);
                            if (control == nullptr)
                            {
                                QdfRibbonWidgetControl *widgetControl =
                                        new QdfRibbonWidgetControl(this, false);
                                widgetControl->setContentWidget(widget);
                                control = widgetControl;
                                control->setDefaultAction(action);
                                widgetAction->setDefaultWidget(control);
                            }
                        }
                        else
                        {
                            Q_ASSERT(false);
                            return;
                        }
                    }
                    else if (action->isSeparator())
                    {
                        control = new QdfRibbonColumnBreakControl(this);
                    }

                    if (control == nullptr)
                    {
                        control = new QdfRibbonButtonControl(this);
                    }

                    control->setDefaultAction(action);
                    addControl(control);
                }
            }
            break;
        case QEvent::ActionRemoved:
            {
                if (control != nullptr)
                {
                    removeControl(control);
                    if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action))
                    {
                        widgetAction->releaseWidget(control);
                        if (widgetAction->defaultWidget() == nullptr)
                        {
                            delete control;
                        }
                    }
                    else
                    {
                        control->hide();
                        delete control;
                    }
                }
            }
            break;
        case QEvent::ActionChanged:
            {
                Q_ASSERT(control != nullptr);
                if (control == nullptr)
                {
                    control->actionChanged();
                }
                d->m_dirty = true;
                QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
                break;
            }
        default:
            Q_ASSERT_X(false, "QdfRibbonGroup::actionEvent", "internal error");
    }
}

void QdfRibbonGroup::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();
}

void QdfRibbonGroup::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();
}

void QdfRibbonGroup::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfRibbonGroup);
    if (isReduced())
    {
        Q_ASSERT(d->m_groupPopup != nullptr);
        if (event->button() == Qt::LeftButton)
        {
            QPoint pnt(0, 0);
            QRect rc = rect();

            QRect screen = QApplication::desktop()->availableGeometry(this);
            int h = rc.height();
            if (this->mapToGlobal(QPoint(0, rc.bottom())).y() + h <= screen.height())
            {
                pnt = this->mapToGlobal(rc.bottomLeft());
            }
            else
            {
                pnt = this->mapToGlobal(rc.topLeft() - QPoint(0, h));
            }

            const int desktopFrame = style()->pixelMetric(QStyle::PM_MenuDesktopFrameWidth, 0, this);
            if (pnt.x() + rc.width() - 1 > screen.right() - desktopFrame)
            {
                pnt.setX(screen.right() - desktopFrame - rc.width() + 1);
            }
            if (pnt.x() < screen.left() + desktopFrame)
            {
                pnt.setX(screen.left() + desktopFrame);
            }

            d->m_groupPopup->popup(pnt, 0);
            return;
        }
    }
    QWidget::mousePressEvent(event);
}
