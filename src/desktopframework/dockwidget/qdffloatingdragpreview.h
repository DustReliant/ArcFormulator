#ifndef DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_H
#define DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_H

#include <QWidget>
#include <dockwidget/qdffloatingwidget.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfDockAreaWidget;

class QdfFloatingDragPreviewPrivate;
class QdfFloatingDragPreview : public QWidget, public QdfFloatingWidget
{
    Q_OBJECT
public:
    QdfFloatingDragPreview(QdfDockWidget *content);
    QdfFloatingDragPreview(QdfDockAreaWidget *content);
    ~QdfFloatingDragPreview();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

    virtual void startFloating(const QPoint &point, const QSize &size, DragState state,
                               QWidget *mouseEventHandler) override;
    virtual void moveFloating() override;
    virtual void finishDragging() override;
    void cleanupAutoHideContainerWidget();

Q_SIGNALS:
    void draggingCanceled();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    QdfFloatingDragPreview(QWidget *content, QWidget *parent);

private Q_SLOTS:
    void onApplicationStateChanged(Qt::ApplicationState state);

private:
    QDF_DECLARE_PRIVATE(QdfFloatingDragPreview)
    Q_DISABLE_COPY(QdfFloatingDragPreview)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGDRAGPREVIEW_H