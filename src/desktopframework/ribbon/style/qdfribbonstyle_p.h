#ifndef DESKTOPFRAMEWORK_QDFRIBBONSTYLE_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONSTYLE_P_H

#include "qdfofficestyle_p.h"
#include <qdf_global.h>
#include <ribbon/qdfribbonstyle.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonPaintManagerInterface
{
public:
    virtual ~QdfRibbonPaintManagerInterface()
    {
    }

public:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *,
                                 bool &ret) const = 0;

    // for QdfRibbonBar
    virtual void drawRibbonBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const = 0;
    virtual void drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;
    virtual void drawRibbonGroups(const QStyleOption *option, QPainter *p,
                                  const QWidget *widget) const = 0;
    virtual void drawGroup(const QStyleOption *option, QPainter *p, const QWidget *widget) const = 0;
    virtual void drawReducedGroup(const QStyleOption *option, QPainter *p,
                                  const QWidget *widget) const = 0;
    // for TitleBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *,
                                               const QWidget *) const = 0;
    virtual bool drawPanelButtonTool(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual void drawContextHeaders(const QStyleOption *opt, QPainter *p) const = 0;

    // for ribbon tab
    virtual void drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const = 0;
    virtual void drawTabShapeLabel(const QStyleOption *opt, QPainter *p,
                                   const QWidget *widget) const = 0;

    // for TabBar
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for QForm
    virtual bool drawFrame(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for Slider
    virtual bool drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const = 0;
    virtual void drawGroupScrollButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const = 0;

    // for menuBar
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;

    // for file menu
    virtual void drawSystemButton(const QStyleOption *option, QPainter *p,
                                  const QWidget *widget) const = 0;
    virtual void drawQuickAccessButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const = 0;
    virtual void drawOptionButton(const QStyleOption *option, QPainter *p,
                                  const QWidget *widget) const = 0;
    virtual void drawPopupResizeGripper(const QStyleOption *option, QPainter *p,
                                        const QWidget *widget) const = 0;

    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const = 0;

    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement, const QStyleOption *, QPainter *,
                                    const QWidget *) const = 0;

    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled,
                               bool checked, bool popuped, Qdf::BarType barType,
                               Qdf::BarPosition barPos) const = 0;

    virtual void drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *widget) const = 0;
    virtual void drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *widget) const = 0;
    virtual void drawRibbonBackstageMenu(const QStyleOption *opt, QPainter *p,
                                         const QWidget *widget) const = 0;
    virtual void drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p,
                                                const QWidget *widget) const = 0;
    virtual bool drawRibbonBackstageSeparator(const QStyleOption *opt, QPainter *p,
                                              const QWidget *widget) const = 0;

    virtual void setupPalette(QWidget *widget) const = 0;

public:
    virtual void drawRibbonSliderButton(const QStyleOption *opt, QPainter *p,
                                        const QWidget *widget) const = 0;
    virtual bool isTopLevelMenuItemUpperCase(const QWidget *) const = 0;
    virtual bool isRibbonBackstageHideTabs() const = 0;
};


QDF_END_NAMESPACE

Q_DECLARE_INTERFACE(Qdf::QdfRibbonPaintManagerInterface, "Qdf.QdfRibbonPaintManagerInterface")

QDF_BEGIN_NAMESPACE

class QdfRibbonStylePrivate : public QdfOfficeStylePrivate
{
public:
    QDF_DECLARE_EX_PUBLIC(QdfRibbonStyle)
public:
    explicit QdfRibbonStylePrivate();

public:
    void initialization();
    void updateColors();

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    void tabLayout(const QStyleOptionTabV3 *opt, const QWidget *widget, QRect *textRect,
                   QRect *iconRect) const;
#else
    void tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect,
                   QRect *iconRect) const;
#endif
    void tabHoverUpdate(QTabBar *tabBar, QEvent *event);

    void setMacSmallSize(QWidget *widget);
    void unsetMacSmallSize(QWidget *widget);

    QdfRibbonPaintManagerInterface *ribbonPaintManager() const;
    virtual void makePaintManager();

