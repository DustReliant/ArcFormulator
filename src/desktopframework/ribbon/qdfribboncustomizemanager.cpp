#include "private/qdfribboncustomizemanager_p.h"
#include "private/qdfribbonpage_p.h"
#include "qdfribbon_def.h"
#include "qdfribbontabbar.h"
#include <ribbon/qdfribboncustomizemanager.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonquickaccessbar.h>

QDF_USE_NAMESPACE

QdfCustomizeEngineAbstract::QdfCustomizeEngineAbstract(QObject *parent) : QObject(parent)
{
}

QdfCustomizeEngineAbstract::~QdfCustomizeEngineAbstract()
{
}

QString QdfCustomizeEngineAbstract::categoryAllActions() const
{
    return m_allCategoryName;
}

QList<QAction *> QdfCustomizeEngineAbstract::actionsAll() const
{
    return m_allNameToActions.values();
}

QList<QAction *> QdfCustomizeEngineAbstract::actionsByCategory(const QString &category) const
{
    QMap<QString, QList<QAction *>>::ConstIterator it = m_categoryToActions.find(category);
    if (it != m_categoryToActions.constEnd())
    {
        return it.value();
    }
    return QList<QAction *>();
}

QStringList QdfCustomizeEngineAbstract::categories() const
{
    return m_lstCategory;
}

QString QdfCustomizeEngineAbstract::actionId(QAction *action) const
{
    return m_allActionsToName.value(action);
}

void QdfCustomizeEngineAbstract::setActionId(QAction *action, const QString &id)
{
    QString cmdName = id;

    if (action == nullptr || action->isSeparator())
    {
        return;
    }

    if (cmdName.isEmpty())
    {
        // qWarning("QdfCustomizeEngineAbstract::setActionId(): 'commandName' not set for QAction "
        //     "%p '%s', using 'text' instead", action, action->text().toLocal8Bit().constData());
        cmdName = action->text();

        if (!cmdName.isEmpty())
        {
            cmdName += "_";
        }

        cmdName += QdfCustomizeEngineAbstract::generateUniqueNameIdentifier();
    }

    if (cmdName.isEmpty())
    {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameToActions.contains(cmdName))
    {
        return;
    }

    if (m_allActionsToName.contains(action))
    {
        return;
    }

    m_allNameToActions.insert(cmdName, action);
    m_allActionsToName.insert(action, cmdName);

    if (!m_allCategoryName.isEmpty())
    {
        m_categoryToActions[m_allCategoryName].append(action);
        m_actionToCategory[action] = m_allCategoryName;
    }
}

void QdfCustomizeEngineAbstract::addAllActionsCategory(const QString &category)
{
    if (!category.isEmpty() && !m_lstCategory.contains(category))
    {
        m_allCategoryName = category;
        m_lstCategory.append(category);
    }
}

void QdfCustomizeEngineAbstract::addToCategory(const QString &category, QAction *action)
{
    if (category.isEmpty() || action == nullptr || action->isSeparator())
    {
        return;
    }

    m_categoryToActions[category].append(action);
    m_actionToCategory[action] = category;

    if (!m_lstCategory.contains(category))
    {
        m_lstCategory.append(category);
    }

    setActionId(action);
}

QString QdfCustomizeEngineAbstract::generateUniqueNameIdentifier(int hint)
{
    static QString prefix = "__qdf_Action_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifier.contains(hint))
    {
        m_setUniqueIdentifier.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifier.contains(id) && id >= UserNameID)
    {
        --id;
    }

    if (id >= UserNameID)
    {
        m_setUniqueIdentifier.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

QString QdfCustomizeEngineAbstract::generateUniqueNameIdentifierGroup(int hint /* = -1*/)
{
    static QString prefix = "__qdf_Group_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifierGroup.contains(hint))
    {
        m_setUniqueIdentifierGroup.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifierGroup.contains(id) && id >= UserNameID)
    {
        --id;
    }

    if (id >= UserNameID)
    {
        m_setUniqueIdentifierGroup.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

QString QdfCustomizeEngineAbstract::generateUniqueNameIdentifierPage(int hint /* = -1*/)
{
    static QString prefix = "__qdf_Page_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifierPage.contains(hint))
    {
        m_setUniqueIdentifierPage.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifierPage.contains(id) && id >= UserNameID)
    {
        --id;
    }

    if (id >= UserNameID)
    {
        m_setUniqueIdentifierPage.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

/* QdfToolBarCustomizeEngine */
QdfToolBarCustomizeEngine::QdfToolBarCustomizeEngine(QObject *parent)
    : QdfCustomizeEngineAbstract(parent)
{
}

QdfToolBarCustomizeEngine::~QdfToolBarCustomizeEngine()
{
}

QList<QAction *> QdfToolBarCustomizeEngine::actions(QToolBar *toolBar) const
{
    if (m_toolBars.contains(toolBar))
    {
        return m_toolBars.value(toolBar);
    }
    return QList<QAction *>();
}

QToolBar *QdfToolBarCustomizeEngine::toolBarWidgetAction(QAction *action) const
{
    if (m_widgetActions.contains(action))
    {
        return m_widgetActions.value(action);
    }
    return nullptr;
}

QToolBar *QdfToolBarCustomizeEngine::findDefaultToolBar(const QString &objectName) const
{
    QMap<QToolBar *, QList<QAction *>>::ConstIterator itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd())
    {
        QToolBar *tb = itToolBar.key();
        if (tb->objectName() == objectName)
        {
            return tb;
        }

        ++itToolBar;
    }

    qWarning("QdfToolBarCustomizeEngine::findDefaultToolBar(): cannot find a QToolBar named "
             "'%s', trying to match using 'windowTitle' instead.",
             objectName.toLocal8Bit().constData());

    itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd())
    {
        QToolBar *tb = itToolBar.key();
        if (tb->windowTitle() == objectName)
        {
            return tb;
        }
        ++itToolBar;
    }
    qWarning("QdfToolBarCustomizeEngine::findDefaultToolBar(): cannot find a QToolBar with "
             "matching 'windowTitle' (looking for '%s').",
             objectName.toLocal8Bit().constData());

    return nullptr;
}

QMap<QToolBar *, QList<QAction *>> QdfToolBarCustomizeEngine::defaultToolBars() const
{
    return m_defaultToolBars;
}

void QdfToolBarCustomizeEngine::removeWidgetActions(const QMap<QToolBar *, QList<QAction *>> &actions)
{
    QMap<QToolBar *, QList<QAction *>>::ConstIterator itWidget = actions.constBegin();
    while (itWidget != actions.constEnd())
    {
        QToolBar *toolBar = itWidget.key();
        QList<QAction *> newActions = m_toolBars.value(toolBar);
        QList<QAction *> newActionsWithSeparators = m_toolBarsWithSeparators.value(toolBar);

        QList<QAction *> removedActions;
        QList<QAction *> actionList = itWidget.value();
        QListIterator<QAction *> itAction(actionList);
        while (itAction.hasNext())
        {
            QAction *action = itAction.next();
            if (newActions.contains(action) && toolBarWidgetAction(action) == toolBar)
            {
                newActions.removeAll(action);
                newActionsWithSeparators.removeAll(action);
                removedActions.append(action);
            }
        }

        m_toolBars.insert(toolBar, newActions);
        m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
        QListIterator<QAction *> itRemovedAction(removedActions);
        while (itRemovedAction.hasNext())
        {
            QAction *oldAction = itRemovedAction.next();
            m_widgetActions.insert(oldAction, 0);
            m_actionToToolBars[oldAction].removeAll(toolBar);
        }
        ++itWidget;
    }
}

void QdfToolBarCustomizeEngine::addDefaultToolBar(QToolBar *toolBar)
{
    if (!toolBar)
    {
        return;
    }

    if (m_toolBars.contains(toolBar))
    {
        return;
    }

    QList<QAction *> newActionsWithSeparators;
    QList<QAction *> newActions;
    QList<QAction *> actions = toolBar->actions();
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext())
    {
        QAction *action = itAction.next();
        if (action->isVisible() &&
            action->property(__qdf_Action_Invisible).toString() !=
                    QLatin1String("__qdf_Action_Invisible") &&
            action->property(__qdf_Quick_Access_Button).toString() !=
                    QLatin1String("__qdf_Quick_Access_Button"))
        {
            setActionId(action);

            if (m_widgetActions.contains(action))
            {
                m_widgetActions.insert(action, toolBar);
            }

            newActionsWithSeparators.append(action);
            if (action->isSeparator())
            {
                action = 0;
            }
            else
            {
                m_actionToToolBars[action].append(toolBar);
            }
            newActions.append(action);
        }
    }

    m_defaultToolBars.insert(toolBar, newActions);
    m_toolBars.insert(toolBar, newActions);
    m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

QList<QToolBar *> QdfToolBarCustomizeEngine::toolBarsByCategory(const QString &category) const
{
    return m_categoryToBars.value(category);
}

void QdfToolBarCustomizeEngine::addToCategory(const QString &category, QToolBar *bar)
{
    if (category.isEmpty() || bar == nullptr)
    {
        return;
    }

    if (m_barToCategory.contains(bar))
    {
        return;
    }

    m_categoryToBars[category].append(bar);
    m_barToCategory[bar] = category;

    if (!m_lstCategory.contains(category))
    {
        m_lstCategory.append(category);
    }
}

void QdfToolBarCustomizeEngine::setToolBar(QToolBar *toolBar, const QList<QAction *> &actions)
{
    if (!toolBar)
    {
        return;
    }
    if (!m_toolBars.contains(toolBar))
    {
        return;
    }
    if (actions == m_toolBars[toolBar])
    {
        return;
    }

    QMap<QToolBar *, QList<QAction *>> toRemove;
    QList<QAction *> newActions;
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext())
    {
        QAction *action = itAction.next();
        if (!action || (!newActions.contains(action) && m_allNameToActions.values().contains(action)))
        {
            newActions.append(action);
        }

        QToolBar *oldToolBar = toolBarWidgetAction(action);
        if (oldToolBar && oldToolBar != toolBar)
        {
            toRemove[oldToolBar].append(action);
        }
    }

    removeWidgetActions(toRemove);

    QList<QAction *> oldActions = m_toolBarsWithSeparators.value(toolBar);
    QListIterator<QAction *> itOldAction(oldActions);
    while (itOldAction.hasNext())
    {
        QAction *action = itOldAction.next();
        if (action == nullptr)
        {
            continue;
        }

        if (toolBarWidgetAction(action) == toolBar)
        {
            m_widgetActions.insert(action, 0);
        }
        toolBar->removeAction(action);
        if (action->isSeparator())
        {
            delete action;
        }
        else
        {
            m_actionToToolBars[action].removeAll(toolBar);
        }
    }

    QList<QAction *> newActionsWithSeparators;
    QListIterator<QAction *> itNewActions(newActions);
    while (itNewActions.hasNext())
    {
        QAction *action = itNewActions.next();
        QAction *newAction = nullptr;
        if (!action)
        {
            newAction = toolBar->insertSeparator(0);
        }

        if (m_allNameToActions.values().contains(action))
        {
            toolBar->insertAction(0, action);
            newAction = action;
            m_actionToToolBars[action].append(toolBar);
        }
        newActionsWithSeparators.append(newAction);
    }
    m_toolBars.insert(toolBar, newActions);
    m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

void QdfToolBarCustomizeEngine::saveStateQuickAccessBar(QXmlStreamWriter &stream) const
{
    // stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_QABAR);

    QMap<QToolBar *, QList<QAction *>>::ConstIterator itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd())
    {
        stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_QABAR);

        QToolBar *tb = itToolBar.key();
        QString title = tb->objectName();
        if (tb->objectName().isEmpty())
        {
            qWarning("QdfToolBarCustomizeEngine::saveStateQuickAccessBar(): 'objectName' not set for "
                     "QToolBar "
                     "%p '%s', using 'windowTitle' instead",
                     tb, tb->windowTitle().toLocal8Bit().constData());
            title = tb->windowTitle();
        }

        stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE, title);

        QListIterator<QAction *> itAction(m_toolBars[tb]);
        while (itAction.hasNext())
        {
            QString id;
            if (QAction *action = itAction.next())
            {
                stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_ACTION);
                id = m_allActionsToName.value(action);
                if (id.isEmpty())
                {
                    qWarning("QdfToolBarCustomizeEngine::saveStateQuickAccessBar(): 'objectName' not "
                             "set for QAction "
                             "%p '%s', using 'text' instead",
                             action, action->text().toLocal8Bit().constData());
                    id = action->text();
                }
                stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_ID, id);
                stream.writeEndElement();
            }
            else
            {
                stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_ACTION);
                stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_ID, id);
                stream.writeEndElement();
            }
        }
        ++itToolBar;
        stream.writeEndElement();
    }

    // stream.writeEndElement();
}

bool QdfToolBarCustomizeEngine::restoreStateQuickAccessBar(QXmlStreamReader &stream)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
    {
        return false;
    }

    if (stream.name() != QDF_DIC_TAGNAME_RIBBON_QABAR)
    {
        return false;
    }

    while (stream.tokenType() == QXmlStreamReader::StartElement)
    {
        QXmlStreamAttributes attrsToolBar = stream.attributes();

        if (!attrsToolBar.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE))
        {
            return false;
        }

        QString title = attrsToolBar.value(QDF_DIC_ATTRNAME_RIBBON_TITLE).toString();

        QList<QAction *> actions;
        if (stream.readNext() == QXmlStreamReader::StartElement)
        {
            if (stream.name() != QDF_DIC_TAGNAME_RIBBON_ACTION)
            {
                return false;
            }

            while (stream.tokenType() == QXmlStreamReader::StartElement)
            {
                QXmlStreamAttributes attrs = stream.attributes();

                if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_ID))
                {
                    return false;
                }

                QString id = attrs.value(QDF_DIC_ATTRNAME_RIBBON_ID).toString();

                if (id.isEmpty())
                {
                    actions.append(0);
                }
                else if (QAction *action = m_allNameToActions.value(id))
                {
                    actions.append(action);
                }

                if (stream.readNext() != QXmlStreamReader::EndElement)
                {
                    return false;
                }

                stream.readNext();
            }
        }

        if (QToolBar *toolBar = findDefaultToolBar(title))
        {
            setToolBar(toolBar, actions);
        }
        /*
        QXmlStreamReader::TokenType type = stream.readNext();
        if (type != QXmlStreamReader::EndElement)
            return false;

        stream.readNext();
        */
    }

    // if (stream.readNext() != QXmlStreamReader::EndElement)
    //     return false;

    return true;
}

void QdfToolBarCustomizeEngine::saveState(QXmlStreamWriter &stream) const
{
    saveStateQuickAccessBar(stream);
}

bool QdfToolBarCustomizeEngine::restoreState(QXmlStreamReader &stream)
{
    return restoreStateQuickAccessBar(stream);
}

/* QdfRibbonBarCustomizeEngine */
QdfRibbonBarCustomizeEngine::QdfRibbonBarCustomizeEngine(QObject *parent, QdfRibbonBar *ribbonBar)
    : QdfToolBarCustomizeEngine(parent), m_ribbonBar(ribbonBar)
{
}

