#include "private/qdfribbon_p.h"
#include "private/qdfribbonbutton_p.h"
#include "private/qdfribbonstatusbar_p.h"
#include "qdfribbon_def.h"
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QEvent>
#include <QLayout>
#include <QPainter>
#include <QSizeGrip>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>
#include <QWidgetAction>
#include <qevent.h>
#include <ribbon/qdfribbonstatusbar.h>

QDF_USE_NAMESPACE

static QToolButton *findButtonByAction(const QWidget *parent, const QAction *action)
{
    const QObjectList &childrenGroup = parent->children();
    for (int i = 0, count = childrenGroup.count(); count > i; i++)
    {
        if (QToolButton *child = qobject_cast<QToolButton *>(childrenGroup.at(i)))
        {
            if (child->defaultAction() == action)
            {
                return child;
            }
        }
    }
    return nullptr;
}

static int findIndexByAction(const QWidget *parent, const QAction *action)
{
    const QObjectList &childrenGroup = parent->children();
    for (int i = 0, count = childrenGroup.count(); count > i; i++)
    {
        if (QToolButton *child = qobject_cast<QToolButton *>(childrenGroup.at(i)))
        {
            if (child->defaultAction() == action)
            {
                return i;
            }
        }
    }
    return -1;
}

static QLayout *qtcfindLayout(QLayout *lay, QWidget *wd)
{
    if (!lay)
    {
        return nullptr;
    }

    for (int i = 0, count = lay->count(); count > i; i++)
    {
        QLayoutItem *item = lay->itemAt(i);

        QWidget *widget = item->widget();
        if (widget == wd)
        {
            return lay;
        }

        if (QLayout *itemLayout = item->layout())
        {
            return qtcfindLayout(itemLayout, wd);
        }
    }

    return nullptr;
}


QdfStatusButton::QdfStatusButton(QWidget *parent) : QToolButton(parent)
{
}

QSize QdfStatusButton::sizeHint() const
{
    return QToolButton::sizeHint();
}

void QdfStatusButton::initStyleOptionButton(QStyleOptionToolButton &opt) const
{
    QToolButton::initStyleOption(&opt);

    int actualArea = opt.rect.height() * opt.rect.width();

    int index = -1;
    int curArea = 0;
    QList<QSize> lSz = opt.icon.availableSizes();
    for (int i = 0, count = lSz.count(); count > i; i++)
    {
        QSize curSz = lSz[i];
        int area = curSz.height() * curSz.width();
        if (actualArea > area)
        {
            if (area > curArea)
            {
                index = i;
            }
            curArea = area;
        }
    }
    opt.iconSize = index != -1 ? opt.icon.actualSize(lSz[index]) : opt.iconSize;
}

bool QdfStatusButton::event(QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        enterEvent(event);
        return true;
    }
    else
    {
        return QToolButton::event(event);
    }
}

void QdfStatusButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOptionButton(opt);
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

/* QdfStatusSeparator */
QdfStatusSeparator::QdfStatusSeparator(QWidget *parent) : QToolButton(parent)
{
}

QSize QdfStatusSeparator::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    QStyleOption opt;
    initStyleOption(&opt);
    const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
    sz.setWidth(extent);
    return sz;
}

void QdfStatusSeparator::initStyleOption(QStyleOption *option) const
{
    option->initFrom(this);
    option->state |= QStyle::State_Horizontal;
}

void QdfStatusSeparator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
}

/* QdfRibbonStatusBarSwitchGroupPrivate */
QdfRibbonStatusBarSwitchGroupPrivate::QdfRibbonStatusBarSwitchGroupPrivate()
{
}

void QdfRibbonStatusBarSwitchGroupPrivate::init()
{
    QDF_Q(QdfRibbonStatusBarSwitchGroup);
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);
}

QdfStatusButton *QdfRibbonStatusBarSwitchGroupPrivate::createButton(QAction *action) const
{
    QdfStatusButton *button = new QdfStatusButton();
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setDefaultAction(action);
    return button;
}

void QdfRibbonStatusBarSwitchGroupPrivate::updateIndexesButtons()
{
    QDF_Q(QdfRibbonStatusBarSwitchGroup)
    int index = 0;
    int switchCount = 0;
    const QObjectList &children = q->children();
    for (int i = 0, count = children.count(); count > i; i++)
    {
        if (QWidget *child = qobject_cast<QWidget *>(children.at(i)))
        {
            switchCount++;
            child->setProperty(_qdf_Index_Group, index);
            index++;
        }
    }
    q->setProperty(_qdf_Switch_Count, switchCount);
}

/* QdfRibbonStatusBarSwitchGroup */
QdfRibbonStatusBarSwitchGroup::QdfRibbonStatusBarSwitchGroup()
{
    QDF_INIT_PRIVATE(QdfRibbonStatusBarSwitchGroup);
    QDF_D(QdfRibbonStatusBarSwitchGroup);
    d->init();
}

QdfRibbonStatusBarSwitchGroup::~QdfRibbonStatusBarSwitchGroup()
{
    QDF_FINI_PRIVATE();
}

void QdfRibbonStatusBarSwitchGroup::actionEvent(QActionEvent *event)
{
    QDF_D(QdfRibbonStatusBarSwitchGroup)

    QAction *action = event->action();
    if (event->type() == QEvent::ActionAdded)
    {
        Q_ASSERT_X(::findButtonByAction(this, action) == nullptr, "QdfRibbonStatusBarSwitchGroup",
                   "widgets cannot be inserted multiple times");

        QdfStatusButton *newButton = d->createButton(action);

        int index = layout()->count();
        if (event->before())
        {
            int ind = ::findIndexByAction(this, event->before());
            if (ind != -1)
            {
                index = ind;
            }
            Q_ASSERT_X(index != -1, "QdfRibbonStatusBarSwitchGroup::insertAction", "internal error");
        }

        d->m_layout->insertWidget(index, newButton);
        d->updateIndexesButtons();
    }
    else if (event->type() == QEvent::ActionRemoved)
    {
        if (QToolButton *button = ::findButtonByAction(this, action))
        {
            d->m_layout->removeWidget(button);
            delete button;
            d->updateIndexesButtons();
        }
    }
}

void QdfRibbonStatusBarSwitchGroup::clear()
{
    QList<QAction *> acts = actions();
    for (int i = acts.size() - 1; i >= 0; --i)
    {
        removeAction(acts.at(i));
    }
}

/* QdfRibbonStatusBarPrivate */
QdfRibbonStatusBarPrivate::QdfRibbonStatusBarPrivate()
{
    m_dirty = false;
    m_addPermanentAction = false;
    m_indexGroupAction = 0;
    m_sizeGripWidget = nullptr;
}

QLayout *QdfRibbonStatusBarPrivate::findLayout()
{
    QDF_Q(QdfRibbonStatusBar)

    QLayout *l = q->layout();
    const QObjectList &children = q->children();
    for (int i = 0; i < children.size(); ++i)
    {
        if (QWidget *child = qobject_cast<QWidget *>(children.at(i)))
        {
            if (QLayout *internalLayout = ::qtcfindLayout(l, child))
            {
                return internalLayout;
            }
            else if (QSizeGrip *grip = qobject_cast<QSizeGrip *>(child))
            {
                m_sizeGripWidget = grip;
            }
        }
    }
    return nullptr;
}

void QdfRibbonStatusBarPrivate::updateLayout()
{
    if (m_dirty)
    {
        if (QLayout *layoutFind = findLayout())
        {
            layoutFind->setSpacing(0);
            layoutFind->setMargin(0);
            int permanentIndex = 0;
            for (int i = layoutFind->count() - 1; i >= 0; --i)
            {
                QLayoutItem *item = layoutFind->itemAt(i);
                if (dynamic_cast<QSpacerItem *>(item))
                {
                    permanentIndex++;
                }

                if (QWidget *widget = item->widget())
                {
                    if (permanentIndex != 2)
                    {
                        m_permanentWidgets.append(widget);
                    }
                    else
                    {
                        m_widgets.append(widget);
                    }
                }
            }
        }
        m_dirty = false;
    }
}

