
#include "qdfribbon_p.h"
#include "../qdfribbon_def.h"
#include "qdfribbonbar_p.h"
#include "qdfribbonbutton_p.h"
#include "qdfribbongroup_p.h"
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

QDF_BEGIN_NAMESPACE

static int keyTipEventNumber = -1;
static int showKeyTipEventNumber = -1;
static int hideKeyTipEventNumber = -1;
static int minimizedEventNumber = -1;

QdfRibbonGroupPopupButton::QdfRibbonGroupPopupButton(QWidget *parent, QdfRibbonGroup *group)
    : QToolButton(parent)
{
    m_ribbonGroup = group;
    m_eventLoop = nullptr;
    m_hasPopup = false;

    m_ribbonGroup->adjustSize();

    ensurePolished();

    setAttribute(Qt::WA_LayoutUsesWidgetRect);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setPopupMode(QToolButton::MenuButtonPopup);
    setText(m_ribbonGroup->title());

    m_popupWidget = new QdfRibbonGroupPopup(group);
    m_popupWidget->setWindowFlags(Qt::Popup);
}

QdfRibbonGroupPopup *QdfRibbonGroupPopupButton::popupWidget() { return m_popupWidget; }

QdfRibbonGroupPopupButton::~QdfRibbonGroupPopupButton()
{
    if (m_eventLoop)
    {
        m_eventLoop->exit();
    }
    delete m_popupWidget;
    m_popupWidget = nullptr;
}

bool QdfRibbonGroupPopupButton::isShowPopup() const { return m_hasPopup; }

void QdfRibbonGroupPopupButton::setVisible(bool visible) { QToolButton::setVisible(visible); }

void QdfRibbonGroupPopupButton::resetReducedGroup()
{
    Q_ASSERT(false);
    if (m_ribbonGroup->isReduced())
    {
        m_ribbonGroup->show();
    }
}

void QdfRibbonGroupPopupButton::resetPopopGroup()
{
    if (m_eventLoop)
    {
        m_eventLoop->exit();
    }
    // m_hasPopup = false;
    update();
}

QSize QdfRibbonGroupPopupButton::sizeHint() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    return QSize(style()->pixelMetric((QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonReducedGroupWidth, &opt, this),
                 50 /*m_ribbonGroup->sizeHint().height()*/);
}

void QdfRibbonGroupPopupButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (m_hasPopup)
    {
        opt.features |= QStyleOptionToolButton::HasMenu;
    }
    QRect rcGroup = opt.rect;
    rcGroup.setBottom(rcGroup.bottom() - 1);
    opt.rect = rcGroup;
    style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_ReducedGroup, &opt, &p, this);
}

void QdfRibbonGroupPopupButton::mousePressEvent(QMouseEvent *event)
{
    if (!m_ribbonGroup)
    {
        return;
    }

    if (m_ribbonGroup->isReduced() /*&& !m_hasPopup*/)
    {
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (event->button() == Qt::LeftButton && popupMode() == QToolButton::MenuButtonPopup)
        {
            QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
            if (popupr.isValid() && popupr.contains(event->pos()))
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

                int width = m_ribbonGroup->qdf_d()->updateControlsLayout(nullptr).width();
                QSize size = m_ribbonGroup->sizeHint();
                size.setWidth(width);
                const int desktopFrame = style()->pixelMetric(QStyle::PM_MenuDesktopFrameWidth, 0, this);

                if (pnt.x() + size.width() - 1 > screen.right() - desktopFrame)
                {
                    pnt.setX(screen.right() - desktopFrame - size.width() + 1);
                }
                if (pnt.x() < screen.left() + desktopFrame)
                {
                    pnt.setX(screen.left() + desktopFrame);
                }

                m_hasPopup = true;
                m_ribbonGroup->setGeometry(QRect(pnt, size));
                m_ribbonGroup->show();

                QObject::connect(m_ribbonGroup, SIGNAL(hidePopup()), this, SLOT(resetPopopGroup()));
                QEventLoop eventLoop;
                m_eventLoop = &eventLoop;
                QPointer<QWidget> guard = m_ribbonGroup;
                (void) eventLoop.exec();
                if (guard.isNull())
                {
                    return;
                }

                m_hasPopup = false;
                QObject::disconnect(m_ribbonGroup, SIGNAL(hidePopup()), this, SLOT(resetPopopGroup()));
                m_eventLoop = nullptr;
                return;
            }
        }
    }
}

void QdfRibbonGroupPopupButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_hasPopup = false;
    QToolButton::mouseReleaseEvent(event);
}

QdfRibbonGroupPopup::QdfRibbonGroupPopup(QdfRibbonGroup *group) : QMenu(), m_group(group)
{
    setFont(group->font());
    setProperty(_qdf_TitleGroupsVisible, true);
    setPalette(group->palette());
}

QdfRibbonGroupPopup::~QdfRibbonGroupPopup() {}

