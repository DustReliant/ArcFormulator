#include "private/qdfdockareawidget_p.h"
#include "qdfdockcomponentsfactory.h"
#include "qdfdockwidgethelper.h"
#include <QDebug>
#include <QList>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedLayout>
#include <QStyle>
#include <QWheelEvent>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfdockareatabbar.h>
#include <dockwidget/qdfdockareatitlebar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdfdockwidgettab.h>
#include <dockwidget/qdfelidinglabel.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

static const char *const INDEX_PROPERTY = "index";
static const char *const ACTION_PROPERTY = "action";

static bool isAutoHideFeatureEnabled()
{
    return QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled);
}

QdfDockAreaLayout::QdfDockAreaLayout(QBoxLayout *parent)
    : m_parentLayout(parent)
{
}

int QdfDockAreaLayout::count() const
{
    return m_widgets.count();
}

void QdfDockAreaLayout::insertWidget(int index, QWidget *widget)
{
    widget->setParent(nullptr);
    if (index < 0)
    {
        index = m_widgets.count();
    }
    m_widgets.insert(index, widget);
    if (m_currentIndex < 0)
    {
        setCurrentIndex(index);
    }
    else
    {
        if (index <= m_currentIndex)
        {
            ++m_currentIndex;
        }
    }
}

void QdfDockAreaLayout::removeWidget(QWidget *widget)
{
    if (currentWidget() == widget)
    {
        auto LayoutItem = m_parentLayout->takeAt(1);
        if (LayoutItem)
        {
            LayoutItem->widget()->setParent(nullptr);
        }
        m_currentWidget = nullptr;
        m_currentIndex = -1;
    }
    else if (indexOf(widget) < m_currentIndex)
    {
        --m_currentIndex;
    }
    m_widgets.removeOne(widget);
}

QWidget *QdfDockAreaLayout::currentWidget() const
{
    return m_currentWidget;
}

void QdfDockAreaLayout::setCurrentIndex(int index)
{
    QWidget *prev = currentWidget();
    QWidget *next = widget(index);
    if (!next || (next == prev && !m_currentWidget))
    {
        return;
    }

    bool reenableUpdates = false;
    QWidget *parent = m_parentLayout->parentWidget();

    if (parent && parent->updatesEnabled())
    {
        reenableUpdates = true;
        parent->setUpdatesEnabled(false);
    }

    auto LayoutItem = m_parentLayout->takeAt(1);
    if (LayoutItem)
    {
        LayoutItem->widget()->setParent(nullptr);
    }
    delete LayoutItem;

    m_parentLayout->addWidget(next);
    if (prev)
    {
        prev->hide();
    }
    m_currentIndex = index;
    m_currentWidget = next;

    if (reenableUpdates)
    {
        parent->setUpdatesEnabled(true);
    }
}

int QdfDockAreaLayout::currentIndex() const
{
    return m_currentIndex;
}

bool QdfDockAreaLayout::isEmpty() const
{
    return m_widgets.empty();
}

int QdfDockAreaLayout::indexOf(QWidget *w) const
{
    return m_widgets.indexOf(w);
}

QWidget *QdfDockAreaLayout::widget(int index) const
{
    return (index < m_widgets.size()) ? m_widgets.at(index) : nullptr;
}

QRect QdfDockAreaLayout::geometry() const
{
    return m_widgets.empty() ? QRect() : currentWidget()->geometry();
}

QdfDockAreaWidgetPrivate::QdfDockAreaWidgetPrivate()
{
}

QdfDockAreaWidgetPrivate::~QdfDockAreaWidgetPrivate()
{
    delete contentsLayout;
    contentsLayout = nullptr;
}

void QdfDockAreaWidgetPrivate::createTitleBar()
{
    QDF_Q(QdfDockAreaWidget)
    titleBar = QdfDockComponentsFactory::instance()->createDockAreaTitleBar(q);
    pLayout->addWidget(titleBar);
    QObject::connect(tabBar(), &QdfDockAreaTabBar::tabCloseRequested, q, &QdfDockAreaWidget::onTabCloseRequested);
    QObject::connect(titleBar, &QdfDockAreaTitleBar::tabBarClicked, q, &QdfDockAreaWidget::setCurrentIndex);
    QObject::connect(tabBar(), &QdfDockAreaTabBar::tabMoved, q, &QdfDockAreaWidget::reorderDockWidget);
}

QdfDockAreaTabBar *QdfDockAreaWidgetPrivate::tabBar() const
{
    return titleBar->tabBar();
}

void QdfDockAreaWidgetPrivate::updateTitleBarButtonStates()
{
    QDF_Q(QdfDockAreaWidget)
    if (q->isHidden())
    {
        updateTitleBarButtons = true;
        return;
    }

    titleBar->button(TitleBarButton::Close)->setEnabled(q->features().testFlag(QdfDockWidget::DockWidgetClosable));
    titleBar->button(TitleBarButton::Undock)->setEnabled(q->features().testFlag(QdfDockWidget::DockWidgetFloatable));
    titleBar->button(TitleBarButton::AutoHide)->setEnabled(q->features().testFlag(QdfDockWidget::DockWidgetPinnable));
    titleBar->updateDockWidgetActionsButtons();
    updateTitleBarButtons = false;
}

void QdfDockAreaWidgetPrivate::updateTitleBarButtonVisibility(bool IsTopLevel)
{
    QDF_Q(QdfDockAreaWidget)
    auto *const container = q->dockContainer();
    if (!container)
    {
        return;
    }

    if (IsTopLevel)
    {
        titleBar->button(TitleBarButton::Close)->setVisible(!container->isFloating());
        titleBar->button(TitleBarButton::AutoHide)->setVisible(!container->isFloating());
        titleBar->button(TitleBarButton::Undock)->setVisible(!container->isFloating() && !q->autoHide());
        titleBar->button(TitleBarButton::TabsMenu)->setVisible(!q->autoHide());
    }
    else
    {
        titleBar->button(TitleBarButton::Close)->setVisible(true);
        titleBar->button(TitleBarButton::AutoHide)->setVisible(true);
        titleBar->button(TitleBarButton::Undock)->setVisible(!q->autoHide());
        titleBar->button(TitleBarButton::TabsMenu)->setVisible(!q->autoHide());
    }
}
void QdfDockAreaWidgetPrivate::updateMinimumSizeHint()
{
    minSizeHint = QSize();
    for (int i = 0; i < contentsLayout->count(); ++i)
    {
        auto widget = contentsLayout->widget(i);
        minSizeHint.setHeight(qMax(minSizeHint.height(), widget->minimumSizeHint().height()));
        minSizeHint.setWidth(qMax(minSizeHint.width(), widget->minimumSizeHint().width()));
    }
}

QdfDockAreaWidget::QdfDockAreaWidget(QdfDockManager *dockManager, QdfDockContainerWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockAreaWidget)
    QDF_D(QdfDockAreaWidget)
    d->dockManager = dockManager;
    d->pLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    d->pLayout->setContentsMargins(0, 0, 0, 0);
    d->pLayout->setSpacing(0);
    setLayout(d->pLayout);

    d->createTitleBar();
    d->contentsLayout = new QdfDockAreaLayout(d->pLayout);
    if (d->dockManager)
    {
        Q_EMIT d->dockManager->dockAreaCreated(this);
    }
}

QdfDockAreaWidget::~QdfDockAreaWidget(){
        QDF_FINI_PRIVATE()}

QdfDockManager *QdfDockAreaWidget::dockManager() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->dockManager;
}

QdfDockContainerWidget *QdfDockAreaWidget::dockContainer() const
{
    return internal::findParent<QdfDockContainerWidget *>(this);
}

QdfAutoHideDockContainer *QdfDockAreaWidget::autoHideDockContainer() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->autoHideDockContainer;
}

bool QdfDockAreaWidget::autoHide() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->autoHideDockContainer != nullptr;
}

void QdfDockAreaWidget::setAutoHideDockContainer(QdfAutoHideDockContainer *autoHideDockContainer)
{
    QDF_D(QdfDockAreaWidget)
    d->autoHideDockContainer = autoHideDockContainer;
    updateAutoHideButtonCheckState();
    updateTitleBarButtonsToolTips();
}

void QdfDockAreaWidget::addDockWidget(QdfDockWidget *dockWidget)
{
    QDF_D(QdfDockAreaWidget)
    insertDockWidget(d->contentsLayout->count(), dockWidget);
}

void QdfDockAreaWidget::insertDockWidget(int index, QdfDockWidget *dockWidget,
                                         bool Activate)
{
    QDF_D(QdfDockAreaWidget)
    if (index < 0 || index > d->contentsLayout->count())
    {
        index = d->contentsLayout->count();
    }
    d->contentsLayout->insertWidget(index, dockWidget);
    dockWidget->setDockArea(this);
    dockWidget->tabWidget()->setDockAreaWidget(this);
    auto TabWidget = dockWidget->tabWidget();
    d->tabBar()->blockSignals(true);
    d->tabBar()->insertTab(index, TabWidget);
    d->tabBar()->blockSignals(false);
    TabWidget->setVisible(!dockWidget->isClosed());
    d->titleBar->autoHideTitleLabel()->setText(dockWidget->windowTitle());
    dockWidget->setProperty(INDEX_PROPERTY, index);
    d->minSizeHint.setHeight(qMax(d->minSizeHint.height(), dockWidget->minimumSizeHint().height()));
    d->minSizeHint.setWidth(qMax(d->minSizeHint.width(), dockWidget->minimumSizeHint().width()));
    if (Activate)
    {
        setCurrentIndex(index);
        dockWidget->setClosedState(false);
        if (!this->isVisible() && d->contentsLayout->count() > 1 && !dockManager()->isRestoringState())
        {
            dockWidget->toggleViewInternal(true);
        }
        d->updateTitleBarButtonStates();
        updateTitleBarVisibility();
    }
}

void QdfDockAreaWidget::removeDockWidget(QdfDockWidget *dockWidget)
{
    QDF_D(QdfDockAreaWidget)
    if (!dockWidget)
    {
        return;
    }

    if (autoHide())
    {
        autoHideDockContainer()->cleanupAndDelete();
        return;
    }

    auto CurrentDockWidget = currentDockWidget();
    auto NextOpenDockWidget = (dockWidget == CurrentDockWidget) ? nextOpenDockWidget(dockWidget) : nullptr;

    d->contentsLayout->removeWidget(dockWidget);
    auto TabWidget = dockWidget->tabWidget();
    TabWidget->hide();
    d->tabBar()->removeTab(TabWidget);
    TabWidget->setParent(dockWidget);
    dockWidget->setDockArea(nullptr);
    QdfDockContainerWidget *DockContainer = dockContainer();
    if (NextOpenDockWidget)
    {
        setCurrentDockWidget(NextOpenDockWidget);
    }
    else if (d->contentsLayout->isEmpty() && DockContainer->dockAreaCount() >= 1)
    {
        DockContainer->removeDockArea(this);
        this->deleteLater();
        if (DockContainer->dockAreaCount() == 0)
        {
            if (QdfFloatingDockContainer *FloatingDockContainer = DockContainer->floatingWidget())
            {
                FloatingDockContainer->hide();
                FloatingDockContainer->deleteLater();
            }
        }
    }
    else if (dockWidget == CurrentDockWidget)
    {
        hideAreaWithNoVisibleContent();
    }

    d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
    d->updateMinimumSizeHint();
    auto TopLevelDockWidget = DockContainer->topLevelDockWidget();
    if (TopLevelDockWidget)
    {
        TopLevelDockWidget->emitTopLevelChanged(true);
    }

#if (ADS_DEBUG_LEVEL > 0)
    DockContainer->dumpLayout();
#endif
}


