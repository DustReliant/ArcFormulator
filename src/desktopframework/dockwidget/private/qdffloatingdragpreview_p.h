#ifndef DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_P_H
#define DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_P_H

#include <QPixmap>
#include <dockwidget/qdfdockmanager.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockAreaWidget;
class QdfDockContainerWidget;

class QdfFloatingDragPreview;
class QdfFloatingDragPreviewPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfFloatingDragPreview)
    QdfFloatingDragPreviewPrivate();
    void updateDropOverlays(const QPoint &GlobalPos);
    void setHidden(bool Value);
    void cancelDragging();
    void createFloatingWidget();
    bool isContentFloatable() const;

private:
    QWidget *content = nullptr;
    QdfDockAreaWidget *contentSourceArea = nullptr;
    QPoint dragStartMousePosition;
    QdfDockManager *dockManager;
    QdfDockContainerWidget *dropContainer = nullptr;
    qreal windowOpacity;
    bool hidden = false;
    QPixmap contentPreviewPixmap;
    bool canceled = false;

private:
    Q_DISABLE_COPY(QdfFloatingDragPreviewPrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_P_H