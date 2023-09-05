#include "private/qdfdockcontainerwidget_p.h"
#include "qdfdockwidgethelper.h"
#include <QAbstractButton>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QMetaObject>
#include <QMetaType>
#include <QPointer>
#include <QTimer>
#include <QVariant>
#include <QXmlStreamWriter>
#include <algorithm>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfautohidesidebar.h>
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockcontainerwidget.h>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

#if QT_VERSION < 0x050900

inline char toHexLower(uint value)
{
    return "0123456789abcdef"[value & 0xF];
}

QByteArray qByteArrayToHex(const QByteArray &src, char separator)
{
    if (src.size() == 0)
    {
        return QByteArray();
    }

    const int length = separator ? (src.size() * 3 - 1) : (src.size() * 2);
    QByteArray hex(length, Qt::Uninitialized);
    char *hexData = hex.data();
    const uchar *data = reinterpret_cast<const uchar *>(src.data());
    for (int i = 0, o = 0; i < src.size(); ++i)
    {
        hexData[o++] = toHexLower(data[i] >> 4);
        hexData[o++] = toHexLower(data[i] & 0xf);

        if ((separator) && (o < length))
        {
            hexData[o++] = separator;
        }
    }
    return hex;
}
#endif

static unsigned int zOrderCounter = 0;


static int areaIdToIndex(DockWidgetArea area)
{
    switch (area)
    {
        case LeftDockWidgetArea:
            return 0;
        case RightDockWidgetArea:
            return 1;
        case TopDockWidgetArea:
            return 2;
        case BottomDockWidgetArea:
            return 3;
        case CenterDockWidgetArea:
            return 4;
        default:
            return 4;
    }
}
static void insertWidgetIntoSplitter(QSplitter *Splitter, QWidget *widget, bool Append)
{
    if (Append)
    {
        Splitter->addWidget(widget);
    }
    else
    {
        Splitter->insertWidget(0, widget);
    }
}

QdfDockContainerWidgetPrivate::QdfDockContainerWidgetPrivate()
{
    std::fill(std::begin(lastAddedAreaCache), std::end(lastAddedAreaCache), nullptr);
    delayedAutoHideTimer.setSingleShot(true);
    delayedAutoHideTimer.setInterval(500);
    QObject::connect(&delayedAutoHideTimer, &QTimer::timeout, [this]() {
        auto globalPos = delayedAutoHideTab->mapToGlobal(QPoint(0, 0));
        qApp->sendEvent(delayedAutoHideTab,
                        new QMouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), globalPos,
                                        Qt::LeftButton, {Qt::LeftButton}, Qt::NoModifier));
    });
}

DropMode QdfDockContainerWidgetPrivate::getDropMode(const QPoint &targetPos)
{
    QDF_Q(QdfDockContainerWidget)
    QdfDockAreaWidget *dockArea = q->dockAreaAt(targetPos);
    auto dropArea = InvalidDockWidgetArea;
    auto ContainerDropArea = dockManager->containerOverlay()->dropAreaUnderCursor();

    if (dockArea)
    {
        auto dropOverlay = dockManager->dockAreaOverlay();
        dropOverlay->setAllowedAreas(dockArea->allowedAreas());
        dropArea = dropOverlay->showOverlay(dockArea);
        if (ContainerDropArea != InvalidDockWidgetArea && ContainerDropArea != dropArea)
        {
            dropArea = InvalidDockWidgetArea;
        }

        if (dropArea != InvalidDockWidgetArea)
        {
            return DropModeIntoArea;
        }
    }

    // mouse is over container
    if (InvalidDockWidgetArea == dropArea)
    {
        dropArea = ContainerDropArea;
        if (dropArea != InvalidDockWidgetArea)
        {
            return DropModeIntoContainer;
        }
    }

    return DropModeInvalid;
}

void QdfDockContainerWidgetPrivate::onVisibleDockAreaCountChanged()
{
    QDF_Q(QdfDockContainerWidget)
    auto topLevelDockArea = q->topLevelDockArea();

    if (topLevelDockArea)
    {
        this->topLevelDockArea = topLevelDockArea;
        topLevelDockArea->updateTitleBarButtonVisibility(true);
    }
    else if (this->topLevelDockArea)
    {
        this->topLevelDockArea->updateTitleBarButtonVisibility(false);
        this->topLevelDockArea = nullptr;
    }
}

void QdfDockContainerWidgetPrivate::emitDockAreasRemoved()
{
    QDF_Q(QdfDockContainerWidget)
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreasRemoved();
}

void QdfDockContainerWidgetPrivate::emitDockAreasAdded()
{
    QDF_Q(QdfDockContainerWidget)
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreasAdded();
}

QdfDockSplitter *QdfDockContainerWidgetPrivate::newSplitter(Qt::Orientation orientation, QWidget *parent)
{
    QdfDockSplitter *s = new QdfDockSplitter(orientation, parent);
    s->setOpaqueResize(QdfDockManager::testConfigFlag(QdfDockManager::OpaqueSplitterResize));
    s->setChildrenCollapsible(false);
    return s;
}

