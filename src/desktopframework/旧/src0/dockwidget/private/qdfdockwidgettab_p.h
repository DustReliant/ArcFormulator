#ifndef DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_P_H
#define DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_P_H

#include "../qdffloatingdragpreview.h"
#include <QAbstractButton>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QSpacerItem>
#include <dockwidget/qdfdockmanager.h>
#include <dockwidget/qdffloatingdockcontainer.h>
#include <dockwidget/qdffloatingwidget.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfElidingLabel;
class QdfDockFocusController;
class QdfDockWidgetTabPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockWidgetTab)

    QdfDockWidgetTabPrivate() = default;
    void createLayout();
    void moveTab(QMouseEvent *event);
    bool isDraggingState(DragState dragState) const;
    bool startFloating(DragState dragState = DragState::DS_FloatingWidget);
    bool testConfigFlag(QdfDockManager::ConfigFlag flag) const;
    QAbstractButton *createCloseButton() const;
    void updateCloseButtonVisibility(bool active);
    void updateCloseButtonSizePolicy();

    template<typename T>
    QdfFloatingWidget *createFloatingWidget(T *widget, bool createContainer)
    {
        QDF_Q(QdfDockWidgetTab)
        if (createContainer)
        {
            return new QdfFloatingDockContainer(widget);
        }
        else
        {
            auto w = new QdfFloatingDragPreview(widget);
            q->connect(w, &QdfFloatingDragPreview::draggingCanceled, [=]() {
                dragState = DragState::DS_Inactive;
            });
            return w;
        }
    }

    void saveDragStartMousePosition(const QPoint &globalPos);
    void updateIcon();
    QdfDockFocusController *focusController() const;
    QAction *createAutoHideToAction(const QString &title, SideBarLocation location,
                                    QMenu *menu);

private:
    QdfDockWidget *dockWidget;
    QLabel *iconLabel = nullptr;
    QdfElidingLabel *titleLabel;
    QPoint globalDragStartMousePosition;
    QPoint dragStartMousePosition;
    bool isActiveTab = false;
    QdfDockAreaWidget *dockArea = nullptr;
    DragState dragState = DragState::DS_Inactive;
    QdfFloatingWidget *floatingWidget = nullptr;
    QIcon icon;
    QAbstractButton *closeButton = nullptr;
    QSpacerItem *iconTextSpacer;
    QPoint tabDragStartPosition;
    QSize iconSize;

    Q_DISABLE_COPY(QdfDockWidgetTabPrivate)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_P_H