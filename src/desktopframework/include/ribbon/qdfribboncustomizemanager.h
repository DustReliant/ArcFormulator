#ifndef DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_H
#define DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_H

#include "qdfribbonbar.h"
#include "qdfribbongroup.h"
#include "qdfribbonpage.h"
#include <QAction>
#include <QList>
#include <QObject>
#include <QString>
#include <QToolBar>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonCustomizeManagerPrivate;
class QDF_EXPORT QdfRibbonCustomizeManager : public QObject
{
    Q_OBJECT
public:
    explicit QdfRibbonCustomizeManager(QdfRibbonBar *ribbonBar);
    virtual ~QdfRibbonCustomizeManager();

public:
    QStringList categories() const;
    QList<QAction *> actionsAll() const;

    void addToolBar(QToolBar *toolBar);

    void addAllActionsCategory(const QString &category);

    QList<QAction *> actionsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QAction *act);

    QList<QToolBar *> toolBarsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QToolBar *toolBar);

    QList<QdfRibbonPage *> pagesByCategory(const QString &category) const;
    void addToCategory(const QString &category, QdfRibbonPage *page);

    QList<QdfRibbonGroup *> groupsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QdfRibbonGroup *group);

    QString actionId(QAction *action) const;
    void setActionId(QAction *action, const QString &id);

    QString pageId(QdfRibbonPage *page) const;
    void setPageId(QdfRibbonPage *page, const QString &id);

    QString groupId(QdfRibbonGroup *group) const;
    void setGroupId(QdfRibbonGroup *group, const QString &id);

    void setEditMode(bool set = true);
    bool isEditMode() const;

    void reset(QToolBar *toolBar = nullptr);
    void commit();
    void cancel();

    QList<QAction *> actions(QToolBar *toolBar) const;
    void insertAction(QToolBar *toolBar, QAction *action, int index);
    void removeActionAt(QToolBar *toolBar, int index);
    bool containsAction(QToolBar *toolBar, QAction *action) const;

    QList<QdfRibbonPage *> pages() const;
    QdfRibbonPage *createPage(const QString &pageName, int index = -1);
    void deletePage(QdfRibbonPage *page);
    int pageIndex(QdfRibbonPage *page) const;
    void movePage(int oldIndex, int newIndex);

    QList<QdfRibbonGroup *> pageGroups(QdfRibbonPage *page) const;
    QdfRibbonGroup *createGroup(QdfRibbonPage *page, const QString &groupName, int index = -1);
    void deleteGroup(QdfRibbonPage *page, int index);
    void moveGroup(QdfRibbonPage *page, int oldIndex, int newIndex);

    void insertAction(QdfRibbonGroup *group, QAction *action, int index);
    void appendActions(QdfRibbonGroup *group, const QList<QAction *> &actions);
    void clearActions(QdfRibbonGroup *group);
    void removeActionAt(QdfRibbonGroup *group, int index);
    bool containsAction(QdfRibbonGroup *group, QAction *action) const;
    QList<QAction *> actionsGroup(QdfRibbonGroup *group) const;

    bool isPageHidden(QdfRibbonPage *page) const;
    void setPageHidden(QdfRibbonPage *page, bool hide);

    bool isPageVisible(QdfRibbonPage *page) const;
    void setPageVisible(QdfRibbonPage *page, bool visible);

    QString pageName(QdfRibbonPage *page) const;
    void setPageName(QdfRibbonPage *page, const QString &pageName);

    QString groupName(QdfRibbonGroup *group) const;
    void setGroupName(QdfRibbonGroup *group, const QString &groupName);

    bool saveStateToDevice(QIODevice *device);
    bool loadStateFromDevice(QIODevice *device);

public:
    void addDefaultStateQAccessBar();
    void addDefaultStateRibbonBar();

private:
    friend class QdfRibbonQuickAccessBar;
    friend class QdfRibbonBarCustomizePagePrivate;
    QDF_DECLARE_PRIVATE(QdfRibbonCustomizeManager)
    Q_DISABLE_COPY(QdfRibbonCustomizeManager)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONCUSTOMIZEMANAGER_H
