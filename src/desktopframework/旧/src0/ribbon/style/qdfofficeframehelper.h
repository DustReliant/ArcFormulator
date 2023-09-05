
#ifndef DESKTOPFRAMEWORK_QDFOFFICEFRAMEHELPER_H
#define DESKTOPFRAMEWORK_QDFOFFICEFRAMEHELPER_H

#include <QIcon>
#include <QPixmap>
#include <QRect>
#include <QStyle>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfContextHeader;
class QdfStyleOptionTitleBar;

class QdfOfficeFrameHelper
{
public:
    virtual ~QdfOfficeFrameHelper() {}

public:
    virtual void enableOfficeFrame(QWidget *parent) = 0;

    virtual bool isDwmEnabled() const = 0;
    virtual bool isActive() const = 0;
    virtual bool isMaximize() const = 0;
    virtual bool canHideTitle() const = 0;

    virtual int frameBorder() const = 0;
    virtual int titleBarSize() const = 0;
    virtual int tabBarHeight() const = 0;
    virtual bool clientMetrics(int &iCaptionWidth, int &iCaptionHeight) const = 0;

    virtual void setHeaderRect(const QRect &rcHeader) = 0;
    virtual void setContextHeader(QList<QdfContextHeader *> *listContextHeaders) = 0;

    virtual QPixmap getFrameSmallIcon() const = 0;
    virtual QString getSystemMenuString(uint item) const = 0;
    virtual QString getWindowText() const = 0;

    virtual void drawTitleBar(QPainter *painter, const QdfStyleOptionTitleBar &opt) = 0;
    virtual void fillSolidRect(QPainter *painter, const QRect &rect, const QRegion &airRegion, QColor clr) = 0;
    virtual void drawDwmCaptionText(QPainter *painter, const QRect &rect, const QString &strWindowText,
                                    const QColor &colText, bool active, bool contextTab) = 0;
    virtual QSize sizeSystemIcon(const QIcon &icon, const QRect &rect) const = 0;
#ifdef Q_OS_WIN
    virtual HICON windowIcon(const QIcon &icon, const QRect &rect) const = 0;
#endif// Q_OS_WIN
    virtual void resetWindowIcon() = 0;
    virtual void recalcFrameLayout() = 0;
    virtual void postRecalcFrameLayout() = 0;
    virtual void processClickedSubControl(QStyle::SubControl subControl) = 0;

#ifdef Q_OS_WIN
    virtual bool winEvent(MSG *message, long *result) = 0;
#endif// Q_OS_WIN
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFOFFICEFRAMEHELPER_H