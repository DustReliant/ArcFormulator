#ifndef DESKTOPFRAMEWORK_QDFELIDINGLABEL_H
#define DESKTOPFRAMEWORK_QDFELIDINGLABEL_H

#include <QLabel>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfElidingLabelPrivate;
class QDF_EXPORT QdfElidingLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QdfElidingLabel(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    explicit QdfElidingLabel(const QString &text, QWidget *parent = nullptr,
                             Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~QdfElidingLabel() override;

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode mode);
    bool isElide() const;

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

    void setText(const QString &text);
    QString text() const;

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void elidedChanged(bool);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QDF_DECLARE_PRIVATE(QdfElidingLabel)
    Q_DISABLE_COPY(QdfElidingLabel)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFELIDINGLABEL_H