QdfRibbonBarCustomizeEngine::~QdfRibbonBarCustomizeEngine()
{
}

QString QdfRibbonBarCustomizeEngine::pageId(QdfRibbonPage *page) const
{
    return m_allPagesToNameId.value(page);
}

void QdfRibbonBarCustomizeEngine::setPageId(QdfRibbonPage *page, const QString &id)
{
    QString strPageName = id;

    if (page == nullptr)
    {
        return;
    }

    if (strPageName.isEmpty())
    {
        // qWarning("QdfRibbonBarCustomizeEngine::setPageId(): 'pageName' not set for QdfRibbonPage "
        //     "%p '%s', using 'text' instead", page, page->title().toLocal8Bit().constData());

        strPageName = page->title();

        if (!strPageName.isEmpty())
        {
            strPageName += "_";
        }
        strPageName += QdfCustomizeEngineAbstract::generateUniqueNameIdentifierPage();
    }

    if (strPageName.isEmpty())
    {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameIdToPages.contains(strPageName))
    {
        return;
    }

    if (m_allPagesToNameId.contains(page))
    {
        return;
    }

    m_allNameIdToPages.insert(strPageName, page);
    m_allPagesToNameId.insert(page, strPageName);
}

QString QdfRibbonBarCustomizeEngine::groupId(QdfRibbonGroup *group) const
{
    return m_allGroupsToNameId.value(group);
}

void QdfRibbonBarCustomizeEngine::setGroupId(QdfRibbonGroup *group, const QString &id)
{
    QString strGroupName = id;

    if (group == nullptr)
    {
        return;
    }

    if (strGroupName.isEmpty())
    {
        // qWarning("QdfRibbonBarCustomizeEngine::setGroupId(): 'groupName' not set for QdfRibbonGroup "
        //     "%p '%s', using 'text' instead", group, group->title().toLocal8Bit().constData());
        strGroupName = QdfCustomizeEngineAbstract::generateUniqueNameIdentifierGroup() + group->title();

        if (!strGroupName.isEmpty())
        {
            strGroupName += "_";
        }
        strGroupName += QdfCustomizeEngineAbstract::generateUniqueNameIdentifierGroup();
    }

    if (strGroupName.isEmpty())
    {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameIdToGroups.contains(strGroupName))
    {
        return;
    }

    if (m_allGroupsToNameId.contains(group))
    {
        return;
    }

    m_allNameIdToGroups.insert(strGroupName, group);
    m_allGroupsToNameId.insert(group, strGroupName);
}

void QdfRibbonBarCustomizeEngine::addToCategory(const QString &category, QAction *act)
{
    QdfCustomizeEngineAbstract::addToCategory(category, act);
}

QList<QdfRibbonPage *> QdfRibbonBarCustomizeEngine::pagesByCategory(const QString &category) const
{
    return m_categoryToPages.value(category);
}

void QdfRibbonBarCustomizeEngine::addToCategory(const QString &category, QdfRibbonPage *page)
{
    if (category.isEmpty() || page == nullptr)
    {
        return;
    }

    if (m_pageToCategory.contains(page))
    {
        return;
    }

    m_categoryToPages[category].append(page);
    m_pageToCategory[page] = category;

    if (!m_lstCategory.contains(category))
    {
        m_lstCategory.append(category);
    }
}

QList<QdfRibbonGroup *> QdfRibbonBarCustomizeEngine::groupsByCategory(const QString &category) const
{
    return m_categoryToGroups.value(category);
}

void QdfRibbonBarCustomizeEngine::addToCategory(const QString &category, QdfRibbonGroup *group)
{
    if (category.isEmpty() || group == nullptr)
    {
        return;
    }

    QString strCategory = category;
    strCategory.remove(QChar::fromLatin1('&'));

    if (m_groupToCategory.contains(group))
    {
        return;
    }

    m_categoryToGroups[strCategory].append(group);
    m_groupToCategory[group] = strCategory;

    QList<QAction *> actions = group->actions();
    QListIterator<QAction *> itActions(actions);
    while (itActions.hasNext())
    {
        QAction *action = itActions.next();
        if (!action->text().isEmpty())
        {
            setActionId(action);
            addToCategory(strCategory, action);
        }
    }

    for (int index = 0, count = group->controlCount(); count > index; ++index)
    {
        QdfRibbonControl *control = group->controlByIndex(index);
        Q_ASSERT(control != nullptr);
        if (QdfRibbonToolBarControl *toolBarControl = qobject_cast<QdfRibbonToolBarControl *>(control))
        {
            QList<QAction *> actions = toolBarControl->actions();
            QListIterator<QAction *> itActions(actions);
            while (itActions.hasNext())
            {
                QAction *action = itActions.next();
                if (!action->text().isEmpty())
                {
                    setActionId(action);
                    addToCategory(strCategory, action);
                }
            }
        }
    }
}

QList<QdfRibbonGroup *> QdfRibbonBarCustomizeEngine::pageDefaultGroups(QdfRibbonPage *page) const
{
    return m_defaultPageGroups.value(page);
}

QMap<QdfRibbonPage *, QString> QdfRibbonBarCustomizeEngine::defaultPagesName() const
{
    if (m_defaultPagesName.size() > 0)
    {
        return m_defaultPagesName;
    }
    return QMap<QdfRibbonPage *, QString>();
}

QString QdfRibbonBarCustomizeEngine::pageDefaultName(QdfRibbonPage *page) const
{
    return m_defaultPagesName.value(page);
}

QList<QdfRibbonPage *> QdfRibbonBarCustomizeEngine::defaultHiddenPages() const
{
    if (m_defaultHiddenPages.size() > 0)
    {
        return m_defaultHiddenPages;
    }
    return QList<QdfRibbonPage *>();
}

QList<QdfRibbonPage *> QdfRibbonBarCustomizeEngine::editHiddenPages() const
{
    if (m_regularHiddenPages.size() > 0)
    {
        return m_regularHiddenPages;
    }
    return QList<QdfRibbonPage *>();
}

QList<QdfRibbonGroup *> QdfRibbonBarCustomizeEngine::regularGroups(QdfRibbonPage *page) const
{
    if (m_regularPageGroups.contains(page))
    {
        return m_regularPageGroups.value(page);
    }
    return QList<QdfRibbonGroup *>();
}

QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>> QdfRibbonBarCustomizeEngine::defaultPageGroups() const
{
    return m_defaultPageGroups;
}

QList<QdfRibbonPage *> QdfRibbonBarCustomizeEngine::defaultRibbonBar() const
{
    return m_defaultPages;
}

QMap<QdfRibbonGroup *, QString> QdfRibbonBarCustomizeEngine::defaultGroupsName() const
{
    return m_defaultNameGroups;
}

QString QdfRibbonBarCustomizeEngine::groupDefaultName(QdfRibbonGroup *group) const
{
    return m_defaultNameGroups.value(group);
}

void QdfRibbonBarCustomizeEngine::addDefaultPages(QdfRibbonBar *ribbonBar)
{
    if (!ribbonBar)
    {
        return;
    }

    QList<QdfRibbonPage *> newPages;
    QList<QdfRibbonPage *> pages = ribbonBar->pages();
    QListIterator<QdfRibbonPage *> itPages(pages);

    while (itPages.hasNext())
    {
        QdfRibbonPage *page = itPages.next();
        // if (page->objectName() != QLatin1String("__qdf_Page_Invisible"))
        {
            setPageId(page);
            newPages.append(page);

            if (page->qdf_d()->m_associativeTab->isHidden())
            {
                m_defaultHiddenPages.append(page);
            }
        }
        m_defaultPagesName.insert(page, page->title());
        addDefaultGroups(page);
    }
    m_defaultPages.append(newPages);
}

void QdfRibbonBarCustomizeEngine::addDefaultGroups(QdfRibbonPage *page)
{
    if (!page)
    {
        return;
    }

    if (m_defaultPageGroups.contains(page))
    {
        return;
    }

    QList<QdfRibbonGroup *> newGroups;
    QList<QdfRibbonGroup *> groups = page->groups();
    QListIterator<QdfRibbonGroup *> itGroups(groups);

    while (itGroups.hasNext())
    {
        QdfRibbonGroup *group = itGroups.next();
        // if (group->objectName() != QLatin1String("__qdf_Group_Invisible") )
        {
            setGroupId(group);
            newGroups.append(group);
        }
        m_defaultNameGroups.insert(group, group->title());
    }
    m_defaultPageGroups.insert(page, newGroups);
}

void QdfRibbonBarCustomizeEngine::setQuickAccessBar()
{
    if (QdfRibbonQuickAccessBar *quickAccessBar = m_ribbonBar->quickAccessBar())
    {
        setToolBar(quickAccessBar, m_regularToolBars.value(quickAccessBar));
    }
}

void QdfRibbonBarCustomizeEngine::setRibbonBar()
{
    if (!m_ribbonBar)
    {
        return;
    }

    m_ribbonBar->setUpdatesEnabled(false);

    QList<QdfRibbonPage *> currentListPage = m_ribbonBar->pages();
    if (m_regularPages.isEmpty())
    {
        QListIterator<QdfRibbonPage *> itRegularPage(currentListPage);
        while (itRegularPage.hasNext())
        {
            QdfRibbonPage *currentPage = itRegularPage.next();
            if (m_defaultPagesName.contains(currentPage))
            {
                QString strTitle = m_defaultPagesName.value(currentPage);
                if (!strTitle.isEmpty())
                {
                    currentPage->setTitle(strTitle);
                }
            }
            currentPage->setVisible(!m_defaultHiddenPages.contains(currentPage));

            if (!m_regularPages.contains(currentPage))
            {
                m_ribbonBar->detachPage(currentPage);
                delete currentPage;
            }
        }
    }
    else
    {
        QListIterator<QdfRibbonPage *> itRegularPage(currentListPage);
        while (itRegularPage.hasNext())
        {
            QdfRibbonPage *currentPage = itRegularPage.next();
            if (!m_regularPages.contains(currentPage))
            {
                if (currentPage->property(__qdf_Widget_Custom).toString() ==
                    QLatin1String("__qdf_Page_Custom"))
                {
                    m_ribbonBar->removePage(currentPage);
                }
            }
        }
    }

    QList<QdfRibbonPage *> newListPage;
    QListIterator<QdfRibbonPage *> itRegularPage(m_regularPages);
    while (itRegularPage.hasNext())
    {
        QdfRibbonPage *regularPage = itRegularPage.next();
        newListPage.append(regularPage);

        if (m_regularPagesName.contains(regularPage))
        {
            QString strTitle = m_regularPagesName.value(regularPage);
            if (!strTitle.isEmpty())
            {
                regularPage->setTitle(strTitle);
            }
        }
        m_ribbonBar->detachPage(regularPage);
    }

    QListIterator<QdfRibbonPage *> itNewPages(newListPage);
    while (itNewPages.hasNext())
    {
        QdfRibbonPage *page = itNewPages.next();
        m_ribbonBar->addPage(page);

        if (m_regularHiddenPages.contains(page))
        {
            page->setVisible(false);
        }
    }
    m_ribbonBar->setUpdatesEnabled(true);
}

void QdfRibbonBarCustomizeEngine::setRibbonGroups()
{
    for (QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>>::ConstIterator itCurPages =
                 m_regularPageGroups.begin();
         itCurPages != m_regularPageGroups.end(); ++itCurPages)
    {
        QdfRibbonPage *page = itCurPages.key();
        Q_ASSERT(page != nullptr);

        QList<QdfRibbonGroup *> groupsPage = itCurPages.value();
        QList<QdfRibbonGroup *> realGroups = page->groups();
        if (groupsPage != realGroups || m_regularNameGroups.size() > 0 ||
            m_regularGroupsActions.size() > 0)
        {
            QList<QdfRibbonGroup *> newGroups;
            QListIterator<QdfRibbonGroup *> itGroups(groupsPage);
            while (itGroups.hasNext())
            {
                QdfRibbonGroup *group = itGroups.next();

                if (!group || (!newGroups.contains(group)))
                {
                    newGroups.append(group);
                }

                if (group)
                {
                    // if (!realGroups.contains(group) && !m_allGroupsToNameId.keys().contains(group))
                    //     setGroupId(group);

                    QListIterator<QdfRibbonGroup *> itRegularGroups(realGroups);
                    while (itRegularGroups.hasNext())
                    {
                        QdfRibbonGroup *regularGroup = itRegularGroups.next();
                        if (!groupsPage.contains(regularGroup) &&
                            (regularGroup->property(__qdf_Widget_Custom).toString() ==
                                     QLatin1String("__qdf_Group_Custom") ||
                             regularGroup->property(__qdf_Widget_Copy).toString() ==
                                     QLatin1String("__qdf_Group_Copy")))
                        {
                            realGroups.removeOne(regularGroup);
                            page->removeGroup(regularGroup);
                        }
                    }
                }
            }

            QListIterator<QdfRibbonGroup *> itOldGroup(realGroups);
            while (itOldGroup.hasNext())
            {
                page->detachGroup(itOldGroup.next());
            }

            QListIterator<QdfRibbonGroup *> itNewGroups(newGroups);
            while (itNewGroups.hasNext())
            {
                QdfRibbonGroup *group = itNewGroups.next();
                page->addGroup(group);
                group->setVisible(true);

                if (m_regularNameGroups.contains(group))
                {
                    group->setTitle(m_regularNameGroups.value(group));
                }

                if (group->property(__qdf_Widget_Custom).toString() ==
                    QLatin1String("__qdf_Group_Custom"))
                {
                    group->clear();
                    QList<QAction *> actions = m_regularGroupsActions[group];
                    QListIterator<QAction *> itActions(actions);
                    while (itActions.hasNext())
                    {
                        group->addAction(itActions.next());
                    }
                }
            }
        }
    }
}

void QdfRibbonBarCustomizeEngine::updateRibbonBar()
{
    int currentIndex = m_ribbonBar->currentPageIndex();
    if (currentIndex != -1)
    {
        if (QdfRibbonPage *page = m_ribbonBar->getPage(currentIndex))
        {
            if (page->isVisible())
            {
                page->updateLayout();
                return;
            }

            if (!m_regularHiddenPages.contains(page))
            {
                return;
            }
        }
    }

    int index = 0;
    const QList<QdfRibbonPage *> &listPage = m_ribbonBar->pages();
    for (QList<QdfRibbonPage *>::const_iterator it = listPage.constBegin(); it != listPage.constBegin();
         ++it)
    {
        QdfRibbonPage *page = (*it);
        if (page->isVisible() ||
            (!m_regularHiddenPages.isEmpty() && !m_regularHiddenPages.contains(page)))
        {
            m_ribbonBar->setCurrentPageIndex(index);
            page->updateLayout();
            break;
        }
        index++;
    }
}

void QdfRibbonBarCustomizeEngine::deleteFreeGroups()
{
    for (QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>>::ConstIterator itPages =
                 m_regularPageGroups.begin();
         itPages != m_regularPageGroups.end(); ++itPages)
    {
        QdfRibbonPage *page = itPages.key();
        QList<QdfRibbonGroup *> listGroup = m_regularPageGroups.value(page);
        for (int i = listGroup.count() - 1; i >= 0; --i)
        {
            QdfRibbonGroup *group = listGroup.at(i);
            if (group && group->parent() == nullptr)
            {
                m_regularPageGroups[page].removeOne(group);
                delete group;
            }
        }
    }
}

void QdfRibbonBarCustomizeEngine::deleteFreePages()
{
    QListIterator<QdfRibbonPage *> itPages(m_regularPages);
    while (itPages.hasNext())
    {
        QdfRibbonPage *page = itPages.next();
        if (page && page->parent() == nullptr)
        {
            delete page;
        }
    }
}

void QdfRibbonBarCustomizeEngine::saveStateGroup(QXmlStreamWriter &stream, QdfRibbonPage *page) const
{
    Q_ASSERT(page != NULL);
    if (page == NULL)
    {
        return;
    }

    QList<QdfRibbonGroup *> groups = page->groups();
    QListIterator<QdfRibbonGroup *> itGroup(groups);
    while (itGroup.hasNext())
    {
        if (QdfRibbonGroup *group = itGroup.next())
        {
            stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_GROUP);

            bool custom = group->property(__qdf_Widget_Custom).toString() ==
                          QLatin1String("__qdf_Group_Custom");
            bool copy =
                    group->property(__qdf_Widget_Copy).toString() == QLatin1String("__qdf_Group_Copy");

            QString nameId = m_allGroupsToNameId.value(group);

            if (nameId.isEmpty())
            {
                if (!(custom || copy))
                {
                    qWarning("QdfRibbonBarCustomizeEngine::saveStateGroup(): 'objectName' not set for "
                             "QdfRibbonGroup "
                             "%p '%s', using 'text' instead",
                             group, group->title().toLocal8Bit().constData());
                }
                nameId = page->title();
            }

            stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_ID, nameId);
            stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE, group->title());

            if (custom)
            {
                stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TYPE, "CustomGroup");
            }
            else if (copy)
            {
                stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TYPE, "CopyGroup");
            }

            if (custom || copy)
            {
                QList<QAction *> actions = group->actions();
                QListIterator<QAction *> itActions(actions);
                while (itActions.hasNext())
                {
                    QString nameAction = m_allActionsToName.value(itActions.next());
                    if (!nameAction.isEmpty())
                    {
                        stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_ACTION);
                        stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_ID, nameAction);
                        stream.writeEndElement();
                    }
                }
            }
            stream.writeEndElement();
        }
    }
}

bool QdfRibbonBarCustomizeEngine::restoreStateGroup(QXmlStreamReader &stream, QdfRibbonPage *page)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
    {
        return false;
    }
    if (stream.name() != QDF_DIC_TAGNAME_RIBBON_GROUP)
    {
        return false;
    }

    QList<QdfRibbonGroup *> groups;
    QMap<QdfRibbonGroup *, QList<QAction *>> groupActions;

    while (stream.tokenType() == QXmlStreamReader::StartElement)
    {
        QXmlStreamAttributes attrs = stream.attributes();
        if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_ID))
        {
            return false;
        }

        QString nameId = attrs.value(QDF_DIC_ATTRNAME_RIBBON_ID).toString();

        if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE))
        {
            return false;
        }

        QString title = attrs.value(QDF_DIC_ATTRNAME_RIBBON_TITLE).toString();

        bool customGroup = false;
        bool copyGroup = false;
        QString strType = attrs.value(QDF_DIC_ATTRNAME_RIBBON_TYPE).toString();
        if (!strType.isEmpty())
        {
            if (strType == "CustomGroup")
            {
                customGroup = true;
            }
            else if (strType == "CopyGroup")
            {
                copyGroup = true;
            }
        }

        bool next = true;

        if (nameId.isEmpty())
        {
            groups.append(0);
        }
        else if (!(customGroup || copyGroup))
        {
            QdfRibbonGroup *group = m_allNameIdToGroups.value(nameId);

            QList<QdfRibbonGroup *> listGroup = m_allNameIdToGroups.values();
            for (QList<QdfRibbonGroup *>::iterator it = listGroup.begin();
                 it != listGroup.end() && group == nullptr; ++it)
            {
                if ((*it)->title() == nameId || (*it)->title() == title)
                {
                    group = (*it);
                }
            }

            if (group)
            {
                groups.append(group);
                group->setTitle(title);
            }
        }
        else if (customGroup || copyGroup)
        {
            QdfRibbonGroup *newGroup = new QdfRibbonGroup(NULL);
            //            newGroup->setObjectName(nameId);
            newGroup->setTitle(title);
            groups.append(newGroup);

            if (customGroup)
            {
                newGroup->setProperty(__qdf_Widget_Custom, QLatin1String("__qdf_Group_Custom"));
            }
            else
            {
                newGroup->setProperty(__qdf_Widget_Copy, QLatin1String("__qdf_Group_Copy"));
            }

            next = false;

            QList<QAction *> actions;

            if (stream.readNext() == QXmlStreamReader::StartElement)
            {
                if (stream.name() == QDF_DIC_TAGNAME_RIBBON_ACTION)
                {
                    while (stream.tokenType() == QXmlStreamReader::StartElement)
                    {
                        QXmlStreamAttributes attrsAction = stream.attributes();
                        QString idName = attrsAction.value(QDF_DIC_ATTRNAME_RIBBON_ID).toString();
                        if (!idName.isEmpty())
                        {
                            QAction *action = m_allNameToActions.value(idName);
                            newGroup->addAction(action);
                            actions.append(action);
                        }

                        if (stream.readNext() != QXmlStreamReader::EndElement)
                        {
                            return false;
                        }

                        stream.readNext();
                    }
                }
            }
            groupActions.insert(newGroup, actions);
        }

        if (next && stream.readNext() != QXmlStreamReader::EndElement)
        {
            return false;
        }

        stream.readNext();
    }
    if (m_ribbonBar)
    {
        m_regularPageGroups.insert(page, groups);
        m_regularGroupsActions = groupActions;
        setRibbonGroups();
        m_regularPageGroups.clear();
        m_regularGroupsActions.clear();
    }
    return true;
}

void QdfRibbonBarCustomizeEngine::saveStatePage(QXmlStreamWriter &stream) const
{
    QListIterator<QdfRibbonPage *> itPage(m_ribbonBar->pages());
    while (itPage.hasNext())
    {
        if (QdfRibbonPage *page = itPage.next())
        {
            stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_PAGE);

            QString nameId = m_allPagesToNameId.value(page);

            if (nameId.isEmpty())
            {
                if (page->property(__qdf_Widget_Custom).toString() != QLatin1String("__qdf_Page_Custom"))
                {
                    qWarning("QdfRibbonBarCustomizeEngine::saveState(): 'pageId' not set for "
                             "QdfRibbonPage "
                             "%p '%s', using 'text' instead",
                             page, page->title().toLocal8Bit().constData());
                }
                nameId = page->title();
            }

            stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_ID, nameId);
            stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE, page->title());
            stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_VISIBLE,
                                  QVariant(page->isVisible()).toString());

            if (page->property(__qdf_Widget_Custom).toString() == QLatin1String("__qdf_Page_Custom"))
            {
                stream.writeAttribute(QDF_DIC_ATTRNAME_RIBBON_TYPE, "CustomPage");
            }

            saveStateGroup(stream, page);

            stream.writeEndElement();
        }
    }
}

