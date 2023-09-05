#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif// Q_OS_WIN

#include "qdfofficepopupwindow_p.h"
#include "qdfpopuphelpers.h"
#include <ribbon/qdfofficepopupwindow.h>

QDF_USE_NAMESPACE


QdfTitleBar::QdfTitleBar(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    horizontalMainLayout = new QHBoxLayout(this);
    horizontalMainLayout->setMargin(0);
    horizontalMainLayout->setSpacing(0);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    labelIcon = new QLabel(this);
    labelIcon->setVisible(false);
    horizontalLayout->addWidget(labelIcon);

    labelTitle = new QLabel(this);
    labelTitle->setVisible(false);
    horizontalLayout->addWidget(labelTitle);

    horizontalLayout->setStretch(1, 1);
    horizontalMainLayout->addLayout(horizontalLayout);
}

QdfTitleBar::~QdfTitleBar()
{
}

void QdfManagerPopup::appendPopup(QdfOfficePopupWindow *popup)
{
    m_listPopup.append(popup);
}

void QdfManagerPopup::removePopup(QdfOfficePopupWindow *popup)
{
    m_listPopup.removeOne(popup);
}

QPoint QdfManagerPopup::findBestPosition(QdfOfficePopupWindow *popup)
{
    if (m_ptPopup != QPoint(-1, -1))
    {
        popup->setPosition(m_ptPopup);
    }

    QPoint ptPopup = popup->getPosition();
    QSize szPopup = popup->sizeHint();
    QRect rcActivePopup(QPoint(ptPopup.x() - szPopup.width(), ptPopup.y() - szPopup.height()), szPopup);

    bool intersect = false;
    do
    {
        intersect = false;
        int index = 0;
        while (index < m_listPopup.count())
        {
            QdfOfficePopupWindow *curPopup = m_listPopup.at(index);
            if (curPopup)
            {
                QSize szCurPopup = curPopup->sizeHint();
                QRect rcPopup(QPoint(curPopup->getPosition().x() - szCurPopup.width(),
                                     curPopup->getPosition().y() - szCurPopup.height()),
                              szCurPopup);

                if (rcPopup.intersects(rcActivePopup))
                {
                    ptPopup.setY(rcPopup.top());
                    rcActivePopup =
                            QRect(QPoint(ptPopup.x() - szPopup.width(), ptPopup.y() - szPopup.height()),
                                  szPopup);
                    intersect = true;
                }
            }
            index++;
        }
    } while (intersect);
    return ptPopup;
}

QdfOfficePopupWindowPrivate::QdfOfficePopupWindowPrivate()
{
    m_form = nullptr;
    m_closeButton = false;
    m_notCloseTimer = false;
}

QdfOfficePopupWindowPrivate::~QdfOfficePopupWindowPrivate()
{
}

void QdfOfficePopupWindowPrivate::init()
{
    QDF_Q(QdfOfficePopupWindow);
    q->setAttribute(Qt::WA_MouseTracking);

    m_titleFont = QApplication::font("QDockWidgetTitle");
    q->setFont(m_titleFont);

    m_buttonDown = QStyle::SC_None;
    m_transparency = 255;
    m_showDelay = 5000;
    m_animationSpeed = 256;
    m_animationInterval = 16;
    m_step = 0;

    m_capture = false;
    m_allowMove = false;
    m_dragging = false;
    m_pressed = false;

    m_positionPopup = QPoint(-1, -1);
    m_dragPressPosition = QPoint(-1, -1);
#ifdef Q_OS_WIN
    m_popupLocation = PopupLocationNearTaskBar;
#else // Q_OS_WIN
    m_popupLocation = PopupLocationBottomRight;
#endif// Q_OS_WIN
    m_popupAnimation = PopupAnimation::PA_None;
    m_popupState = PopupStateClosed;

    connect(&m_showDelayTimer, SIGNAL(timeout()), q, SLOT(showDelayTimer()));
    connect(&m_collapsingTimer, SIGNAL(timeout()), q, SLOT(collapsingTimer()));
    connect(&m_expandingTimer, SIGNAL(timeout()), q, SLOT(expandingTimer()));

    m_title = new QdfTitleBar(q);

    QLabel *label = new QLabel();
    q->setCentralWidget(label);
}

void QdfOfficePopupWindowPrivate::onCollapsing()
{
    QDF_Q(QdfOfficePopupWindow);
    setPopupState(PopupStateCollapsing);

    if (m_popupAnimation == PopupAnimation::PA_None || m_animationSpeed <= 0)
    {
        closePopup();
        return;
    }
    else if (m_popupAnimation == PopupAnimation::PA_Fade)
    {
        m_stateTarget.transparency = 0;
    }
    else if (m_popupAnimation == PopupAnimation::PA_Slide)
    {
        QStyleOption opt;
        opt.init(q);
        QPoint positonPopup = q->getPosition();
        m_stateTarget.rcPopup =
                QRect(QPoint(positonPopup.x() - opt.rect.width(), positonPopup.y()), positonPopup);
    }
    else if (m_popupAnimation == PopupAnimation::PA_Unfold)
    {
        m_stateTarget.rcPopup = QRect(q->getPosition(), QSize(0, 0));
    }

    m_step = qMax((uint) 1, m_animationSpeed / m_animationInterval);

    m_collapsingTimer.start(m_animationInterval);

    updateState(true);
}

void QdfOfficePopupWindowPrivate::onExpanding(bool updateCurrent)
{
    QDF_Q(QdfOfficePopupWindow);
    setPopupState(PopupStateExpanding);

    QPoint positionPopup = q->getPosition();
    QSize sz = q->sizeHint();

    m_stateTarget.rcPopup =
            QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y() - sz.height()), sz);
    m_stateTarget.transparency = m_transparency;

    if (updateCurrent)
    {
        m_stateCurrent = m_stateTarget;
        if (m_popupAnimation == PopupAnimation::PA_None || m_animationSpeed <= 0)
        {
            updateState(true);
            showPopup();
            return;
        }
        else if (m_popupAnimation == PopupAnimation::PA_Fade)
        {
            m_stateCurrent.transparency = 0;
        }
        if (m_popupAnimation == PopupAnimation::PA_Slide)
        {
            m_stateCurrent.rcPopup =
                    QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y()), positionPopup);
        }
        else if (m_popupAnimation == PopupAnimation::PA_Fade)
        {
            m_stateCurrent.rcPopup = QRect(positionPopup, QSize(0, 0));
        }

        m_step = qMax((uint) 1, m_animationSpeed / m_animationInterval);
    }
    else
    {
        m_step = qMax((uint) 1, m_animationSpeed / m_animationInterval - m_step);
    }

    m_expandingTimer.start(m_animationInterval);
    updateState(true);
}

void QdfOfficePopupWindowPrivate::updateState(bool repaint)
{
    QDF_Q(QdfOfficePopupWindow);
    QRect rc = m_stateCurrent.rcPopup;

    q->setGeometry(rc);

    if (repaint)
    {
        q->update();
    }

    if (!m_capture)
    {
        q->setWindowOpacity((qreal) m_stateCurrent.transparency / 255.0);
    }
    q->update();
}

bool QdfOfficePopupWindowPrivate::closePopup()
{
    QDF_Q(QdfOfficePopupWindow);
    bool result = false;
    m_capture = false;

    if (q->testAttribute(Qt::WA_DeleteOnClose))
    {
        setPopupState(PopupStateClosed);
        result = q->close();
        q->deleteLater();
        return result;
    }

    result = q->close();
    setPopupState(PopupStateClosed);
    return result;
}

void QdfOfficePopupWindowPrivate::showPopup()
{
    QDF_Q(QdfOfficePopupWindow);

    setPopupState(PopupStateShow);

    QPoint positionPopup = q->getPosition();
    QSize sizePopup = q->size();
    m_stateCurrent.rcPopup =
            QRect(QPoint(positionPopup.x() - sizePopup.width(), positionPopup.y() - sizePopup.height()),
                  sizePopup);
    m_stateCurrent.transparency = m_transparency;

    if (m_showDelay != (uint) -1)
    {
        m_showDelayTimer.start(m_showDelay);
    }
}

static void moveTo(int &x1, int &x2, int step)
{
    if (x1 != x2)
    {
        x1 += qMax(1, ::abs(x1 - x2) / step) * (x1 > x2 ? -1 : 1);
    }
}

void QdfOfficePopupWindowPrivate::animate(int step)
{
    if (step < 1)
    {
        m_stateCurrent = m_stateTarget;
    }
    else
    {
        int x11, y11, x12, y12;
        m_stateCurrent.rcPopup.getCoords(&x11, &y11, &x12, &y12);
        int x21, y21, x22, y22;
        m_stateTarget.rcPopup.getCoords(&x21, &y21, &x22, &y22);

        moveTo(y11, y21, step);
        moveTo(x11, x21, step);
        moveTo(x12, x22, step);
        moveTo(y12, y22, step);
        m_stateCurrent.rcPopup.setCoords(x11, y11, x12, y12);
        moveTo(m_stateCurrent.transparency, m_stateTarget.transparency, step);
    }
    updateState(true);
}

void QdfOfficePopupWindowPrivate::handleMousePressEvent(QMouseEvent *event)
{
    QDF_Q(QdfOfficePopupWindow);

    QdfStyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);

    if (event->button() == Qt::LeftButton)
    {
        QStyle::SubControl ctrl =
                q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), q);
        m_buttonDown = ctrl;
        m_pressed = true;
        if (m_buttonDown == QStyle::SC_TitleBarCloseButton)
        {
            q->update();
            return;
        }
    }

    if (getPopupState() == PopupStateExpanding)
    {
        m_step = 0;
        animate(0);
        m_expandingTimer.stop();
        showPopup();
    }

    if (m_allowMove && event->button() == Qt::LeftButton)
    {
        const int titleBarHeight = q->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, q);
        QRect rect = optTitle.rect;
        rect.setHeight(titleBarHeight);
        if (rect.contains(event->pos()))
        {
            m_dragging = true;
            m_dragPressPosition = event->pos();
            return;
        }
    }
}

void QdfOfficePopupWindowPrivate::handleMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QDF_Q(QdfOfficePopupWindow);

    if (event->button() == Qt::LeftButton)
    {
        m_pressed = false;

        QdfStyleOptionPopupTitleBar optTitle;
        initTitleBarStyleOption(&optTitle);
        QStyle::SubControl ctrl =
                q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), q);
        m_buttonDown = ctrl;
        if (m_buttonDown == QStyle::SC_TitleBarCloseButton)
        {
            closePopup();
            return;
        }
    }

    if (m_dragging)
    {
        m_stateTarget.rcPopup = m_stateCurrent.rcPopup = q->geometry();
        m_positionPopup = m_stateCurrent.rcPopup.bottomRight();

        m_dragging = false;
        m_dragPressPosition = QPoint(-1, -1);
    }
}

void QdfOfficePopupWindowPrivate::handleMouseMoveEvent(QMouseEvent *event)
{
    QDF_Q(QdfOfficePopupWindow);
    if (m_dragging && m_form)
    {
        const QPoint delta = event->pos() - m_dragPressPosition;

        QRect rc = q->rect();
        QRect rcScreenGeometry = QApplication::desktop()->screenGeometry();

        rc.translate(q->pos() + delta);

        if (rc.left() < rcScreenGeometry.left())
        {
            rc.translate(rcScreenGeometry.left() - rc.left(), 0);
        }
        if (rc.top() < rcScreenGeometry.top())
        {
            rc.translate(0, rcScreenGeometry.top() - rc.top());
        }
        if (rc.right() > rcScreenGeometry.right())
        {
            rc.translate(rcScreenGeometry.right() - rc.right(), 0);
        }
        if (rc.bottom() > rcScreenGeometry.bottom())
        {
            rc.translate(0, rcScreenGeometry.bottom() - rc.bottom());
        }

        q->setGeometry(rc);
        // move(this->pos() + delta);

        m_stateTarget.rcPopup = m_stateCurrent.rcPopup = q->geometry();
        m_positionPopup = m_stateCurrent.rcPopup.bottomRight();

        QdfManagerPopup::getMngPopup().m_ptPopup = q->getPosition();
        return;
    }

    QRect rc = q->rect();
    bool contains = rc.contains(event->pos());

    if (contains && !m_capture)
    {
        m_capture = true;
        q->setWindowOpacity(255.0);
    }
    if (!contains && m_capture)
    {
        m_capture = false;
        q->setWindowOpacity((qreal) m_transparency / 255.0);
    }
    if (m_popupState == PopupStateCollapsing)
    {
        m_collapsingTimer.stop();

        if (m_popupAnimation == PopupAnimation::PA_Fade)
        {
            showPopup();
        }
        else
        {
            onExpanding(false);
        }
    }

    QdfStyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);
    QStyle::SubControl last_ctrl = m_buttonDown;
    m_buttonDown = q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), q);
    if (last_ctrl != m_buttonDown)
    {
        q->update();
    }
}

void QdfOfficePopupWindowPrivate::initTitleBarStyleOption(QdfStyleOptionPopupTitleBar *option) const
{
    QDF_Q(const QdfOfficePopupWindow);
    option->init(q);
    option->palette = m_titlePalette;
    option->text = m_titleTxt;
    QFontMetrics fm(m_titleFont);
    option->fontMetrics = fm;
    option->subControls = QStyle::SubControls();

    if (!m_titleTxt.isEmpty())
    {
        option->subControls = QStyle::SC_TitleBarLabel;
    }

    if (m_closeButton)
    {
        option->subControls |= QStyle::SC_TitleBarCloseButton;
    }

    if (!m_titleIcon.isNull())
    {
        option->subControls |= QStyle::SC_TitleBarSysMenu;
        option->icon = m_titleIcon;
    }

    if (!m_closePixmap.isNull())
    {
        option->pixmapCloseButton = m_closePixmap;
    }

    option->activeSubControls = m_buttonDown ? QStyle::SC_TitleBarCloseButton : QStyle::SC_None;

    if (m_pressed)
    {
        option->state |= QStyle::State_Sunken;
    }

    const int titleBarHeight = q->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, q);
    const int lineWidth = q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, q) + 2;

    QSize szTitle = m_title->sizeHint();

    QSize size = m_form->sizeHint();
    if (!size.isValid())
    {
        size = m_form->size();
    }

    option->rect.setHeight(titleBarHeight);
    option->rect.setWidth(qMax((size.width() + lineWidth * 2), szTitle.width()));
}

void QdfOfficePopupWindowPrivate::initFormStyleOption(QStyleOptionFrame *option) const
{
    QDF_Q(const QdfOfficePopupWindow);
    option->init(q);
    option->palette = m_titlePalette;

    const int lineWidth = q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, q) + 2;
    option->lineWidth = lineWidth;
    option->midLineWidth = 1;

    QSize size = m_form->sizeHint();
    if (!size.isValid())
    {
        size = m_form->size();
    }

    QdfStyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);
    QRect titleBar = optTitle.rect;

    const int titleBarHeight = q->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, q);
    QSize szTitle = m_title->sizeHint();

    if (m_closeButton && szTitle.isNull())
    {
        QRect buttonRect = q->style()->subControlRect(QStyle::CC_TitleBar, &optTitle,
                                                      QStyle::SC_TitleBarCloseButton, q);
        szTitle.setHeight(buttonRect.height());
    }

    QRect labelRect = QRect(QPoint(0, 0), QSize(szTitle.width(),
                                                titleBarHeight > 7 ? titleBarHeight
                                                                   : titleBarHeight + szTitle.height()));

    titleBar = titleBar.united(labelRect);

    int additive = lineWidth * 2;
    option->rect.setHeight(size.height() + titleBar.height() + additive);
    option->rect.setWidth(size.width() + additive);
}

void QdfOfficePopupWindowPrivate::setPopupState(PopupState popupState)
{
    if (m_popupState == popupState)
    {
        return;
    }
    m_popupState = popupState;
}

PopupState QdfOfficePopupWindowPrivate::getPopupState() const
{
    return m_popupState;
}

void QdfOfficePopupWindowPrivate::createStyle()
{
    QDF_Q(QdfOfficePopupWindow);
    if (QdfOfficeStyle *officeStyle = qobject_cast<QdfOfficeStyle *>(q->style()))
    {
        officeStyle->setPopupDecoration(QdfOfficeStyle::PopupSystemDecoration);
    }
    else
    {
        q->setStyle(new QdfPopupDrawHelper());
    }
}

