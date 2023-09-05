#ifndef DESKTOPFRAMEWORK_QDFRESIZEHANDLE_H
#define DESKTOPFRAMEWORK_QDFRESIZEHANDLE_H

#include <QFrame>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfResizeHandlePrivate;
class QdfResizeHandle : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
public:
    explicit QdfResizeHandle(Qt::Edge handlePostion, QWidget *parent);
    ~QdfResizeHandle() override;

    void setHandlePosition(Qt::Edge handlePositon);
    Qt::Edge handlePosition() const;
    Qt::Orientation orientation() const;

    QSize sizeHint() const override;
    bool isResizing() const;
    void setMaxResizeSize(int size);
    void setMinResizeSize(int size);

    void setOpaqueResize(bool opaque = true);
    bool opaqueResize() const;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QDF_DECLARE_PRIVATE(QdfResizeHandle)
    Q_DISABLE_COPY(QdfResizeHandle)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRESIZEHANDLE_H