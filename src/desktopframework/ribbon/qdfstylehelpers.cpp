#include <QApplication>
#include <QBasicTimer>
#include <QBitmap>
#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QMdiSubWindow>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QSettings>
#include <QSpinBox>
#include <QStyleFactory>
#include <QStyleOption>
#include <QToolButton>
#include <QVariant>
#include <qevent.h>
#include <qmath.h>
#include <ribbon/qdfstylehelpers.h>

#include "private/qdfribbon_p.h"
#include "private/qdfribbonbutton_p.h"

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif /* Q_OS_WIN*/

QDF_USE_NAMESPACE

double QdfDrawHelpers::m_colorTolerance = 0.2;

QdfDrawHelpers::QdfDrawHelpers()
{
}

QdfDrawHelpers::~QdfDrawHelpers()
{
}

void QdfDrawHelpers::draw3DRect(QPainter &painter, QColor &col, int x, int y, int w, int h, bool up)
{
    QPen savePen = painter.pen();

    QColor light = col.lighter(135);
    QColor dark = col.darker(140);
    painter.setPen(up ? light : dark);
    painter.drawLine(x, y, x + w, y);
    painter.drawLine(x, y, x, y + h);
    painter.setPen(up ? dark : light);
    painter.drawLine(x, y + h, x + w, y + h);
    painter.drawLine(x + w, y, x + w, y + h);
    painter.setPen(col);
    painter.drawPoint(x + w, y);
    painter.drawPoint(x, y + h);

    painter.setPen(savePen);
}

void QdfDrawHelpers::draw3DRect(QPainter &painter, const QColor &colLight, const QColor &colDark, int x,
                                int y, int w, int h, bool up)
{
    QPen savePen = painter.pen();

    QColor light = colLight;
    QColor dark = colDark;
    painter.setPen(up ? light : dark);
    painter.drawLine(x, y, x + w, y);
    painter.drawLine(x, y, x, y + h);

    painter.setPen(up ? dark : light);
    painter.drawLine(x, y + h, x + w, y + h);
    painter.drawLine(x + w, y, x + w, y + h);

    painter.setPen(colLight);
    painter.drawPoint(x + w, y);
    painter.drawPoint(x, y + h);

    painter.setPen(savePen);
}

void QdfDrawHelpers::draw3dRectEx(QPainter &painter, const QRect &rect, const QColor &clrTopLeft,
                                  const QColor &clrBottomRight)
{
    QdfDrawHelpers::draw3dRectEx(painter, rect.x(), rect.y(), rect.width(), rect.height(), clrTopLeft,
                                 clrBottomRight);
}

void QdfDrawHelpers::draw3dRectEx(QPainter &painter, int x, int y, int cx, int cy,
                                  const QColor &clrTopLeft, const QColor &clrBottomRight)
{
    painter.fillRect(x, y, cx - 1, 1, clrTopLeft);
    painter.fillRect(x, y, 1, cy - 1, clrTopLeft);
    painter.fillRect(x + cx, y, -1, cy, clrBottomRight);
    painter.fillRect(x, y + cy, cx, -1, clrBottomRight);
}

void QdfDrawHelpers::drawRectangle(QPainter &painter, const QRect &rect, const QColor &pen,
                                   const QColor &brush)
{
    if (brush.isValid())
    {
        painter.fillRect(rect, QBrush(brush));
    }

    if (pen != brush && pen.isValid())
    {
        draw3DRect(painter, pen, pen, rect.x(), rect.y(), rect.width(), rect.height(), true);
    }
}

void QdfDrawHelpers::drawTriangle(QPainter &painter, const QPoint &pt0, const QPoint &pt1,
                                  const QPoint &pt2, const QColor &color)
{
    QPen oldPen = painter.pen();
    QBrush oldBrush = painter.brush();

    painter.setPen(color);
    painter.setBrush(QBrush(color));

    QPoint pts[] = {pt0, pt1, pt2};
    painter.drawPolygon(pts, 3);

    painter.setBrush(oldBrush);
    painter.setPen(oldPen);
}

void QdfDrawHelpers::drawGradientFill(QPainter &painter, const QRect &rect, const QColor &crFrom,
                                      const QColor &crTo, bool horz)
{
    painter.save();
    painter.setBrush(generateGradient(rect, crFrom, crTo, horz));
    painter.setPen(Qt::transparent);
    painter.drawRect(rect);
    painter.restore();
}