void QdfDockContainerWidgetPrivate::dropIntoContainer(QdfFloatingDockContainer *floatingWidget,
                                                      DockWidgetArea area)
{
    QDF_Q(QdfDockContainerWidget)
    auto insertParam = internal::dockAreaInsertParameters(area);
    QdfDockContainerWidget *FloatingDockContainer = floatingWidget->dockContainer();
    auto newDockAreas = FloatingDockContainer->findChildren<QdfDockAreaWidget *>(
            QString(), Qt::FindChildrenRecursively);
    QSplitter *splitter = rootSplitter;

    if (dockAreas.count() <= 1)
    {
        splitter->setOrientation(insertParam.orientation());
    }
    else if (splitter->orientation() != insertParam.orientation())
    {
        QSplitter *NewSplitter = newSplitter(insertParam.orientation());
        QLayoutItem *li = pLayout->replaceWidget(splitter, NewSplitter);
        NewSplitter->addWidget(splitter);
        updateSplitterHandles(NewSplitter);
        splitter = NewSplitter;
        delete li;
    }

    // Now we can insert the floating widget content into this container
    auto FloatingSplitter = FloatingDockContainer->rootSplitter();
    if (FloatingSplitter->count() == 1)
    {
        insertWidgetIntoSplitter(splitter, FloatingSplitter->widget(0), insertParam.append());
        updateSplitterHandles(splitter);
    }
    else if (FloatingSplitter->orientation() == insertParam.orientation())
    {
        int InsertIndex = insertParam.append() ? splitter->count() : 0;
        while (FloatingSplitter->count())
        {
            splitter->insertWidget(InsertIndex++, FloatingSplitter->widget(0));
            updateSplitterHandles(splitter);
        }
    }
    else
    {
        insertWidgetIntoSplitter(splitter, FloatingSplitter, insertParam.append());
    }

    rootSplitter = splitter;
    addDockAreasToList(newDockAreas);

    if (!splitter->isVisible())
    {
        splitter->show();
    }
    q->dumpLayout();
}

void QdfDockContainerWidgetPrivate::dropIntoCenterOfSection(QdfFloatingDockContainer *floatingWidget,
                                                            QdfDockAreaWidget *targetArea)
{
    QdfDockContainerWidget *floatingContainer = floatingWidget->dockContainer();
    auto newDockWidgets = floatingContainer->dockWidgets();
    auto topLevelDockArea = floatingContainer->topLevelDockArea();
    int newCurrentIndex = -1;

    if (topLevelDockArea)
    {
        newCurrentIndex = topLevelDockArea->currentIndex();
    }

    for (int i = 0; i < newDockWidgets.count(); ++i)
    {
        QdfDockWidget *dockWidget = newDockWidgets[i];
        targetArea->insertDockWidget(i, dockWidget, false);
        if (newCurrentIndex < 0 && !dockWidget->isClosed())
        {
            newCurrentIndex = i;
        }
    }
    targetArea->setCurrentIndex(newCurrentIndex);
    targetArea->updateTitleBarVisibility();
    return;
}

void QdfDockContainerWidgetPrivate::dropIntoSection(QdfFloatingDockContainer *floatingWidget,
                                                    QdfDockAreaWidget *targetArea, DockWidgetArea area)
{
    QDF_Q(QdfDockContainerWidget)
    if (CenterDockWidgetArea == area)
    {
        dropIntoCenterOfSection(floatingWidget, targetArea);
        return;
    }

    QdfDockContainerWidget *floatingContainer = floatingWidget->dockContainer();
    auto insertParam = internal::dockAreaInsertParameters(area);
    auto NewDockAreas =
            floatingContainer->findChildren<QdfDockAreaWidget *>(QString(), Qt::FindChildrenRecursively);
    QSplitter *targetAreaSplitter = internal::findParent<QSplitter *>(targetArea);

    if (!targetAreaSplitter)
    {
        QSplitter *splitter = newSplitter(insertParam.orientation());
        pLayout->replaceWidget(targetArea, splitter);
        splitter->addWidget(targetArea);
        updateSplitterHandles(splitter);
        targetAreaSplitter = splitter;
    }
    int areaIndex = targetAreaSplitter->indexOf(targetArea);
    auto floatingSplitter = floatingContainer->rootSplitter();
    if (targetAreaSplitter->orientation() == insertParam.orientation())
    {
        auto Sizes = targetAreaSplitter->sizes();
        int TargetAreaSize = (insertParam.orientation() == Qt::Horizontal) ? targetArea->width()
                                                                           : targetArea->height();
        bool AdjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != insertParam.orientation()) &&
            floatingSplitter->count() > 1)
        {
            targetAreaSplitter->insertWidget(areaIndex + insertParam.insertOffset(), floatingSplitter);
            updateSplitterHandles(targetAreaSplitter);
        }
        else
        {
            AdjustSplitterSizes = (floatingSplitter->count() == 1);
            int InsertIndex = areaIndex + insertParam.insertOffset();
            while (floatingSplitter->count())
            {
                targetAreaSplitter->insertWidget(InsertIndex++, floatingSplitter->widget(0));
                updateSplitterHandles(targetAreaSplitter);
            }
        }

        if (AdjustSplitterSizes)
        {
            int Size = (TargetAreaSize - targetAreaSplitter->handleWidth()) / 2;
            Sizes[areaIndex] = Size;
            Sizes.insert(areaIndex, Size);
            targetAreaSplitter->setSizes(Sizes);
        }
    }
    else
    {
        QSplitter *pNewSplitter = newSplitter(insertParam.orientation());
        int TargetAreaSize = (insertParam.orientation() == Qt::Horizontal) ? targetArea->width()
                                                                           : targetArea->height();
        bool AdjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != insertParam.orientation()) &&
            floatingSplitter->count() > 1)
        {
            pNewSplitter->addWidget(floatingSplitter);
            updateSplitterHandles(pNewSplitter);
        }
        else
        {
            AdjustSplitterSizes = (floatingSplitter->count() == 1);
            while (floatingSplitter->count())
            {
                pNewSplitter->addWidget(floatingSplitter->widget(0));
                updateSplitterHandles(pNewSplitter);
            }
        }

        auto sizes = targetAreaSplitter->sizes();
        insertWidgetIntoSplitter(pNewSplitter, targetArea, !insertParam.append());
        updateSplitterHandles(pNewSplitter);
        if (AdjustSplitterSizes)
        {
            int Size = TargetAreaSize / 2;
            pNewSplitter->setSizes({Size, Size});
        }
        targetAreaSplitter->insertWidget(areaIndex, pNewSplitter);
        targetAreaSplitter->setSizes(sizes);
        updateSplitterHandles(targetAreaSplitter);
    }

    addDockAreasToList(NewDockAreas);
    q->dumpLayout();
}

