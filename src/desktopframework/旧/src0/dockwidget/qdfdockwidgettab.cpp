#include "private/qdfdockwidgettab_p.h"
#include "qdfdockwidgethelper.h"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QToolButton>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockfocuscontroller.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdfdockwidgettab.h>
#include <dockwidget/qdfelidinglabel.h>

QDF_USE_NAMESPACE

static const char *const LocationProperty = "Location";

void QdfDockWidgetTabPrivate::createLayout()
{
    QDF_Q(QdfDockWidgetTab)
    titleLabel = new QdfElidingLabel();
    titleLabel->setElideMode(Qt::ElideRight);
    titleLabel->setText(dockWidget->windowTitle());
    titleLabel->setObjectName("dockWidgetTabLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    q->connect(titleLabel, SIGNAL(elidedChanged(bool)), SIGNAL(elidedChanged(bool)));

    closeButton = createCloseButton();
    closeButton->setObjectName("tabCloseButton");
    internal::setButtonIcon(closeButton, QStyle::SP_TitleBarCloseButton, DockWidgetTabIcon::TabClose);
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setFocusPolicy(Qt::NoFocus);
    updateCloseButtonSizePolicy();
    internal::setToolTip(closeButton, QObject::tr("Close Tab"));
    q->connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

    QFontMetrics fm(titleLabel->font());
    int spacing = qRound(fm.height() / 4.0);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(2 * spacing, 0, 0, 0);
    layout->setSpacing(0);
    q->setLayout(layout);
    layout->addWidget(titleLabel, 1);
    layout->addSpacing(spacing);
    layout->addWidget(closeButton);
    layout->addSpacing(qRound(spacing * 4.0 / 3.0));
    layout->setAlignment(Qt::AlignCenter);

    titleLabel->setVisible(true);
}

void QdfDockWidgetTabPrivate::moveTab(QMouseEvent *event)
{
    QDF_Q(QdfDockWidgetTab)
    event->accept();
    QPoint Distance = internal::globalPositionOf(event) - globalDragStartMousePosition;
    Distance.setY(0);
    auto targetPos = Distance + tabDragStartPosition;
    targetPos.rx() = qMax(targetPos.x(), 0);
    targetPos.rx() = qMin(q->parentWidget()->rect().right() - q->width() + 1, targetPos.rx());
    q->move(targetPos);
    q->raise();
}

bool QdfDockWidgetTabPrivate::isDraggingState(DragState state) const
{
    return this->dragState == state;
}

bool QdfDockWidgetTabPrivate::startFloating(DragState state)
{
    QDF_Q(QdfDockWidgetTab)
    auto dockContainer = dockWidget->dockContainer();
    if (dockContainer->isFloating() && (dockContainer->visibleDockAreaCount() == 1) && (dockWidget->dockAreaWidget()->dockWidgetsCount() == 1))
    {
        return false;
    }
    dragState = state;
    QdfFloatingWidget *floatingWidget = nullptr;
    bool createContainer = (DragState::DS_FloatingWidget != state);

    QSize Size;
    if (dockArea->dockWidgetsCount() > 1)
    {
        floatingWidget = createFloatingWidget(dockWidget, createContainer);
        Size = dockWidget->size();
    }
    else
    {
        floatingWidget = createFloatingWidget(dockArea, createContainer);
        Size = dockArea->size();
    }

    if (DragState::DS_FloatingWidget == state)
    {
        floatingWidget->startFloating(dragStartMousePosition, Size, DragState::DS_FloatingWidget, q);
        auto dockManager = dockWidget->dockManager();
        auto overlay = dockManager->containerOverlay();
        overlay->setAllowedAreas(OuterDockAreas);
        this->floatingWidget = floatingWidget;
        qApp->postEvent(dockWidget, new QEvent((QEvent::Type) internal::DockedWidgetDragStartEvent));
    }
    else
    {
        floatingWidget->startFloating(dragStartMousePosition, Size, DragState::DS_Inactive, nullptr);
    }

    return true;
}

bool QdfDockWidgetTabPrivate::testConfigFlag(QdfDockManager::ConfigFlag flag) const
{
    return QdfDockManager::testConfigFlag(flag);
}

QAbstractButton *QdfDockWidgetTabPrivate::createCloseButton() const
{
    if (testConfigFlag(QdfDockManager::TabCloseButtonIsToolButton))
    {
        auto Button = new QToolButton();
        Button->setAutoRaise(true);
        return Button;
    }
    else
    {
        return new QPushButton();
    }
}

void QdfDockWidgetTabPrivate::updateCloseButtonVisibility(bool active)
{
    bool dockWidgetClosable = dockWidget->features().testFlag(QdfDockWidget::DockWidgetClosable);
    bool activeTabHasCloseButton = testConfigFlag(QdfDockManager::ActiveTabHasCloseButton);
    bool allTabsHaveCloseButton = testConfigFlag(QdfDockManager::AllTabsHaveCloseButton);
    bool tabHasCloseButton = (activeTabHasCloseButton && active) | allTabsHaveCloseButton;
    closeButton->setVisible(dockWidgetClosable && tabHasCloseButton);
}

void QdfDockWidgetTabPrivate::updateCloseButtonSizePolicy()
{
    auto features = dockWidget->features();
    auto sizePolicy = closeButton->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden(features.testFlag(QdfDockWidget::DockWidgetClosable) && testConfigFlag(QdfDockManager::RetainTabSizeWhenCloseButtonHidden));
    closeButton->setSizePolicy(sizePolicy);
}

void QdfDockWidgetTabPrivate::saveDragStartMousePosition(const QPoint &globalPos)
{
    QDF_Q(QdfDockWidgetTab)
    globalDragStartMousePosition = globalPos;
    dragStartMousePosition = q->mapFromGlobal(globalPos);
}

void QdfDockWidgetTabPrivate::updateIcon()
{
    QDF_Q(QdfDockWidgetTab)
    if (!iconLabel || icon.isNull())
    {
        return;
    }

    if (iconSize.isValid())
    {
        iconLabel->setPixmap(icon.pixmap(iconSize));
    }
    else
    {
        iconLabel->setPixmap(icon.pixmap(q->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, q)));
    }
    iconLabel->setVisible(true);
}

QdfDockFocusController *QdfDockWidgetTabPrivate::focusController() const
{
    return dockWidget->dockManager()->dockFocusController();
}

QAction *QdfDockWidgetTabPrivate::createAutoHideToAction(const QString &title, SideBarLocation location, QMenu *menu)
{
    QDF_Q(QdfDockWidgetTab)
    auto action = menu->addAction(title);
    action->setProperty("Location", location);
    QObject::connect(action, &QAction::triggered, q, &QdfDockWidgetTab::onAutoHideToActionClicked);
    return action;
}

QdfDockWidgetTab::QdfDockWidgetTab(QdfDockWidget *dockWidget, QWidget *parent)
    : QFrame(parent)
{
    QDF_INIT_PRIVATE(QdfDockWidgetTab)
    QDF_D(QdfDockWidgetTab)
    setAttribute(Qt::WA_NoMousePropagation, true);
    d->dockWidget = dockWidget;
    d->createLayout();
    setFocusPolicy(Qt::NoFocus);
}

QdfDockWidgetTab::~QdfDockWidgetTab()
{
    QDF_FINI_PRIVATE()
}

void QdfDockWidgetTab::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfDockWidgetTab)
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
        d->saveDragStartMousePosition(internal::globalPositionOf(event));
        d->dragState = DragState::DS_MousePressed;
        if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
        {
            d->focusController()->setDockWidgetTabFocused(this);
        }
        Q_EMIT clicked();
        return;
    }
    QFrame::mousePressEvent(event);
}

void QdfDockWidgetTab::mouseReleaseEvent(QMouseEvent *event)
{
    QDF_D(QdfDockWidgetTab)
    if (event->button() == Qt::LeftButton)
    {
        auto currentDragState = d->dragState;
        d->globalDragStartMousePosition = QPoint();
        d->dragStartMousePosition = QPoint();
        d->dragState = DragState::DS_Inactive;

        switch (currentDragState)
        {
            case DragState::DS_Tab:
                if (d->dockArea)
                {
                    event->accept();
                    Q_EMIT moved(internal::globalPositionOf(event));
                }
                break;

            case DragState::DS_FloatingWidget:
                event->accept();
                d->floatingWidget->finishDragging();
                break;

            default:;
        }
    }
    else if (event->button() == Qt::MiddleButton)
    {
        if (QdfDockManager::testConfigFlag(QdfDockManager::MiddleMouseButtonClosesTab) &&
            d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetClosable))
        {
            if (rect().contains(mapFromGlobal(QCursor::pos())))
            {
                event->accept();
                Q_EMIT closeRequested();
            }
        }
    }

    QFrame::mouseReleaseEvent(event);
}

void QdfDockWidgetTab::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfDockWidgetTab)
    if (!(event->buttons() & Qt::LeftButton) || d->isDraggingState(DragState::DS_Inactive))
    {
        d->dragState = DragState::DS_Inactive;
        QFrame::mouseMoveEvent(event);
        return;
    }

    if (d->isDraggingState(DragState::DS_FloatingWidget))
    {
        d->floatingWidget->moveFloating();
        QFrame::mouseMoveEvent(event);
        return;
    }

    if (d->isDraggingState(DragState::DS_Tab))
    {
        d->moveTab(event);
    }

    auto MappedPos = mapToParent(event->pos());
    bool MouseOutsideBar = (MappedPos.x() < 0) || (MappedPos.x() > parentWidget()->rect().right());
    int DragDistanceY = qAbs(d->globalDragStartMousePosition.y() - internal::globalPositionOf(event).y());
    if (DragDistanceY >= QdfDockManager::startDragDistance() || MouseOutsideBar)
    {
        if (d->dockArea->dockContainer()->isFloating() && d->dockArea->openedDockWidgetsCount() == 1 && d->dockArea->dockContainer()->visibleDockAreaCount() == 1)
        {
            return;
        }

        auto Features = d->dockWidget->features();
        if (Features.testFlag(QdfDockWidget::DockWidgetFloatable) || (Features.testFlag(QdfDockWidget::DockWidgetMovable)))
        {
            if (d->isDraggingState(DragState::DS_Tab))
            {
                parentWidget()->layout()->update();
            }
            d->startFloating();
        }
        return;
    }
    else if (d->dockArea->openedDockWidgetsCount() > 1 && (internal::globalPositionOf(event) - d->globalDragStartMousePosition).manhattanLength() >= QApplication::startDragDistance())
    {
        if (DragState::DS_Tab != d->dragState)
        {
            d->tabDragStartPosition = this->pos();
        }
        d->dragState = DragState::DS_Tab;
        return;
    }

    QFrame::mouseMoveEvent(event);
}

void QdfDockWidgetTab::contextMenuEvent(QContextMenuEvent *event)
{
    QDF_D(QdfDockWidgetTab)
    event->accept();
    if (d->isDraggingState(DragState::DS_FloatingWidget))
    {
        return;
    }

    d->saveDragStartMousePosition(event->globalPos());

    const bool isFloatable = d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetFloatable);
    const bool isNotOnlyTabInContainer = !d->dockArea->dockContainer()->hasTopLevelDockWidget();
    const bool isTopLevelArea = d->dockArea->isTopLevelArea();
    const bool isDetachable = isFloatable && isNotOnlyTabInContainer;
    QAction *action;
    QMenu pMenu(this);

    if (!isTopLevelArea)
    {
        action = pMenu.addAction(tr("Detach"), this, SLOT(detachDockWidget()));
        action->setEnabled(isDetachable);
        if (QdfDockManager::testAutoHideConfigFlag(QdfDockManager::AutoHideFeatureEnabled))
        {
            action = pMenu.addAction(tr("Pin"), this, SLOT(autoHideDockWidget()));
            auto IsPinnable = d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetPinnable);
            action->setEnabled(IsPinnable);

            auto menu = pMenu.addMenu(tr("Pin To..."));
            menu->setEnabled(IsPinnable);
            d->createAutoHideToAction(tr("Top"), SideBarLocation::SL_Top, menu);
            d->createAutoHideToAction(tr("Left"), SideBarLocation::SL_Left, menu);
            d->createAutoHideToAction(tr("Right"), SideBarLocation::SL_Right, menu);
            d->createAutoHideToAction(tr("Bottom"), SideBarLocation::SL_Bottom, menu);
        }
    }

    pMenu.addSeparator();
    action = pMenu.addAction(tr("Close"), this, SIGNAL(closeRequested()));
    action->setEnabled(isClosable());
    if (d->dockArea->openedDockWidgetsCount() > 1)
    {
        action = pMenu.addAction(tr("Close Others"), this, SIGNAL(closeOtherTabsRequested()));
    }
    pMenu.exec(event->globalPos());
}

bool QdfDockWidgetTab::isActiveTab() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->isActiveTab;
}

void QdfDockWidgetTab::setActiveTab(bool active)
{
    QDF_D(QdfDockWidgetTab)
    d->updateCloseButtonVisibility(active);

    if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting) && !d->dockWidget->dockManager()->isRestoringState())
    {
        bool UpdateFocusStyle = false;
        if (active && !hasFocus())
        {
            d->focusController()->setDockWidgetTabFocused(this);
            UpdateFocusStyle = true;
        }

        if (d->isActiveTab == active)
        {
            if (UpdateFocusStyle)
            {
                updateStyle();
            }
            return;
        }
    }
    else if (d->isActiveTab == active)
    {
        return;
    }

    d->isActiveTab = active;
    updateStyle();
    update();
    updateGeometry();

    Q_EMIT activeTabChanged();
}

QdfDockWidget *QdfDockWidgetTab::dockWidget() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->dockWidget;
}