void QdfDrawHelpers::drawGradientFill4(QPainter &painter, const QRect &rect, const QColor &crFrom1,
                                       const QColor &crTo1, const QColor &crFrom2, const QColor &crTo2,
                                       bool horz, int percentage)
{
    percentage = qMin(qMax(0, percentage), 100);

    QRect rectFirst = rect;
    QRect rectSecond = rect;

    if (!horz)
    {
        rectFirst.setRight(rectFirst.left() + rectFirst.width() * percentage / 100);
        rectSecond.setLeft(rectFirst.right());
    }
    else
    {
        rectFirst.setBottom(rectFirst.top() + rectFirst.height() * percentage / 100);
        rectSecond.setTop(rectFirst.bottom());
    }

    drawGradientFill(painter, rectFirst, crFrom1, crTo1, horz);
    drawGradientFill(painter, rectSecond, crFrom2, crTo2, horz);
}

QRgb QdfDrawHelpers::lightColor(QRgb clr, int nTint)
{
    double h, s, l;

    RGBtoHSL(clr, h, s, l);
    clr = HSLtoRGB(h, s, l + (1.0 - l) * (double) nTint / 100.0);

    return clr;
}

QRgb QdfDrawHelpers::darkColor(QRgb clr, int nShade)
{
    double h, s, l;

    RGBtoHSL(clr, h, s, l);
    clr = HSLtoRGB(h, s, l * (1 + (double) nShade / 100.0));

    return clr;
}

void QdfDrawHelpers::RGBtoHSL(QRgb clr, double &h, double &s, double &l)
{
    double r = (double) qRed(clr) / 255.0;
    double g = (double) qGreen(clr) / 255.0;
    double b = (double) qBlue(clr) / 255.0;

    double maxcolor = qMax(r, qMax(g, b));
    double mincolor = qMin(r, qMin(g, b));

    l = (maxcolor + mincolor) / 2;

    if (maxcolor == mincolor)
    {
        h = 0;
        s = 0;
    }
    else
    {
        if (l < 0.5)
        {
            s = (maxcolor - mincolor) / (maxcolor + mincolor);
        }
        else
        {
            s = (maxcolor - mincolor) / (2.0 - maxcolor - mincolor);
        }

        if (r == maxcolor)
        {
            h = (g - b) / (maxcolor - mincolor);
        }
        else if (g == maxcolor)
        {
            h = 2.0 + (b - r) / (maxcolor - mincolor);
        }
        else
        {
            h = 4.0 + (r - g) / (maxcolor - mincolor);
        }

        h /= 6.0;
        if (h < 0.0)
        {
            h += 1;
        }
    }
}

QRgb QdfDrawHelpers::HSLtoRGB(double h, double s, double l)
{
    double r, g, b;
    double temp1, temp2;

    if (s == 0)
    {
        r = g = b = l;
    }
    else
    {
        if (l < 0.5)
        {
            temp2 = l * (1.0 + s);
        }
        else
        {
            temp2 = l + s - l * s;
        }

        temp1 = 2.0 * l - temp2;

        r = HueToRGB(temp1, temp2, h + 1.0 / 3.0);
        g = HueToRGB(temp1, temp2, h);
        b = HueToRGB(temp1, temp2, h - 1.0 / 3.0);
    }

    return qRgb((int) (r * 255.0), (int) (g * 255.0), (int) (b * 255.0));
}

QRgb QdfDrawHelpers::blendColors(QRgb crA, QRgb crB, double fAmountA)
{
    double fAmountB = (1.0 - fAmountA);
    int btR = (int) (qRed(crA) * fAmountA + qRed(crB) * fAmountB);
    int btG = (int) (qGreen(crA) * fAmountA + qGreen(crB) * fAmountB);
    int btB = (int) (qBlue(crA) * fAmountA + qBlue(crB) * fAmountB);

    return qRgb(qMin(255, btR), qMin(255, btG), qMin(255, btB));
}

double QdfDrawHelpers::HueToRGB(double temp1, double temp2, double temp3)
{
    if (temp3 < 0)
    {
        temp3 = temp3 + 1.0;
    }
    if (temp3 > 1)
    {
        temp3 = temp3 - 1.0;
    }

    if (6.0 * temp3 < 1)
    {
        return (temp1 + (temp2 - temp1) * temp3 * 6.0);
    }

    else if (2.0 * temp3 < 1)
    {
        return temp2;
    }

    else if (3.0 * temp3 < 2.0)
    {
        return (temp1 + (temp2 - temp1) * ((2.0 / 3.0) - temp3) * 6.0);
    }

    return temp1;
}

int QdfDrawHelpers::HueToRGB(int m1, int m2, int h)
{
    if (h < 0)
    {
        h += 255;
    }

    if (h > 255)
    {
        h -= 255;
    }

    if ((6 * h) < 255)
    {
        return ((m1 + ((m2 - m1) / 255 * h * 6)) / 255);
    }

    if ((2 * h) < 255)
    {
        return m2 / 255;
    }

    if ((3 * h) < (2 * 255))
    {
        return ((m1 + (m2 - m1) / 255 * ((255 * 2 / 3) - h) * 6) / 255);
    }

    return (m1 / 255);
}

QRgb QdfDrawHelpers::HLStoRGB_ONE(double h, double l, double s)
{
    double r, g, b;
    double m1, m2;

    if (s == 0)
    {
        r = g = b = l;
    }
    else
    {
        if (l <= 0.5)
        {
            m2 = l * (1.0 + s);
        }
        else
        {
            m2 = l + s - l * s;
        }
        m1 = 2.0 * l - m2;
        r = HueToRGB(m1, m2, h + 1.0 / 3.0);
        g = HueToRGB(m1, m2, h);
        b = HueToRGB(m1, m2, h - 1.0 / 3.0);
    }
    return qRgb((unsigned char) (r * 255), (unsigned char) (g * 255), (unsigned char) (b * 255));
}

double QdfDrawHelpers::MakeHue_ONE(double H)
{
    H -= (int) (H);
    if (H < 0.0)
    {
        H += 1.0;
    }
    return H;
}

double QdfDrawHelpers::MakeHue_TWO(double H)
{
    return MakeHue_ONE(H / 360.0) * 360.0;
}

QRgb QdfDrawHelpers::colorMakePale(const QRgb &clr, double lum)
{
    double h, s, l;
    QdfDrawHelpers::RGBtoHSL(clr, h, s, l);
    return QdfDrawHelpers::HLStoRGB_ONE(h, lum, s);
}

QRgb QdfDrawHelpers::colorMakeDarker(const QRgb &clr, double ratio)
{
    double h;
    double s;
    double l;

    ratio = qMax(0., 1. - ratio);
    RGBtoHSL(clr, h, s, l);

    return HLStoRGB_ONE(h, qMin(1., l * ratio), qMin(1., s * ratio));
}

inline int qtc_round(double val)
{
    return int((val - int(val - 0.5)) >= 0.5 ? val + 0.5 : val - 0.5);
}

QRgb QdfDrawHelpers::colorMakeLighter(const QRgb &clr, double ratio)
{
    double h;
    double s;
    double l;
    double min = 0.01;

    QdfDrawHelpers::RGBtoHSL(clr, h, s, l);

    if (h < min && s < min && l < min)
    {
        int val = qtc_round(ratio * 255.0);
        return qRgb(val, val, val);
    }
    else
    {
        ratio += 1.;
        return HLStoRGB_ONE(h, qMin(1., l * ratio), qMin(1., s * ratio));
    }
}

QRgb QdfDrawHelpers::pixelAlpha(const QRgb &srcPixel, int percent)
{
    QRgb clrFinal = qRgb(qMin(255, (qRed(srcPixel) * percent) / 100),
                         qMin(255, (qGreen(srcPixel) * percent) / 100),
                         qMin(255, (qBlue(srcPixel) * percent) / 100));
    return clrFinal;
}

QLinearGradient QdfDrawHelpers::generateGradient(const QRect &rect, const QColor &crFrom,
                                                 const QColor &crTo, bool bHorz)
{
    QLinearGradient gradient(rect.topLeft(), bHorz ? rect.bottomLeft() : rect.topRight());
    gradient.setColorAt(0.0, crFrom);
    gradient.setColorAt(1.0, crTo);
    return gradient;
}

QImage QdfDrawHelpers::updateImage(const QImage &imageSrc, QRgb clrBase, QRgb clrHighlight)
{
    QImage image = imageSrc;

    QColor clrTr = clrBase;

    QColor clrHL = clrHighlight;
    QColor clrShadow = pixelAlpha(clrHL.rgb(), 67);
    QRgb iClrShadow = /*6579300;*/ clrShadow.rgb();

    for (int x = 0; x < image.width(); x++)
    {
        for (int y = 0; y < image.height(); y++)
        {
            QColor clr = image.pixel(x, y);
            if (clr != clrTr)
            {
                image.setPixel(x, y, iClrShadow);
            }
        }
    }
    return image;
}

QImage QdfDrawHelpers::addaptColors(const QImage &imageSrc, QRgb clrBase, QRgb clrTone,
                                    bool clampHue /*= true*/)
{
    double dSrcH, dSrcS, dSrcL;
    QdfDrawHelpers::RGBtoHSL(clrBase, dSrcH, dSrcS, dSrcL);

    double dDestH, dDestS, dDestL;
    QdfDrawHelpers::RGBtoHSL(clrTone, dDestH, dDestS, dDestL);

    double DH = dDestH - dSrcH;
    double DL = dDestL - dSrcL;
    double DS = dDestS - dSrcS;

    clampHue = (dDestH > 0.5) && clampHue;
    int bitsPerPixel = imageSrc.depth();
    if (bitsPerPixel >= 24)
    {
        QImage image = imageSrc;
        QRgb *bits = (QRgb *) image.bits();
        for (int i = 0; i < image.width() * image.height(); i++)
        {
            QRgb *bit = (QRgb *) bits + i;

            int alpha = qAlpha(*bit);
            if (alpha == 0)
            {
                continue;
            }

            double aR = 255.0 / (double) alpha;
            QRgb clrOrig = qRgb(qRed(*bit) * aR, qGreen(*bit) * aR, qBlue(*bit) * aR);

            double H, S, L;
            QdfDrawHelpers::RGBtoHSL(clrOrig, H, S, L);

            if (qFabs(dSrcH - H) < m_colorTolerance)
            {
                double HNew = QdfDrawHelpers::MakeHue_ONE(H + DH);
                double SNew = qMax(0., qMin(1.00, S + DS));
                double LNew = qMax(0., qMin(1.00, L + DL));

                QRgb color = QdfDrawHelpers::HLStoRGB_ONE(HNew, clampHue ? L : LNew, SNew);
                *bit = qRgb(qRed(color), qGreen(color), qBlue(color));
            }
        }
        return image;
    }
    else if (bitsPerPixel < 24)
    {
        Q_ASSERT(false);
    }

    return QImage(imageSrc);
}

QImage QdfDrawHelpers::invertColors(const QImage &imageSrc)
{
    QImage image(imageSrc.size(), QImage::Format_ARGB32_Premultiplied);
    for (int y = 0; y < imageSrc.height(); ++y)
    {
        uint *dest = (uint *) image.scanLine(y);
        const uint *src = (uint *) imageSrc.scanLine(y);
        for (int x = 0; x < imageSrc.width(); ++x)
        {
            if (qRed(src[x]) == 0 && qGreen(src[x]) == 0 && qBlue(src[x]) == 0)
            {
                continue;
            }
            dest[x] = 0xffffffff - (0x00ffffff & src[x]);
        }
    }
    return image;
}

#ifdef Q_OS_MAC
double QdfDrawHelpers::logPixel = 72.0;
#else /* Q_OS_MAC */
double QdfDrawHelpers::logPixel = 96.0;
#endif

int QdfDrawHelpers::defaultDpiX()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (const QScreen *screen = QGuiApplication::primaryScreen())
    {
        return qRound(screen->logicalDotsPerInchX());
    }
#else// (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #ifdef Q_OS_WIN
    HDC displayDC = GetDC(0);
    int currentlogPixel = GetDeviceCaps(displayDC, LOGPIXELSY);
    ReleaseDC(0, displayDC);
    return currentlogPixel;
    #endif /* Q_OS_WIN*/
#endif     // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // PI has not been initialised, or it is being initialised. Give a default dpi
    return static_cast<int>(logPixel);
}

qreal QdfDrawHelpers::dpiScaled(qreal value)
{
    static const qreal scale = static_cast<qreal>(defaultDpiX()) / logPixel;
    return value * scale;
}

int QdfDrawHelpers::getDPI()
{
    return defaultDpiX();
}

int QdfDrawHelpers::getDPIToPercent()
{
    return getDPI() * 100 / logPixel;
}

QIcon QdfDrawHelpers::createIconStyleWidget(QWidget *widget)
{
    if (qobject_cast<QCheckBox *>(widget) || qobject_cast<QRadioButton *>(widget))
    {
        bool isRadio = qobject_cast<QRadioButton *>(widget);
        QStyleOptionButton opt;
        opt.initFrom(widget);
        int h = qApp->style()->proxy()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, widget);
        opt.rect.setSize(QSize(h, h));

        QPixmap px(opt.rect.width(), opt.rect.height());
        px.fill(Qt::transparent);
        QPainter p(&px);

        opt.state &= ~QStyle::State_Off;
        opt.state &= ~QStyle::State_On;
        opt.state &= ~QStyle::State_NoChange;
        opt.state |= QStyle::State_On;

        qApp->style()->drawPrimitive(
                isRadio ? QStyle::PE_IndicatorRadioButton : QStyle::PE_IndicatorCheckBox, &opt, &p, 0);

        return QIcon(px);
    }
    return QIcon();
}

QPixmap QdfDrawHelpers::createIconExStyleWidget(QWidget *widget, int width, int height, bool &iconView)
{
    if (widget == nullptr)
    {
        return QPixmap();
    }

    if (qobject_cast<QToolButton *>(widget))
    {
        iconView = true;

        class QtcHackToolButton : public QToolButton
        {
            friend class Qdf::QdfDrawHelpers;
        };

        QStyleOptionToolButton opt;
        ((QtcHackToolButton *) widget)->initStyleOption(&opt);

        if (opt.features & QStyleOptionToolButton::HasMenu)
        {
            int mbi = qApp->style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, widget);
            opt.rect.setSize(QSize(mbi, mbi));

            QPixmap px(opt.rect.width(), opt.rect.height());
            px.fill(Qt::transparent);
            QPainter p(&px);
            qApp->style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, &p, widget);
            return px;
        }
    }
    else if (qobject_cast<QLineEdit *>(widget))
    {
        iconView = false;

        class QtcHackLineEdit : public QLineEdit
        {
            friend class Qdf::QdfDrawHelpers;
        };
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        QStyleOptionFrameV2 opt;
#else
        QStyleOptionFrame opt;
#endif
        ((QtcHackLineEdit *) widget)->initStyleOption(&opt);
        if (opt.lineWidth <= 0)
        {
            opt.lineWidth = 1;
        }

        opt.rect = QRect(QPoint(0, 0), QSize(width, height));

        QPixmap px(opt.rect.width(), opt.rect.height());
        px.fill(Qt::transparent);
        QPainter p(&px);
        qApp->style()->drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &p, widget);

        QRect rectCaret = opt.rect;
        rectCaret.adjust(3, 2, -3, -2);
        rectCaret.setRight(rectCaret.left() + 4);

        QPen oldPen = p.pen();
        p.setPen(Qt::black);

        QPainterPath path;
        path.moveTo(rectCaret.left(), rectCaret.top());
        path.lineTo(rectCaret.right(), rectCaret.top());

        path.moveTo(rectCaret.center().x(), rectCaret.top());
        path.lineTo(rectCaret.center().x(), rectCaret.bottom());

        path.moveTo(rectCaret.left(), rectCaret.bottom());
        path.lineTo(rectCaret.right(), rectCaret.bottom());

        p.drawPath(path);
        p.setPen(oldPen);

        return px;
    }
    else if (QSpinBox *wd = qobject_cast<QSpinBox *>(widget))
    {
        class QtcHackSpinBox : public QSpinBox
        {
            friend class Qdf::QdfDrawHelpers;
        };
        return createIconExStyleWidget(((QtcHackSpinBox *) wd)->lineEdit(), width, height, iconView);
    }
    else if (QComboBox *cb = qobject_cast<QComboBox *>(widget))
    {
        iconView = false;

        class QtcHackComboBox : public QComboBox
        {
            friend class Qdf::QdfDrawHelpers;
        };
        QStyleOptionComboBox opt;
        ((QtcHackComboBox *) cb)->initStyleOption(&opt);

        opt.rect = QRect(QPoint(0, 0), QSize(width, height));

        QPixmap px(opt.rect.width(), opt.rect.height());
        px.fill(Qt::transparent);
        QPainter p(&px);
        qApp->style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &p, widget);

        return px;
    }

    return QPixmap();
}


