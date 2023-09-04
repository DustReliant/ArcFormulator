#ifndef DESKTOPFRAMEWORK_QDFDOCKMANAGER_P_H
#define DESKTOPFRAMEWORK_QDFDOCKMANAGER_P_H

#include "../qdfdockwidgethelper.h"
#include <QList>
#include <QMap>
#include <QMenu>
#include <QString>
#include <QVector>
#include <dockwidget/qdfdockmanager.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfFloatingDockContainer;
class QdfDockOverlay;
class QdfDockWidget;

class QdfDockManagerPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockManager)

public:
    QdfDockManagerPrivate();
    void restoreDockWidgetsOpenState();
    void restoreDockAreasIndices();
    void emitTopLevelEvents();
    void hideFloatingWidgets();
    void markDockWidgetsDirty();
    void loadStylesheet();
    void addActionToMenu(QAction *action, QMenu *menu, bool insertSorted);


    QList<QdfFloatingDockContainer *> floatingWidgets;
    QList<QdfFloatingDockContainer *> hiddenFloatingWidgets;
    QList<QdfDockContainerWidget *> containers;
    QdfDockOverlay *containerOverlay;
    QdfDockOverlay *dockAreaOverlay;
    QMap<QString, QdfDockWidget *> dockWidgetsMap;
    QMap<QString, QMenu *> viewMenuGroups;
    QMenu *viewMenu;
    QdfDockManager::ViewMenuInsertionOrder menuInsertionOrder = QdfDockManager::MenuAlphabeticallySorted;
    bool restoringState = false;
    QVector<QdfFloatingDockContainer *> uninitializedFloatingWidgets;
    QdfDockFocusController *focusController = nullptr;
    QdfDockWidget *centralWidget = nullptr;
    bool isLeavingMinimized = false;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKMANAGER_P_H