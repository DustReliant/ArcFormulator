#include "private/qdfautohidedockcontainer_p.h"
#include "qdfdockcomponentsfactory.h"
#include "qdfdockwidgethelper.h"
#include "qdfresizehandle.h"
#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QPainter>
#include <QPointer>
#include <QSplitter>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockcontainerwidget.h>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

static const int ResizeMargin = 30;

bool static isHorizontalArea(SideBarLocation Area)
{
    switch (Area)
    {
        case SideBarLocation::SL_Top:
        case SideBarLocation::SL_Bottom:
            return true;
        case SideBarLocation::SL_Left:
        case SideBarLocation::SL_Right:
            return false;
        default:
            return true;
    }
}

QdfAutoHideDockContainerPrivate::QdfAutoHideDockContainerPrivate()
{
    dockArea = nullptr;
    dockWidget = nullptr;
    sideTabBarArea = SideBarLocation::SL_None;
    pLayout = nullptr;
    resizeHandle = nullptr;
}

QdfAutoHideDockContainerPrivate::~QdfAutoHideDockContainerPrivate()
{
    if (sideTab)
    {
        delete sideTab;
    }
    sideTab = nullptr;
}

DockWidgetArea QdfAutoHideDockContainerPrivate::getDockWidgetArea(SideBarLocation area)
{
    switch (area)
    {
        case SideBarLocation::SL_Left:
            return LeftDockWidgetArea;
        case SideBarLocation::SL_Right:
            return RightDockWidgetArea;
        case SideBarLocation::SL_Bottom:
            return BottomDockWidgetArea;
        case SideBarLocation::SL_Top:
            return TopDockWidgetArea;
        default:
            return LeftDockWidgetArea;
    }
}

void QdfAutoHideDockContainerPrivate::updateResizeHandleSizeLimitMax()
{
    QDF_Q(QdfAutoHideDockContainer);
    auto Rect = q->dockContainer()->contentRect();
    const auto maxResizeHandleSize =
            resizeHandle->orientation() == Qt::Horizontal ? Rect.width() : Rect.height();
    resizeHandle->setMaxResizeSize(maxResizeHandleSize - ResizeMargin);
}
bool QdfAutoHideDockContainerPrivate::isHorizontal() const
{
    return isHorizontalArea(sideTabBarArea);
}

void QdfAutoHideDockContainerPrivate::forwardEventToDockContainer(QEvent *event)
{
    QDF_Q(QdfAutoHideDockContainer);
    auto widget = q->dockContainer();
    if (widget)
    {
        widget->handleAutoHideWidgetEvent(event, q);
    }
}

Qt::Edge static edgeFromSideTabBarArea(SideBarLocation Area)
{
    switch (Area)
    {
        case SideBarLocation::SL_Top:
            return Qt::BottomEdge;
        case SideBarLocation::SL_Bottom:
            return Qt::TopEdge;
        case SideBarLocation::SL_Left:
            return Qt::RightEdge;
        case SideBarLocation::SL_Right:
            return Qt::LeftEdge;
        default:
            return Qt::LeftEdge;
    }
}

int resizeHandleLayoutPosition(SideBarLocation Area)
{
    switch (Area)
    {
        case SideBarLocation::SL_Bottom:
        case SideBarLocation::SL_Right:
            return 0;

        case SideBarLocation::SL_Top:
        case SideBarLocation::SL_Left:
            return 1;

        default:
            return 0;
    }
}

QdfDockContainerWidget *QdfAutoHideDockContainer::dockContainer() const
{
    return internal::findParent<QdfDockContainerWidget *>(this);
}

QdfAutoHideDockContainer::QdfAutoHideDockContainer(QdfDockWidget *DockWidget, SideBarLocation area,
                                                   QdfDockContainerWidget *parent)
    : QPushButton(parent)
{
    QDF_INIT_PRIVATE(QdfAutoHideDockContainer)
    hide();
    QDF_D(QdfAutoHideDockContainer);
    d->sideTabBarArea = area;
    d->sideTab = QdfDockComponentsFactory::instance()->createDockWidgetSideTab(nullptr);
    connect(d->sideTab, &QdfAutoHideTab::pressed, this, &QdfAutoHideDockContainer::toggleCollapseState);
    d->dockArea = new QdfDockAreaWidget(DockWidget->dockManager(), parent);
    d->dockArea->setObjectName("autoHideDockArea");
    d->dockArea->setAutoHideDockContainer(this);

    setObjectName("autoHideDockContainer");

    d->pLayout =
            new QBoxLayout(isHorizontalArea(area) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->pLayout->setContentsMargins(0, 0, 0, 0);
    d->pLayout->setSpacing(0);
    setLayout(d->pLayout);
    d->resizeHandle = new QdfResizeHandle(edgeFromSideTabBarArea(area), this);
    d->resizeHandle->setMinResizeSize(64);
    bool OpaqueResize = QdfDockManager::testConfigFlag(QdfDockManager::OpaqueSplitterResize);
    d->resizeHandle->setOpaqueResize(OpaqueResize);
    d->size = d->dockArea->size();

    addDockWidget(DockWidget);
    parent->registerAutoHideWidget(this);
    d->pLayout->addWidget(d->dockArea);
    d->pLayout->insertWidget(resizeHandleLayoutPosition(area), d->resizeHandle);
}

void QdfAutoHideDockContainer::updateSize()
{
    QDF_D(QdfAutoHideDockContainer)
    auto dockContainerParent = dockContainer();
    if (!dockContainerParent)
    {
        return;
    }

    auto rect = dockContainerParent->contentRect();

    switch (sideBarLocation())
    {
        case SideBarLocation::SL_Top:
            resize(rect.width(), qMin(rect.height() - ResizeMargin, d->size.height()));
            move(rect.topLeft());
            break;

        case SideBarLocation::SL_Left:
            resize(qMin(d->size.width(), rect.width() - ResizeMargin), rect.height());
            move(rect.topLeft());
            break;

        case SideBarLocation::SL_Right:
            {
                resize(qMin(d->size.width(), rect.width() - ResizeMargin), rect.height());
                QPoint p = rect.topRight();
                p.rx() -= (width() - 1);
                move(p);
            }
            break;

        case SideBarLocation::SL_Bottom:
            {
                resize(rect.width(), qMin(rect.height() - ResizeMargin, d->size.height()));
                QPoint p = rect.bottomLeft();
                p.ry() -= (height() - 1);
                move(p);
            }
            break;

        default:
            break;
    }
}

QdfAutoHideDockContainer::~QdfAutoHideDockContainer()
{
    QDF_D(QdfAutoHideDockContainer)
    qApp->removeEventFilter(this);
    if (dockContainer())
    {
        dockContainer()->removeAutoHideWidget(this);
    }

    QDF_FINI_PRIVATE()
}

QdfAutoHideSideBar *QdfAutoHideDockContainer::sideBar() const
{
    QDF_D(const QdfAutoHideDockContainer)
    if (d->sideTab)
    {
        return d->sideTab->sideBar();
    }
    else
    {
        auto DockContainer = dockContainer();
        return DockContainer ? DockContainer->sideTabBar(d->sideTabBarArea) : nullptr;
    }
}

QdfAutoHideTab *QdfAutoHideDockContainer::autoHideTab() const
{
    QDF_D(const QdfAutoHideDockContainer)
    return d->sideTab;
}

QdfDockWidget *QdfAutoHideDockContainer::dockWidget() const
{
    QDF_D(const QdfAutoHideDockContainer)
    return d->dockWidget;
}

void QdfAutoHideDockContainer::addDockWidget(QdfDockWidget *DockWidget)
{
    QDF_D(QdfAutoHideDockContainer)
    if (d->dockWidget)
    {
        d->dockArea->removeDockWidget(d->dockWidget);
    }

    d->dockWidget = DockWidget;
    d->sideTab->setDockWidget(DockWidget);
    QdfDockAreaWidget *OldDockArea = DockWidget->dockAreaWidget();
    auto IsRestoringState = DockWidget->dockManager()->isRestoringState();
    if (OldDockArea && !IsRestoringState)
    {
        d->size = OldDockArea->size() + QSize(16, 16);
        OldDockArea->removeDockWidget(DockWidget);
    }
    d->dockArea->addDockWidget(DockWidget);
    updateSize();
}

SideBarLocation QdfAutoHideDockContainer::sideBarLocation() const
{
    QDF_D(const QdfAutoHideDockContainer)
    return d->sideTabBarArea;
}

void QdfAutoHideDockContainer::setSideBarLocation(SideBarLocation sideBarLocation)
{
    QDF_D(QdfAutoHideDockContainer)
    if (d->sideTabBarArea == sideBarLocation)
    {
        return;
    }

    d->sideTabBarArea = sideBarLocation;
    d->pLayout->removeWidget(d->resizeHandle);
    d->pLayout->setDirection(isHorizontalArea(sideBarLocation) ? QBoxLayout::TopToBottom
                                                               : QBoxLayout::LeftToRight);
    d->pLayout->insertWidget(resizeHandleLayoutPosition(sideBarLocation), d->resizeHandle);
    d->resizeHandle->setHandlePosition(edgeFromSideTabBarArea(sideBarLocation));
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

QdfDockAreaWidget *QdfAutoHideDockContainer::dockAreaWidget() const
{
    QDF_D(const QdfAutoHideDockContainer)
    return d->dockArea;
}

void QdfAutoHideDockContainer::moveContentsToParent()
{
    QDF_D(QdfAutoHideDockContainer)
    cleanupAndDelete();
    d->dockWidget->setDockArea(nullptr);
    auto DockContainer = dockContainer();
    DockContainer->addDockWidget(d->getDockWidgetArea(d->sideTabBarArea), d->dockWidget);
}

void QdfAutoHideDockContainer::cleanupAndDelete()
{
    QDF_D(QdfAutoHideDockContainer)
    const auto dockWidget = d->dockWidget;
    if (dockWidget)
    {

        auto SideTab = d->sideTab;
        SideTab->removeFromSideBar();
        SideTab->setParent(nullptr);
        SideTab->hide();
    }

    hide();
    deleteLater();
}

void QdfAutoHideDockContainer::toggleView(bool enable)
{
    QDF_D(QdfAutoHideDockContainer)
    if (enable)
    {
        if (d->sideTab)
        {
            d->sideTab->show();
        }
    }
    else
    {
        if (d->sideTab)
        {
            d->sideTab->hide();
        }
        hide();
        qApp->removeEventFilter(this);
    }
}

void QdfAutoHideDockContainer::collapseView(bool enable)
{
    QDF_D(QdfAutoHideDockContainer)
    if (enable)
    {
        hide();
        qApp->removeEventFilter(this);
    }
    else
    {
        updateSize();
        d->updateResizeHandleSizeLimitMax();
        raise();
        show();
        d->dockWidget->dockManager()->setDockWidgetFocused(d->dockWidget);
        qApp->installEventFilter(this);
    }
    d->sideTab->updateStyle();
}

void QdfAutoHideDockContainer::toggleCollapseState()
{
    collapseView(isVisible());
}

void QdfAutoHideDockContainer::setSize(int size)
{
    QDF_D(QdfAutoHideDockContainer)
    if (d->isHorizontal())
    {
        d->size.setHeight(size);
    }
    else
    {
        d->size.setWidth(size);
    }

    updateSize();
}

static bool objectIsAncestorOf(const QObject *descendant, const QObject *ancestor)
{
    if (!ancestor)
    {
        return false;
    }
    while (descendant)
    {
        if (descendant == ancestor)
        {
            return true;
        }
        descendant = descendant->parent();
    }
    return false;
}

static bool isObjectOrAncestor(const QObject *descendant, const QObject *ancestor)
{
    if (ancestor && (descendant == ancestor))
    {
        return true;
    }
    else
    {
        return objectIsAncestorOf(descendant, ancestor);
    }
}

bool QdfAutoHideDockContainer::eventFilter(QObject *watched, QEvent *event)
{
    QDF_D(QdfAutoHideDockContainer)
    if (event->type() == QEvent::Resize)
    {
        if (!d->resizeHandle->isResizing())
        {
            updateSize();
        }
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        auto widget = qobject_cast<QWidget *>(watched);
        if (!widget)
        {
            return QPushButton::eventFilter(watched, event);
        }
        if (widget == d->sideTab.data())
        {
            return QPushButton::eventFilter(watched, event);
        }
        if (isObjectOrAncestor(widget, this))
        {
            return QPushButton::eventFilter(watched, event);
        }
        if (!isObjectOrAncestor(widget, dockContainer()))
        {
            return QPushButton::eventFilter(watched, event);
        }

        collapseView(true);
    }
    else if (event->type() == internal::FloatingWidgetDragStartEvent)
    {
        auto FloatingWidget = dockContainer()->floatingWidget();
        if (FloatingWidget != watched)
        {
            collapseView(true);
        }
    }
    else if (event->type() == internal::DockedWidgetDragStartEvent)
    {
        collapseView(true);
    }

    return QPushButton::eventFilter(watched, event);
}

void QdfAutoHideDockContainer::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfAutoHideDockContainer)
    QPushButton::resizeEvent(event);
    if (d->resizeHandle->isResizing())
    {
        d->size = this->size();
        d->updateResizeHandleSizeLimitMax();
    }
}

void QdfAutoHideDockContainer::leaveEvent(QEvent *event)
{
    QDF_D(QdfAutoHideDockContainer)
    auto pos = mapFromGlobal(QCursor::pos());
    if (!rect().contains(pos))
    {
        d->forwardEventToDockContainer(event);
    }
    QPushButton::leaveEvent(event);
}

bool QdfAutoHideDockContainer::event(QEvent *event)
{
    QDF_D(QdfAutoHideDockContainer)
    switch (event->type())
    {
        case QEvent::Enter:
        case QEvent::Hide:
            d->forwardEventToDockContainer(event);
            break;

        case QEvent::MouseButtonPress:
            return true;
            break;

        default:
            break;
    }

    return QPushButton::event(event);
}