bool QdfRibbonBarCustomizeEngine::restoreStatePage(QXmlStreamReader &stream)
{
    bool ok = true;

    if (stream.readNext() != QXmlStreamReader::StartElement)
    {
        return false;
    }
    if (stream.name() != QDF_DIC_TAGNAME_RIBBON_PAGE)
    {
        return false;
    }

    m_regularHiddenPages.clear();

    //    QList<QdfRibbonPage*> pages;
    while (stream.tokenType() == QXmlStreamReader::StartElement)
    {
        QXmlStreamAttributes attrs = stream.attributes();
        if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_ID))
        {
            return false;
        }

        QString nameId = attrs.value(QDF_DIC_ATTRNAME_RIBBON_ID).toString();

        if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_TITLE))
        {
            return false;
        }

        QString title = attrs.value(QDF_DIC_ATTRNAME_RIBBON_TITLE).toString();

        if (!attrs.hasAttribute(QDF_DIC_ATTRNAME_RIBBON_VISIBLE))
        {
            return false;
        }

        bool visible = QVariant(attrs.value(QDF_DIC_ATTRNAME_RIBBON_VISIBLE).toString()).toBool();

        QString strType = attrs.value(QDF_DIC_ATTRNAME_RIBBON_TYPE).toString();
        bool customPage = !strType.isEmpty() && strType == "CustomPage";

        if (nameId.isEmpty())
        {
            m_regularPages.append(0);
        }
        else if (!customPage)
        {
            QdfRibbonPage *page = m_allNameIdToPages.value(nameId);

            QList<QdfRibbonPage *> listPage = m_allNameIdToPages.values();
            for (QList<QdfRibbonPage *>::iterator it = listPage.begin();
                 it != listPage.end() && page == nullptr; ++it)
            {
                if ((*it)->title() == nameId || (*it)->title() == title)
                {
                    page = (*it);
                }
            }

            if (page)
            {
                m_regularPages.append(page);

                if (!visible)
                {
                    m_regularHiddenPages.append(page);
                }
                page->setTitle(title);
                restoreStateGroup(stream, page);
            }
            else
            {
                stream.skipCurrentElement();
            }
        }
        else if (customPage)
        {
            QdfRibbonPage *newPage = new QdfRibbonPage();
            newPage->setTitle(title);
            newPage->setObjectName(nameId);
            m_regularPages.append(newPage);
            newPage->setProperty(__qdf_Widget_Custom, QLatin1String("__qdf_Page_Custom"));

            if (!visible)
            {
                m_regularHiddenPages.append(newPage);
            }
            restoreStateGroup(stream, newPage);
        }
        stream.readNext();
    }

    if (stream.tokenType() != QXmlStreamReader::EndElement)
    {
        return false;
    }

    ((QdfRibbonBarCustomizeEngine *) this)->setRibbonBar();
    updateRibbonBar();
    m_regularPages.clear();
    m_regularHiddenPages.clear();
    return ok;
}

void QdfRibbonBarCustomizeEngine::saveStateRibbonBar(QXmlStreamWriter &stream) const
{
    stream.writeStartElement(QDF_DIC_TAGNAME_RIBBON_BAR);
    saveStatePage(stream);
}

bool QdfRibbonBarCustomizeEngine::restoreStateRibbonBar(QXmlStreamReader &stream)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
    {
        return false;
    }

    if (stream.name() != QDF_DIC_TAGNAME_RIBBON_BAR)
    {
        return false;
    }

    bool ok = restoreStatePage(stream);
    Q_ASSERT(ok);

    if (stream.readNext() != QXmlStreamReader::EndElement)
    {
        return false;
    }

    return ok;
}

void QdfRibbonBarCustomizeEngine::saveState(QXmlStreamWriter &stream) const
{
    QdfToolBarCustomizeEngine::saveState(stream);
    saveStateRibbonBar(stream);
}

bool QdfRibbonBarCustomizeEngine::restoreState(QXmlStreamReader &stream)
{
    bool ok = QdfToolBarCustomizeEngine::restoreState(stream);
    Q_ASSERT(ok);
    ok = restoreStateRibbonBar(stream);
    Q_ASSERT(ok);
    return ok;
}

/* QdfRibbonCustomizeManagerPrivate */
QdfRibbonCustomizeManagerPrivate::QdfRibbonCustomizeManagerPrivate()
{
    m_editMode = false;
    m_ribbonManager = nullptr;
    m_ribbonBar = nullptr;
}

QdfRibbonCustomizeManagerPrivate::~QdfRibbonCustomizeManagerPrivate()
{
}

void QdfRibbonCustomizeManagerPrivate::init(QdfRibbonBar *ribbonBar)
{
    QDF_Q(QdfRibbonCustomizeManager)
    m_ribbonBar = ribbonBar;
    m_ribbonManager = new QdfRibbonBarCustomizeEngine(q, ribbonBar);
}

QdfRibbonCustomizeManager::QdfRibbonCustomizeManager(QdfRibbonBar *ribbonBar) : QObject(ribbonBar)
{
    QDF_INIT_PRIVATE(QdfRibbonCustomizeManager);
    QDF_D(QdfRibbonCustomizeManager);
    d->init(ribbonBar);
}

QdfRibbonCustomizeManager::~QdfRibbonCustomizeManager()
{
    QDF_FINI_PRIVATE();
}


QStringList QdfRibbonCustomizeManager::categories() const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->categories();
}

QList<QAction *> QdfRibbonCustomizeManager::actionsAll() const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->actionsAll();
}

void QdfRibbonCustomizeManager::addToolBar(QToolBar *toolBar)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->addDefaultToolBar(toolBar);
}

void QdfRibbonCustomizeManager::addAllActionsCategory(const QString &category)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->addAllActionsCategory(category);
}

QList<QAction *> QdfRibbonCustomizeManager::actionsByCategory(const QString &category) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->actionsByCategory(category);
}

void QdfRibbonCustomizeManager::addToCategory(const QString &category, QAction *action)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->addToCategory(category, action);
}

QList<QToolBar *> QdfRibbonCustomizeManager::toolBarsByCategory(const QString &category) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->toolBarsByCategory(category);
}

void QdfRibbonCustomizeManager::addToCategory(const QString &category, QToolBar *toolBar)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->QdfToolBarCustomizeEngine::addToCategory(category, toolBar);
}

QList<QdfRibbonPage *> QdfRibbonCustomizeManager::pagesByCategory(const QString &category) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->pagesByCategory(category);
}

void QdfRibbonCustomizeManager::addToCategory(const QString &category, QdfRibbonPage *page)
{
    QDF_D(QdfRibbonCustomizeManager)
    return d->m_ribbonManager->addToCategory(category, page);
}

QList<QdfRibbonGroup *> QdfRibbonCustomizeManager::groupsByCategory(const QString &category) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->groupsByCategory(category);
}

void QdfRibbonCustomizeManager::addToCategory(const QString &category, QdfRibbonGroup *group)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->addToCategory(category, group);
}

QString QdfRibbonCustomizeManager::actionId(QAction *action) const
{
    QDF_D(const QdfRibbonCustomizeManager);
    return d->m_ribbonManager->actionId(action);
}

void QdfRibbonCustomizeManager::setActionId(QAction *action, const QString &id)
{
    QDF_D(QdfRibbonCustomizeManager);
    d->m_ribbonManager->setActionId(action, id);
}

QString QdfRibbonCustomizeManager::pageId(QdfRibbonPage *page) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->pageId(page);
}

void QdfRibbonCustomizeManager::setPageId(QdfRibbonPage *page, const QString &id)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->setPageId(page, id);
}

QString QdfRibbonCustomizeManager::groupId(QdfRibbonGroup *group) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->groupId(group);
}

