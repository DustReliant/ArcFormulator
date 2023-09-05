#ifndef DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_H
#define DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_H

#include <QTimer>
#include <QWidget>
#include <qdf_global.h>
#include <ribbon/qdfofficestyle.h>

QDF_BEGIN_NAMESPACE

class QdfOfficePopupWindowPrivate;

class QDF_EXPORT QdfOfficePopupWindow : public QWidget
{
    Q_OBJECT
    Q_ENUMS(PopupAnimation)
    Q_ENUMS(PopupLocation)
    Q_PROPERTY(PopupAnimation animation READ animation WRITE setAnimation)
    Q_PROPERTY(qreal transparency READ transparency WRITE setTransparency)
    Q_PROPERTY(int displayTime READ displayTime WRITE setDisplayTime)
    Q_PROPERTY(int animationSpeed READ animationSpeed WRITE setAnimationSpeed)
    Q_PROPERTY(QIcon titleIcon READ titleIcon WRITE setTitleIcon)
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText)
    Q_PROPERTY(QString bodyText READ bodyText WRITE setBodyText)
    Q_PROPERTY(
            bool titleCloseButtonVisible READ isTitleCloseButtonVisible WRITE setTitleCloseButtonVisible)
    Q_PROPERTY(PopupLocation location READ location WRITE setLocation)
public:
    QdfOfficePopupWindow(QWidget *parent);
    virtual ~QdfOfficePopupWindow();

    static void
    showPopup(QWidget *parent, const QIcon &titleIcon, const QString &titleText, const QString &bodyText,
              bool closeButtonVisible = true,
              QdfOfficeStyle::OfficePopupDecoration decoration = QdfOfficeStyle::PopupSystemDecoration);

public:
    bool showPopup();
    bool showPopup(const QPoint &pos);

    void setCentralWidget(QWidget *widget);
    QWidget *centralWidget() const;

    void setBodyText(const QString &text);
    const QString &bodyText() const;

    void setTitleIcon(const QIcon &icon);
    const QIcon &titleIcon() const;

    void setTitleText(const QString &text);
    const QString &titleText() const;

    void setCloseButtonPixmap(const QPixmap &pm);
    const QPixmap &closeButtonPixmap() const;

    void setTitleCloseButtonVisible(bool visible = true);
    bool isTitleCloseButtonVisible() const;

    QPoint getPosition() const;
    void setPosition(const QPoint &pos);

    void setAnimation(PopupAnimation popupAnimation);
    PopupAnimation animation() const;

    void setTransparency(qreal transparency);
    qreal transparency() const;

    void setDisplayTime(int time);
    int displayTime() const;

    void setAnimationSpeed(int time);
    int animationSpeed() const;

    void setDragDropEnabled(bool enabled);
    bool dragDropEnabled() const;

    void setLocation(PopupLocation location);
    PopupLocation location() const;

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();

public Q_SLOTS:
    void closePopup();

protected Q_SLOTS:
    void showDelayTimer();
    void collapsingTimer();
    void expandingTimer();

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfOfficePopupWindow)
    Q_DISABLE_COPY(QdfOfficePopupWindow)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_H