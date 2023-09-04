#include "private/qdfautohidesidebar_p.h"
#include <QBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QXmlStreamWriter>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfautohidesidebar.h>
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockcontainerwidget.h>
#include <dockwidget/qdfdockfocuscontroller.h>
#include <dockwidget/qdfdockmanager.h>

QDF_USE_NAMESPACE

QdfAutoHideSideBarPrivate::QdfAutoHideSideBarPrivate()
{
}

void QdfAutoHideSideBarPrivate::handleViewportEvent(QEvent *event)
{
    QDF_Q(QdfAutoHideSideBar)
    switch (event->type())
    {
        case QEvent::ChildRemoved:
            if (tabsLayout->isEmpty())
            {
                q->hide();
            }
            break;

        case QEvent::Resize:
            if (q->tabCount())
            {
                auto ev = static_cast<QResizeEvent *>(event);
                auto Tab = q->tabAt(0);
                int Size = isHorizontal() ? ev->size().height() : ev->size().width();
                int TabSize = isHorizontal() ? Tab->size().height() : Tab->size().width();
                if (Size < TabSize)
                {
                    q->hide();
                }
            }
            else
            {
                q->hide();
            }
            break;

        default:
            break;
    }
}

QdfAutoHideSideBar::QdfAutoHideSideBar(QdfDockContainerWidget *parent, SideBarLocation area)
    : QScrollArea(parent)
{
    QDF_INIT_PRIVATE(QdfAutoHideSideBar)
    QDF_D(QdfAutoHideSideBar)
    d->sideTabArea = area;
    d->containerWidget = parent;
    d->orientation = (area == SideBarLocation::SL_Bottom || area == SideBarLocation::SL_Top)
                             ? Qt::Horizontal
                             : Qt::Vertical;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d->tabsContainerWidget = new QdfTabsWidget();
    d->tabsContainerWidget->eventHandler = d;
    d->tabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->tabsContainerWidget->setObjectName("sideTabsContainerWidget");

    d->tabsLayout = new QBoxLayout(d->orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->tabsLayout->setContentsMargins(0, 0, 0, 0);
    d->tabsLayout->setSpacing(12);
    d->tabsLayout->addStretch(1);
    d->tabsContainerWidget->setLayout(d->tabsLayout);
    setWidget(d->tabsContainerWidget);

    setFocusPolicy(Qt::NoFocus);
    if (d->isHorizontal())
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    else
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    hide();
}

QdfAutoHideSideBar::~QdfAutoHideSideBar()
{
    auto Tabs = findChildren<QdfAutoHideTab *>(QString(), Qt::FindDirectChildrenOnly);
    for (auto Tab: Tabs)
    {
        Tab->setParent(nullptr);
    }
    QDF_FINI_PRIVATE()
}

void QdfAutoHideSideBar::insertTab(int index, QdfAutoHideTab *sideTab)
{
    QDF_D(QdfAutoHideSideBar)
    sideTab->setSideBar(this);
    sideTab->installEventFilter(this);
    if (index < 0)
    {
        d->tabsLayout->insertWidget(d->tabsLayout->count() - 1, sideTab);
    }
    else
    {
        d->tabsLayout->insertWidget(index, sideTab);
    }
    show();
}

QdfAutoHideDockContainer *QdfAutoHideSideBar::insertDockWidget(int index, QdfDockWidget *dockWidget)
{
    QDF_D(QdfAutoHideSideBar)
    auto autoHideContainer = new QdfAutoHideDockContainer(dockWidget, d->sideTabArea, d->containerWidget);
    dockWidget->dockManager()->dockFocusController()->clearDockWidgetFocus(dockWidget);
    auto tab = autoHideContainer->autoHideTab();
    dockWidget->setSideTabWidget(tab);
    insertTab(index, tab);
    return autoHideContainer;
}

void QdfAutoHideSideBar::removeAutoHideWidget(QdfAutoHideDockContainer *autoHideWidget)
{
    autoHideWidget->autoHideTab()->removeFromSideBar();
    auto DockContainer = autoHideWidget->dockContainer();
    if (DockContainer)
    {
        DockContainer->removeAutoHideWidget(autoHideWidget);
    }
    autoHideWidget->setParent(nullptr);
}

void QdfAutoHideSideBar::addAutoHideWidget(QdfAutoHideDockContainer *autoHideWidget)
{
    QDF_D(QdfAutoHideSideBar)
    auto sideBar = autoHideWidget->autoHideTab()->sideBar();
    if (sideBar == this)
    {
        return;
    }

    if (sideBar)
    {
        sideBar->removeAutoHideWidget(autoHideWidget);
    }
    autoHideWidget->setParent(d->containerWidget);
    autoHideWidget->setSideBarLocation(d->sideTabArea);
    d->containerWidget->registerAutoHideWidget(autoHideWidget);
    insertTab(-1, autoHideWidget->autoHideTab());
}

void QdfAutoHideSideBar::removeTab(QdfAutoHideTab *sideTab)
{
    QDF_D(QdfAutoHideSideBar)
    sideTab->removeEventFilter(this);
    d->tabsLayout->removeWidget(sideTab);
    if (d->tabsLayout->isEmpty())
    {
        hide();
    }
}

bool QdfAutoHideSideBar::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::ShowToParent)
    {
        return false;
    }

    auto Tab = qobject_cast<QdfAutoHideTab *>(watched);
    if (Tab)
    {
        show();
    }
    return false;
}

Qt::Orientation QdfAutoHideSideBar::orientation() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->orientation;
}

QdfAutoHideTab *QdfAutoHideSideBar::tabAt(int index) const
{
    QDF_D(const QdfAutoHideSideBar)
    return qobject_cast<QdfAutoHideTab *>(d->tabsLayout->itemAt(index)->widget());
}

int QdfAutoHideSideBar::tabCount() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->tabsLayout->count() - 1;
}

SideBarLocation QdfAutoHideSideBar::sideBarLocation() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->sideTabArea;
}

QSize QdfAutoHideSideBar::minimumSizeHint() const
{
    QSize Size = sizeHint();
    Size.setWidth(10);
    return Size;
}

QSize QdfAutoHideSideBar::sizeHint() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->tabsContainerWidget->sizeHint();
}

int QdfAutoHideSideBar::spacing() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->tabsLayout->spacing();
}

void QdfAutoHideSideBar::setSpacing(int Spacing)
{
    QDF_D(QdfAutoHideSideBar)
    d->tabsLayout->setSpacing(Spacing);
}

QdfDockContainerWidget *QdfAutoHideSideBar::dockContainer() const
{
    QDF_D(const QdfAutoHideSideBar)
    return d->containerWidget;
}