QSize QdfRibbonGroupPopup::sizeHint() const
{
    QSize size = m_group->qdf_d()->sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void QdfRibbonGroupPopup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect rectEvent = event->rect();
    p.setClipRect(rectEvent);

    QStyle *ribbonStyle = style();

    if (windowFlags() & Qt::Popup)
    {
        QdfStyleOptionRibbon opt;
        opt.init(this);
        opt.rect.adjust(-1, 0, 2, 4);
        style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonGroups, &opt, &p, this);
    }

    QStyleOptionGroupBox opt;
    m_group->qdf_d()->initStyleOption(opt);
    opt.rect.setWidth(width());
    opt.rect.adjust(0, 0, 1, 1);
    if (rectEvent.intersects(opt.rect))
    {
        ribbonStyle->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_Group, &opt, &p, this);
    }

    int flags = opt.textAlignment;
    flags |= Qt::TextHideMnemonic;

    QRect rcEntryCaption = opt.rect;
    rcEntryCaption.setTop(rcEntryCaption.bottom() - opt.lineWidth);

    int indent = 0;
    if (m_group->qdf_d()->m_optionButton->isVisible())
    {
        indent = m_group->qdf_d()->m_optionButton->width();
    }

    rcEntryCaption.adjust(0, -1, -indent, -2);
    opt.text =
            p.fontMetrics().elidedText(opt.text, m_group->titleElideMode(), rcEntryCaption.adjusted(2, 0, -2, 0).width());
    ribbonStyle->drawItemText(&p, rcEntryCaption, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                              QPalette::WindowText);
}

void QdfRibbonGroupPopup::mousePressEvent(QMouseEvent *event)
{
    if ((windowFlags() & Qt::Popup) && !rect().contains(event->pos()) && m_group->isVisible())
    {
        QRect rect = m_group->rect();
        rect.moveTopLeft(m_group->mapToGlobal(rect.topLeft()));
        if (rect.contains(event->globalPos()))
        {
            setAttribute(Qt::WA_NoMouseReplay);
        }
        hide();
        return;
    }

    QMenu::mousePressEvent(event);
}

/* QdfRibbonGroupOption */
QdfRibbonGroupOption::QdfRibbonGroupOption(QWidget *parent) : QToolButton(parent) {}

QdfRibbonGroupOption::~QdfRibbonGroupOption() {}

QString QdfRibbonGroupOption::text() const { return ""; }

QSize QdfRibbonGroupOption::sizeHint() const { return QSize(15, 14); }

void QdfRibbonGroupOption::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonOptionButton, &opt, &p, this);
}

void QdfRibbonGroupOption::actionEvent(QActionEvent *event)
{
    QToolButton::actionEvent(event);
    if (event->type() == QEvent::ActionChanged)
    {
        QAction *action = event->action();
        setPopupMode(action->menu() ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup);
    }
}

QdfRibbonGroupScroll::QdfRibbonGroupScroll(QWidget *parent, bool scrollLeft) : QToolButton(parent), m_scrollLeft(scrollLeft)
{
}

QdfRibbonGroupScroll::~QdfRibbonGroupScroll() {}

void QdfRibbonGroupScroll::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.arrowType = m_scrollLeft ? Qt::LeftArrow : Qt::RightArrow;
    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonGroupScrollButton, &opt, &p, this);
}

void QdfRibbonGroupScroll::leaveEvent(QEvent *event)
{
    QToolButton::leaveEvent(event);
    emit stopScrollTimer();
}


QdfKeyTipEvent::QdfKeyTipEvent(QdfRibbonKeyTip *kTip) : QEvent(QdfKeyTipEvent::eventNumber()) { keyTip = kTip; }

QdfRibbonKeyTip *QdfKeyTipEvent::getKeyTip() const { return keyTip; }

QEvent::Type QdfKeyTipEvent::eventNumber()
{
    if (keyTipEventNumber < 0)
    {
        keyTipEventNumber = QEvent::registerEventType();
    }
    return (QEvent::Type) keyTipEventNumber;
}

QdfShowKeyTipEvent::QdfShowKeyTipEvent(QWidget *w) : QEvent(QdfShowKeyTipEvent::eventNumber()) { m_buddy = w; }

QWidget *QdfShowKeyTipEvent::buddy() const { return m_buddy; }

QEvent::Type QdfShowKeyTipEvent::eventNumber()
{
    if (showKeyTipEventNumber < 0)
    {
        showKeyTipEventNumber = QEvent::registerEventType();
    }
    return (QEvent::Type) showKeyTipEventNumber;
}

QdfHideKeyTipEvent::QdfHideKeyTipEvent() : QEvent(QdfHideKeyTipEvent::eventNumber()) {}

QEvent::Type QdfHideKeyTipEvent::eventNumber()
{
    if (hideKeyTipEventNumber < 0)
    {
        hideKeyTipEventNumber = QEvent::registerEventType();
    }
    return (QEvent::Type) hideKeyTipEventNumber;
}


QdfMinimizedEvent::QdfMinimizedEvent() : QEvent(QdfMinimizedEvent::eventNumber()) {}

QEvent::Type QdfMinimizedEvent::eventNumber()
{
    if (minimizedEventNumber < 0)
    {
        minimizedEventNumber = QEvent::registerEventType();
    }
    return (QEvent::Type) minimizedEventNumber;
}

QDF_END_NAMESPACE