void QdfDockContainerWidgetPrivate::moveIntoCenterOfSection(QWidget *widget,
                                                            QdfDockAreaWidget *targetArea)
{
    auto droppedDockWidget = qobject_cast<QdfDockWidget *>(widget);
    auto droppedArea = qobject_cast<QdfDockAreaWidget *>(widget);

    if (droppedDockWidget)
    {
        QdfDockAreaWidget *oldDockArea = droppedDockWidget->dockAreaWidget();
        if (oldDockArea == targetArea)
        {
            return;
        }

        if (oldDockArea)
        {
            oldDockArea->removeDockWidget(droppedDockWidget);
        }
        targetArea->insertDockWidget(0, droppedDockWidget, true);
    }
    else
    {
        QList<QdfDockWidget *> newDockWidgets = droppedArea->dockWidgets();
        int newCurrentIndex = droppedArea->currentIndex();
        for (int i = 0; i < newDockWidgets.count(); ++i)
        {
            QdfDockWidget *dockWidget = newDockWidgets[i];
            targetArea->insertDockWidget(i, dockWidget, false);
        }
        targetArea->setCurrentIndex(newCurrentIndex);
        droppedArea->dockContainer()->removeDockArea(droppedArea);
        droppedArea->deleteLater();
    }

    targetArea->updateTitleBarVisibility();
    return;
}

void QdfDockContainerWidgetPrivate::moveToNewSection(QWidget *widget, QdfDockAreaWidget *targetArea,
                                                     DockWidgetArea area)
{
    QDF_Q(QdfDockContainerWidget)
    if (CenterDockWidgetArea == area)
    {
        moveIntoCenterOfSection(widget, targetArea);
        return;
    }

    QdfDockWidget *droppedDockWidget = qobject_cast<QdfDockWidget *>(widget);
    QdfDockAreaWidget *droppedDockArea = qobject_cast<QdfDockAreaWidget *>(widget);
    QdfDockAreaWidget *newDockArea;
    if (droppedDockWidget)
    {
        newDockArea = new QdfDockAreaWidget(dockManager, q);
        QdfDockAreaWidget *oldDockArea = droppedDockWidget->dockAreaWidget();
        if (oldDockArea)
        {
            oldDockArea->removeDockWidget(droppedDockWidget);
        }
        newDockArea->addDockWidget(droppedDockWidget);
    }
    else
    {
        droppedDockArea->dockContainer()->removeDockArea(droppedDockArea);
        newDockArea = droppedDockArea;
    }

    auto insertParam = internal::dockAreaInsertParameters(area);
    QSplitter *targetAreaSplitter = internal::findParent<QSplitter *>(targetArea);
    int areaIndex = targetAreaSplitter->indexOf(targetArea);
    auto sizes = targetAreaSplitter->sizes();
    if (targetAreaSplitter->orientation() == insertParam.orientation())
    {
        int TargetAreaSize = (insertParam.orientation() == Qt::Horizontal) ? targetArea->width()
                                                                           : targetArea->height();
        targetAreaSplitter->insertWidget(areaIndex + insertParam.insertOffset(), newDockArea);
        updateSplitterHandles(targetAreaSplitter);
        int size = (TargetAreaSize - targetAreaSplitter->handleWidth()) / 2;
        sizes[areaIndex] = size;
        sizes.insert(areaIndex, size);
    }
    else
    {
        int targetAreaSize = (insertParam.orientation() == Qt::Horizontal) ? targetArea->width()
                                                                           : targetArea->height();
        QSplitter *pNewSplitter = newSplitter(insertParam.orientation());
        pNewSplitter->addWidget(targetArea);
        insertWidgetIntoSplitter(pNewSplitter, newDockArea, insertParam.append());
        updateSplitterHandles(pNewSplitter);
        int size = targetAreaSize / 2;
        pNewSplitter->setSizes({size, size});
        targetAreaSplitter->insertWidget(areaIndex, pNewSplitter);
        updateSplitterHandles(targetAreaSplitter);
    }
    targetAreaSplitter->setSizes(sizes);

    addDockAreasToList({newDockArea});
}

void QdfDockContainerWidgetPrivate::adjustSplitterSizesOnInsertion(QSplitter *splitter, qreal lastRatio)
{
    int areaSize = (splitter->orientation() == Qt::Horizontal) ? splitter->width() : splitter->height();
    auto splitterSizes = splitter->sizes();

    qreal totRatio = splitterSizes.size() - 1.0 + lastRatio;
    for (int i = 0; i < splitterSizes.size() - 1; i++)
    {
        splitterSizes[i] = areaSize / totRatio;
    }
    splitterSizes.back() = areaSize * lastRatio / totRatio;
    splitter->setSizes(splitterSizes);
}

void QdfDockContainerWidgetPrivate::updateSplitterHandles(QSplitter *splitter)
{
    if (!dockManager->centralWidget() || !splitter)
    {
        return;
    }

    for (int i = 0; i < splitter->count(); ++i)
    {
        splitter->setStretchFactor(i, widgetResizesWithContainer(splitter->widget(i)) ? 1 : 0);
    }
}

bool QdfDockContainerWidgetPrivate::widgetResizesWithContainer(QWidget *widget)
{
    if (!dockManager->centralWidget())
    {
        return true;
    }

    auto area = qobject_cast<QdfDockAreaWidget *>(widget);
    if (area)
    {
        return area->isCentralWidgetArea();
    }

    auto innerSplitter = qobject_cast<QdfDockSplitter *>(widget);
    if (innerSplitter)
    {
        return innerSplitter->isResizingWithContainer();
    }

    return false;
}

void QdfDockContainerWidgetPrivate::onDockAreaViewToggled(bool visible)
{
    QDF_Q(QdfDockContainerWidget)
    QdfDockAreaWidget *dockArea = qobject_cast<QdfDockAreaWidget *>(q->sender());
    VisibleDockAreaCount += visible ? 1 : -1;
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreaViewToggled(dockArea, visible);
}

void QdfDockContainerWidgetPrivate::moveToContainer(QWidget *widget, DockWidgetArea area)
{
    QDF_Q(QdfDockContainerWidget)
    QdfDockWidget *droppedDockWidget = qobject_cast<QdfDockWidget *>(widget);
    QdfDockAreaWidget *droppedDockArea = qobject_cast<QdfDockAreaWidget *>(widget);
    QdfDockAreaWidget *newDockArea;

    if (droppedDockWidget)
    {
        newDockArea = new QdfDockAreaWidget(dockManager, q);
        QdfDockAreaWidget *OldDockArea = droppedDockWidget->dockAreaWidget();
        if (OldDockArea)
        {
            OldDockArea->removeDockWidget(droppedDockWidget);
        }
        newDockArea->addDockWidget(droppedDockWidget);
    }
    else
    {
        auto pSplitter = internal::findParent<QdfDockSplitter *>(droppedDockArea);
        auto insertParam = internal::dockAreaInsertParameters(area);
        if (pSplitter == rootSplitter && insertParam.orientation() == pSplitter->orientation())
        {
            if (insertParam.append() && pSplitter->lastWidget() == droppedDockArea)
            {
                return;
            }
            else if (!insertParam.append() && pSplitter->firstWidget() == droppedDockArea)
            {
                return;
            }
        }
        droppedDockArea->dockContainer()->removeDockArea(droppedDockArea);
        newDockArea = droppedDockArea;
    }

    addDockArea(newDockArea, area);
    lastAddedAreaCache[areaIdToIndex(area)] = newDockArea;
}

void QdfDockContainerWidgetPrivate::addDockAreasToList(const QList<QdfDockAreaWidget *> newDockAreas)
{
    int countBefore = dockAreas.count();
    int newAreaCount = newDockAreas.count();
    appendDockAreas(newDockAreas);
    for (auto &dockArea: newDockAreas)
    {
        dockArea->titleBarButton(TitleBarButton::Close)->setVisible(true);
        dockArea->titleBarButton(TitleBarButton::AutoHide)->setVisible(true);
    }

    if (1 == countBefore)
    {
        dockAreas.at(0)->updateTitleBarVisibility();
    }

    if (1 == newAreaCount)
    {
        dockAreas.last()->updateTitleBarVisibility();
    }

    emitDockAreasAdded();
}

void QdfDockContainerWidgetPrivate::appendDockAreas(const QList<QdfDockAreaWidget *> newDockAreas)
{
    QDF_Q(QdfDockContainerWidget)
    dockAreas.append(newDockAreas);
    for (auto &dockArea: newDockAreas)
    {
        QObject::connect(dockArea, &QdfDockAreaWidget::viewToggled, q,
                         std::bind(&QdfDockContainerWidgetPrivate::onDockAreaViewToggled, this,
                                   std::placeholders::_1));
    }
}

QdfDockAreaWidget *QdfDockContainerWidgetPrivate::addDockWidgetToContainer(DockWidgetArea area,
                                                                           QdfDockWidget *dockwidget)
{
    QDF_Q(QdfDockContainerWidget)
    QdfDockAreaWidget *newDockArea = new QdfDockAreaWidget(dockManager, q);
    newDockArea->addDockWidget(dockwidget);
    addDockArea(newDockArea, area);
    newDockArea->updateTitleBarVisibility();
    lastAddedAreaCache[areaIdToIndex(area)] = newDockArea;
    return newDockArea;
}

void QdfDockContainerWidgetPrivate::addDockArea(QdfDockAreaWidget *newDockArea, DockWidgetArea area)
{
    auto insertParam = internal::dockAreaInsertParameters(area);
    if (dockAreas.count() <= 1)
    {
        rootSplitter->setOrientation(insertParam.orientation());
    }

    QSplitter *Splitter = rootSplitter;
    if (Splitter->orientation() == insertParam.orientation())
    {
        insertWidgetIntoSplitter(Splitter, newDockArea, insertParam.append());
        updateSplitterHandles(Splitter);
        if (Splitter->isHidden())
        {
            Splitter->show();
        }
    }
    else
    {
        QSplitter *NewSplitter = newSplitter(insertParam.orientation());
        if (insertParam.append())
        {
            QLayoutItem *li = pLayout->replaceWidget(Splitter, NewSplitter);
            NewSplitter->addWidget(Splitter);
            NewSplitter->addWidget(newDockArea);
            updateSplitterHandles(NewSplitter);
            delete li;
        }
        else
        {
            NewSplitter->addWidget(newDockArea);
            QLayoutItem *li = pLayout->replaceWidget(Splitter, NewSplitter);
            NewSplitter->addWidget(Splitter);
            updateSplitterHandles(NewSplitter);
            delete li;
        }
        rootSplitter = NewSplitter;
    }

    addDockAreasToList({newDockArea});
}