void QdfRibbonCustomizeManager::setGroupId(QdfRibbonGroup *group, const QString &id)
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->setGroupId(group, id);
}

void QdfRibbonCustomizeManager::setEditMode(bool set)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (d->m_editMode == set)
    {
        return;
    }

    d->m_editMode = set;
    if (d->m_editMode)
    {
        d->m_ribbonManager->m_regularToolBars.clear();
        d->m_ribbonManager->m_regularHiddenPages.clear();
        d->m_ribbonManager->m_regularPagesName.clear();
        d->m_ribbonManager->m_regularPages.clear();
        d->m_ribbonManager->m_regularPageGroups.clear();
        d->m_ribbonManager->m_regularGroupsActions.clear();

        if (QdfRibbonQuickAccessBar *toolBar = d->m_ribbonManager->m_ribbonBar->quickAccessBar())
        {
            QList<QAction *> listAction = toolBar->actions();
            for (QList<QAction *>::iterator it = listAction.begin(); it != listAction.end(); ++it)
            {
                if (d->m_ribbonManager->m_allActionsToName.contains(*it))
                {
                    d->m_ribbonManager->m_regularToolBars[toolBar].append(*it);
                }
                else if ((*it)->isSeparator())
                {
                    d->m_ribbonManager->m_regularToolBars[toolBar].append(0);
                }
            }
        }

        d->m_ribbonManager->m_regularPages = d->m_ribbonManager->m_ribbonBar->pages();
        for (QList<QdfRibbonPage *>::iterator it = d->m_ribbonManager->m_regularPages.begin();
             it != d->m_ribbonManager->m_regularPages.end(); ++it)
        {
            QdfRibbonPage *page = *it;
            setPageName(page, page->title());
            setPageHidden(page, !page->isVisible());

            QList<QdfRibbonGroup *> listGrops = page->groups();
            d->m_ribbonManager->m_regularPageGroups.insert(page, listGrops);

            for (QList<QdfRibbonGroup *>::iterator iGroup = listGrops.begin(); iGroup != listGrops.end();
                 ++iGroup)
            {
                QdfRibbonGroup *group = *iGroup;
                QList<QAction *> actions = group->actions();
                if (!actions.isEmpty())
                {
                    d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                }

                for (int index = 0, count = group->controlCount(); count > index; ++index)
                {
                    QdfRibbonControl *control = group->controlByIndex(index);
                    Q_ASSERT(control != nullptr);
                    if (QdfRibbonToolBarControl *toolBarControl =
                                qobject_cast<QdfRibbonToolBarControl *>(control))
                    {
                        QList<QAction *> actions = toolBarControl->actions();
                        d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                    }
                }
            }
        }
    }
    else
    {
        cancel();
    }
}

bool QdfRibbonCustomizeManager::isEditMode() const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_editMode;
}

void QdfRibbonCustomizeManager::reset(QToolBar *toolBar)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (toolBar == nullptr)
    {
        d->m_ribbonManager->m_regularHiddenPages.clear();
        d->m_ribbonManager->m_regularHiddenPages = d->m_ribbonManager->defaultHiddenPages();
        d->m_ribbonManager->m_regularPagesName.clear();
        d->m_ribbonManager->m_regularPagesName = d->m_ribbonManager->defaultPagesName();
        d->m_ribbonManager->m_regularNameGroups.clear();
        d->m_ribbonManager->m_regularNameGroups = d->m_ribbonManager->defaultGroupsName();

        d->m_ribbonManager->m_regularPages.clear();
        d->m_ribbonManager->m_regularPages = d->m_ribbonManager->defaultRibbonBar();
        d->m_ribbonManager->m_regularPageGroups.clear();
        d->m_ribbonManager->m_regularPageGroups = d->m_ribbonManager->defaultPageGroups();
        d->m_ribbonManager->m_regularGroupsActions.clear();

        for (QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>>::iterator it =
                     d->m_ribbonManager->m_regularPageGroups.begin();
             it != d->m_ribbonManager->m_regularPageGroups.end(); ++it)
        {
            QList<QdfRibbonGroup *> listGrops = it.value();
            for (QList<QdfRibbonGroup *>::iterator iGroup = listGrops.begin(); iGroup != listGrops.end();
                 ++iGroup)
            {
                QdfRibbonGroup *group = *iGroup;
                QList<QAction *> actions = group->actions();
                if (!actions.isEmpty())
                {
                    d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                }

                for (int index = 0, count = group->controlCount(); count > index; ++index)
                {
                    QdfRibbonControl *control = group->controlByIndex(index);
                    Q_ASSERT(control != nullptr);
                    if (QdfRibbonToolBarControl *toolBarControl =
                                qobject_cast<QdfRibbonToolBarControl *>(control))
                    {
                        QList<QAction *> actions = toolBarControl->actions();
                        d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                    }
                }
            }
        }
    }
    else
    {
        d->m_ribbonManager->m_regularToolBars.clear();
        QList<QAction *> listAction = d->m_ribbonManager->defaultToolBars().value(toolBar);
        for (QList<QAction *>::iterator it = listAction.begin(); it != listAction.end(); ++it)
        {
            if (d->m_ribbonManager->m_allActionsToName.contains(*it))
            {
                d->m_ribbonManager->m_regularToolBars[toolBar].append(*it);
            }
            else if ((*it)->isSeparator())
            {
                d->m_ribbonManager->m_regularToolBars[toolBar].append(0);
            }
        }
    }
}

void QdfRibbonCustomizeManager::cancel()
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->m_regularToolBars.clear();
    d->m_ribbonManager->m_regularHiddenPages.clear();
    d->m_ribbonManager->m_regularPagesName.clear();
    d->m_ribbonManager->m_regularNameGroups.clear();
    d->m_ribbonManager->deleteFreePages();
    d->m_ribbonManager->m_regularPages.clear();
    d->m_ribbonManager->deleteFreeGroups();
    d->m_ribbonManager->m_regularPageGroups.clear();
    d->m_ribbonManager->m_regularGroupsActions.clear();
}

void QdfRibbonCustomizeManager::commit()
{
    QDF_D(QdfRibbonCustomizeManager)
    if (isEditMode())
    {
        d->m_ribbonManager->setQuickAccessBar();
        d->m_ribbonManager->setRibbonBar();
        d->m_ribbonManager->setRibbonGroups();
        d->m_ribbonManager->updateRibbonBar();
    }
}

QList<QAction *> QdfRibbonCustomizeManager::actions(QToolBar *toolBar) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    if (qobject_cast<QdfRibbonQuickAccessBar *>(toolBar))
    {
        return d->m_ribbonManager->m_regularToolBars.value(toolBar);
    }
    else
    {
        return d->m_ribbonManager->actions(toolBar);
    }
}

void QdfRibbonCustomizeManager::insertAction(QToolBar *toolBar, QAction *action, int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    d->m_ribbonManager->m_regularToolBars[toolBar].insert(index, action);
}

void QdfRibbonCustomizeManager::removeActionAt(QToolBar *toolBar, int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    d->m_ribbonManager->m_regularToolBars[toolBar].removeAt(index);
}

bool QdfRibbonCustomizeManager::containsAction(QToolBar *toolBar, QAction *action) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularToolBars[toolBar].contains(action);
}

QList<QdfRibbonPage *> QdfRibbonCustomizeManager::pages() const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPages;
}