void QdfDockAreaWidget::hideAreaWithNoVisibleContent()
{
    this->toggleView(false);

    auto Splitter = internal::findParent<QdfDockSplitter *>(this);
    internal::hideEmptyParentSplitters(Splitter);

    QdfDockContainerWidget *Container = this->dockContainer();
    if (!Container->isFloating() && !QdfDockManager::testConfigFlag(QdfDockManager::HideSingleCentralWidgetTitleBar))
    {
        return;
    }

    updateTitleBarVisibility();
    auto TopLevelWidget = Container->topLevelDockWidget();
    auto FloatingWidget = Container->floatingWidget();
    if (TopLevelWidget)
    {
        if (FloatingWidget)
        {
            FloatingWidget->updateWindowTitle();
        }
        QdfDockWidget::emitTopLevelEventForWidget(TopLevelWidget, true);
    }
    else if (Container->openedDockAreas().isEmpty() && FloatingWidget)
    {
        FloatingWidget->hide();
    }
    if (autoHide())
    {
        autoHideDockContainer()->hide();
    }
}

void QdfDockAreaWidget::onTabCloseRequested(int index)
{
    auto *pDockWidget = dockWidget(index);
    if (pDockWidget->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose) || pDockWidget->features().testFlag(QdfDockWidget::CustomCloseHandling))
    {
        pDockWidget->closeDockWidgetInternal();
    }
    else
    {
        pDockWidget->toggleView(false);
    }
}

QdfDockWidget *QdfDockAreaWidget::currentDockWidget() const
{
    int index = currentIndex();
    if (index < 0)
    {
        return nullptr;
    }

    return dockWidget(index);
}

void QdfDockAreaWidget::setCurrentDockWidget(QdfDockWidget *dockWidget)
{
    if (dockManager()->isRestoringState())
    {
        return;
    }

    internalSetCurrentDockWidget(dockWidget);
}

void QdfDockAreaWidget::internalSetCurrentDockWidget(QdfDockWidget *dockWidget)
{
    int i = index(dockWidget);
    if (i < 0)
    {
        return;
    }

    setCurrentIndex(i);
    dockWidget->setClosedState(false);
}

void QdfDockAreaWidget::setCurrentIndex(int index)
{
    QDF_D(QdfDockAreaWidget)
    auto TabBar = d->tabBar();
    if (index < 0 || index > (TabBar->count() - 1))
    {
        qWarning() << Q_FUNC_INFO << "Invalid index" << index;
        return;
    }

    auto cw = d->contentsLayout->currentWidget();
    auto nw = d->contentsLayout->widget(index);
    if (cw == nw && !nw->isHidden())
    {
        return;
    }

    Q_EMIT currentChanging(index);
    TabBar->setCurrentIndex(index);
    d->contentsLayout->setCurrentIndex(index);
    d->contentsLayout->currentWidget()->show();
    Q_EMIT currentChanged(index);
}

int QdfDockAreaWidget::currentIndex() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->contentsLayout->currentIndex();
}

QRect QdfDockAreaWidget::titleBarGeometry() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->titleBar->geometry();
}

QRect QdfDockAreaWidget::contentAreaGeometry() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->contentsLayout->geometry();
}

int QdfDockAreaWidget::index(QdfDockWidget *dockWidget)
{
    QDF_D(const QdfDockAreaWidget)
    return d->contentsLayout->indexOf(dockWidget);
}

QList<QdfDockWidget *> QdfDockAreaWidget::dockWidgets() const
{
    QDF_D(const QdfDockAreaWidget)
    QList<QdfDockWidget *> dockWidgetList;
    for (int i = 0; i < d->contentsLayout->count(); ++i)
    {
        dockWidgetList.append(dockWidget(i));
    }
    return dockWidgetList;
}

int QdfDockAreaWidget::openedDockWidgetsCount() const
{
    QDF_D(const QdfDockAreaWidget)
    int count = 0;
    for (int i = 0; i < d->contentsLayout->count(); ++i)
    {
        if (!dockWidget(i)->isClosed())
        {
            ++count;
        }
    }
    return count;
}

QList<QdfDockWidget *> QdfDockAreaWidget::openedDockWidgets() const
{
    QDF_D(const QdfDockAreaWidget)
    QList<QdfDockWidget *> dockWidgetList;
    for (int i = 0; i < d->contentsLayout->count(); ++i)
    {
        QdfDockWidget *widget = dockWidget(i);
        if (!widget->isClosed())
        {
            dockWidgetList.append(dockWidget(i));
        }
    }
    return dockWidgetList;
}

