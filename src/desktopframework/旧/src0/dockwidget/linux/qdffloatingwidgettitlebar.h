#ifndef DESKTOPFRAMEWORK_QDFFLOATINGWIDGETTITLEBAR_H
#define DESKTOPFRAMEWORK_QDFFLOATINGWIDGETTITLEBAR_H

#include <QFrame>
#include <QIcon>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfFloatingDockContainer;
struct QdfFloatingWidgetTitleBarPrivate;

class QdfFloatingWidgetTitleBar : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QIcon maximizeIcon READ maximizeIcon WRITE setMaximizeIcon)
    Q_PROPERTY(QIcon normalIcon READ normalIcon WRITE setNormalIcon)

public:
    explicit QdfFloatingWidgetTitleBar(QdfFloatingDockContainer *parent = nullptr);
    virtual ~QdfFloatingWidgetTitleBar();
    void enableCloseButton(bool Enable);
    void setTitle(const QString &Text);
    void updateStyle();
    void setMaximizedIcon(bool maximized);

signals:
    void closeRequested();
    void maximizeRequested();

protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    void setMaximizeIcon(const QIcon &Icon);
    QIcon maximizeIcon() const;
    void setNormalIcon(const QIcon &Icon);
    QIcon normalIcon() const;

private:
    QDF_DECLARE_PRIVATE(QdfFloatingWidgetTitleBar)
    Q_DISABLE_COPY(QdfFloatingWidgetTitleBar)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGWIDGETTITLEBAR_H
