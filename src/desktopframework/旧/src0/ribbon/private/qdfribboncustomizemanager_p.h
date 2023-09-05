#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_P_H

#include <QAction>
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QToolBar>
#include <QXmlStreamWriter>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE
#define UserNameID 1000    // first user name id
#define MaxUserNameID 65535// last user name id


class QdfCustomizeEngineAbstract : public QObject
{
public:
    explicit QdfCustomizeEngineAbstract(QObject *parent);
    virtual ~QdfCustomizeEngineAbstract();

public:
    QString categoryAllActions() const;
    QList<QAction *> actionsAll() const;

    QList<QAction *> actionsByCategory(const QString &category) const;
    QStringList categories() const;

    QString actionId(QAction *action) const;
    void setActionId(QAction *action, const QString &id = QString());

    void addToCategory(const QString &category, QAction *action);
    void addAllActionsCategory(const QString &category);

    virtual void saveState(QXmlStreamWriter &stream) const = 0;
    virtual bool restoreState(QXmlStreamReader &stream) = 0;

protected:
    QString generateUniqueNameIdentifier(int hint = -1);
    QString generateUniqueNameIdentifierGroup(int hint = -1);
    QString generateUniqueNameIdentifierPage(int hint = -1);

protected:
    QSet<int> m_setUniqueIdentifier;
    QSet<int> m_setUniqueIdentifierGroup;
    QSet<int> m_setUniqueIdentifierPage;

    QString m_allCategoryName;
    QList<QString> m_lstCategory;

    QMap<QString, QAction *> m_allNameToActions;
    QMap<QAction *, QString> m_allActionsToName;

    QMap<QString, QList<QAction *>> m_categoryToActions;
    QMap<QAction *, QString> m_actionToCategory;

private:
    Q_DISABLE_COPY(QdfCustomizeEngineAbstract)
};


class QdfToolBarCustomizeEngine : public QdfCustomizeEngineAbstract
{
public:
    explicit QdfToolBarCustomizeEngine(QObject *parent);
    virtual ~QdfToolBarCustomizeEngine();

public:
    QList<QAction *> actions(QToolBar *toolBar) const;
    QToolBar *toolBarWidgetAction(QAction *action) const;
    QToolBar *findDefaultToolBar(const QString &objectName) const;
    QMap<QToolBar *, QList<QAction *>> defaultToolBars() const;

    void removeWidgetActions(const QMap<QToolBar *, QList<QAction *>> &actions);
    void addDefaultToolBar(QToolBar *toolBar);

    QList<QToolBar *> toolBarsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QToolBar *bar);

    void setToolBar(QToolBar *toolBar, const QList<QAction *> &actions);

    void saveStateQuickAccessBar(QXmlStreamWriter &stream) const;
    bool restoreStateQuickAccessBar(QXmlStreamReader &stream);

    void saveState(QXmlStreamWriter &stream) const;
    bool restoreState(QXmlStreamReader &stream);

protected:
    QMap<QAction *, QToolBar *> m_widgetActions;
    QMap<QAction *, QList<QToolBar *>> m_actionToToolBars;

    QMap<QString, QList<QToolBar *>> m_categoryToBars;
    QMap<QToolBar *, QString> m_barToCategory;

    QMap<QToolBar *, QList<QAction *>> m_defaultToolBars;
    QMap<QToolBar *, QList<QAction *>> m_toolBarsWithSeparators;
    QMap<QToolBar *, QList<QAction *>> m_toolBars;
    QMap<QToolBar *, QList<QAction *>> m_regularToolBars;

private:
    Q_DISABLE_COPY(QdfToolBarCustomizeEngine)
};

class QdfRibbonPage;
class QdfRibbonGroup;
class QdfRibbonBar;
class QdfRibbonBarCustomizeEngine : public QdfToolBarCustomizeEngine
{
public:
    explicit QdfRibbonBarCustomizeEngine(QObject *parent, QdfRibbonBar *ribbonBar);
    virtual ~QdfRibbonBarCustomizeEngine();

public:
    QString pageId(QdfRibbonPage *page) const;
    void setPageId(QdfRibbonPage *page, const QString &id = QString());

    QString groupId(QdfRibbonGroup *group) const;
    void setGroupId(QdfRibbonGroup *group, const QString &id = QString());

    void addToCategory(const QString &category, QAction *act);

    QList<QdfRibbonPage *> pagesByCategory(const QString &category) const;
    void addToCategory(const QString &category, QdfRibbonPage *page);

    QList<QdfRibbonGroup *> groupsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QdfRibbonGroup *group);

    QList<QdfRibbonGroup *> pageDefaultGroups(QdfRibbonPage *page) const;
    QList<QdfRibbonPage *> defaultHiddenPages() const;
    QList<QdfRibbonPage *> editHiddenPages() const;

    QMap<QdfRibbonPage *, QString> defaultPagesName() const;
    QString pageDefaultName(QdfRibbonPage *page) const;

    QList<QdfRibbonGroup *> regularGroups(QdfRibbonPage *page) const;
    QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>> defaultPageGroups() const;
    QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>> regularPageGroups() const;
    QMap<QdfRibbonGroup *, QString> defaultGroupsName() const;
    QString groupDefaultName(QdfRibbonGroup *group) const;

    QList<QdfRibbonPage *> defaultRibbonBar() const;

    void addDefaultPages(QdfRibbonBar *ribbonBar);
    void addDefaultGroups(QdfRibbonPage *page);
    void addNonContextCategories();

    void setQuickAccessBar();
    void setRibbonBar();
    void setRibbonGroups();
    void updateRibbonBar();
    void deleteFreeGroups();
    void deleteFreePages();

    void saveStateGroup(QXmlStreamWriter &stream, QdfRibbonPage *page) const;
    bool restoreStateGroup(QXmlStreamReader &stream, QdfRibbonPage *page);

    void saveStatePage(QXmlStreamWriter &stream) const;
    bool restoreStatePage(QXmlStreamReader &stream);

    void saveStateRibbonBar(QXmlStreamWriter &stream) const;
    bool restoreStateRibbonBar(QXmlStreamReader &stream);

    void saveState(QXmlStreamWriter &stream) const;
    bool restoreState(QXmlStreamReader &stream);

protected:
    QdfRibbonBar *m_ribbonBar;

    QMap<QString, QList<QdfRibbonPage *>> m_categoryToPages;
    QMap<QdfRibbonPage *, QString> m_pageToCategory;

    QMap<QString, QList<QdfRibbonGroup *>> m_categoryToGroups;
    QMap<QdfRibbonGroup *, QString> m_groupToCategory;

    QList<QdfRibbonPage *> m_defaultPages;
    QList<QdfRibbonPage *> m_regularPages;
    QList<QdfRibbonPage *> m_defaultHiddenPages;
    QList<QdfRibbonPage *> m_regularHiddenPages;

    QMap<QdfRibbonPage *, QString> m_defaultPagesName;
    QMap<QdfRibbonPage *, QString> m_regularPagesName;

    QMap<QString, QdfRibbonPage *> m_allNameIdToPages;
    QMap<QdfRibbonPage *, QString> m_allPagesToNameId;

    QMap<QString, QdfRibbonGroup *> m_allNameIdToGroups;
    QMap<QdfRibbonGroup *, QString> m_allGroupsToNameId;

    QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>> m_defaultPageGroups;
    QMap<QdfRibbonPage *, QList<QdfRibbonGroup *>> m_regularPageGroups;
    QMap<QdfRibbonGroup *, QString> m_defaultNameGroups;
    QMap<QdfRibbonGroup *, QString> m_regularNameGroups;

    QMap<QdfRibbonGroup *, QList<QAction *>> m_regularGroupsActions;

private:
    friend class QdfRibbonCustomizeManager;
    Q_DISABLE_COPY(QdfRibbonBarCustomizeEngine)
};


class QdfRibbonCustomizeManagerPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonCustomizeManager)
public:
    explicit QdfRibbonCustomizeManagerPrivate();
    virtual ~QdfRibbonCustomizeManagerPrivate();

public:
    void init(QdfRibbonBar *ribbonBar);

public:
    bool m_editMode;
    QdfRibbonBar *m_ribbonBar;
    QdfRibbonBarCustomizeEngine *m_ribbonManager;

private:
    Q_DISABLE_COPY(QdfRibbonCustomizeManagerPrivate)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_P_H