void QdfDockWidgetTab::setDockAreaWidget(QdfDockAreaWidget *dockArea)
{
    QDF_D(QdfDockWidgetTab)
    d->dockArea = dockArea;
}

QdfDockAreaWidget *QdfDockWidgetTab::dockAreaWidget() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->dockArea;
}

void QdfDockWidgetTab::setIcon(const QIcon &icon)
{
    QDF_D(QdfDockWidgetTab)
    QBoxLayout *Layout = qobject_cast<QBoxLayout *>(layout());
    if (!d->iconLabel && icon.isNull())
    {
        return;
    }

    if (!d->iconLabel)
    {
        d->iconLabel = new QLabel();
        d->iconLabel->setAlignment(Qt::AlignVCenter);
        d->iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        internal::setToolTip(d->iconLabel, d->titleLabel->toolTip());
        Layout->insertWidget(0, d->iconLabel, Qt::AlignVCenter);
        Layout->insertSpacing(1, qRound(1.5 * Layout->contentsMargins().left() / 2.0));
    }
    else if (icon.isNull())
    {
        Layout->removeWidget(d->iconLabel);
        Layout->removeItem(Layout->itemAt(0));
        delete d->iconLabel;
        d->iconLabel = nullptr;
    }

    d->icon = icon;
    d->updateIcon();
}

const QIcon &QdfDockWidgetTab::icon() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->icon;
}

QString QdfDockWidgetTab::text() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->titleLabel->text();
}

void QdfDockWidgetTab::mouseDoubleClickEvent(QMouseEvent *event)
{
    QDF_D(QdfDockWidgetTab)
    if (event->button() == Qt::LeftButton)
    {
        if ((!d->dockArea->dockContainer()->isFloating() || d->dockArea->dockWidgetsCount() > 1) && d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetFloatable))
        {
            event->accept();
            d->saveDragStartMousePosition(internal::globalPositionOf(event));
            d->startFloating(DragState::DS_Inactive);
        }
    }

    QFrame::mouseDoubleClickEvent(event);
}

void QdfDockWidgetTab::setVisible(bool visible)
{
    QDF_D(QdfDockWidgetTab)
    visible &= !d->dockWidget->features().testFlag(QdfDockWidget::NoTab);
    QFrame::setVisible(visible);
}

void QdfDockWidgetTab::setText(const QString &title)
{
    QDF_D(const QdfDockWidgetTab)
    d->titleLabel->setText(title);
}

bool QdfDockWidgetTab::isTitleElided() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->titleLabel->isElide();
}

bool QdfDockWidgetTab::isClosable() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->dockWidget && d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetClosable);
}

void QdfDockWidgetTab::detachDockWidget()
{
    QDF_D(QdfDockWidgetTab)
    if (!d->dockWidget->features().testFlag(QdfDockWidget::DockWidgetFloatable))
    {
        return;
    }

    d->saveDragStartMousePosition(QCursor::pos());
    d->startFloating(DragState::DS_Inactive);
}

void QdfDockWidgetTab::autoHideDockWidget()
{
    QDF_D(QdfDockWidgetTab)
    d->dockWidget->setAutoHide(true);
}

void QdfDockWidgetTab::onAutoHideToActionClicked()
{
    QDF_D(QdfDockWidgetTab)
    int Location = sender()->property(LocationProperty).toInt();
    d->dockWidget->toggleAutoHide((SideBarLocation) Location);
}

bool QdfDockWidgetTab::event(QEvent *event)
{
    QDF_D(QdfDockWidgetTab)
#ifndef QT_NO_TOOLTIP
    if (event->type() == QEvent::ToolTipChange)
    {
        const auto text = toolTip();
        d->titleLabel->setToolTip(text);
        if (d->iconLabel)
        {
            d->iconLabel->setToolTip(text);
        }
    }
#endif
    if (event->type() == QEvent::StyleChange)
    {
        d->updateIcon();
    }
    return QFrame::event(event);
}

void QdfDockWidgetTab::onDockWidgetFeaturesChanged()
{
    QDF_D(QdfDockWidgetTab)
    d->updateCloseButtonSizePolicy();
    d->updateCloseButtonVisibility(isActiveTab());
}

void QdfDockWidgetTab::setElideMode(Qt::TextElideMode mode)
{
    QDF_D(QdfDockWidgetTab)
    d->titleLabel->setElideMode(mode);
}

void QdfDockWidgetTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

QSize QdfDockWidgetTab::iconSize() const
{
    QDF_D(const QdfDockWidgetTab)
    return d->iconSize;
}

void QdfDockWidgetTab::setIconSize(const QSize &size)
{
    QDF_D(QdfDockWidgetTab)
    d->iconSize = size;
    d->updateIcon();
}