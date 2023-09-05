#include "private/qdfribbonbar_p.h"
#include "private/qdfribboncustomizemanager_p.h"
#include "private/qdfribbonquickaccessbar_p.h"
#include "qdfribbon_def.h"
#include <QApplication>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <ribbon/qdfribboncustomizemanager.h>
#include <ribbon/qdfribbonquickaccessbar.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

QDF_USE_NAMESPACE

QdfRibbonQuickAccessButton::QdfRibbonQuickAccessButton(QWidget *parent) : QToolButton(parent)
{
    setObjectName("QdfRibbonQuickAccessButton");
}

QdfRibbonQuickAccessButton::~QdfRibbonQuickAccessButton()
{
}

QSize QdfRibbonQuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    if (sz.isNull())
    {
        sz = QSize(16, 16);
    }
    return QSize(sz.height() * 13 / sz.width() + 1, sz.height());
}

void QdfRibbonQuickAccessButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonQuickAccessButton, &opt,
                           &p, this);
}

QdfRibbonQuickAccessBarPrivate::QdfRibbonQuickAccessBarPrivate()
{
    m_accessPopup = nullptr;
    m_menu = nullptr;
    m_actionAccessPopup = nullptr;
    m_customizeGroupAction = nullptr;
    m_removeAction = false;
    m_customizeAction = false;
}

QdfRibbonQuickAccessBarPrivate::~QdfRibbonQuickAccessBarPrivate()
{
}

void QdfRibbonQuickAccessBarPrivate::init()
{
    QDF_Q(QdfRibbonQuickAccessBar);
    q->setIconSize(QSize(16, 16));
    m_accessPopup = new QdfRibbonQuickAccessButton(q);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);
    m_actionAccessPopup->setProperty(__qdf_Quick_Access_Button,
                                     QLatin1String("__qtc_Quick_Access_Button"));

    m_customizeGroupAction = new QActionGroup(q);
    m_customizeGroupAction->setExclusive(false);
    QObject::connect(m_customizeGroupAction, SIGNAL(triggered(QAction *)), q,
                     SLOT(customizeAction(QAction *)));

    QObject::connect(m_menu, SIGNAL(aboutToShow()), q, SLOT(aboutToShowCustomizeMenu()));
    QObject::connect(m_menu, SIGNAL(aboutToHide()), q, SLOT(aboutToHideCustomizeMenu()));

    q->addAction(new QdfQuickAccessActionInvisible(q, m_customizeGroupAction));
}

QdfQuickAccessAction *QdfRibbonQuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
{
    if (m_customizeGroupAction == nullptr)
    {
        return nullptr;
    }

    QList<QAction *> list = m_customizeGroupAction->actions();
    for (int i = 0; i < list.count(); ++i)
    {
        QdfQuickAccessAction *act = dynamic_cast<QdfQuickAccessAction *>(list[i]);
        if (act && action == act->m_srcAction)
        {
            return act;
        }
    }
    return nullptr;
}

QAction *QdfRibbonQuickAccessBarPrivate::findBeforeAction(QAction *action) const
{
    QList<QAction *> list = m_customizeGroupAction->actions();
    bool find = false;
    for (int i = 0, count = list.count(); i < count; ++i)
    {
        if (find)
        {
            if (QdfQuickAccessAction *beforeAct = dynamic_cast<QdfQuickAccessAction *>(list[i]))
            {
                if (beforeAct->isChecked())
                {
                    return beforeAct->m_srcAction;
                }
            }
        }
        if (!find && action == list[i])
        {
            find = true;
        }
    }
    return nullptr;
}

void QdfRibbonQuickAccessBarPrivate::updateAction(QAction *action)
{
    if (QdfQuickAccessAction *wrapper = findQuickAccessAction(action))
    {
        wrapper->update();
    }
}

