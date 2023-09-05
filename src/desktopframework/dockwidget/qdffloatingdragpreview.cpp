#include "qdffloatingdragpreview.h"
#include "private/qdffloatingdragpreview_p.h"
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockcontainerwidget.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

void QdfFloatingDragPreviewPrivate::setHidden(bool value)
{
    QDF_Q(QdfFloatingDragPreview)
    hidden = value;
    q->update();
}

void QdfFloatingDragPreviewPrivate::cancelDragging()
{
    QDF_Q(QdfFloatingDragPreview)
    canceled = true;
    Q_EMIT q->draggingCanceled();
    dockManager->containerOverlay()->hideOverlay();
    dockManager->dockAreaOverlay()->hideOverlay();
    q->close();
}
bool QdfFloatingDragPreviewPrivate::isContentFloatable() const
{
    QdfDockWidget *widget = qobject_cast<QdfDockWidget *>(content);
    if (widget && widget->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        return true;
    }

    QdfDockAreaWidget *dockArea = qobject_cast<QdfDockAreaWidget *>(content);
    if (dockArea && dockArea->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        return true;
    }

    return false;
}

QdfFloatingDragPreviewPrivate::QdfFloatingDragPreviewPrivate()
{
}

void QdfFloatingDragPreviewPrivate::updateDropOverlays(const QPoint &globalPos)
{
    QDF_Q(QdfFloatingDragPreview)
    if (!q->isVisible() || !dockManager)
    {
        return;
    }

    auto Containers = dockManager->dockContainers();
    QdfDockContainerWidget *topContainer = nullptr;
    for (auto ContainerWidget: Containers)
    {
        if (!ContainerWidget->isVisible())
        {
            continue;
        }

        QPoint MappedPos = ContainerWidget->mapFromGlobal(globalPos);
        if (ContainerWidget->rect().contains(MappedPos))
        {
            if (!topContainer || ContainerWidget->isInFrontOf(topContainer))
            {
                topContainer = ContainerWidget;
            }
        }
    }

    dropContainer = topContainer;
    auto ContainerOverlay = dockManager->containerOverlay();
    auto DockAreaOverlay = dockManager->dockAreaOverlay();
    auto DockDropArea = DockAreaOverlay->dropAreaUnderCursor();
    auto ContainerDropArea = ContainerOverlay->dropAreaUnderCursor();

    if (!topContainer)
    {
        ContainerOverlay->hideOverlay();
        DockAreaOverlay->hideOverlay();
        if (QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewIsDynamic))
        {
            setHidden(false);
        }
        return;
    }

    int VisibleDockAreas = topContainer->visibleDockAreaCount();
    auto dockAreaWidget = qobject_cast<QdfDockAreaWidget *>(content);
    if (dockAreaWidget && dockAreaWidget->autoHide())
    {
        VisibleDockAreas++;
    }

    ContainerOverlay->setAllowedAreas(VisibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
    auto DockArea = topContainer->dockAreaAt(globalPos);
    if (DockArea && DockArea->isVisible() && VisibleDockAreas >= 0 && DockArea != contentSourceArea)
    {
        DockAreaOverlay->enableDropPreview(true);
        DockAreaOverlay->setAllowedAreas((VisibleDockAreas == 1) ? NoDockWidgetArea
                                                                 : DockArea->allowedAreas());

        DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);
        if ((Area == CenterDockWidgetArea) && (ContainerDropArea != InvalidDockWidgetArea))
        {
            DockAreaOverlay->enableDropPreview(false);
            ContainerOverlay->enableDropPreview(true);
        }
        else
        {
            ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
        }
        ContainerOverlay->showOverlay(topContainer);
    }
    else
    {
        DockAreaOverlay->hideOverlay();
        if (VisibleDockAreas == 1)
        {
            ContainerOverlay->hideOverlay();
        }
        else
        {
            ContainerOverlay->showOverlay(topContainer);
        }


        if (DockArea == contentSourceArea && InvalidDockWidgetArea == ContainerDropArea)
        {
            dropContainer = nullptr;
        }
    }

    if (QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewIsDynamic))
    {
        setHidden(DockDropArea != InvalidDockWidgetArea || ContainerDropArea != InvalidDockWidgetArea);
    }
}

