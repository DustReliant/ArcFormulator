#include "private/qdfdockareatabbar_p.h"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QtGlobal>
#include <dockwidget/qdfdockareatabbar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockwidgettab.h>

QDF_USE_NAMESPACE

void QdfDockAreaTabBarPrivate::updateTabs()
{
    QDF_Q(QdfDockAreaTabBar)
    for (int i = 0; i < q->count(); ++i)
    {
        auto tabWidget = q->tab(i);
        if (!tabWidget)
        {
            continue;
        }

        if (i == currentIndex)
        {
            tabWidget->show();
            tabWidget->setActiveTab(true);
            q->ensureWidgetVisible(tabWidget);
        }
        else
        {
            tabWidget->setActiveTab(false);
        }
    }
}
QdfDockWidgetTab *QdfDockAreaTabBarPrivate::firstTab() const
{
    QDF_Q(const QdfDockAreaTabBar)
    return q->tab(0);
}
QdfDockWidgetTab *QdfDockAreaTabBarPrivate::lastTab() const
{
    QDF_Q(const QdfDockAreaTabBar)
    return q->tab(q->count() - 1);
}

QdfDockAreaTabBar::QdfDockAreaTabBar(QdfDockAreaWidget *parent) : QScrollArea(parent)
{
    QDF_INIT_PRIVATE(QdfDockAreaTabBar)
    QDF_D(QdfDockAreaTabBar)
    d->dockArea = parent;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d->tabsContainerWidget = new QWidget();
    d->tabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->tabsContainerWidget->setObjectName("tabsContainerWidget");
    d->tabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    d->tabsLayout->setContentsMargins(0, 0, 0, 0);
    d->tabsLayout->setSpacing(0);
    d->tabsLayout->addStretch(1);
    d->tabsContainerWidget->setLayout(d->tabsLayout);
    setWidget(d->tabsContainerWidget);

    setFocusPolicy(Qt::NoFocus);
}

QdfDockAreaTabBar::~QdfDockAreaTabBar()
{
    QDF_FINI_PRIVATE()
}

void QdfDockAreaTabBar::wheelEvent(QWheelEvent *event)
{
    event->accept();
    const int direction = event->angleDelta().y();
    if (direction < 0)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
    }
    else
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
    }
}

void QdfDockAreaTabBar::setCurrentIndex(int index)
{
    QDF_D(QdfDockAreaTabBar)
    if (index == d->currentIndex)
    {
        return;
    }

    if (index < -1 || index > (count() - 1))
    {
        qWarning() << Q_FUNC_INFO << "Invalid index" << index;
        return;
    }

    Q_EMIT currentChanging(index);
    d->currentIndex = index;
    d->updateTabs();
    updateGeometry();
    Q_EMIT currentChanged(index);
}

int QdfDockAreaTabBar::count() const
{
    QDF_D(const QdfDockAreaTabBar)
    return d->tabsLayout->count() - 1;
}

void QdfDockAreaTabBar::insertTab(int index, QdfDockWidgetTab *tab)
{
    QDF_D(QdfDockAreaTabBar)
    d->tabsLayout->insertWidget(index, tab);
    connect(tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
    connect(tab, SIGNAL(closeRequested()), this, SLOT(onTabCloseRequested()));
    connect(tab, SIGNAL(closeOtherTabsRequested()), this, SLOT(onCloseOtherTabsRequested()));
    connect(tab, SIGNAL(moved(QPoint)), this, SLOT(onTabWidgetMoved(QPoint)));
    connect(tab, SIGNAL(elidedChanged(bool)), this, SIGNAL(elidedChanged(bool)));
    tab->installEventFilter(this);
    Q_EMIT tabInserted(index);
    if (index <= d->currentIndex)
    {
        setCurrentIndex(d->currentIndex + 1);
    }
    else if (d->currentIndex == -1)
    {
        setCurrentIndex(index);
    }

    updateGeometry();
}

void QdfDockAreaTabBar::removeTab(QdfDockWidgetTab *Tab)
{
    QDF_D(QdfDockAreaTabBar)
    if (!count())
    {
        return;
    }
    int newCurrentIndex = currentIndex();
    int removeIndex = d->tabsLayout->indexOf(Tab);
    if (count() == 1)
    {
        newCurrentIndex = -1;
    }
    if (newCurrentIndex > removeIndex)
    {
        newCurrentIndex--;
    }
    else if (newCurrentIndex == removeIndex)
    {
        newCurrentIndex = -1;
        for (int i = (removeIndex + 1); i < count(); ++i)
        {
            if (tab(i)->isVisibleTo(this))
            {
                newCurrentIndex = i - 1;
                break;
            }
        }
        if (newCurrentIndex < 0)
        {
            for (int i = (removeIndex - 1); i >= 0; --i)
            {
                if (tab(i)->isVisibleTo(this))
                {
                    newCurrentIndex = i;
                    break;
                }
            }
        }
    }

    Q_EMIT removingTab(removeIndex);
    d->tabsLayout->removeWidget(Tab);
    Tab->disconnect(this);
    Tab->removeEventFilter(this);
    if (newCurrentIndex != d->currentIndex)
    {
        setCurrentIndex(newCurrentIndex);
    }
    else
    {
        d->updateTabs();
    }

    updateGeometry();
}

int QdfDockAreaTabBar::currentIndex() const
{
    QDF_D(const QdfDockAreaTabBar)
    return d->currentIndex;
}

QdfDockWidgetTab *QdfDockAreaTabBar::currentTab() const
{
    QDF_D(const QdfDockAreaTabBar)
    if (d->currentIndex < 0)
    {
        return nullptr;
    }
    else
    {
        return qobject_cast<QdfDockWidgetTab *>(d->tabsLayout->itemAt(d->currentIndex)->widget());
    }
}

void QdfDockAreaTabBar::onTabClicked()
{
    QDF_D(QdfDockAreaTabBar)
    QdfDockWidgetTab *Tab = qobject_cast<QdfDockWidgetTab *>(sender());
    if (!Tab)
    {
        return;
    }

    int index = d->tabsLayout->indexOf(Tab);
    if (index < 0)
    {
        return;
    }
    setCurrentIndex(index);
    Q_EMIT tabBarClicked(index);
}

void QdfDockAreaTabBar::onTabCloseRequested()
{
    QDF_D(QdfDockAreaTabBar)
    QdfDockWidgetTab *Tab = qobject_cast<QdfDockWidgetTab *>(sender());
    int index = d->tabsLayout->indexOf(Tab);
    closeTab(index);
}

void QdfDockAreaTabBar::onCloseOtherTabsRequested()
{
    QDF_D(QdfDockAreaTabBar)
    auto obj = qobject_cast<QdfDockWidgetTab *>(sender());
    for (int i = 0; i < count(); ++i)
    {
        auto Tab = tab(i);
        if (Tab->isClosable() && !Tab->isHidden() && Tab != obj)
        {
            int Offset = Tab->dockWidget()->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose)
                                 ? 1
                                 : 0;
            closeTab(i);
            if (Tab->dockWidget()->isClosed())
            {
                i -= Offset;
            }
        }
    }
}