public:
    int m_lineWidthTop;
    QColor m_clrRibbonGroupCaptionText;
    // [Ribbon]
    QColor m_clrRibbonFace;
    QColor m_clrRibbonTabBarBackground;
    QColor m_clrRibbonTabBarText;
    QColor m_clrRibbonLight;
    QColor m_clrRibbonText;
    QColor m_clrGroupClientText;
    QColor m_clrGroupClientGrayText;
    QColor m_clrRibbonInactiveFace;
    QColor m_clrRibbonSeparator;
    QColor m_clrRibbonGrayText;
    QColor m_clrFileButtonText;
    QColor m_clrMinimizedFrameEdgeHighLight;
    QColor m_clrMinimizedFrameEdgeShadow;

    int m_fileButtonImageCount;
    bool m_flatFrame;
    bool m_destroyKeyTips;
    bool m_completeKey;
    bool m_blockKeyTips;
    bool m_isActiveTabAccented;
    QRect m_oldHoverRectTab;
    QHash<const QWidget *, bool> m_macSmallSizeWidgets;

private:
    Q_DISABLE_COPY(QdfRibbonStylePrivate)
};

class QdfRibbonPaintManager : public QdfOfficePaintManager, public QdfRibbonPaintManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(Qdf::QdfRibbonPaintManagerInterface)
public:
    explicit QdfRibbonPaintManager(QdfCommonStyle *baseStyle);
    virtual ~QdfRibbonPaintManager();

public:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *,
                                 bool &ret) const;

    // for QdfRibbonBar
    virtual void drawRibbonBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawPixTitleBackground(QPainter *p, const QRect &rectDst,
                                        const QPixmap &pixTitleBackground, const QRect &rectSrc,
                                        const QWidget *widget) const;
    virtual void drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual void drawRibbonGroups(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawGroup(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawReducedGroup(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    // for TitleBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelButtonTool(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual void drawContextHeaders(const QStyleOption *opt, QPainter *p) const;
    // for ribbon tab
    virtual void drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    virtual void drawTabShapeLabel(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    // for TabBar
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    // for QForm
    virtual bool drawFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    // for Slider
    virtual bool drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    virtual void drawGroupScrollButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const;
    // for menuBar
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const;

    // for file menu
    virtual void drawSystemButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawQuickAccessButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const;
    virtual void drawOptionButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawPopupResizeGripper(const QStyleOption *option, QPainter *p,
                                        const QWidget *widget) const;

    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const;

    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement, const QStyleOption *, QPainter *,
                                    const QWidget *) const;

    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled,
                               bool checked, bool popuped, Qdf::BarType barType,
                               Qdf::BarPosition barPos) const;

    virtual void drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    virtual void drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    virtual void drawRibbonBackstageMenu(const QStyleOption *opt, QPainter *p,
                                         const QWidget *widget) const;
    virtual void drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p,
                                                const QWidget *widget) const;
    virtual bool drawRibbonBackstageSeparator(const QStyleOption *opt, QPainter *p,
                                              const QWidget *widget) const;

    virtual void setupPalette(QWidget *widget) const;

public:
    virtual void modifyColors();
    virtual QColor titleButtonHiliteColor(bool disabled, const QWidget *w) const;
    virtual QColor buttonHiliteColor(const QWidget *widget, bool highlightDn) const;
    virtual QColor ribbonBarColor(bool isBackstageVisible) const;
    virtual QColor backstageCloseButton() const;

public:
    virtual void drawRibbonSliderButton(const QStyleOption *opt, QPainter *p,
                                        const QWidget *widget) const;
    virtual bool isTopLevelMenuItemUpperCase(const QWidget *w) const;
    virtual bool isRibbonBackstageHideTabs() const
    {
        return true;
    }

    void fillRibbonButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void fillRibbonTabControl(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;

protected:
    // for BackstageButton
    virtual void drawPanelBackstageButton(const QStyleOption *, QPainter *, const QWidget *) const;

protected:
    QColor m_clrRibbonGroupsFill;

private:
    Q_DISABLE_COPY(QdfRibbonPaintManager)
};

template<class T>
const T *getParentWidget(const QWidget *widget)
{
    while (widget)
    {
        if (const T *parent = qobject_cast<const T *>(widget))
        {
            return parent;
        }
        widget = widget->parentWidget();
    }
    return nullptr;
}

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSTYLE_P_H