void QdfRibbonQuickAccessBarPrivate::updateParentLayout() const
{
    QDF_Q(const QdfRibbonQuickAccessBar);
    if (!q->isVisible())
    {
        return;
    }

    if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(q->parentWidget()))
    {
        if (ribbonBar->quickAccessBarPosition() == QdfRibbonBar::QATopPosition)
        {
            QApplication::postEvent(ribbonBar, new QResizeEvent(ribbonBar->size(), QSize(0, 0)));
        }
    }
}

void QdfRibbonQuickAccessBarPrivate::setActionVisible(QAction *action, bool visible)
{
    QDF_Q(QdfRibbonQuickAccessBar);
    if (QdfQuickAccessAction *wrapper = findQuickAccessAction(action))
    {
        if (visible)
        {
            if (m_customizeAction)
            {
                QAction *beforeAct = findBeforeAction(wrapper);
                q->insertAction(beforeAct, action);
            }
            else
            {
                q->addAction(action);
            }
        }
        else
        {
            q->removeAction(action);
            m_removeAction = false;
        }
        wrapper->update();
        q->adjustSize();
    }
}

QdfRibbonQuickAccessBar::QdfRibbonQuickAccessBar(QWidget *parent) : QToolBar(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonQuickAccessBar);
    QDF_D(QdfRibbonQuickAccessBar);
    d->init();
}

QdfRibbonQuickAccessBar::~QdfRibbonQuickAccessBar()
{
    QDF_FINI_PRIVATE();
}

QAction *QdfRibbonQuickAccessBar::actionCustomizeButton() const
{
    QDF_D(const QdfRibbonQuickAccessBar);
    return d->m_actionAccessPopup;
}

void QdfRibbonQuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    QDF_D(QdfRibbonQuickAccessBar);
    d->setActionVisible(action, visible);
}

bool QdfRibbonQuickAccessBar::isActionVisible(QAction *action) const
{
    QDF_D(const QdfRibbonQuickAccessBar);
    if (QdfQuickAccessAction *wrapper = d->findQuickAccessAction(action))
    {
        return wrapper->isChecked();
    }
    return false;
}

int QdfRibbonQuickAccessBar::visibleCount() const
{
    QDF_D(const QdfRibbonQuickAccessBar);
    int visibleCount = 0;
    QList<QAction *> list = d->m_customizeGroupAction->actions();
    for (int i = 0, count = list.count(); i < count; ++i)
    {
        if (QdfQuickAccessAction *beforeAct = dynamic_cast<QdfQuickAccessAction *>(list[i]))
        {
            if (beforeAct->isChecked())
            {
                visibleCount++;
            }
        }
    }
    return visibleCount;
}

QSize QdfRibbonQuickAccessBar::sizeHint() const
{
    QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget());
    if (ribbonBar == nullptr)
    {
        return QToolBar::sizeHint();
    }

    int heightTabs = ribbonBar->qdf_d()->heightTabs();
    return QSize(QToolBar::sizeHint().width(), heightTabs + 1).expandedTo(QApplication::globalStrut());
}

void QdfRibbonQuickAccessBar::customizeAction(QAction *action)
{
    QDF_D(QdfRibbonQuickAccessBar);
    d->m_customizeAction = true;
    if (QdfQuickAccessAction *act = dynamic_cast<QdfQuickAccessAction *>(action))
    {
        setActionVisible(act->m_srcAction, !widgetForAction(act->m_srcAction));
        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            if (QdfRibbonCustomizeManager *manager = ribbonBar->customizeManager())
            {
                manager->setEditMode();
                manager->qdf_d()->m_ribbonManager->setQuickAccessBar();
                manager->setEditMode(false);
            }
        }
    }
    d->m_customizeAction = false;
}

void QdfRibbonQuickAccessBar::aboutToShowCustomizeMenu()
{
    QDF_D(QdfRibbonQuickAccessBar);
    d->m_menu->clear();

    d->m_menu->setSeparatorsCollapsible(false);
    d->m_menu->addSeparator()->setText(
            QdfRibbonBar::tr_compatible(RibbonCustomizeQuickAccessToolBarString));

    QListIterator<QAction *> itAction(d->m_actionList);
    while (itAction.hasNext())
    {
        QAction *action = itAction.next();
        d->m_menu->addAction(action);
    }

    emit showCustomizeMenu(d->m_menu);
}

