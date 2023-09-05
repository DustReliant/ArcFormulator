#ifndef DESKTOPFRAMEWORK_QDFOFFICEPOPUPCOLORBUTTON_H
#define DESKTOPFRAMEWORK_QDFOFFICEPOPUPCOLORBUTTON_H

#include <QStyleOption>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfPopupColorButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    QdfPopupColorButton(QWidget *parent = nullptr);
    virtual ~QdfPopupColorButton();

public:
    const QColor &color() const;
    void setColor(const QColor &color);

public:
    virtual QSize sizeHint() const;

Q_SIGNALS:
    void colorChanged(const QColor &color);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

protected:
    QColor m_color;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFOFFICEPOPUPCOLORBUTTON_H