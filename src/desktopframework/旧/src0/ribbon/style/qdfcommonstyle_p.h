#ifndef DESKTOPFRAMEWORK_QDFCOMMONSTYLE_P_H
#define DESKTOPFRAMEWORK_QDFCOMMONSTYLE_P_H

#include <QBasicTimer>
#include <QColor>
#include <QEvent>
#include <QPainter>
#include <QSettings>
#include <QStyleOption>
#include <QTime>
#include <QWidget>
#include <ribbon/qdfcommonstyle.h>

QDF_BEGIN_NAMESPACE


class QdfStyleHelper : public QObject
{
public:
    QdfStyleHelper(QObject *pParent);
    virtual ~QdfStyleHelper();

public:
    void openIniFile(const QString &fileName);

public:
    QString getValue(const QString &prefix, const QString &key) const;
    QColor getColor(const QString &prefix, const QString &key, QColor color = QColor()) const;
    int getInteger(const QString &prefix, const QString &key, int val) const;

public:
    static QPixmap alphaBlend(const QPixmap &src);

protected:
    QSettings *m_pSettings;
};

class QdfCommonAnimation : public QObject
{
public:
    QdfCommonAnimation() : m_widget(0), m_running(true) {}
    virtual ~QdfCommonAnimation();

public:
    QWidget *widget() const { return m_widget; }
    bool running() const { return m_running; }
    const QTime &startTime() const { return m_startTime; }
    void setRunning(bool val) { m_running = val; }
    void setWidget(QWidget *widget);
    void setStartTime(const QTime &startTime) { m_startTime = startTime; }

public:
    virtual void paint(QPainter *painter, const QStyleOption *option);

protected:
    void drawBlendedImage(QPainter *painter, QRect rect, float value);
    virtual bool eventFilter(QObject *obj, QEvent *event);

protected:
    QTime m_startTime;
    QWidget *m_widget;
    QImage m_primaryImage;
    QImage m_secondaryImage;
    QImage m_tempImage;
    bool m_running;
};

class QdfCommonTransition : public QdfCommonAnimation
{
public:
    QdfCommonTransition();
    virtual ~QdfCommonTransition();

public:
    void setDuration(int duration) { m_duration = duration; }
    void setStartImage(const QImage &image) { m_primaryImage = image; }
    void setEndImage(const QImage &image) { m_secondaryImage = image; }
    virtual void paint(QPainter *painter, const QStyleOption *option);
    int duration() const { return m_duration; }

public:
    int m_duration;
};

class QdfCommonPaintManager : public QObject
{
public:
    QdfCommonPaintManager(QdfCommonStyle *baseStyle);
    virtual ~QdfCommonPaintManager();

public:
    QdfCommonStyle *baseStyle() const;
    QdfStyleHelper &helper() const;
    QPixmap cached(const QString &img) const;
    QPixmap cachedPath(const QString &img) const;
    QRect sourceRectImage(QRect rcSrc, int state = 0, int count = 1) const;

    void drawImage(const QPixmap &soSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, QRect rcSizingMargins,
                   QColor clrTransparent, bool alphaBlend = false) const;
    void drawImage(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc,
                   QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;
    void drawPixmap(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, bool alpha,
                    QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

    virtual void modifyColors() = 0;

protected:
    QdfCommonStyle *m_base;

private:
    Q_DISABLE_COPY(QdfCommonPaintManager)
};

class QdfCommonStylePrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfCommonStyle)
public:
    explicit QdfCommonStylePrivate();
    virtual ~QdfCommonStylePrivate();

public:
    void initialization();

    void doTimer();
    void startAnimation(QdfCommonAnimation *);
    void stopAnimation(const QWidget *);
    QdfCommonAnimation *widgetAnimation(const QWidget *widget) const;

    void setPalette(QWidget *widget);
    void unsetPalette(QWidget *widget);

    void setAutoFillBackground(QWidget *widget);
    void unsetAutoFillBackground(QWidget *widget);

    void setPaintManager(QdfCommonPaintManager &paintManager);
    virtual void makePaintManager() = 0;

public:
    QdfCommonPaintManager *m_paintManager;
    QStyle *m_defaultStyle;
    QdfStyleHelper *m_helper;
    QList<QdfCommonAnimation *> m_animations;
    QBasicTimer m_animationTimer;
    QHash<const QWidget *, QPalette> m_customPaletteWidgets;
    QHash<const QWidget *, bool> m_customAutoFillBackground;
};

#define QDF_D_STYLE(Class) Class##Private *d = static_cast<Class *>(baseStyle())->qdf_d();

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFCOMMONSTYLE_P_H