void QdfOfficePopupWindowPrivate::calclayout()
{
    QDF_Q(QdfOfficePopupWindow);
    if (QdfOfficeStyle *officeStyle = qobject_cast<QdfOfficeStyle *>(q->style()))
    {
        if (!officeStyle->isExistPopupProxy())
        {
            createStyle();
        }
    }
    else
    {
        createStyle();
    }

    QdfStyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);

    QSize szTitle = m_title->sizeHint();
    QRect buttonRect = q->style()->subControlRect(QStyle::CC_TitleBar, &optTitle,
                                                  QStyle::SC_TitleBarCloseButton, q);

    if (m_closeButton && szTitle.isNull())
    {
        szTitle.setHeight(buttonRect.height());
    }

    const int titleBarHeight = q->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, q);
    const int lineWidth = q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, q) + 2;

    int y = titleBarHeight > 7 ? /*lineWidth*/ 0 : titleBarHeight + lineWidth / 2;
    szTitle.setWidth(optTitle.rect.width() - buttonRect.width() - lineWidth * 4);
    szTitle.setHeight(titleBarHeight > 7 ? titleBarHeight : szTitle.height());
    QRect rectTitle(QPoint(lineWidth, y), szTitle);
    m_title->setGeometry(rectTitle);

    if (!optTitle.icon.isNull())
    {
        QSize sz = optTitle.icon.actualSize(rectTitle.size());
        QImage px(sz, QImage::Format_ARGB32_Premultiplied);
        QPainter p(&px);
        px.fill(0);
        optTitle.icon.paint(&p, 0, 0, sz.width(), sz.height());
        m_title->labelIcon->setPixmap(QPixmap::fromImage(px));
        m_title->labelIcon->setVisible(true);
    }

    m_form->move(lineWidth,
                 szTitle.height() > 0 ? rectTitle.bottom() + lineWidth : titleBarHeight + lineWidth);
    setPopupState(PopupStateShow);

    QPoint positionPopup = q->getPosition();
    QSize sz = q->sizeHint();

    m_stateCurrent.rcPopup =
            QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y() - sz.height()), sz);
    m_stateCurrent.transparency = m_transparency;

    if (m_showDelay != (uint) -1)
    {
        m_showDelayTimer.start(m_showDelay);
    }
}


QdfOfficePopupWindow::QdfOfficePopupWindow(QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    QDF_INIT_PRIVATE(QdfOfficePopupWindow);
    QDF_D(QdfOfficePopupWindow);
    d->init();
}

QdfOfficePopupWindow::~QdfOfficePopupWindow()
{
    if (QdfOfficeStyle *officeStyle = qobject_cast<QdfOfficeStyle *>(style()))
    {
        officeStyle->unsetPopupProxy();
    }
    QDF_FINI_PRIVATE();
}

void QdfOfficePopupWindow::showPopup(QWidget *parent, const QIcon &titleIcon, const QString &titleText,
                                     const QString &bodyText, bool closeButtonVisible,
                                     QdfOfficeStyle::OfficePopupDecoration decoration)
{
    QdfOfficePopupWindow *popup = new QdfOfficePopupWindow(parent);
    popup->setTitleIcon(titleIcon);
    popup->setTitleText(titleText);
    popup->setTitleCloseButtonVisible(closeButtonVisible);
    popup->setBodyText(bodyText);

    if (QLabel *label = qobject_cast<QLabel *>(popup->centralWidget()))
    {
        label->setOpenExternalLinks(true);
    }

    if (QdfOfficeStyle *officeStyle = qobject_cast<QdfOfficeStyle *>(popup->style()))
    {
        officeStyle->setPopupDecoration(decoration);
    }

    popup->setDragDropEnabled(true);
    popup->setTransparency(200.0 / 255.0);
    popup->setDisplayTime(7500);
    popup->setAnimationSpeed(250);
    popup->setAnimation(PopupAnimation::PA_Fade);

    popup->setPosition(popup->getPosition());
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->showPopup();
}

void QdfOfficePopupWindow::setCentralWidget(QWidget *widget)
{
    QDF_D(QdfOfficePopupWindow);
    if (!widget)
    {
        return;
    }

    if (d->m_form)
    {
        delete d->m_form;
        d->m_form = nullptr;
    }

    d->m_form = widget;
    d->m_form->setAttribute(Qt::WA_MouseTracking);
    d->m_form->setParent(this);
}

QWidget *QdfOfficePopupWindow::centralWidget() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_form;
}

void QdfOfficePopupWindow::setBodyText(const QString &text)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_bodyTxtRef = text;
    if (QLabel *label = qobject_cast<QLabel *>(d->m_form))
    {
        label->setText(text);
    }
}

const QString &QdfOfficePopupWindow::bodyText() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_bodyTxtRef;
}

void QdfOfficePopupWindow::setTitleIcon(const QIcon &icon)
{
    QDF_D(QdfOfficePopupWindow);
    if (!icon.isNull())
    {
        d->m_title->labelIcon->setVisible(true);
    }
    else
    {
        d->m_title->labelIcon->setVisible(false);
    }
    d->m_titleIcon = icon;
}


