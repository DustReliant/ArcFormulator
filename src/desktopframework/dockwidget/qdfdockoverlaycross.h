#ifndef DESKTOPFRAMEWORK_QDFDOCKOVERLAYCROSS_H
#define DESKTOPFRAMEWORK_QDFDOCKOVERLAYCROSS_H

#include <QWidget>
#include <dockwidget/qdfdockoverlay.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockOverlayCrossPrivate;
class QdfDockOverlayCross : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString iconColors READ iconColors WRITE setIconColors)
    Q_PROPERTY(QColor iconFrameColor READ iconColor WRITE setIconFrameColor)
    Q_PROPERTY(QColor iconBackgroundColor READ iconColor WRITE setIconBackgroundColor)
    Q_PROPERTY(QColor iconOverlayColor READ iconColor WRITE setIconOverlayColor)
    Q_PROPERTY(QColor iconArrowColor READ iconColor WRITE setIconArrowColor)
    Q_PROPERTY(QColor iconShadowColor READ iconColor WRITE setIconShadowColor)

public:
    enum IconColor
    {
        FrameColor,
        WindowBackgroundColor,
        OverlayColor,
        ArrowColor,
        ShadowColor
    };
    Q_ENUM(IconColor)

public:
    QdfDockOverlayCross(QdfDockOverlay *overlay);
    virtual ~QdfDockOverlayCross();

    void setIconColor(IconColor index, const QColor &color);
    QColor iconColor(IconColor index) const;
    DockWidgetArea cursorLocation() const;
    void setupOverlayCross(QdfDockOverlay::Mode mode);
    void updateOverlayIcons();

    void reset();
    void updatePosition();
    void setIconColors(const QString &colors);

protected:
    virtual void showEvent(QShowEvent *event) override;
    void setAreaWidgets(const QHash<DockWidgetArea, QWidget *> &widgets);

    QString iconColors() const;
    QColor iconColor() const
    {
        return QColor();
    }
    int areaAlignment(const DockWidgetArea &area);

    void setIconFrameColor(const QColor &color)
    {
        setIconColor(FrameColor, color);
    }
    void setIconBackgroundColor(const QColor &color)
    {
        setIconColor(WindowBackgroundColor, color);
    }
    void setIconOverlayColor(const QColor &color)
    {
        setIconColor(OverlayColor, color);
    }
    void setIconArrowColor(const QColor &color)
    {
        setIconColor(ArrowColor, color);
    }
    void setIconShadowColor(const QColor &color)
    {
        setIconColor(ShadowColor, color);
    }

private:
    QDF_DECLARE_PRIVATE(QdfDockOverlayCross)
    Q_DISABLE_COPY(QdfDockOverlayCross)
    friend class QdfDockOverlay;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKOVERLAYCROSS_H