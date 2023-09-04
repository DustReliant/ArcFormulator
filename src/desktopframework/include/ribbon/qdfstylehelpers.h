#ifndef DESKTOPFRAMEWORK_QDFSTYLEHELPER_H
#define DESKTOPFRAMEWORK_QDFSTYLEHELPER_H

#include <QStyleOption>
#include <qdf_global.h>
#include <ribbon/qdfribbonbar.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfDrawHelpers
{
public:
    QdfDrawHelpers();
    virtual ~QdfDrawHelpers();

public:
    static void draw3DRect(QPainter &painter, QColor &col, int x, int y, int w, int h, bool up);
    static void draw3DRect(QPainter &painter, const QColor &colLight, const QColor &colDark, int x, int y, int w, int h,
                           bool up);

    static void draw3dRectEx(QPainter &painter, const QRect &rect, const QColor &clrTopLeft, const QColor &clrBottomRight);
    static void draw3dRectEx(QPainter &painter, int x, int y, int cx, int cy, const QColor &clrTopLeft,
                             const QColor &clrBottomRight);

    static void drawRectangle(QPainter &painter, const QRect &rc, const QColor &pen, const QColor &brush);
    static void drawTriangle(QPainter &painter, const QPoint &pt0, const QPoint &pt1, const QPoint &pt2,
                             const QColor &color);

    static void drawGradientFill(QPainter &painter, const QRect &rect, const QColor &crFrom, const QColor &crTo, bool horz);
    static void drawGradientFill4(QPainter &painter, const QRect &rect, const QColor &crFrom1, const QColor &crTo1,
                                  const QColor &crFrom2, const QColor &crTo2, bool horz = true, int percentage = 50);

    static QRgb lightColor(QRgb clr, int nTint);
    static QRgb darkColor(QRgb clr, int nShade);

    static void RGBtoHSL(QRgb clr, double &h, double &s, double &l);
    static QRgb HSLtoRGB(double h, double s, double l);
    static QRgb HLStoRGB_ONE(double h, double l, double s);
    static double MakeHue_ONE(double H);
    static double MakeHue_TWO(double H);

    static QRgb colorMakePale(const QRgb &clr, double lum = .97);
    static QRgb colorMakeDarker(const QRgb &clr, double ratio = .1);
    static QRgb colorMakeLighter(const QRgb &clr, double ratio = .1);

    static QRgb pixelAlpha(const QRgb &srcPixel, int percent);

    static QRgb blendColors(QRgb crA, QRgb crB, double fAmountA);

    static QImage updateImage(const QImage &imageSrc, QRgb clrBase, QRgb clrTone);
    static QImage addaptColors(const QImage &imageSrc, QRgb clrBase, QRgb clrTone, bool clampHue = true);
    static QImage invertColors(const QImage &pxSrc);

    static int defaultDpiX();

    static qreal dpiScaled(qreal value);
    static int getDPI();
    static int getDPIToPercent();

    static QIcon createIconStyleWidget(QWidget *widget);
    static QPixmap createIconExStyleWidget(QWidget *widget, int width, int height, bool &iconView);

private:
    static double HueToRGB(double temp1, double temp2, double temp3);
    static int HueToRGB(int m1, int m2, int h);
    static QLinearGradient generateGradient(const QRect &rect, const QColor &crFrom, const QColor &crTo, bool bHorz);

private:
    static double logPixel;
    static double m_colorTolerance;
};

class QdfOfficeFrameHelper;

class QDF_EXPORT QdfStyleOptionFrame : public QStyleOption
{
public:
    QdfStyleOptionFrame();
    QdfStyleOptionFrame(const QdfStyleOptionFrame &other);
    ~QdfStyleOptionFrame();

public:
    void *hdc;
    QRect clientRect;
    bool active;
    bool hasStatusBar;
    bool titleVisible;
    bool maximizeFlags;
    int frameBorder;
    int titleBarSize;
    int tabBarSize;
    int statusHeight;
    bool isBackstageVisible;
    QdfRibbonBar::TabBarPosition tabBarPosition;
};

class QDF_EXPORT QdfStyleOptionBackstageButton : public QStyleOptionToolButton
{
public:
    QdfStyleOptionBackstageButton();
    QdfStyleOptionBackstageButton(const QdfStyleOptionBackstageButton &other);

public:
    bool tabStyle;
    bool flatStyle;
};

class QDF_EXPORT QdfStyleOptionRibbonBackstage : public QStyleOption
{
public:
    QdfStyleOptionRibbonBackstage();
    QdfStyleOptionRibbonBackstage(const QdfStyleOptionRibbonBackstage &other);

public:
    int menuWidth;
};

class QDF_EXPORT QdfStyleOptionRibbon : public QStyleOption
{
public:
    QdfStyleOptionRibbon();
    QdfStyleOptionRibbon(const QdfStyleOptionRibbon &other);
    ~QdfStyleOptionRibbon();

public:
    QRect rectTabBar;
    int titleBarHeight;
    bool minimized;
    bool isBackstageVisible;
    QdfOfficeFrameHelper *frameHelper;
    QRegion airRegion;
    QPixmap pixTitleBackground;
    QdfRibbonBar::TabBarPosition tabBarPosition;
};

class QdfContextHeader;
class QDF_EXPORT QdfStyleOptionTitleBar : public QStyleOptionTitleBar
{
public:
    QdfStyleOptionTitleBar();
    QdfStyleOptionTitleBar(const QdfStyleOptionTitleBar &other);

public:
    QdfOfficeFrameHelper *frameHelper;
    QRect rcTitleText;
    QRect rectTabBar;
    QRect rcRibbonClient;
    QRect rcQuickAccess;
    bool drawIcon;
    int quickAccessVisibleCount;
    bool quickAccessOnTop;
    bool quickAccessVisible;
    bool existSysButton;
    bool normalSysButton;
    bool isBackstageVisible;
    QList<QdfContextHeader *> *listContextHeaders;
    QRegion airRegion;
    QColor clrText;

protected:
    QdfStyleOptionTitleBar(int version);
};

class QDF_EXPORT QdfStyleCaptionButton : public QStyleOptionToolButton
{
public:
    QdfStyleCaptionButton();
    QdfStyleCaptionButton(const QdfStyleCaptionButton &other);
};

class QDF_EXPORT QdfStyleSystemToolButton : public QStyleOptionToolButton
{
public:
    QdfStyleSystemToolButton();
    QdfStyleSystemToolButton(const QdfStyleSystemToolButton &other);
};

class QDF_EXPORT QdfStyleHintReturnThemeColor : public QStyleHintReturn
{
public:
    QdfStyleHintReturnThemeColor(const QString &secName, const QString &kName, QColor col = QColor());
    ~QdfStyleHintReturnThemeColor();

public:
    QString sectionName;
    QString keyName;
    QColor color;
    bool isValid;
};

class QDF_EXPORT QdfStyleRibbonQuickAccessBar : public QStyleOption
{
public:
    QdfStyleRibbonQuickAccessBar();
    QdfStyleRibbonQuickAccessBar(const QdfStyleRibbonQuickAccessBar &other);

public:
    QdfRibbonBar::QuickAccessBarPosition quickAccessBarPosition;
};

class QDF_EXPORT QdfStyleRibbonOptionHeader : public QStyleOptionHeader
{
public:
    QdfStyleRibbonOptionHeader();
    QdfStyleRibbonOptionHeader(const QdfStyleRibbonOptionHeader &other);

public:
    QString contextText;
    QdfRibbonBar::TabBarPosition tabBarPosition;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFSTYLEHELPER_H