void QdfFloatingDragPreviewPrivate::createFloatingWidget()
{
    QDF_Q(QdfFloatingDragPreview)
    QdfDockWidget *DockWidget = qobject_cast<QdfDockWidget *>(content);
    QdfDockAreaWidget *DockArea = qobject_cast<QdfDockAreaWidget *>(content);

    QdfFloatingDockContainer *FloatingWidget = nullptr;

    if (DockWidget && DockWidget->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        FloatingWidget = new QdfFloatingDockContainer(DockWidget);
    }
    else if (DockArea && DockArea->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        FloatingWidget = new QdfFloatingDockContainer(DockArea);
    }

    if (FloatingWidget)
    {
        FloatingWidget->setGeometry(q->geometry());
        FloatingWidget->show();
        if (!QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewHasWindowFrame))
        {
            QApplication::processEvents();
            int FrameHeight =
                    FloatingWidget->frameGeometry().height() - FloatingWidget->geometry().height();
            QRect FixedGeometry = q->geometry();
            FixedGeometry.adjust(0, FrameHeight, 0, 0);
            FloatingWidget->setGeometry(FixedGeometry);
        }
    }
}

QdfFloatingDragPreview::QdfFloatingDragPreview(QWidget *content, QWidget *parent) : QWidget(parent)
{
    QDF_INIT_PRIVATE(QdfFloatingDragPreview)
    QDF_D(QdfFloatingDragPreview)
    d->content = content;
    setAttribute(Qt::WA_DeleteOnClose);
    if (QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewHasWindowFrame))
    {
        setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    }
    else
    {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    auto GsFlags = windowFlags();
    GsFlags |= Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint;
    setWindowFlags(GsFlags);
#endif

    setWindowOpacity(0.6);

    if (QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewShowsContentPixmap))
    {
        d->contentPreviewPixmap = QPixmap(content->size());
        content->render(&d->contentPreviewPixmap);
    }

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            SLOT(onApplicationStateChanged(Qt::ApplicationState)));
    qApp->installEventFilter(this);
}

QdfFloatingDragPreview::QdfFloatingDragPreview(QdfDockWidget *content)
    : QdfFloatingDragPreview((QWidget *) content, content->dockManager())
{
    QDF_INIT_PRIVATE(QdfFloatingDragPreview)
    QDF_D(QdfFloatingDragPreview)
    d->dockManager = content->dockManager();
    if (content->dockAreaWidget()->openedDockWidgetsCount() == 1)
    {
        d->contentSourceArea = content->dockAreaWidget();
    }
    setWindowTitle(content->windowTitle());
}

QdfFloatingDragPreview::QdfFloatingDragPreview(QdfDockAreaWidget *content)
    : QdfFloatingDragPreview((QWidget *) content, content->dockManager())
{
    QDF_D(QdfFloatingDragPreview)
    d->dockManager = content->dockManager();
    d->contentSourceArea = content;
    setWindowTitle(content->currentDockWidget()->windowTitle());
}

QdfFloatingDragPreview::~QdfFloatingDragPreview()
{
    QDF_FINI_PRIVATE()
}

void QdfFloatingDragPreview::moveFloating()
{
    QDF_D(QdfFloatingDragPreview)
    int BorderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->dragStartMousePosition - QPoint(BorderSize, 0);
    move(moveToPos);
    d->updateDropOverlays(QCursor::pos());
}

void QdfFloatingDragPreview::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                           DragState dragState, QWidget *mouseEventHandler)
{
    QDF_D(QdfFloatingDragPreview)
    Q_UNUSED(mouseEventHandler)
    Q_UNUSED(dragState)
    resize(size);
    d->dragStartMousePosition = dragStartMousePos;
    moveFloating();
    show();
}

void QdfFloatingDragPreview::finishDragging()
{
    QDF_D(QdfFloatingDragPreview)
    auto DockDropArea = d->dockManager->dockAreaOverlay()->visibleDropAreaUnderCursor();
    auto ContainerDropArea = d->dockManager->containerOverlay()->visibleDropAreaUnderCursor();
    bool ValidDropArea =
            (DockDropArea != InvalidDockWidgetArea) || (ContainerDropArea != InvalidDockWidgetArea);
    if (ValidDropArea || d->isContentFloatable())
    {
        cleanupAutoHideContainerWidget();
    }

    if (!d->dropContainer)
    {
        d->createFloatingWidget();
    }
    else if (DockDropArea != InvalidDockWidgetArea)
    {
        d->dropContainer->dropWidget(d->content, DockDropArea,
                                     d->dropContainer->dockAreaAt(QCursor::pos()));
    }
    else if (ContainerDropArea != InvalidDockWidgetArea)
    {
        if (d->dropContainer->visibleDockAreaCount() <= 1 && CenterDockWidgetArea == ContainerDropArea)
        {
            d->dropContainer->dropWidget(d->content, ContainerDropArea,
                                         d->dropContainer->dockAreaAt(QCursor::pos()));
        }
        else
        {
            d->dropContainer->dropWidget(d->content, ContainerDropArea, nullptr);
        }
    }
    else
    {
        d->createFloatingWidget();
    }

    this->close();
    d->dockManager->containerOverlay()->hideOverlay();
    d->dockManager->dockAreaOverlay()->hideOverlay();
}

void QdfFloatingDragPreview::cleanupAutoHideContainerWidget()
{
    QDF_D(QdfFloatingDragPreview)
    auto droppedDockWidget = qobject_cast<QdfDockWidget *>(d->content);
    auto droppedArea = qobject_cast<QdfDockAreaWidget *>(d->content);
    if (droppedDockWidget && droppedDockWidget->autoHideDockContainer())
    {
        droppedDockWidget->autoHideDockContainer()->cleanupAndDelete();
    }
    if (droppedArea && droppedArea->autoHideDockContainer())
    {
        droppedArea->autoHideDockContainer()->cleanupAndDelete();
    }
}

void QdfFloatingDragPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QDF_D(QdfFloatingDragPreview)
    if (d->hidden)
    {
        return;
    }

    QPainter painter(this);
    if (QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewShowsContentPixmap))
    {
        painter.drawPixmap(QPoint(0, 0), d->contentPreviewPixmap);
    }

    if (!QdfDockManager::testConfigFlag(QdfDockManager::DragPreviewHasWindowFrame))
    {
        QColor color = palette().color(QPalette::Active, QPalette::Highlight);
        QPen Pen = painter.pen();
        Pen.setColor(color.darker(120));
        Pen.setStyle(Qt::SolidLine);
        Pen.setWidth(1);
        Pen.setCosmetic(true);
        painter.setPen(Pen);
        color = color.lighter(130);
        color.setAlpha(64);
        painter.setBrush(color);
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

void QdfFloatingDragPreview::onApplicationStateChanged(Qt::ApplicationState state)
{
    QDF_D(QdfFloatingDragPreview)
    if (state != Qt::ApplicationActive)
    {
        disconnect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
                   SLOT(onApplicationStateChanged(Qt::ApplicationState)));
        d->cancelDragging();
    }
}

bool QdfFloatingDragPreview::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    QDF_D(QdfFloatingDragPreview)
    if (!d->canceled && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        if (e->key() == Qt::Key_Escape)
        {
            watched->removeEventFilter(this);
            d->cancelDragging();
        }
    }

    return false;
}