#ifndef DESKTOPFRAMEWORK_QDFPOPUPHELPERS_H
#define DESKTOPFRAMEWORK_QDFPOPUPHELPERS_H

#include <QStyleOption>
#include <qdf_global.h>
#include <ribbon/qdfofficestyle.h>

QDF_BEGIN_NAMESPACE

class QdfStyleOptionPopupTitleBar : public QStyleOptionTitleBar
{
public:
    QPixmap pixmapCloseButton;

public:
    QdfStyleOptionPopupTitleBar();
    QdfStyleOptionPopupTitleBar(const QdfStyleOptionPopupTitleBar &other) : QStyleOptionTitleBar(Version)
    {
        *this = other;
    }

protected:
    QdfStyleOptionPopupTitleBar(int version);
};

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QdfPopupDrawHelper : public QCommonStyle
#else
class QdfPopupDrawHelper : public QWindowsStyle
#endif
{
    Q_OBJECT
public:
    QdfPopupDrawHelper();
    virtual ~QdfPopupDrawHelper();

public:
    virtual void polish(QWidget *widget);

public:
    virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                 SubControl subControl, const QWidget *widget) const;
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;
    virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *opt,
                                             const QPoint &pt, const QWidget *widget = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *widget) const;
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget) const;

public:
    virtual void refreshPalette();

protected:
    QPalette m_palBackground;
    QPalette m_palFrame;
    QColor m_clrText;
    bool m_fontBold;

private:
    friend class QdfOfficePopupWindow;
    Q_DISABLE_COPY(QdfPopupDrawHelper)
};

class PopupOffice2000DrawHelper : public QdfPopupDrawHelper
{
    Q_OBJECT
public:
    PopupOffice2000DrawHelper();
    virtual ~PopupOffice2000DrawHelper();

public:
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget) const;

public:
    virtual void refreshPalette();

private:
    Q_DISABLE_COPY(PopupOffice2000DrawHelper)
};

class PopupOffice2003DrawHelper : public QdfPopupDrawHelper
{
    Q_OBJECT
public:
    enum OptionsStyle
    {
        OS_SYSTEMBLUE,
        OS_SYSTEMROYALE,
        OS_SYSTEMAERO,
        OS_SYSTEMOLIVE,
        OS_SYSTEMSILVER,
        OS_SYSTEMWHITE,
        OS_SYSTEMGRAY,
    };

public:
    PopupOffice2003DrawHelper();
    virtual ~PopupOffice2003DrawHelper();

public:
    virtual void polish(QWidget *widget);

public:
    void setDecoration(OptionsStyle style);
    OptionsStyle getDecoration() const;

public:
    virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                 SubControl subControl, const QWidget *widget) const;
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *widget) const;
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget) const;

public:
    virtual void refreshPalette();

protected:
    OptionsStyle m_styleType;
    QPalette m_palGripper;
    QColor m_clrButtonSelected;
    QColor m_clrButtonPressed;
    QColor m_clrButtonSelectedBorder;
    QColor m_clrButtonPressedBorder;

private:
    Q_DISABLE_COPY(PopupOffice2003DrawHelper)
};

class QdfStyleHelper;
class PopupOffice2007DrawHelper : public PopupOffice2003DrawHelper
{
    Q_OBJECT
public:
    PopupOffice2007DrawHelper();
    virtual ~PopupOffice2007DrawHelper();

public:
    virtual void refreshPalette();

protected:
    QdfStyleHelper *m_helper;

private:
    Q_DISABLE_COPY(PopupOffice2007DrawHelper)
};

class PopupOffice2010DrawHelper : public PopupOffice2007DrawHelper
{
    Q_OBJECT
public:
    PopupOffice2010DrawHelper();
    virtual ~PopupOffice2010DrawHelper();

private:
    Q_DISABLE_COPY(PopupOffice2010DrawHelper)
};

class PopupMSNDrawHelper : public PopupOffice2000DrawHelper
{
    Q_OBJECT
public:
    PopupMSNDrawHelper();
    virtual ~PopupMSNDrawHelper();

public:
    virtual void refreshPalette();

public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget) const;

private:
    Q_DISABLE_COPY(PopupMSNDrawHelper)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFPOPUPHELPERS_H