void QdfDockContainerWidgetPrivate::dumpRecursive(int level, QWidget *widget)
{
#if defined(QT_DEBUG)
    QSplitter *Splitter = qobject_cast<QSplitter *>(widget);
    QByteArray buf;
    buf.fill(' ', level * 4);
    if (Splitter)
    {
        for (int i = 0; i < Splitter->count(); ++i)
        {
            dumpRecursive(level + 1, Splitter->widget(i));
        }
    }
    else
    {
        QdfDockAreaWidget *DockArea = qobject_cast<QdfDockAreaWidget *>(widget);
        if (!DockArea)
        {
            return;
        }
    }
#else
    Q_UNUSED(level);
    Q_UNUSED(widget);
#endif
}

QdfDockAreaWidget *
QdfDockContainerWidgetPrivate::addDockWidgetToDockArea(DockWidgetArea area, QdfDockWidget *dockwidget,
                                                       QdfDockAreaWidget *targetDockArea, int Index)
{
    QDF_Q(QdfDockContainerWidget)
    if (CenterDockWidgetArea == area)
    {
        targetDockArea->insertDockWidget(Index, dockwidget);
        targetDockArea->updateTitleBarVisibility();
        return targetDockArea;
    }

    QdfDockAreaWidget *newDockArea = new QdfDockAreaWidget(dockManager, q);
    newDockArea->addDockWidget(dockwidget);
    auto insertParam = internal::dockAreaInsertParameters(area);

    QSplitter *targetAreaSplitter = internal::findParent<QSplitter *>(targetDockArea);
    int index = targetAreaSplitter->indexOf(targetDockArea);
    if (targetAreaSplitter->orientation() == insertParam.orientation())
    {
        targetAreaSplitter->insertWidget(index + insertParam.insertOffset(), newDockArea);
        updateSplitterHandles(targetAreaSplitter);
        if (QdfDockManager::testConfigFlag(QdfDockManager::EqualSplitOnInsertion))
        {
            adjustSplitterSizesOnInsertion(targetAreaSplitter);
        }
    }
    else
    {
        auto targetAreaSizes = targetAreaSplitter->sizes();
        QSplitter *pNewSplitter = newSplitter(insertParam.orientation());
        pNewSplitter->addWidget(targetDockArea);

        insertWidgetIntoSplitter(pNewSplitter, newDockArea, insertParam.append());
        updateSplitterHandles(pNewSplitter);
        targetAreaSplitter->insertWidget(index, pNewSplitter);
        updateSplitterHandles(targetAreaSplitter);
        if (QdfDockManager::testConfigFlag(QdfDockManager::EqualSplitOnInsertion))
        {
            targetAreaSplitter->setSizes(targetAreaSizes);
            adjustSplitterSizesOnInsertion(pNewSplitter);
        }
    }

    addDockAreasToList({newDockArea});
    return newDockArea;
}

QdfDockContainerWidget::QdfDockContainerWidget(QdfDockManager *dockManager, QWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockContainerWidget)
    QDF_D(QdfDockContainerWidget)
    d->dockManager = dockManager;
    d->isFloating = floatingWidget() != nullptr;

    d->pLayout = new QGridLayout();
    d->pLayout->setContentsMargins(0, 0, 0, 0);
    d->pLayout->setSpacing(0);
    d->pLayout->setColumnStretch(1, 1);
    d->pLayout->setRowStretch(1, 1);
    setLayout(d->pLayout);

    if (dockManager != this)
    {
        d->dockManager->registerDockContainer(this);
        createRootSplitter();
        createSideTabBarWidgets();
    }
}

QdfDockContainerWidget::~QdfDockContainerWidget()
{
    QDF_D(QdfDockContainerWidget)
    if (d->dockManager)
    {
        d->dockManager->removeDockContainer(this);
    }

    QDF_FINI_PRIVATE()
}

QdfDockAreaWidget *QdfDockContainerWidget::addDockWidget(DockWidgetArea area, QdfDockWidget *dockwidget,
                                                         QdfDockAreaWidget *dockAreaWidget, int index)
{
    QDF_D(QdfDockContainerWidget)
    auto TopLevelDockWidget = topLevelDockWidget();
    QdfDockAreaWidget *OldDockArea = dockwidget->dockAreaWidget();
    if (OldDockArea)
    {
        OldDockArea->removeDockWidget(dockwidget);
    }

    dockwidget->setDockManager(d->dockManager);
    QdfDockAreaWidget *dockArea;
    if (dockAreaWidget)
    {
        dockArea = d->addDockWidgetToDockArea(area, dockwidget, dockAreaWidget, index);
    }
    else
    {
        dockArea = d->addDockWidgetToContainer(area, dockwidget);
    }

    if (TopLevelDockWidget)
    {
        auto NewTopLevelDockWidget = topLevelDockWidget();
        if (!NewTopLevelDockWidget)
        {
            QdfDockWidget::emitTopLevelEventForWidget(TopLevelDockWidget, false);
        }
    }
    return dockArea;
}

QdfAutoHideDockContainer *
QdfDockContainerWidget::createAndSetupAutoHideContainer(SideBarLocation area, QdfDockWidget *dockWidget)
{
    QDF_D(QdfDockContainerWidget)
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
    {
        Q_ASSERT_X(false, "QdfDockContainerWidget::createAndInitializeDockWidgetOverlayContainer",
                   "Requested area does not exist in config");
        return nullptr;
    }
    if (d->dockManager != dockWidget->dockManager())
    {
        dockWidget->setDockManager(d->dockManager);
    }

    return sideTabBar(area)->insertDockWidget(-1, dockWidget);
}

void QdfDockContainerWidget::removeDockWidget(QdfDockWidget *dockwidget)
{
    QdfDockAreaWidget *area = dockwidget->dockAreaWidget();
    if (area)
    {
        area->removeDockWidget(dockwidget);
    }
}

unsigned int QdfDockContainerWidget::zOrderIndex() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->zOrderIndex;
}

bool QdfDockContainerWidget::isInFrontOf(QdfDockContainerWidget *other) const
{
    return this->zOrderIndex() > other->zOrderIndex();
}

bool QdfDockContainerWidget::event(QEvent *event)
{
    QDF_D(QdfDockContainerWidget)
    bool result = QWidget::event(event);
    if (event->type() == QEvent::WindowActivate)
    {
        d->zOrderIndex = ++zOrderCounter;
    }
    else if (event->type() == QEvent::Show && !d->zOrderIndex)
    {
        d->zOrderIndex = ++zOrderCounter;
    }

    return result;
}

QList<QdfAutoHideDockContainer *> QdfDockContainerWidget::autoHideWidgets() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->autoHideWidgets;
}

void QdfDockContainerWidget::addDockArea(QdfDockAreaWidget *dockAreaWidget, DockWidgetArea area)
{
    QDF_D(QdfDockContainerWidget)
    QdfDockContainerWidget *container = dockAreaWidget->dockContainer();
    if (container && container != this)
    {
        container->removeDockArea(dockAreaWidget);
    }

    d->addDockArea(dockAreaWidget, area);
}

void QdfDockContainerWidget::removeDockArea(QdfDockAreaWidget *area)
{
    QDF_D(QdfDockContainerWidget)
    if (area->autoHide())
    {
        area->setAutoHideDockContainer(nullptr);
        return;
    }

    area->disconnect(this);
    d->dockAreas.removeAll(area);
    QdfDockSplitter *splitter = internal::findParent<QdfDockSplitter *>(area);

    area->setParent(nullptr);
    internal::hideEmptyParentSplitters(splitter);

    auto p = std::find(std::begin(d->lastAddedAreaCache), std::end(d->lastAddedAreaCache), area);
    if (p != std::end(d->lastAddedAreaCache))
    {
        *p = nullptr;
    }

    if (splitter->count() > 1)
    {
        goto emitAndExit;
    }
    if (splitter == d->rootSplitter)
    {
        if (!splitter->count())
        {
            splitter->hide();
            goto emitAndExit;
        }

        QWidget *widget = splitter->widget(0);
        QSplitter *childSplitter = qobject_cast<QSplitter *>(widget);
        if (!childSplitter)
        {
            goto emitAndExit;
        }

        childSplitter->setParent(nullptr);
        QLayoutItem *li = d->pLayout->replaceWidget(splitter, childSplitter);
        d->rootSplitter = childSplitter;
        delete li;
    }
    else if (splitter->count() == 1)
    {
        QSplitter *parentSplitter = internal::findParent<QSplitter *>(splitter);
        auto Sizes = parentSplitter->sizes();
        QWidget *widget = splitter->widget(0);
        widget->setParent(this);
        internal::replaceSplitterWidget(parentSplitter, splitter, widget);
        parentSplitter->setSizes(Sizes);
    }

    delete splitter;
    splitter = nullptr;

emitAndExit:
    updateSplitterHandles(splitter);
    QdfDockWidget *TopLevelWidget = topLevelDockWidget();
    QdfDockWidget::emitTopLevelEventForWidget(TopLevelWidget, true);
    dumpLayout();
    d->emitDockAreasRemoved();
}

QdfDockAreaWidget *QdfDockContainerWidget::dockAreaAt(const QPoint &globalPos) const
{
    QDF_D(const QdfDockContainerWidget)
    for (const auto &dockArea: d->dockAreas)
    {
        if (dockArea->isVisible() && dockArea->rect().contains(dockArea->mapFromGlobal(globalPos)))
        {
            return dockArea;
        }
    }

    return nullptr;
}

QdfDockAreaWidget *QdfDockContainerWidget::dockArea(int index) const
{
    QDF_D(const QdfDockContainerWidget)
    return (index < dockAreaCount()) ? d->dockAreas[index] : nullptr;
}

bool QdfDockContainerWidget::isFloating() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->isFloating;
}

int QdfDockContainerWidget::dockAreaCount() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->dockAreas.count();
}

int QdfDockContainerWidget::visibleDockAreaCount() const
{
    QDF_D(const QdfDockContainerWidget)
    int count = 0;
    for (auto &dockArea: d->dockAreas)
    {
        count += dockArea->isHidden() ? 0 : 1;
    }

    return count;
}

void QdfDockContainerWidget::dropFloatingWidget(QdfFloatingDockContainer *floatingWidget,
                                                const QPoint &targetPos)
{
    QDF_D(QdfDockContainerWidget)
    QdfDockWidget *SingleDroppedDockWidget = floatingWidget->topLevelDockWidget();
    QdfDockWidget *SingleDockWidget = topLevelDockWidget();
    QdfDockAreaWidget *DockArea = dockAreaAt(targetPos);
    auto dropArea = InvalidDockWidgetArea;
    auto ContainerDropArea = d->dockManager->containerOverlay()->dropAreaUnderCursor();
    bool Dropped = false;

    if (DockArea)
    {
        auto dropOverlay = d->dockManager->dockAreaOverlay();
        dropOverlay->setAllowedAreas(DockArea->allowedAreas());
        dropArea = dropOverlay->showOverlay(DockArea);
        if (ContainerDropArea != InvalidDockWidgetArea && ContainerDropArea != dropArea)
        {
            dropArea = InvalidDockWidgetArea;
        }

        if (dropArea != InvalidDockWidgetArea)
        {
            d->dropIntoSection(floatingWidget, DockArea, dropArea);
            Dropped = true;
        }
    }

    if (InvalidDockWidgetArea == dropArea)
    {
        dropArea = ContainerDropArea;
        if (dropArea != InvalidDockWidgetArea)
        {
            d->dropIntoContainer(floatingWidget, dropArea);
            Dropped = true;
        }
    }
    for (auto AutohideWidget: floatingWidget->dockContainer()->autoHideWidgets())
    {
        auto SideBar = sideTabBar(AutohideWidget->sideBarLocation());
        SideBar->addAutoHideWidget(AutohideWidget);
    }

    if (Dropped)
    {
        floatingWidget->hideAndDeleteLater();
        QdfDockWidget::emitTopLevelEventForWidget(SingleDroppedDockWidget, false);
        QdfDockWidget::emitTopLevelEventForWidget(SingleDockWidget, false);
    }

    window()->activateWindow();
    if (SingleDroppedDockWidget)
    {
        d->dockManager->notifyWidgetOrAreaRelocation(SingleDroppedDockWidget);
    }
    d->dockManager->notifyFloatingWidgetDrop(floatingWidget);
}

void QdfDockContainerWidget::dropWidget(QWidget *widget, DockWidgetArea dropArea,
                                        QdfDockAreaWidget *targetAreaWidget)
{
    QDF_D(QdfDockContainerWidget)
    QdfDockWidget *singleDockWidget = topLevelDockWidget();
    if (targetAreaWidget)
    {
        d->moveToNewSection(widget, targetAreaWidget, dropArea);
    }
    else
    {
        d->moveToContainer(widget, dropArea);
    }

    QdfDockWidget::emitTopLevelEventForWidget(singleDockWidget, false);

    window()->activateWindow();
    d->dockManager->notifyWidgetOrAreaRelocation(widget);
}

QList<QdfDockAreaWidget *> QdfDockContainerWidget::openedDockAreas() const
{
    QDF_D(const QdfDockContainerWidget)
    QList<QdfDockAreaWidget *> result;
    for (auto &dockArea: d->dockAreas)
    {
        if (!dockArea->isHidden())
        {
            result.append(dockArea);
        }
    }

    return result;
}

QList<QdfDockWidget *> QdfDockContainerWidget::openedDockWidgets() const
{
    QDF_D(const QdfDockContainerWidget)
    QList<QdfDockWidget *> dockWidgetList;
    for (auto &dockArea: d->dockAreas)
    {
        if (!dockArea->isHidden())
        {
            dockWidgetList.append(dockArea->openedDockWidgets());
        }
    }

    return dockWidgetList;
}

bool QdfDockContainerWidget::hasOpenDockAreas() const
{
    QDF_D(const QdfDockContainerWidget)
    for (auto &dockArea: d->dockAreas)
    {
        if (!dockArea->isHidden())
        {
            return true;
        }
    }

    return false;
}

QSplitter *QdfDockContainerWidget::rootSplitter() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->rootSplitter;
}

void QdfDockContainerWidget::createRootSplitter()
{
    QDF_D(QdfDockContainerWidget)
    if (d->rootSplitter)
    {
        return;
    }
    d->rootSplitter = d->newSplitter(Qt::Horizontal);
    d->pLayout->addWidget(d->rootSplitter, 1, 1);
}

void QdfDockContainerWidget::createSideTabBarWidgets()
{
    QDF_D(QdfDockContainerWidget)
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
    {
        return;
    }

    {
        auto area = SideBarLocation::SL_Left;
        d->sideTabBarWidgets[area] = new QdfAutoHideSideBar(this, area);
        d->pLayout->addWidget(d->sideTabBarWidgets[area], 1, 0);
    }

    {
        auto area = SideBarLocation::SL_Right;
        d->sideTabBarWidgets[area] = new QdfAutoHideSideBar(this, area);
        d->pLayout->addWidget(d->sideTabBarWidgets[area], 1, 2);
    }

    {
        auto area = SideBarLocation::SL_Bottom;
        d->sideTabBarWidgets[area] = new QdfAutoHideSideBar(this, area);
        d->pLayout->addWidget(d->sideTabBarWidgets[area], 2, 1);
    }

    {
        auto area = SideBarLocation::SL_Top;
        d->sideTabBarWidgets[area] = new QdfAutoHideSideBar(this, area);
        d->pLayout->addWidget(d->sideTabBarWidgets[area], 0, 1);
    }
}

void QdfDockContainerWidget::dumpLayout()
{
#if (ADS_DEBUG_LEVEL > 0)
    qDebug("\n\nDumping layout --------------------------");
    std::cout << "\n\nDumping layout --------------------------" << std::endl;
    d->dumpRecursive(0, d->rootSplitter);
    qDebug("--------------------------\n\n");
    std::cout << "--------------------------\n\n" << std::endl;
#endif
}

QdfDockAreaWidget *QdfDockContainerWidget::lastAddedDockAreaWidget(DockWidgetArea area) const
{
    QDF_D(const QdfDockContainerWidget)
    return d->lastAddedAreaCache[areaIdToIndex(area)];
}

bool QdfDockContainerWidget::hasTopLevelDockWidget() const
{
    QDF_D(const QdfDockContainerWidget)
    auto DockAreas = openedDockAreas();
    if (DockAreas.count() != 1)
    {
        return false;
    }

    return DockAreas[0]->openedDockWidgetsCount() == 1;
}