QdfStyleOptionFrame::QdfStyleOptionFrame()
{
    hdc = 0;
    frameBorder = 4;
    titleBarSize = 30;
    tabBarSize = 24;
    statusHeight = 0;
    active = false;
    hasStatusBar = false;
    titleVisible = true;
    maximizeFlags = false;
    isBackstageVisible = false;
    tabBarPosition = QdfRibbonBar::TabBarTopPosition;
}

QdfStyleOptionFrame::QdfStyleOptionFrame(const QdfStyleOptionFrame &other) : QStyleOption(Version, Type)
{
    *this = other;
}

QdfStyleOptionFrame::~QdfStyleOptionFrame()
{
}


QdfStyleOptionBackstageButton::QdfStyleOptionBackstageButton()
{
    tabStyle = false;
    flatStyle = false;
}

QdfStyleOptionBackstageButton::QdfStyleOptionBackstageButton(const QdfStyleOptionBackstageButton &other)
    : QStyleOptionToolButton(other)
{
    *this = other;
}


QdfStyleOptionRibbonBackstage::QdfStyleOptionRibbonBackstage()
{
    menuWidth = 132;
}

QdfStyleOptionRibbonBackstage::QdfStyleOptionRibbonBackstage(const QdfStyleOptionRibbonBackstage &other)
    : QStyleOption(Version, Type)
{
    *this = other;
}


QdfStyleOptionRibbon::QdfStyleOptionRibbon()
{
    frameHelper = nullptr;
    titleBarHeight = 0;
    minimized = false;
    isBackstageVisible = false;
    tabBarPosition = QdfRibbonBar::TabBarTopPosition;
}

QdfStyleOptionRibbon::QdfStyleOptionRibbon(const QdfStyleOptionRibbon &other)
    : QStyleOption(Version, Type)
{
    *this = other;
}

QdfStyleOptionRibbon::~QdfStyleOptionRibbon()
{
}


QdfStyleOptionTitleBar::QdfStyleOptionTitleBar()
{
    frameHelper = nullptr;
    drawIcon = true;
    quickAccessVisibleCount = 0;
    quickAccessOnTop = true;
    quickAccessVisible = true;
    existSysButton = true;
    normalSysButton = false;
    isBackstageVisible = false;
    clrText = QColor();
}

QdfStyleOptionTitleBar::QdfStyleOptionTitleBar(const QdfStyleOptionTitleBar &other)
    : QStyleOptionTitleBar(other)
{
    *this = other;
}


QdfStyleCaptionButton::QdfStyleCaptionButton()
{
}

QdfStyleCaptionButton::QdfStyleCaptionButton(const QdfStyleCaptionButton &other)
    : QStyleOptionToolButton(other)
{
    *this = other;
}


QdfStyleSystemToolButton::QdfStyleSystemToolButton()
{
}

QdfStyleSystemToolButton::QdfStyleSystemToolButton(const QdfStyleSystemToolButton &other)
    : QStyleOptionToolButton(other)
{
    *this = other;
}


QdfStyleHintReturnThemeColor::QdfStyleHintReturnThemeColor(const QString &secName, const QString &kName,
                                                           QColor col)
    : QStyleHintReturn()
{
    sectionName = secName;
    keyName = kName;
    color = col;
    isValid = true;
}

QdfStyleHintReturnThemeColor::~QdfStyleHintReturnThemeColor()
{
}


QdfStyleRibbonQuickAccessBar::QdfStyleRibbonQuickAccessBar()
{
    quickAccessBarPosition = QdfRibbonBar::QATopPosition;
}

QdfStyleRibbonQuickAccessBar::QdfStyleRibbonQuickAccessBar(const QdfStyleRibbonQuickAccessBar &other)
    : QStyleOption(other)
{
    *this = other;
}


QdfStyleRibbonOptionHeader::QdfStyleRibbonOptionHeader()
    : QStyleOptionHeader(), contextText(QString("")), tabBarPosition(QdfRibbonBar::TabBarTopPosition)
{
}

QdfStyleRibbonOptionHeader::QdfStyleRibbonOptionHeader(const QdfStyleRibbonOptionHeader &other)
    : QStyleOptionHeader(other)
{
    *this = other;
    contextText = other.contextText;
    tabBarPosition = other.tabBarPosition;
}