int QdfDockAreaWidget::indexOfFirstOpenDockWidget() const
{
    QDF_D(const QdfDockAreaWidget)
    for (int i = 0; i < d->contentsLayout->count(); ++i)
    {
        if (!dockWidget(i)->isClosed())
        {
            return i;
        }
    }

    return -1;
}

int QdfDockAreaWidget::dockWidgetsCount() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->contentsLayout->count();
}

QdfDockWidget *QdfDockAreaWidget::dockWidget(int index) const
{
    QDF_D(const QdfDockAreaWidget)
    return qobject_cast<QdfDockWidget *>(d->contentsLayout->widget(index));
}

void QdfDockAreaWidget::reorderDockWidget(int fromIndex, int toIndex)
{
    QDF_D(QdfDockAreaWidget)
    if (fromIndex >= d->contentsLayout->count() || fromIndex < 0 || toIndex >= d->contentsLayout->count() || toIndex < 0 || fromIndex == toIndex)
    {
        return;
    }

    auto widget = d->contentsLayout->widget(fromIndex);
    d->contentsLayout->removeWidget(widget);
    d->contentsLayout->insertWidget(toIndex, widget);
    setCurrentIndex(toIndex);
}

void QdfDockAreaWidget::toggleDockWidgetView(QdfDockWidget *dockWidget, bool Open)
{
    Q_UNUSED(dockWidget);
    Q_UNUSED(Open);
    updateTitleBarVisibility();
}

void QdfDockAreaWidget::updateTitleBarVisibility()
{
    QDF_D(QdfDockAreaWidget)
    QdfDockContainerWidget *container = dockContainer();
    if (!container)
    {
        return;
    }

    if (!d->titleBar)
    {
        return;
    }

    bool bAutoHide = autoHide();
    if (!QdfDockManager::testConfigFlag(QdfDockManager::AlwaysShowTabs))
    {
        bool hidden = container->hasTopLevelDockWidget() && (container->isFloating() || QdfDockManager::testConfigFlag(QdfDockManager::HideSingleCentralWidgetTitleBar));
        hidden |= (d->flags.testFlag(HideSingleWidgetTitleBar) && openedDockWidgetsCount() == 1);
        hidden &= !bAutoHide;
        d->titleBar->setVisible(!hidden);
    }

    if (isAutoHideFeatureEnabled())
    {
        auto tabBar = d->titleBar->tabBar();
        tabBar->setVisible(!bAutoHide);
        d->titleBar->autoHideTitleLabel()->setVisible(bAutoHide);
        updateTitleBarButtonVisibility(container->topLevelDockArea() == this);
    }
}

void QdfDockAreaWidget::markTitleBarMenuOutdated()
{
    QDF_D(QdfDockAreaWidget)
    if (d->titleBar)
    {
        d->titleBar->markTabsMenuOutdated();
    }
}

void QdfDockAreaWidget::updateAutoHideButtonCheckState()
{
    auto autoHideButton = titleBarButton(TitleBarButton::AutoHide);
    autoHideButton->blockSignals(true);
    autoHideButton->setChecked(autoHide());
    autoHideButton->blockSignals(false);
}

void QdfDockAreaWidget::updateTitleBarButtonVisibility(bool isTopLevel)
{
    QDF_D(QdfDockAreaWidget)
    d->updateTitleBarButtonVisibility(isTopLevel);
}

void QdfDockAreaWidget::updateTitleBarButtonsToolTips()
{
    internal::setToolTip(titleBarButton(TitleBarButton::Close),
                         titleBar()->titleBarButtonToolTip(TitleBarButton::Close));
    internal::setToolTip(titleBarButton(TitleBarButton::AutoHide),
                         titleBar()->titleBarButtonToolTip(TitleBarButton::AutoHide));
}

QdfDockWidget *QdfDockAreaWidget::nextOpenDockWidget(QdfDockWidget *dockWidget) const
{
    auto openDockWidgets = openedDockWidgets();
    if (openDockWidgets.count() > 1 || (openDockWidgets.count() == 1 && openDockWidgets[0] != dockWidget))
    {
        if (openDockWidgets.last() == dockWidget)
        {
            QdfDockWidget *NextDockWidget = openDockWidgets[openDockWidgets.count() - 2];
            for (int i = openDockWidgets.count() - 2; i >= 0; --i)
            {
                auto dw = openDockWidgets[i];
                if (!dw->features().testFlag(QdfDockWidget::NoTab))
                {
                    return dw;
                }
            }

            return NextDockWidget;
        }
        else
        {
            int IndexOfDockWidget = openDockWidgets.indexOf(dockWidget);
            QdfDockWidget *NextDockWidget = openDockWidgets[IndexOfDockWidget + 1];
            for (int i = IndexOfDockWidget + 1; i < openDockWidgets.count(); ++i)
            {
                auto dw = openDockWidgets[i];
                if (!dw->features().testFlag(QdfDockWidget::NoTab))
                {
                    return dw;
                }
            }

            for (int i = IndexOfDockWidget - 1; i >= 0; --i)
            {
                auto dw = openDockWidgets[i];
                if (!dw->features().testFlag(QdfDockWidget::NoTab))
                {
                    return dw;
                }
            }

            return NextDockWidget;
        }
    }
    else
    {
        return nullptr;
    }
}

QdfDockWidget::DockWidgetFeatures QdfDockAreaWidget::features(BitwiseOperator Mode) const
{
    if (BitwiseOperator::BitwiseAnd == Mode)
    {
        QdfDockWidget::DockWidgetFeatures Features(QdfDockWidget::AllDockWidgetFeatures);
        for (const auto dockWidget: dockWidgets())
        {
            Features &= dockWidget->features();
        }
        return Features;
    }
    else
    {
        QdfDockWidget::DockWidgetFeatures Features(QdfDockWidget::NoDockWidgetFeatures);
        for (const auto dockWidget: dockWidgets())
        {
            Features |= dockWidget->features();
        }
        return Features;
    }
}

void QdfDockAreaWidget::toggleView(bool open)
{
    setVisible(open);
    Q_EMIT viewToggled(open);
}

void QdfDockAreaWidget::setVisible(bool visible)
{
    QDF_D(QdfDockAreaWidget)
    QFrame::setVisible(visible);
    if (d->updateTitleBarButtons)
    {
        d->updateTitleBarButtonStates();
    }
}

void QdfDockAreaWidget::setAllowedAreas(DockWidgetAreas areas)
{
    QDF_D(QdfDockAreaWidget)
    d->allowedAreas = areas;
}

DockWidgetAreas QdfDockAreaWidget::allowedAreas() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->allowedAreas;
}

QdfDockAreaWidget::DockAreaFlags QdfDockAreaWidget::dockAreaFlags() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->flags;
}

void QdfDockAreaWidget::setDockAreaFlags(DockAreaFlags flags)
{
    QDF_D(QdfDockAreaWidget)
    auto changedFlags = d->flags ^ flags;
    d->flags = flags;
    if (changedFlags.testFlag(HideSingleWidgetTitleBar))
    {
        updateTitleBarVisibility();
    }
}

void QdfDockAreaWidget::setDockAreaFlag(eDockAreaFlag flag, bool on)
{
    auto flags = dockAreaFlags();
    internal::setFlag(flags, flag, on);
    setDockAreaFlags(flags);
}

QAbstractButton *QdfDockAreaWidget::titleBarButton(TitleBarButton which) const
{
    QDF_D(const QdfDockAreaWidget)
    return d->titleBar->button(which);
}

