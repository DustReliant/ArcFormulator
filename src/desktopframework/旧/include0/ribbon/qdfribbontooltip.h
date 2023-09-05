#ifndef DESKTOPFRAMEWORK_QDFRIBBONTOOLTIP_H
#define DESKTOPFRAMEWORK_QDFRIBBONTOOLTIP_H

#include <QEvent>
#include <QIcon>
#include <QLabel>
#include <QTimer>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonToolTipPrivate;
class QDF_EXPORT QdfRibbonToolTip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
public:
    QdfRibbonToolTip(const QString &title, const QString &text, const QIcon &icon = QIcon(), QWidget *parent = nullptr);
    virtual ~QdfRibbonToolTip();
    static QdfRibbonToolTip *instance();

public:
    static void showToolTip(const QPoint &pos, const QString &title, const QString &text, const QIcon &icon = QIcon(),
                            QWidget *parent = nullptr);
    static void hideToolTip();
    static bool isToolTipVisible();

    enum WrapMode : uchar
    {
        NoWrap = 0,
        NativeWrap = NoWrap,
        AutoWrap,
    };

    static void setWrapMode(WrapMode mode);
    static WrapMode wordWrap();

    static void setIconVisible(bool visible);
    static bool isIconVisible();

public:
    int margin() const;
    void setMargin(int);

    const QString &title() const;
    const QString &text() const;

    void setIcon(const QIcon &icon);
    const QIcon &icon() const;

public:
    void reuseTip(const QString &textTitle, const QString &text);
    void hideTip();
    void closeToolTip();
    void setTipRect(QWidget *w, const QRect &r);
    void restartExpireTimer();
    bool tipChanged(const QPoint &pos, const QString &text, const QString &textTitle, QObject *o);
    void placeTip(const QPoint &pos, QWidget *w);

    static int getTipScreen(const QPoint &pos, QWidget *w);

public Q_SLOTS:
    void setTitle(const QString &);
    void setText(const QString &);

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool eventFilter(QObject *, QEvent *event);
    virtual void timerEvent(QTimerEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

public:
    bool fadingOut() const;

private:
    QDF_DECLARE_PRIVATE(QdfRibbonToolTip)
    Q_DISABLE_COPY(QdfRibbonToolTip)
};

class QdfRibbonKeyTipPrivate;
class QDF_EXPORT QdfRibbonKeyTip : public QFrame
{
    Q_OBJECT
public:
    enum AlignTip
    {
        AlignTipTop = 0x00000000,
        AlignTipLeft = 0x00000000,
        AlignTipCenter = 0x00000001,
        AlignTipRight = 0x00000002,
        AlignTipVCenter = 0x00000004,
        AlignTipBottom = 0x00000008,
    };

public:
    QdfRibbonKeyTip(QWidget *bars, QWidget *owner, const QString &caption, const QPoint &pt, uint align, bool enabled,
                    QAction *action = nullptr);
    virtual ~QdfRibbonKeyTip();

public:
    QPoint posTip() const;
    void setVisibleTip(bool visible);
    bool isVisibleTip() const;
    bool isEnabledTip() const;

    QWidget *getBars() const;
    uint getAlign() const;

    void setExplicit(bool exp);
    bool isExplicit() const;

    QString getStringTip() const;
    void setStringTip(const QString &str);

    QString getCaption() const;
    void setCaption(const QString &str);

    QString getPrefix() const;
    void setPrefix(const QString &pref);

    QWidget *getOwner() const;
    QAction *getAction() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonKeyTip)
    Q_DISABLE_COPY(QdfRibbonKeyTip)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONTOOLTIP_H