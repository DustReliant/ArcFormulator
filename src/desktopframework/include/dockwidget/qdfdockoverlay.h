#ifndef DESKTOPFRAMEWORK_QDFDOCKOVERLAY_H
#define DESKTOPFRAMEWORK_QDFDOCKOVERLAY_H

#include <QFrame>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockOverlayPrivate;
class QDF_EXPORT QdfDockOverlay : public QFrame
{
    Q_OBJECT
public:
    enum Mode
    {
        ModeDockAreaOverlay,
        ModeContainerOverlay
    };
    Q_ENUM(Mode)

public:
    QdfDockOverlay(QWidget *parent, Mode Mode = ModeDockAreaOverlay);
    virtual ~QdfDockOverlay() override;

    void setAllowedAreas(DockWidgetAreas areas);

    DockWidgetAreas allowedAreas() const;
    DockWidgetArea dropAreaUnderCursor() const;
    DockWidgetArea visibleDropAreaUnderCursor() const;
    DockWidgetArea showOverlay(QWidget *target);

    void hideOverlay();
    void enableDropPreview(bool enable);
    bool dropPreviewEnabled() const;
    QRect dropOverlayRect() const;
    bool event(QEvent *event) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QDF_DECLARE_PRIVATE(QdfDockOverlay)
    Q_DISABLE_COPY(QdfDockOverlay)
    friend class QdfDockOverlayCross;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKOVERLAY_H