void QdfDockAreaWidget::closeArea()
{
    auto OpenDockWidgets = openedDockWidgets();
    if (OpenDockWidgets.count() == 1 &&
        (OpenDockWidgets[0]->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose) || OpenDockWidgets[0]->features().testFlag(QdfDockWidget::CustomCloseHandling)) && !autoHide())
    {
        OpenDockWidgets[0]->closeDockWidgetInternal();
    }
    else
    {
        for (auto dockWidget: openedDockWidgets())
        {
            if ((dockWidget->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose) && dockWidget->features().testFlag(QdfDockWidget::DockWidgetForceCloseWithArea)) ||
                dockWidget->features().testFlag(QdfDockWidget::CustomCloseHandling))
            {
                dockWidget->closeDockWidgetInternal();
            }
            else if (dockWidget->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose) && autoHide())
            {
                dockWidget->closeDockWidgetInternal();
            }
            else
            {
                dockWidget->toggleView(false);
            }
        }
    }
}

enum eBorderLocation
{
    BorderNone = 0,
    BorderLeft = 0x01,
    BorderRight = 0x02,
    BorderTop = 0x04,
    BorderBottom = 0x08,
    BorderVertical = BorderLeft | BorderRight,
    BorderHorizontal = BorderTop | BorderBottom,
    BorderTopLeft = BorderTop | BorderLeft,
    BorderTopRight = BorderTop | BorderRight,
    BorderBottomLeft = BorderBottom | BorderLeft,
    BorderBottomRight = BorderBottom | BorderRight,
    BorderVerticalBottom = BorderVertical | BorderBottom,
    BorderVerticalTop = BorderVertical | BorderTop,
    BorderHorizontalLeft = BorderHorizontal | BorderLeft,
    BorderHorizontalRight = BorderHorizontal | BorderRight,
    BorderAll = BorderVertical | BorderHorizontal
};

SideBarLocation QdfDockAreaWidget::calculateSideTabBarArea() const
{
    auto Container = dockContainer();
    auto ContentRect = Container->contentRect();

    int borders = BorderNone;
    auto DockAreaTopLeft = mapTo(Container, rect().topLeft());
    auto DockAreaRect = rect();
    DockAreaRect.moveTo(DockAreaTopLeft);
    const qreal aspectRatio = DockAreaRect.width() / (qMax(1, DockAreaRect.height()) * 1.0);
    const qreal sizeRatio = (qreal) ContentRect.width() / DockAreaRect.width();
    static const int MinBorderDistance = 16;
    bool HorizontalOrientation = (aspectRatio > 1.0) && (sizeRatio < 3.0);

    int BorderDistance[4];

    int Distance = qAbs(ContentRect.topLeft().y() - DockAreaRect.topLeft().y());
    BorderDistance[SideBarLocation::SL_Top] = (Distance < MinBorderDistance) ? 0 : Distance;
    if (!BorderDistance[SideBarLocation::SL_Top])
    {
        borders |= BorderTop;
    }

    Distance = qAbs(ContentRect.bottomRight().y() - DockAreaRect.bottomRight().y());
    BorderDistance[SideBarLocation::SL_Bottom] = (Distance < MinBorderDistance) ? 0 : Distance;
    if (!BorderDistance[SideBarLocation::SL_Bottom])
    {
        borders |= BorderBottom;
    }

    Distance = qAbs(ContentRect.topLeft().x() - DockAreaRect.topLeft().x());
    BorderDistance[SideBarLocation::SL_Left] = (Distance < MinBorderDistance) ? 0 : Distance;
    if (!BorderDistance[SideBarLocation::SL_Left])
    {
        borders |= BorderLeft;
    }

    Distance = qAbs(ContentRect.bottomRight().x() - DockAreaRect.bottomRight().x());
    BorderDistance[SideBarLocation::SL_Right] = (Distance < MinBorderDistance) ? 0 : Distance;
    if (!BorderDistance[SideBarLocation::SL_Right])
    {
        borders |= BorderRight;
    }

    auto SideTab = SideBarLocation::SL_Right;
    switch (borders)
    {
        case BorderAll:
            SideTab = HorizontalOrientation ? SideBarLocation::SL_Bottom : SideBarLocation::SL_Right;
            break;

        case BorderVerticalBottom:
            SideTab = SideBarLocation::SL_Bottom;
            break;
        case BorderVerticalTop:
            SideTab = SideBarLocation::SL_Top;
            break;
        case BorderHorizontalLeft:
            SideTab = SideBarLocation::SL_Left;
            break;
        case BorderHorizontalRight:
            SideTab = SideBarLocation::SL_Right;
            break;

        case BorderVertical:
            SideTab = SideBarLocation::SL_Bottom;
            break;
        case BorderHorizontal:
            SideTab = SideBarLocation::SL_Right;
            break;

        case BorderTopLeft:
            SideTab = HorizontalOrientation ? SideBarLocation::SL_Top : SideBarLocation::SL_Left;
            break;
        case BorderTopRight:
            SideTab = HorizontalOrientation ? SideBarLocation::SL_Top : SideBarLocation::SL_Right;
            break;
        case BorderBottomLeft:
            SideTab = HorizontalOrientation ? SideBarLocation::SL_Bottom : SideBarLocation::SL_Left;
            break;
        case BorderBottomRight:
            SideTab = HorizontalOrientation ? SideBarLocation::SL_Bottom : SideBarLocation::SL_Right;
            break;

        case BorderLeft:
            SideTab = SideBarLocation::SL_Left;
            break;
        case BorderRight:
            SideTab = SideBarLocation::SL_Right;
            break;
        case BorderTop:
            SideTab = SideBarLocation::SL_Top;
            break;
        case BorderBottom:
            SideTab = SideBarLocation::SL_Bottom;
            break;
    }

    return SideTab;
}

