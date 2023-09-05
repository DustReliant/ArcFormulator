#include "private/qdfdockareatitlebar_p.h"
#include "qdfdockcomponentsfactory.h"
#include "qdfdockwidgethelper.h"
#include "qdffloatingdragpreview.h"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QToolButton>
#include <dockwidget/qdfautohidedockcontainer.h>
#include <dockwidget/qdfdockareatabbar.h>
#include <dockwidget/qdfdockareatitlebar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockfocuscontroller.h>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdfdockwidgettab.h>
#include <dockwidget/qdfelidinglabel.h>
#include <dockwidget/qdffloatingdockcontainer.h>

QDF_USE_NAMESPACE

static const char *const LocationProperty = "Location";

QdfDockManager *QdfDockAreaTitleBarPrivate::dockManager() const
{
    return dockArea->dockManager();
}
bool QdfDockAreaTitleBarPrivate::testConfigFlag(QdfDockManager::ConfigFlag flag)
{
    return QdfDockManager::testConfigFlag(flag);
}
bool QdfDockAreaTitleBarPrivate::testAutoHideConfigFlag(QdfDockManager::AutoHideFlag flag)
{
    return QdfDockManager::testAutoHideConfigFlag(flag);
}
bool QdfDockAreaTitleBarPrivate::isDraggingState(DragState state) const
{
    return this->dragState == state;
}

void QdfDockAreaTitleBarPrivate::createButtons()
{
    QDF_Q(QdfDockAreaTitleBar)
    QSizePolicy buttonSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    tabsMenuButton = new QdfTitleBarButton(testConfigFlag(QdfDockManager::DockAreaHasTabsMenuButton));
    tabsMenuButton->setObjectName("tabsMenuButton");
    tabsMenuButton->setAutoRaise(true);
    tabsMenuButton->setPopupMode(QToolButton::InstantPopup);
    internal::setButtonIcon(tabsMenuButton, QStyle::SP_TitleBarUnshadeButton, DockWidgetTabIcon::DockAreaMenu);
    QMenu *tabsMenu = new QMenu(tabsMenuButton);
#ifndef QT_NO_TOOLTIP
    tabsMenu->setToolTipsVisible(true);
#endif
    q->connect(tabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
    tabsMenuButton->setMenu(tabsMenu);
    internal::setToolTip(tabsMenuButton, QObject::tr("List All Tabs"));
    tabsMenuButton->setSizePolicy(buttonSizePolicy);
    pLayout->addWidget(tabsMenuButton, 0);
    q->connect(tabsMenuButton->menu(), SIGNAL(triggered(QAction *)),
               SLOT(onTabsMenuActionTriggered(QAction *)));

    undockButton = new QdfTitleBarButton(testConfigFlag(QdfDockManager::DockAreaHasUndockButton));
    undockButton->setObjectName("detachGroupButton");
    undockButton->setAutoRaise(true);
    internal::setToolTip(undockButton, QObject::tr("Detach Group"));
    internal::setButtonIcon(undockButton, QStyle::SP_TitleBarNormalButton, DockWidgetTabIcon::DockAreaUndock);
    undockButton->setSizePolicy(buttonSizePolicy);
    pLayout->addWidget(undockButton, 0);
    q->connect(undockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));

    const auto autoHideEnabled = testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled);
    autoHideButton = new QdfTitleBarButton(testAutoHideConfigFlag(QdfDockManager::DockAreaHasAutoHideButton) && autoHideEnabled);
    autoHideButton->setObjectName("dockAreaAutoHideButton");
    autoHideButton->setAutoRaise(true);
    internal::setToolTip(autoHideButton, q->titleBarButtonToolTip(TitleBarButton::AutoHide));
    internal::setButtonIcon(autoHideButton, QStyle::SP_DialogOkButton, DockWidgetTabIcon::AutoHide);
    autoHideButton->setSizePolicy(buttonSizePolicy);
    autoHideButton->setCheckable(testAutoHideConfigFlag(QdfDockManager::AutoHideButtonCheckable));
    autoHideButton->setChecked(false);
    pLayout->addWidget(autoHideButton, 0);
    q->connect(autoHideButton, SIGNAL(clicked()), SLOT(onAutoHideButtonClicked()));

    closeButton = new QdfTitleBarButton(testConfigFlag(QdfDockManager::DockAreaHasCloseButton));
    closeButton->setObjectName("dockAreaCloseButton");
    closeButton->setAutoRaise(true);
    internal::setButtonIcon(closeButton, QStyle::SP_TitleBarCloseButton, DockWidgetTabIcon::DockAreaClose);
    internal::setToolTip(closeButton, q->titleBarButtonToolTip(TitleBarButton::Close));
    closeButton->setSizePolicy(buttonSizePolicy);
    closeButton->setIconSize(QSize(16, 16));
    pLayout->addWidget(closeButton, 0);
    q->connect(closeButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
}

void QdfDockAreaTitleBarPrivate::createAutoHideTitleLabel()
{
    autoHideTitleLabel = new QdfElidingLabel("");
    autoHideTitleLabel->setObjectName("autoHideTitleLabel");
    pLayout->addWidget(autoHideTitleLabel);
}

void QdfDockAreaTitleBarPrivate::createTabBar()
{
    QDF_Q(QdfDockAreaTitleBar)
    tabBar = QdfDockComponentsFactory::instance()->createDockAreaTabBar(dockArea);
    tabBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pLayout->addWidget(tabBar);
    q->connect(tabBar, SIGNAL(tabClosed(int)), SLOT(markTabsMenuOutdated()));
    q->connect(tabBar, SIGNAL(tabOpened(int)), SLOT(markTabsMenuOutdated()));
    q->connect(tabBar, SIGNAL(tabInserted(int)), SLOT(markTabsMenuOutdated()));
    q->connect(tabBar, SIGNAL(removingTab(int)), SLOT(markTabsMenuOutdated()));
    q->connect(tabBar, SIGNAL(tabMoved(int, int)), SLOT(markTabsMenuOutdated()));
    q->connect(tabBar, SIGNAL(currentChanged(int)), SLOT(onCurrentTabChanged(int)));
    q->connect(tabBar, SIGNAL(tabBarClicked(int)), SIGNAL(tabBarClicked(int)));
    q->connect(tabBar, SIGNAL(elidedChanged(bool)), SLOT(markTabsMenuOutdated()));
}

QdfFloatingWidget *QdfDockAreaTitleBarPrivate::makeAreaFloating(const QPoint &offset, DragState state)
{
    QSize Size = dockArea->size();
    this->dragState = state;
    bool createFloatingDockContainer = (DragState::DS_FloatingWidget != state);
    QdfFloatingDockContainer *floatingDockContainer = nullptr;
    QdfFloatingWidget *floatingWidget = nullptr;
    if (createFloatingDockContainer)
    {
        if (dockArea->autoHideDockContainer())
        {
            dockArea->autoHideDockContainer()->cleanupAndDelete();
        }
        floatingWidget = floatingDockContainer = new QdfFloatingDockContainer(dockArea);
    }
    else
    {
        auto w = new QdfFloatingDragPreview(dockArea);
        QObject::connect(w, &QdfFloatingDragPreview::draggingCanceled, [=]() { this->dragState = DragState::DS_Inactive; });
        pFloatingWidget = w;
    }

    pFloatingWidget->startFloating(offset, Size, state, nullptr);
    if (floatingDockContainer)
    {
        auto topLevelDockWidget = floatingDockContainer->topLevelDockWidget();
        if (topLevelDockWidget)
        {
            topLevelDockWidget->emitTopLevelChanged(true);
        }
    }

    return floatingWidget;
}

void QdfDockAreaTitleBarPrivate::startFloating(const QPoint &offset)
{
    if (dockArea->autoHideDockContainer())
    {
        dockArea->autoHideDockContainer()->hide();
    }
    pFloatingWidget = makeAreaFloating(offset, DragState::DS_FloatingWidget);
    qApp->postEvent(dockArea, new QEvent((QEvent::Type) internal::DockedWidgetDragStartEvent));
}

QAction *QdfDockAreaTitleBarPrivate::createAutoHideToAction(const QString &title, SideBarLocation location,
                                                            QMenu *menu)
{
    QDF_Q(QdfDockAreaTitleBar)
    auto Action = menu->addAction(title);
    Action->setProperty("Location", QVariant(static_cast<int>(location)));
    QObject::connect(Action, &QAction::triggered, q, &QdfDockAreaTitleBar::onAutoHideToActionClicked);
    return Action;
}

QdfDockAreaTitleBar::QdfDockAreaTitleBar(QdfDockAreaWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockAreaTitleBar)
    QDF_D(QdfDockAreaTitleBar)
    d->dockArea = parent;

    setObjectName("dockAreaTitleBar");
    d->pLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    d->pLayout->setContentsMargins(0, 0, 0, 0);
    d->pLayout->setSpacing(0);
    setLayout(d->pLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    d->createTabBar();
    d->createAutoHideTitleLabel();
    d->autoHideTitleLabel->setVisible(false);// Default hidden
    d->pLayout->addWidget(new QdfSpacerWidget(this));
    d->createButtons();

    setFocusPolicy(Qt::NoFocus);
}

QdfDockAreaTitleBar::~QdfDockAreaTitleBar()
{
    QDF_D(QdfDockAreaTitleBar)
    if (!d->closeButton.isNull())
    {
        delete d->closeButton;
    }

    if (!d->tabsMenuButton.isNull())
    {
        delete d->tabsMenuButton;
    }

    if (!d->undockButton.isNull())
    {
        delete d->undockButton;
    }
    QDF_FINI_PRIVATE()
}

QdfDockAreaTabBar *QdfDockAreaTitleBar::tabBar() const
{
    QDF_D(const QdfDockAreaTitleBar)
    return d->tabBar;
}

void QdfDockAreaTitleBar::markTabsMenuOutdated()
{
    QDF_D(QdfDockAreaTitleBar)
    if (QdfDockAreaTitleBarPrivate::testConfigFlag(QdfDockManager::DockAreaDynamicTabsMenuButtonVisibility))
    {
        bool hasElidedTabTitle = false;
        for (int i = 0; i < d->tabBar->count(); ++i)
        {
            if (!d->tabBar->isTabOpen(i))
            {
                continue;
            }
            QdfDockWidgetTab *Tab = d->tabBar->tab(i);
            if (Tab->isTitleElided())
            {
                hasElidedTabTitle = true;
                break;
            }
        }
        bool visible = (hasElidedTabTitle && (d->tabBar->count() > 1));
        QMetaObject::invokeMethod(d->tabsMenuButton, "setVisible", Qt::QueuedConnection, Q_ARG(bool, visible));
    }
    d->menuOutdated = true;
}

void QdfDockAreaTitleBar::onTabsMenuAboutToShow()
{
    QDF_D(QdfDockAreaTitleBar)
    if (!d->menuOutdated)
    {
        return;
    }

    QMenu *menu = d->tabsMenuButton->menu();
    menu->clear();
    for (int i = 0; i < d->tabBar->count(); ++i)
    {
        if (!d->tabBar->isTabOpen(i))
        {
            continue;
        }
        auto Tab = d->tabBar->tab(i);
        QAction *Action = menu->addAction(Tab->icon(), Tab->text());
        internal::setToolTip(Action, Tab->toolTip());
        Action->setData(i);
    }

    d->menuOutdated = false;
}

void QdfDockAreaTitleBar::onCloseButtonClicked()
{
    QDF_D(QdfDockAreaTitleBar)
    if (QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideCloseButtonCollapsesDock) &&
        d->dockArea->autoHideDockContainer())
    {
        d->dockArea->autoHideDockContainer()->collapseView(true);
    }
    else if (d->testConfigFlag(QdfDockManager::DockAreaCloseButtonClosesTab))
    {
        d->tabBar->closeTab(d->tabBar->currentIndex());
    }
    else
    {
        d->dockArea->closeArea();
    }
}

void QdfDockAreaTitleBar::onUndockButtonClicked()
{
    QDF_D(QdfDockAreaTitleBar)
    if (d->dockArea->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        d->makeAreaFloating(mapFromGlobal(QCursor::pos()), DragState::DS_Inactive);
    }
}

void QdfDockAreaTitleBar::onTabsMenuActionTriggered(QAction *action)
{
    QDF_D(QdfDockAreaTitleBar)
    int index = action->data().toInt();
    d->tabBar->setCurrentIndex(index);
    Q_EMIT tabBarClicked(index);
}

void QdfDockAreaTitleBar::updateDockWidgetActionsButtons()
{
    QDF_D(QdfDockAreaTitleBar)
    auto Tab = d->tabBar->currentTab();
    if (!Tab)
    {
        return;
    }

    QdfDockWidget *dockWidget = Tab->dockWidget();
    if (!d->dockWidgetActionsButtons.isEmpty())
    {
        for (auto Button: d->dockWidgetActionsButtons)
        {
            d->pLayout->removeWidget(Button);
            delete Button;
        }
        d->dockWidgetActionsButtons.clear();
    }

    auto actions = dockWidget->titleBarActions();
    if (actions.isEmpty())
    {
        return;
    }

    int insertIndex = indexOf(d->tabsMenuButton);
    for (auto &action: actions)
    {
        auto button = new QdfTitleBarButton(true, this);
        button->setDefaultAction(action);
        button->setAutoRaise(true);
        button->setPopupMode(QToolButton::InstantPopup);
        button->setObjectName(action->objectName());
        d->pLayout->insertWidget(insertIndex++, button, 0);
        d->dockWidgetActionsButtons.append(button);
    }
}

void QdfDockAreaTitleBar::onCurrentTabChanged(int index)
{
    QDF_D(QdfDockAreaTitleBar)
    if (index < 0)
    {
        return;
    }

    if (d->testConfigFlag(QdfDockManager::DockAreaCloseButtonClosesTab))
    {
        QdfDockWidget *DockWidget = d->tabBar->tab(index)->dockWidget();
        d->closeButton->setEnabled(DockWidget->features().testFlag(QdfDockWidget::DockWidgetClosable));
    }

    updateDockWidgetActionsButtons();
}

void QdfDockAreaTitleBar::onAutoHideButtonClicked()
{
    QDF_D(QdfDockAreaTitleBar)
    if (QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideButtonTogglesArea) || qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        d->dockArea->toggleAutoHide();
    }
    else
    {
        d->dockArea->currentDockWidget()->toggleAutoHide();
    }
}

void QdfDockAreaTitleBar::onAutoHideDockAreaActionClicked()
{
    QDF_D(QdfDockAreaTitleBar)
    d->dockArea->toggleAutoHide();
}

void QdfDockAreaTitleBar::onAutoHideToActionClicked()
{
    QDF_D(QdfDockAreaTitleBar)
    int location = sender()->property(LocationProperty).toInt();
    d->dockArea->toggleAutoHide((SideBarLocation) location);
}

QAbstractButton *QdfDockAreaTitleBar::button(TitleBarButton which) const
{
    QDF_D(const QdfDockAreaTitleBar)
    switch (which)
    {
        case TitleBarButton::TabsMenu:
            return d->tabsMenuButton;
        case TitleBarButton::Undock:
            return d->undockButton;
        case TitleBarButton::Close:
            return d->closeButton;
        case TitleBarButton::AutoHide:
            return d->autoHideButton;
        default:
            return nullptr;
    }
}

QdfElidingLabel *QdfDockAreaTitleBar::autoHideTitleLabel() const
{
    QDF_D(const QdfDockAreaTitleBar)
    return d->autoHideTitleLabel;
}

void QdfDockAreaTitleBar::setVisible(bool visible)
{
    QFrame::setVisible(visible);
    markTabsMenuOutdated();
}

void QdfDockAreaTitleBar::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfDockAreaTitleBar)
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
        d->dragStartMousePos = event->pos();
        d->dragState = DragState::DS_MousePressed;

        if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
        {
            d->dockManager()->dockFocusController()->setDockWidgetTabFocused(d->tabBar->currentTab());
        }
        return;
    }
    QFrame::mousePressEvent(event);
}

void QdfDockAreaTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    QDF_D(QdfDockAreaTitleBar)
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
        auto currentDragState = d->dragState;
        d->dragStartMousePos = QPoint();
        d->dragState = DragState::DS_Inactive;
        if (DragState::DS_FloatingWidget == currentDragState)
        {
            d->pFloatingWidget->finishDragging();
        }

        return;
    }
    QFrame::mouseReleaseEvent(event);
}

void QdfDockAreaTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfDockAreaTitleBar)
    QFrame::mouseMoveEvent(event);
    if (!(event->buttons() & Qt::LeftButton) || d->isDraggingState(DragState::DS_Inactive))
    {
        d->dragState = DragState::DS_Inactive;
        return;
    }

    if (d->isDraggingState(DragState::DS_FloatingWidget))
    {
        d->pFloatingWidget->moveFloating();
        return;
    }

    if (d->dockArea->dockContainer()->isFloating() && d->dockArea->dockContainer()->visibleDockAreaCount() == 1 && !d->dockArea->autoHide())
    {
        return;
    }
    auto Features = d->dockArea->features();
    if (!Features.testFlag(QdfDockWidget::DockWidgetFloatable) && !(Features.testFlag(QdfDockWidget::DockWidgetMovable)))
    {
        return;
    }

    int DragDistance = (d->dragStartMousePos - event->pos()).manhattanLength();
    if (DragDistance >= QdfDockManager::startDragDistance())
    {
        d->startFloating(d->dragStartMousePos);
        auto Overlay = d->dockArea->dockManager()->containerOverlay();
        Overlay->setAllowedAreas(OuterDockAreas);
    }

    return;
}

void QdfDockAreaTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    QDF_D(QdfDockAreaTitleBar)
    if (d->dockArea->dockContainer()->isFloating() && d->dockArea->dockContainer()->dockAreaCount() == 1)
    {
        return;
    }

    if (!d->dockArea->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        return;
    }

    d->makeAreaFloating(event->pos(), DragState::DS_Inactive);
}

void QdfDockAreaTitleBar::contextMenuEvent(QContextMenuEvent *event)
{
    QDF_D(QdfDockAreaTitleBar)
    event->accept();
    if (d->isDraggingState(DragState::DS_FloatingWidget))
    {
        return;
    }

    const bool autoHide = d->dockArea->autoHide();
    const bool isTopLevelArea = d->dockArea->isTopLevelArea();
    QAction *action;
    QMenu pMenu(this);
    if (!isTopLevelArea)
    {
        action = pMenu.addAction(autoHide ? tr("Detach") : tr("Detach Group"),
                                 this, SLOT(onUndockButtonClicked()));
        action->setEnabled(d->dockArea->features().testFlag(QdfDockWidget::DockWidgetFloatable));
        if (QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
        {
            action = pMenu.addAction(autoHide ? tr("Unpin (Dock)") : tr("Pin Group"), this, SLOT(onAutoHideDockAreaActionClicked()));
            auto areaIsPinnable = d->dockArea->features().testFlag(QdfDockWidget::DockWidgetPinnable);
            action->setEnabled(areaIsPinnable);

            if (!autoHide)
            {
                auto menu = pMenu.addMenu(tr("Pin Group To..."));
                menu->setEnabled(areaIsPinnable);
                d->createAutoHideToAction(tr("Top"), SideBarLocation::SL_Top, menu);
                d->createAutoHideToAction(tr("Left"), SideBarLocation::SL_Left, menu);
                d->createAutoHideToAction(tr("Right"), SideBarLocation::SL_Right, menu);
                d->createAutoHideToAction(tr("Bottom"), SideBarLocation::SL_Bottom, menu);
            }
        }
        pMenu.addSeparator();
    }
    action = pMenu.addAction(autoHide ? tr("Close") : tr("Close Group"), this, SLOT(onCloseButtonClicked()));
    action->setEnabled(d->dockArea->features().testFlag(QdfDockWidget::DockWidgetClosable));
    if (!autoHide && !isTopLevelArea)
    {
        action = pMenu.addAction(tr("Close Other Groups"), d->dockArea, SLOT(closeOtherAreas()));
    }
    pMenu.exec(event->globalPos());
}

void QdfDockAreaTitleBar::insertWidget(int index, QWidget *widget)
{
    QDF_D(QdfDockAreaTitleBar)
    d->pLayout->insertWidget(index, widget);
}

int QdfDockAreaTitleBar::indexOf(QWidget *widget) const
{
    QDF_D(const QdfDockAreaTitleBar)
    return d->pLayout->indexOf(widget);
}

QString QdfDockAreaTitleBar::titleBarButtonToolTip(TitleBarButton Button) const
{
    QDF_D(const QdfDockAreaTitleBar)
    switch (Button)
    {
        case TitleBarButton::AutoHide:
            if (d->dockArea->autoHide())
            {
                return tr("Unpin (Dock)");
            }

            if (QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideButtonTogglesArea))
            {
                return tr("Pin Group");
            }
            else
            {
                return tr("Pin Active Tab (Press Ctrl to Pin Group)");
            }
            break;

        case TitleBarButton::Close:
            if (d->dockArea->autoHide())
            {
                return tr("Close");
            }

            if (QdfDockManager::testConfigFlag(QdfDockManager::DockAreaCloseButtonClosesTab))
            {
                return tr("Close Active Tab");
            }
            else
            {
                return tr("Close Group");
            }
            break;

        default:
            break;
    }

    return QString();
}

QdfTitleBarButton::QdfTitleBarButton(bool visible, QWidget *parent)
    : QToolButton(parent),
      Visible(visible),
      HideWhenDisabled(QdfDockManager::testConfigFlag(QdfDockManager::DockAreaHideDisabledButtons))
{
    setFocusPolicy(Qt::NoFocus);
}

void QdfTitleBarButton::setVisible(bool visible)
{
    visible = visible && this->Visible;

    if (visible && HideWhenDisabled)
    {
        visible = isEnabled();
    }

    QToolButton::setVisible(visible);
}

bool QdfTitleBarButton::event(QEvent *event)
{
    if (QEvent::EnabledChange == event->type() && HideWhenDisabled)
    {
        QMetaObject::invokeMethod(this, "setVisible", Qt::QueuedConnection, Q_ARG(bool, isEnabled()));
    }

    return QToolButton::event(event);
}

QdfSpacerWidget::QdfSpacerWidget(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("border: none; background: none;");
}