QdfRibbonPage *QdfRibbonCustomizeManager::createPage(const QString &pageName, int index)
{
    QDF_D(QdfRibbonCustomizeManager)

    if (!isEditMode())
    {
        setEditMode(true);
    }

    QdfRibbonPage *newPage = new QdfRibbonPage();
    newPage->setTitle(pageName);
    newPage->setProperty(__qdf_Widget_Custom, QLatin1String("__qdf_Page_Custom"));

    if (index == -1)
    {
        d->m_ribbonManager->m_regularPages.append(newPage);
    }
    else
    {
        d->m_ribbonManager->m_regularPages.insert(index, newPage);
    }
    return newPage;
}

void QdfRibbonCustomizeManager::deletePage(QdfRibbonPage *page)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    d->m_ribbonManager->deleteFreeGroups();
    d->m_ribbonManager->m_regularPages.removeOne(page);
    d->m_ribbonManager->m_regularPageGroups.remove(page);

    if (page->parentWidget() == nullptr)
    {
        delete page;
    }
}

int QdfRibbonCustomizeManager::pageIndex(QdfRibbonPage *page) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPages.indexOf(page);
}

void QdfRibbonCustomizeManager::movePage(int oldIndex, int newIndex)
{
    QDF_D(QdfRibbonCustomizeManager)

    QdfRibbonPage *page = d->m_ribbonManager->m_regularPages.at(oldIndex);
    d->m_ribbonManager->m_regularPages.removeAt(oldIndex);
    d->m_ribbonManager->m_regularPages.insert(newIndex, page);
}

QList<QdfRibbonGroup *> QdfRibbonCustomizeManager::pageGroups(QdfRibbonPage *page) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPageGroups.value(page);
}

QdfRibbonGroup *QdfRibbonCustomizeManager::createGroup(QdfRibbonPage *page, const QString &groupName,
                                                       int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    QdfRibbonGroup *newGroup = new QdfRibbonGroup();
    newGroup->setTitle(groupName);

    if (index == -1)
    {
        d->m_ribbonManager->m_regularPageGroups[page].append(newGroup);
    }
    else
    {
        d->m_ribbonManager->m_regularPageGroups[page].insert(index, newGroup);
    }
    return newGroup;
}

void QdfRibbonCustomizeManager::deleteGroup(QdfRibbonPage *page, int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    QdfRibbonGroup *group = d->m_ribbonManager->m_regularPageGroups[page].takeAt(index);
    Q_ASSERT(group != nullptr);
    if (group->parent() == nullptr)
    {
        delete group;
    }
}

void QdfRibbonCustomizeManager::moveGroup(QdfRibbonPage *page, int oldIndex, int newIndex)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }

    QdfRibbonGroup *group = d->m_ribbonManager->m_regularPageGroups[page].at(oldIndex);
    d->m_ribbonManager->m_regularPageGroups[page].removeAt(oldIndex);
    d->m_ribbonManager->m_regularPageGroups[page].insert(newIndex, group);
}

void QdfRibbonCustomizeManager::insertAction(QdfRibbonGroup *group, QAction *action, int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }
    d->m_ribbonManager->m_regularGroupsActions[group].insert(index, action);
}

void QdfRibbonCustomizeManager::appendActions(QdfRibbonGroup *group, const QList<QAction *> &actions)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }
    d->m_ribbonManager->m_regularGroupsActions[group] = actions;
}

void QdfRibbonCustomizeManager::clearActions(QdfRibbonGroup *group)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }
    d->m_ribbonManager->m_regularGroupsActions.remove(group);
}

void QdfRibbonCustomizeManager::removeActionAt(QdfRibbonGroup *group, int index)
{
    QDF_D(QdfRibbonCustomizeManager)
    if (!isEditMode())
    {
        setEditMode(true);
    }
    d->m_ribbonManager->m_regularGroupsActions[group].removeAt(index);
}

bool QdfRibbonCustomizeManager::containsAction(QdfRibbonGroup *group, QAction *action) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularGroupsActions.value(group).contains(action);
}

QList<QAction *> QdfRibbonCustomizeManager::actionsGroup(QdfRibbonGroup *group) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularGroupsActions.value(group);
}

bool QdfRibbonCustomizeManager::isPageHidden(QdfRibbonPage *page) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularHiddenPages.contains(page);
}

void QdfRibbonCustomizeManager::setPageHidden(QdfRibbonPage *page, bool hide)
{
    if (!isEditMode())
    {
        setEditMode(true);
    }

    QDF_D(QdfRibbonCustomizeManager)
    if (hide)
    {
        d->m_ribbonManager->m_regularHiddenPages.append(page);
    }
    else
    {
        d->m_ribbonManager->m_regularHiddenPages.removeOne(page);
    }
}

bool QdfRibbonCustomizeManager::isPageVisible(QdfRibbonPage *page) const
{
    return !isPageHidden(page);
}

void QdfRibbonCustomizeManager::setPageVisible(QdfRibbonPage *page, bool visible)
{
    setPageHidden(page, !visible);
}

QString QdfRibbonCustomizeManager::pageName(QdfRibbonPage *page) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPagesName.value(page);
}

void QdfRibbonCustomizeManager::setPageName(QdfRibbonPage *page, const QString &pageName)
{
    QDF_D(const QdfRibbonCustomizeManager)
    d->m_ribbonManager->m_regularPagesName.insert(page, pageName);
}

QString QdfRibbonCustomizeManager::groupName(QdfRibbonGroup *group) const
{
    QDF_D(const QdfRibbonCustomizeManager)
    return d->m_ribbonManager->m_regularNameGroups.value(group);
}

void QdfRibbonCustomizeManager::setGroupName(QdfRibbonGroup *group, const QString &groupName)
{
    QDF_D(const QdfRibbonCustomizeManager)
    d->m_ribbonManager->m_regularNameGroups.insert(group, groupName);
}

void QdfRibbonCustomizeManager::addDefaultStateQAccessBar()
{
    QDF_D(QdfRibbonCustomizeManager)
    if (QdfRibbonQuickAccessBar *quickAccessBar = d->m_ribbonBar->quickAccessBar())
    {
        d->m_ribbonManager->addDefaultToolBar(quickAccessBar);
    }
}

void QdfRibbonCustomizeManager::addDefaultStateRibbonBar()
{
    QDF_D(QdfRibbonCustomizeManager)
    d->m_ribbonManager->addDefaultPages(d->m_ribbonBar);
}

bool QdfRibbonCustomizeManager::saveStateToDevice(QIODevice *device)
{
    QDF_D(const QdfRibbonCustomizeManager)

    QXmlStreamWriter xmlwriter(device);

    xmlwriter.writeStartDocument("1.0");
    xmlwriter.writeNamespace("http://gitee.com/icanpool/qtcanpool", "QRibbon");
    xmlwriter.writeStartElement(QDF_DIC_TAGNAME_RIBBON);
    xmlwriter.writeAttribute("type", "customize");
    xmlwriter.writeAttribute("version", "1.0");

    d->m_ribbonManager->saveState(xmlwriter);

    xmlwriter.writeEndElement();
    xmlwriter.writeEndDocument();

    return true;
}

bool QdfRibbonCustomizeManager::loadStateFromDevice(QIODevice *device)
{
    QDF_D(const QdfRibbonCustomizeManager)

    QXmlStreamReader xmlreader(device);
    if (xmlreader.readNext() != QXmlStreamReader::StartDocument)
    {
        return false;
    }
    if (xmlreader.readNext() != QXmlStreamReader::StartElement)
    {
        return false;
    }

    if (xmlreader.name() != QDF_DIC_TAGNAME_RIBBON)
    {
        return false;
    }

    d->m_ribbonManager->restoreState(xmlreader);

    if (xmlreader.readNext() != QXmlStreamReader::EndElement)
    {
        return false;
    }
    if (xmlreader.readNext() != QXmlStreamReader::EndDocument)
    {
        return false;
    }

    return true;
}