const QIcon &QdfOfficePopupWindow::titleIcon() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_titleIcon;
}

void QdfOfficePopupWindow::setTitleText(const QString &text)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_titleTxtRef = text;
    d->m_title->labelTitle->setText(text);
    d->m_title->labelTitle->setVisible(!text.isEmpty());
}

const QString &QdfOfficePopupWindow::titleText() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_titleTxtRef;
}

void QdfOfficePopupWindow::setCloseButtonPixmap(const QPixmap &pixmap)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_closePixmap = pixmap;
}

const QPixmap &QdfOfficePopupWindow::closeButtonPixmap() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_closePixmap;
}

void QdfOfficePopupWindow::setTitleCloseButtonVisible(bool visible)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_closeButton = visible;
}

bool QdfOfficePopupWindow::isTitleCloseButtonVisible() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_closeButton;
}

bool QdfOfficePopupWindow::showPopup()
{
    QPoint pt = QdfManagerPopup::getMngPopup().findBestPosition(this);
    return showPopup(pt);
}

bool QdfOfficePopupWindow::showPopup(const QPoint &pos)
{
    QDF_D(QdfOfficePopupWindow);

    if (QdfOfficeStyle *officeStyle = qobject_cast<QdfOfficeStyle *>(style()))
    {
        officeStyle->createPopupProxy();
    }

    if (d->m_popupState != PopupStateClosed)
    {
        return false;
    }

    setPosition(pos);

    emit aboutToShow();

    d->calclayout();
    d->onExpanding(true);
    setVisible(true);
    return true;
}

QPoint QdfOfficePopupWindow::getPosition() const
{
    QDF_D(const QdfOfficePopupWindow);
    if (d->m_positionPopup != QPoint(-1, -1))
    {
        return d->m_positionPopup;
    }

    QSize sz = size();

#ifdef Q_OS_WIN
    if (d->m_popupLocation == PopupLocationNearTaskBar)
    {
        APPBARDATA abd;
        ZeroMemory(&abd, sizeof(APPBARDATA));
        abd.cbSize = sizeof(APPBARDATA);
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd))
        {
            RECT rcWork;
            if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, static_cast<void *>(&rcWork), 0))
            {
                return QPoint(0, 0);
            }

            QRect rc(QPoint(rcWork.left, rcWork.top), QPoint(rcWork.right, rcWork.bottom));

            QRect taskRc = QRect(QPoint(abd.rc.left, abd.rc.top), QPoint(abd.rc.right, abd.rc.bottom));

            if (rc.center().y() < taskRc.top())
            {
                return QPoint(rc.right(), rc.bottom());
            }

            if (rc.center().x() > taskRc.right())
            {
                return QPoint(rc.left() + sz.width(), rc.bottom());
            }

            if (rc.center().y() > taskRc.bottom())
            {
                return QPoint(rc.right(), rc.top() + sz.height());
            }

            if (rc.center().x() < taskRc.left())
            {
                return QPoint(rc.right(), rc.bottom());
            }
        }
    }
#endif// Q_OS_WIN

    QRect screen = QApplication::desktop()->screenGeometry();

    if (d->m_popupLocation == PopupLocationCenter)
    {
        return QPoint(screen.center().x() + sz.width() / 2, screen.center().y() + sz.height() / 2);
    }
    return screen.bottomRight();
}

void QdfOfficePopupWindow::setPosition(const QPoint &pos)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_positionPopup = pos;
}

void QdfOfficePopupWindow::setAnimation(PopupAnimation popupAnimation)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_popupAnimation = popupAnimation;
}

PopupAnimation QdfOfficePopupWindow::animation() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_popupAnimation;
}

void QdfOfficePopupWindow::setTransparency(qreal transparency)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_transparency = (int) (transparency * 255.0);
}

qreal QdfOfficePopupWindow::transparency() const
{
    QDF_D(const QdfOfficePopupWindow);
    return (qreal) d->m_transparency / 255.0;
}

void QdfOfficePopupWindow::setDisplayTime(int time)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_notCloseTimer = time < 0;
    d->m_showDelay = time;
}

int QdfOfficePopupWindow::displayTime() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_showDelay;
}

void QdfOfficePopupWindow::setAnimationSpeed(int time)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_animationSpeed = time;
}

int QdfOfficePopupWindow::animationSpeed() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_animationSpeed;
}

void QdfOfficePopupWindow::setDragDropEnabled(bool enabled)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_allowMove = enabled;
}

bool QdfOfficePopupWindow::dragDropEnabled() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_allowMove;
}

void QdfOfficePopupWindow::setLocation(PopupLocation location)
{
    QDF_D(QdfOfficePopupWindow);
    d->m_popupLocation = location;
}

PopupLocation QdfOfficePopupWindow::location() const
{
    QDF_D(const QdfOfficePopupWindow);
    return d->m_popupLocation;
}

void QdfOfficePopupWindow::showDelayTimer()
{
    QDF_D(QdfOfficePopupWindow);
    if (!d->m_capture)
    {
        d->m_showDelayTimer.stop();
        d->onCollapsing();
    }
}

void QdfOfficePopupWindow::collapsingTimer()
{
    QDF_D(QdfOfficePopupWindow);
    d->animate(d->m_step);
    d->m_step--;

    if (d->m_step <= 0)
    {
        d->m_collapsingTimer.stop();
        if (!d->m_notCloseTimer)
        {
            d->closePopup();
        }
    }
}

void QdfOfficePopupWindow::expandingTimer()
{
    QDF_D(QdfOfficePopupWindow);
    d->animate(d->m_step);
    d->m_step--;

    if (d->m_step <= 0)
    {
        d->m_expandingTimer.stop();
        d->showPopup();
    }
}

QSize QdfOfficePopupWindow::sizeHint() const
{
    QDF_D(const QdfOfficePopupWindow);
    QStyleOptionFrame option;
    d->initFormStyleOption(&option);
    return option.rect.size().expandedTo(QApplication::globalStrut());
}

void QdfOfficePopupWindow::closePopup()
{
    QDF_D(QdfOfficePopupWindow);
    d->closePopup();
}

static void setChildStyle(QWidget *widget, QStyle *style)
{
    widget->setStyle(style);
    QList<QWidget *> widgets = widget->findChildren<QWidget *>();
    foreach (QWidget *w, widgets)
    {
        ::setChildStyle(w, style);
    }
}

bool QdfOfficePopupWindow::event(QEvent *event)
{
    QDF_D(QdfOfficePopupWindow);
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
            {
                d->handleMousePressEvent((QMouseEvent *) event);
                break;
            }
        case QEvent::MouseButtonRelease:
            {
                d->handleMouseReleaseEvent((QMouseEvent *) event);
                break;
            }
        case QEvent::MouseMove:
            {
                d->handleMouseMoveEvent((QMouseEvent *) event);
                break;
            }
        case QEvent::Show:
            QdfManagerPopup::getMngPopup().appendPopup(this);
            break;
        case QEvent::Hide:
            {
                emit aboutToHide();

                if (d->m_popupState == PopupStateShow)
                {
                    d->m_showDelayTimer.stop();
                    d->onCollapsing();
                }
                QdfManagerPopup::getMngPopup().removePopup(this);
            }
            break;
        case QEvent::StyleChange:
            {
                QdfPopupDrawHelper *popupStyle = qobject_cast<QdfPopupDrawHelper *>(style());
                if (popupStyle)
                {
                    popupStyle->refreshPalette();
                }

                if (qobject_cast<PopupOffice2003DrawHelper *>(style()) ||
                    qobject_cast<PopupOffice2007DrawHelper *>(style()))
                {
                    d->m_titlePalette = palette();
                }

                if (d->m_form)
                {
                    ::setChildStyle(d->m_form, style());
                }
            }
            break;
        default:
            break;
    }
    return QWidget::event(event);
}

void QdfOfficePopupWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfOfficePopupWindow);
    QPainter p(this);

    QStyleOptionFrame opt;
    d->initFormStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_FrameWindow, &opt, &p, this);

    QdfStyleOptionPopupTitleBar optTitle;
    d->initTitleBarStyleOption(&optTitle);
    style()->drawComplexControl(QStyle::CC_TitleBar, &optTitle, &p, this);
}

void QdfOfficePopupWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfOfficePopupWindow);
    QMouseEvent ev(QEvent::MouseMove, QPoint(1, 1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    d->handleMouseMoveEvent(&ev);
}

void QdfOfficePopupWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfOfficePopupWindow);
    QMouseEvent ev(QEvent::MouseMove, QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    d->handleMouseMoveEvent(&ev);
}