QdfDockWidget *QdfDockContainerWidget::topLevelDockWidget() const
{
    auto TopLevelDockArea = topLevelDockArea();
    if (!TopLevelDockArea)
    {
        return nullptr;
    }

    auto dockWidgets = TopLevelDockArea->openedDockWidgets();
    if (dockWidgets.count() != 1)
    {
        return nullptr;
    }

    return dockWidgets[0];
}

QdfDockAreaWidget *QdfDockContainerWidget::topLevelDockArea() const
{
    auto dockAreas = openedDockAreas();
    if (dockAreas.count() != 1)
    {
        return nullptr;
    }

    return dockAreas[0];
}

QList<QdfDockWidget *> QdfDockContainerWidget::dockWidgets() const
{
    QDF_D(const QdfDockContainerWidget)
    QList<QdfDockWidget *> result;
    for (const auto &dockArea: d->dockAreas)
    {
        result.append(dockArea->dockWidgets());
    }

    return result;
}

void QdfDockContainerWidget::updateSplitterHandles(QSplitter *splitter)
{
    QDF_D(QdfDockContainerWidget)
    d->updateSplitterHandles(splitter);
}

void QdfDockContainerWidget::registerAutoHideWidget(QdfAutoHideDockContainer *AutohideWidget)
{
    QDF_D(QdfDockContainerWidget)
    d->autoHideWidgets.append(AutohideWidget);
    Q_EMIT autoHideWidgetCreated(AutohideWidget);
}

void QdfDockContainerWidget::removeAutoHideWidget(QdfAutoHideDockContainer *AutohideWidget)
{
    QDF_D(QdfDockContainerWidget)
    d->autoHideWidgets.removeAll(AutohideWidget);
}

QdfDockWidget::DockWidgetFeatures QdfDockContainerWidget::features() const
{
    QDF_D(const QdfDockContainerWidget)
    QdfDockWidget::DockWidgetFeatures Features(QdfDockWidget::AllDockWidgetFeatures);
    for (const auto &dockArea: d->dockAreas)
    {
        Features &= dockArea->features();
    }

    return Features;
}

QdfFloatingDockContainer *QdfDockContainerWidget::floatingWidget() const
{
    return internal::findParent<QdfFloatingDockContainer *>(this);
}

void QdfDockContainerWidget::closeOtherAreas(QdfDockAreaWidget *KeepOpenArea)
{
    QDF_D(QdfDockContainerWidget)
    for (const auto &dockArea: d->dockAreas)
    {
        if (dockArea == KeepOpenArea)
        {
            continue;
        }

        if (!dockArea->features(BitwiseOperator::BitwiseAnd).testFlag(QdfDockWidget::DockWidgetClosable))
        {
            continue;
        }

        if (dockArea->features(BitwiseOperator::BitwiseOr).testFlag(QdfDockWidget::CustomCloseHandling))
        {
            continue;
        }

        dockArea->closeArea();
    }
}

QdfAutoHideSideBar *QdfDockContainerWidget::sideTabBar(SideBarLocation area) const
{
    QDF_D(const QdfDockContainerWidget)
    return d->sideTabBarWidgets[area];
}

QRect QdfDockContainerWidget::contentRect() const
{
    QDF_D(const QdfDockContainerWidget)
    if (!d->rootSplitter)
    {
        return QRect();
    }

    return d->rootSplitter->geometry();
}

QRect QdfDockContainerWidget::contentRectGlobal() const
{
    QDF_D(const QdfDockContainerWidget)
    if (!d->rootSplitter)
    {
        return QRect();
    }
    return internal::globalGeometry(d->rootSplitter);
}

QdfDockManager *QdfDockContainerWidget::dockManager() const
{
    QDF_D(const QdfDockContainerWidget)
    return d->dockManager;
}

void QdfDockContainerWidget::handleAutoHideWidgetEvent(QEvent *event, QWidget *widget)
{
    QDF_D(QdfDockContainerWidget)
    if (!QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideShowOnMouseOver))
    {
        return;
    }

    if (dockManager()->isRestoringState())
    {
        return;
    }

    auto AutoHideTab = qobject_cast<QdfAutoHideTab *>(widget);
    if (AutoHideTab)
    {
        switch (event->type())
        {
            case QEvent::Enter:
                if (!AutoHideTab->dockWidget()->isVisible())
                {
                    d->delayedAutoHideTab = AutoHideTab;
                    d->delayedAutoHideShow = true;
                    d->delayedAutoHideTimer.start();
                }
                else
                {
                    d->delayedAutoHideTimer.stop();
                }
                break;

            case QEvent::MouseButtonPress:
                d->delayedAutoHideTimer.stop();
                break;

            case QEvent::Leave:
                if (AutoHideTab->dockWidget()->isVisible())
                {
                    d->delayedAutoHideTab = AutoHideTab;
                    d->delayedAutoHideShow = false;
                    d->delayedAutoHideTimer.start();
                }
                else
                {
                    d->delayedAutoHideTimer.stop();
                }
                break;

            default:
                break;
        }
        return;
    }

    auto autoHideContainer = qobject_cast<QdfAutoHideDockContainer *>(widget);
    if (autoHideContainer)
    {
        switch (event->type())
        {
            case QEvent::Enter:
            case QEvent::Hide:
                d->delayedAutoHideTimer.stop();
                break;

            case QEvent::Leave:
                if (autoHideContainer->isVisible())
                {
                    d->delayedAutoHideTab = autoHideContainer->autoHideTab();
                    d->delayedAutoHideShow = false;
                    d->delayedAutoHideTimer.start();
                }
                break;

            default:
                break;
        }
        return;
        return;
    }
}