void QdfRibbonQuickAccessBar::aboutToHideCustomizeMenu()
{
    QDF_D(QdfRibbonQuickAccessBar);
    d->m_menu->clear();
}

bool QdfRibbonQuickAccessBar::event(QEvent *event)
{
    if ((QEvent::Hide == event->type() || QEvent::Show == event->type()) && parentWidget())
    {
        adjustSize();
        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            ribbonBar->updateLayout();
        }
    }
    return QToolBar::event(event);
}

void QdfRibbonQuickAccessBar::actionEvent(QActionEvent *event)
{
    QDF_D(QdfRibbonQuickAccessBar);

    QToolBar::actionEvent(event);

    if (d->m_actionAccessPopup)
    {
        if (event->type() == QEvent::ActionAdded)
        {
            if (!d->m_removeAction)
            {
                removeAction(d->m_actionAccessPopup);
            }

            QdfQuickAccessAction *quickAccessAction = d->findQuickAccessAction(event->action());
            if (event->action() != d->m_actionAccessPopup && !quickAccessAction &&
                !dynamic_cast<QdfQuickAccessActionInvisible *>(event->action()))
            {
                QdfQuickAccessAction *act = new QdfQuickAccessAction(this, event->action());

                bool addActionToMenu = true;

                if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
                {
                    if (QdfRibbonCustomizeManager *manager = ribbonBar->customizeManager())
                    {
                        if (manager->isEditMode())
                        {
                            QList<QAction *> defaultActions =
                                    manager->qdf_d()->m_ribbonManager->defaultToolBars().value(this);
                            if (!defaultActions.contains(event->action()))
                            {
                                addActionToMenu = false;
                            }
                        }
                    }
                }

                if (addActionToMenu)
                {
                    d->m_actionList.append(act);
                }

                d->m_customizeGroupAction->addAction(act);
                adjustSize();
            }
            else if (quickAccessAction)
            {
                quickAccessAction->update();
            }
        }
        else if (event->type() == QEvent::ActionRemoved)
        {
            if (event->action() == d->m_actionAccessPopup)
            {
                d->m_removeAction = true;
                addAction(d->m_actionAccessPopup);
                d->m_removeAction = false;
            }
            else if (QdfQuickAccessActionInvisible *actInvisible =
                             dynamic_cast<QdfQuickAccessActionInvisible *>(event->action()))
            {
                d->m_actionList.clear();
                QList<QAction *> actList = actInvisible->m_data->actions();
                for (int i = actList.size() - 1; i >= 0; i--)
                {
                    QAction *actionWrapper = actList[i];
                    d->m_customizeGroupAction->removeAction(actionWrapper);
                    delete actionWrapper;
                }
                delete actInvisible;
                addAction(new QdfQuickAccessActionInvisible(this, d->m_customizeGroupAction));
            }
            else
            {
                d->updateAction(event->action());
            }
        }
        else if (event->type() == QEvent::ActionChanged)
        {
            if (event->action() == d->m_actionAccessPopup)
            {
                d->m_accessPopup->setDefaultAction(d->m_actionAccessPopup);
            }
            else if (!dynamic_cast<QdfQuickAccessAction *>(event->action()))
            {
                d->updateAction(event->action());
            }
        }
    }
}

void QdfRibbonQuickAccessBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
    {
        QPainter p(this);
        QdfStyleRibbonQuickAccessBar opt;
        opt.init(this);
        opt.quickAccessBarPosition = ribbonBar->quickAccessBarPosition();
        style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);
    }
}

void QdfRibbonQuickAccessBar::resizeEvent(QResizeEvent *event)
{
    QToolBar::resizeEvent(event);
    QDF_D(QdfRibbonQuickAccessBar);
    d->updateParentLayout();
}