QRect QdfRibbonStatusBarPrivate::childrenPermanentRect() const
{
    QRect permanentRect;
    for (int i = 0, count = m_permanentWidgets.count(); count > i; i++)
    {
        permanentRect |= m_permanentWidgets.at(i)->geometry();
    }

    if (m_sizeGripWidget && m_sizeGripWidget->isVisible())
    {
        permanentRect |= m_sizeGripWidget->geometry();
    }
    return permanentRect;
}

QdfStatusButton *QdfRibbonStatusBarPrivate::createButton(QAction *action) const
{
    QdfStatusButton *button = new QdfStatusButton();
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setDefaultAction(action);
    return button;
}

QdfStatusSeparator *QdfRibbonStatusBarPrivate::createSeparator(QAction *action) const
{
    Q_UNUSED(action);
    QdfStatusSeparator *sep = new QdfStatusSeparator();
    return sep;
}

/* QdfRibbonStatusBar */
QdfRibbonStatusBar::QdfRibbonStatusBar(QWidget *parent) : QStatusBar(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonStatusBar);
}

QdfRibbonStatusBar::~QdfRibbonStatusBar()
{
    QDF_FINI_PRIVATE();
}

QAction *QdfRibbonStatusBar::addAction(const QString &text)
{
    QAction *action = new QAction(text, this);
    QStatusBar::addAction(action);
    return action;
}

QAction *QdfRibbonStatusBar::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    QStatusBar::addAction(action);
    return action;
}

void QdfRibbonStatusBar::addPermanentAction(QAction *action)
{
    QDF_D(QdfRibbonStatusBar);
    d->m_addPermanentAction = true;
    QStatusBar::addAction(action);
    d->m_addPermanentAction = false;
}

QRect QdfRibbonStatusBar::childrenPermanentRect() const
{
    QDF_D(const QdfRibbonStatusBar);
    return d->childrenPermanentRect();
}

bool QdfRibbonStatusBar::event(QEvent *event)
{
    QDF_D(QdfRibbonStatusBar);
    bool result = QStatusBar::event(event);

    if (!d->m_dirty && QEvent::ChildRemoved == event->type())
    {
        d->m_dirty = true;
        d->m_permanentWidgets.clear();
        d->m_widgets.clear();
        d->m_sizeGripWidget = nullptr;
    }

    return result;
}

void QdfRibbonStatusBar::paintEvent(QPaintEvent *event)
{
    QStatusBar::paintEvent(event);
}

void QdfRibbonStatusBar::actionEvent(QActionEvent *event)
{
    QDF_D(QdfRibbonStatusBar);
    QAction *action = event->action();
    if (event->type() == QEvent::ActionAdded)
    {
        Q_ASSERT_X(::findButtonByAction(this, action) == nullptr, "QdfRibbonStatusBar",
                   "widgets cannot be inserted multiple times");

        int index = d->m_addPermanentAction ? d->m_permanentWidgets.count() : d->m_widgets.count();
        if (event->before())
        {
            int ind = ::findIndexByAction(this, event->before());
            if (ind != -1)
            {
                index = ind;
            }
            Q_ASSERT_X(index != -1, "QdfRibbonStatusBar::insertAction", "internal error");
        }

        if (action->isSeparator())
        {
            QdfStatusSeparator *sep = d->createSeparator(action);
            if (!d->m_addPermanentAction)
            {
                insertWidget(index, sep);
            }
            else
            {
                insertPermanentWidget(index, sep);
            }
        }
        else
        {
            QdfStatusButton *button = d->createButton(action);
            if (!d->m_addPermanentAction)
            {
                insertWidget(index, button);
            }
            else
            {
                insertPermanentWidget(index, button);
            }
        }
    }
    else if (event->type() == QEvent::ActionRemoved)
    {
        if (QToolButton *button = ::findButtonByAction(this, action))
        {
            removeWidget(button);
            delete button;
            d->updateLayout();
        }
    }
}

void QdfRibbonStatusBar::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfRibbonStatusBar);
    QStatusBar::resizeEvent(event);
    d->updateLayout();
}
