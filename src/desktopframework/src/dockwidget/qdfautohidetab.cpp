#include "private/qdfautohidetab_p.h"
#include "qdfdockwidgethelper.h"
#include <QApplication>
#include <QBoxLayout>
#include <QElapsedTimer>
#include <QPaintEvent>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfautohidesidebar.h>
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockmanager.h>

QDF_USE_NAMESPACE

QdfAutoHideTabPrivate::QdfAutoHideTabPrivate()
{
}

void QdfAutoHideTabPrivate::updateOrientation()
{
    QDF_Q(QdfAutoHideTab)
    bool IconOnly = QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideSideBarsIconOnly);
    if (IconOnly && !q->icon().isNull())
    {
        q->setText("");
        q->setOrientation(Qt::Horizontal);
    }
    else
    {
        auto area = sideBar->sideBarLocation();
        q->setOrientation((area == SideBarLocation::SL_Bottom || area == SideBarLocation::SL_Top) ? Qt::Horizontal : Qt::Vertical);
    }
}

QdfDockContainerWidget *QdfAutoHideTabPrivate::dockContainer() const
{
    return dockWidget ? dockWidget->dockContainer() : nullptr;
}

void QdfAutoHideTabPrivate::forwardEventToDockContainer(QEvent *event)
{
    QDF_Q(QdfAutoHideTab)
    auto DockContainer = dockContainer();
    if (DockContainer)
    {
        DockContainer->handleAutoHideWidgetEvent(event, q);
    }
}

void QdfAutoHideTab::setSideBar(QdfAutoHideSideBar *sideTabBar)
{
    QDF_D(QdfAutoHideTab)
    d->sideBar = sideTabBar;
    if (d->sideBar)
    {
        d->updateOrientation();
    }
}

QdfAutoHideSideBar *QdfAutoHideTab::sideBar() const
{
    QDF_D(const QdfAutoHideTab)
    return d->sideBar;
}

QSize QdfAutoHideTab::sizeHint() const
{
    QDF_D(const QdfAutoHideTab)
    QSize sh = QPushButton::sizeHint();

    if (d->buttonOrientation != QdfAutoHideTab::Horizontal)
    {
        sh.transpose();
    }
    return sh;
}

QdfAutoHideTab::Orientation QdfAutoHideTab::buttonOrientation() const
{
    QDF_D(const QdfAutoHideTab)
    return d->buttonOrientation;
}

void QdfAutoHideTab::setButtonOrientation(Orientation orientation)
{
    QDF_D(QdfAutoHideTab)
    d->buttonOrientation = orientation;
    updateGeometry();
}

void QdfAutoHideTab::removeFromSideBar()
{
    QDF_D(QdfAutoHideTab)
    if (d->sideBar == nullptr)
    {
        return;
    }
    d->sideBar->removeTab(this);
    setSideBar(nullptr);
}

QdfAutoHideTab::QdfAutoHideTab(QWidget *parent)
    : QPushButton(parent)
{
    QDF_INIT_PRIVATE(QdfAutoHideTab)
    setAttribute(Qt::WA_NoMousePropagation);
    setFocusPolicy(Qt::NoFocus);
}

QdfAutoHideTab::~QdfAutoHideTab()
{
    QDF_FINI_PRIVATE()
}

void QdfAutoHideTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
    update();
}

SideBarLocation QdfAutoHideTab::sideBarLocation() const
{
    QDF_D(const QdfAutoHideTab)
    if (d->sideBar)
    {
        return d->sideBar->sideBarLocation();
    }

    return SideBarLocation::SL_Left;
}

void QdfAutoHideTab::setOrientation(Qt::Orientation Orientation)
{
    QDF_D(QdfAutoHideTab)
    d->orientation = Orientation;
    if (orientation() == Qt::Horizontal)
    {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    else
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    }
    setButtonOrientation((Qt::Horizontal == Orientation)
                                 ? QdfAutoHideTab::Horizontal
                                 : QdfAutoHideTab::VerticalTopToBottom);
    updateStyle();
}

Qt::Orientation QdfAutoHideTab::orientation() const
{
    QDF_D(const QdfAutoHideTab)
    return d->orientation;
}

bool QdfAutoHideTab::isActiveTab() const
{
    QDF_D(const QdfAutoHideTab)
    if (d->dockWidget && d->dockWidget->autoHideDockContainer())
    {
        return d->dockWidget->autoHideDockContainer()->isVisible();
    }

    return false;
}

QdfDockWidget *QdfAutoHideTab::dockWidget() const
{
    QDF_D(const QdfAutoHideTab)
    return d->dockWidget;
}

void QdfAutoHideTab::setDockWidget(QdfDockWidget *dockWidget)
{
    QDF_D(QdfAutoHideTab)
    if (!dockWidget)
    {
        return;
    }
    d->dockWidget = dockWidget;
    setText(dockWidget->windowTitle());
    setIcon(d->dockWidget->icon());
    setToolTip(dockWidget->windowTitle());
}

bool QdfAutoHideTab::event(QEvent *event)
{
    QDF_D(QdfAutoHideTab)
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideShowOnMouseOver))
    {
        return QPushButton::event(event);
    }

    switch (event->type())
    {
        case QEvent::Enter:
        case QEvent::Leave:
            d->forwardEventToDockContainer(event);
            break;

        case QEvent::MouseButtonPress:
            if (!event->spontaneous())
            {
                d->timeSinceHoverMousePress.restart();
                d->forwardEventToDockContainer(event);
            }
            else if (d->timeSinceHoverMousePress.hasExpired(500))
            {
                d->forwardEventToDockContainer(event);
            }
            break;

        default:
            break;
    }
    return QPushButton::event(event);
}

void QdfAutoHideTab::paintEvent(QPaintEvent *event)
{
    QDF_D(QdfAutoHideTab)
    QStylePainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    if (d->buttonOrientation == VerticalTopToBottom)
    {
        painter.rotate(90);
        painter.translate(0, -1 * width());
        option.rect = option.rect.transposed();
    }
    else if (d->buttonOrientation == VerticalBottomToTop)
    {
        painter.rotate(-90);
        painter.translate(-1 * height(), 0);
        option.rect = option.rect.transposed();
    }

    painter.drawControl(QStyle::CE_PushButton, option);
}

bool QdfAutoHideTab::iconOnly() const
{
    return QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideSideBarsIconOnly) && !icon().isNull();
}