void QdfDockAreaWidget::setAutoHide(bool Enable, SideBarLocation Location)
{
    if (!isAutoHideFeatureEnabled())
    {
        return;
    }

    if (!Enable)
    {
        if (autoHide())
        {
            autoHideDockContainer()->moveContentsToParent();
        }
        return;
    }

    auto area = (SideBarLocation::SL_None == Location) ? calculateSideTabBarArea() : Location;
    for (const auto dockWidget: openedDockWidgets())
    {
        if (Enable == autoHide())
        {
            continue;
        }

        if (!dockWidget->features().testFlag(QdfDockWidget::DockWidgetPinnable))
        {
            continue;
        }

        dockContainer()->createAndSetupAutoHideContainer(area, dockWidget);
    }
}

void QdfDockAreaWidget::toggleAutoHide(SideBarLocation Location)
{
    if (!isAutoHideFeatureEnabled())
    {
        return;
    }

    setAutoHide(!autoHide(), Location);
}

void QdfDockAreaWidget::closeOtherAreas()
{
    dockContainer()->closeOtherAreas(this);
}

QdfDockAreaTitleBar *QdfDockAreaWidget::titleBar() const
{
    QDF_D(const QdfDockAreaWidget)
    return d->titleBar;
}

bool QdfDockAreaWidget::isCentralWidgetArea() const
{
    if (dockWidgetsCount() != 1)
    {
        return false;
    }

    return dockManager()->centralWidget() == dockWidgets().constFirst();
}

bool QdfDockAreaWidget::containsCentralWidget() const
{
    auto centralWidget = dockManager()->centralWidget();
    for (const auto &dockWidget: dockWidgets())
    {
        if (dockWidget == centralWidget)
        {
            return true;
        }
    }

    return false;
}

QSize QdfDockAreaWidget::minimumSizeHint() const
{
    QDF_D(const QdfDockAreaWidget)
    if (!d->minSizeHint.isValid())
    {
        return QFrame::minimumSizeHint();
    }

    if (d->titleBar->isVisible())
    {
        return d->minSizeHint + QSize(0, d->titleBar->minimumSizeHint().height());
    }
    else
    {
        return d->minSizeHint;
    }
}

void QdfDockAreaWidget::onDockWidgetFeaturesChanged()
{
    QDF_D(QdfDockAreaWidget)
    if (d->titleBar)
    {
        d->updateTitleBarButtonStates();
    }
}

bool QdfDockAreaWidget::isTopLevelArea() const
{
    auto container = dockContainer();
    if (!container)
    {
        return false;
    }

    return (container->topLevelDockArea() == this);
}

#ifdef Q_OS_WIN

bool QdfDockAreaWidget::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::PlatformSurface:
            return true;
        default:
            break;
    }

    return QFrame::event(e);
}
#endif