QdfDockWidgetTab *QdfDockAreaTabBar::tab(int index) const
{
    QDF_D(const QdfDockAreaTabBar)
    if (index >= count() || index < 0)
    {
        return nullptr;
    }
    return qobject_cast<QdfDockWidgetTab *>(d->tabsLayout->itemAt(index)->widget());
}

void QdfDockAreaTabBar::onTabWidgetMoved(const QPoint &globalPos)
{
    QDF_D(QdfDockAreaTabBar)
    QdfDockWidgetTab *movingTab = qobject_cast<QdfDockWidgetTab *>(sender());
    if (!movingTab)
    {
        return;
    }

    int fromIndex = d->tabsLayout->indexOf(movingTab);
    auto mousePos = mapFromGlobal(globalPos);
    mousePos.rx() = qMax(d->firstTab()->geometry().left(), mousePos.x());
    mousePos.rx() = qMin(d->lastTab()->geometry().right(), mousePos.x());
    int toIndex = -1;
    for (int i = 0; i < count(); ++i)
    {
        QdfDockWidgetTab *dropTab = tab(i);
        if (dropTab == movingTab || !dropTab->isVisibleTo(this) ||
            !dropTab->geometry().contains(mousePos))
        {
            continue;
        }

        toIndex = d->tabsLayout->indexOf(dropTab);
        if (toIndex == fromIndex)
        {
            toIndex = -1;
        }
        break;
    }

    if (toIndex > -1)
    {
        d->tabsLayout->removeWidget(movingTab);
        d->tabsLayout->insertWidget(toIndex, movingTab);
        Q_EMIT tabMoved(fromIndex, toIndex);
        setCurrentIndex(toIndex);
    }
    else
    {
        d->tabsLayout->update();
    }
}

void QdfDockAreaTabBar::closeTab(int index)
{
    if (index < 0 || index >= count())
    {
        return;
    }

    auto Tab = tab(index);
    if (Tab->isHidden())
    {
        return;
    }
    Q_EMIT tabCloseRequested(index);
}

bool QdfDockAreaTabBar::eventFilter(QObject *watched, QEvent *event)
{
    QDF_D(QdfDockAreaTabBar)
    bool result = QScrollArea::eventFilter(watched, event);
    QdfDockWidgetTab *Tab = qobject_cast<QdfDockWidgetTab *>(watched);
    if (!Tab)
    {
        return result;
    }

    switch (event->type())
    {
        case QEvent::Hide:
            Q_EMIT tabClosed(d->tabsLayout->indexOf(Tab));
            updateGeometry();
            break;

        case QEvent::Show:
            Q_EMIT tabOpened(d->tabsLayout->indexOf(Tab));
            updateGeometry();
            break;

        case QEvent::LayoutRequest:
            updateGeometry();
            break;

        default:
            break;
    }

    return result;
}

bool QdfDockAreaTabBar::isTabOpen(int index) const
{
    if (index < 0 || index >= count())
    {
        return false;
    }

    return !tab(index)->isHidden();
}

QSize QdfDockAreaTabBar::minimumSizeHint() const
{
    QSize size = sizeHint();
    size.setWidth(10);
    return size;
}

QSize QdfDockAreaTabBar::sizeHint() const
{
    QDF_D(const QdfDockAreaTabBar)
    return d->tabsContainerWidget->sizeHint();
}