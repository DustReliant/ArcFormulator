#ifndef DESKTOPFRAMEWORK_QDFOFFICESTYLE_H
#define DESKTOPFRAMEWORK_QDFOFFICESTYLE_H

#include <QMainWindow>
#include <QPalette>
#include <QStringList>
#include <QStyle>
#include <QStylePlugin>
#include <qdf_global.h>
#include <ribbon/qdfcommonstyle.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonGalleryItem;

class QdfOfficeStylePrivate;
class QDF_EXPORT QdfOfficeStyle : public QdfCommonStyle
{
    Q_OBJECT
    Q_ENUMS(Theme)
    Q_PROPERTY(Theme theme READ getTheme WRITE setTheme)
    Q_PROPERTY(bool animationEnabled READ isAnimationEnabled WRITE setAnimationEnabled)
    Q_PROPERTY(bool ignoreDialogs READ isDialogsIgnored WRITE setDialogsIgnored)
    Q_PROPERTY(bool ignoreScrollBars READ isScrollBarsIgnored WRITE setScrollBarsIgnored)
    Q_PROPERTY(bool ignoreMDIWindowTitle READ isMDIWindowTitleIgnored WRITE setMDIWindowTitleIgnored)
    Q_PROPERTY(bool DPIAware READ isDPIAware WRITE setDPIAware)
public:
    enum Theme
    {
        OfficeWhite,
        OfficeGray,
        OfficeDark,
    };

    enum OfficePopupDecoration
    {
        PopupSystemDecoration,
        PopupCompatibleDecoration,
        PopupOfficeFlatDecoration,
        PopupMSNDecoration
    };

    enum AccentColor
    {
        AccentColorBlue,
        AccentColorBrown,
        AccentColorGreen,
        AccentColorLime,
        AccentColorMagenta,
        AccentColorOrange,
        AccentColorPink,
        AccentColorPurple,
        AccentColorRed,
        AccentColorTeal,
    };

public:
    QdfOfficeStyle();
    QdfOfficeStyle(QMainWindow *mainWindow);
    virtual ~QdfOfficeStyle();

protected:
    QdfOfficeStyle(QdfOfficeStylePrivate *d);
    QdfOfficeStyle(QMainWindow *mainWindow, QdfOfficeStylePrivate *d);

public:
    QColor accentColor() const;
    void setAccentColor(const QColor &accentcolor);

    QdfOfficeStyle::AccentColor accentIndexColor() const;
    void setAccentColor(AccentColor indexcolor);

    void setTheme(Theme theme);
    Theme getTheme() const;

    void setAnimationEnabled(bool enable = true);
    bool isAnimationEnabled() const;

    void setDialogsIgnored(bool ignore = false);
    bool isDialogsIgnored() const;

    void setScrollBarsIgnored(bool ignore = false);
    bool isScrollBarsIgnored() const;

    void setMDIWindowTitleIgnored(bool ignore = false);
    bool isMDIWindowTitleIgnored() const;

    void setDPIAware(bool aware);
    bool isDPIAware() const;

    int getDPI() const;
    int getDPIToPercent() const;

    QdfOfficeStyle::OfficePopupDecoration popupDecoration() const;
    void setPopupDecoration(QdfOfficeStyle::OfficePopupDecoration typeStyle);

public:
    virtual void polish(QApplication *);
    virtual void polish(QWidget *widget);
    virtual void polish(QPalette &palette);
    virtual void unpolish(QApplication *);
    virtual void unpolish(QWidget *widget);

public:
    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize,
                                   const QWidget *widget = nullptr) const;
    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                 const QWidget *widget) const;
    virtual int pixelMetric(PixelMetric pm, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const;
    virtual int styleHint(StyleHint hint, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr,
                          QStyleHintReturn *returnData = nullptr) const;
    virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *opt, const QPoint &pt,
                                             const QWidget *widget = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *widget) const;
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget) const;

protected:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *, bool &ret) const;

    virtual bool drawWorkspace(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawWidget(const QStyleOption *, QPainter *, const QWidget *) const;

    // for QForm
    virtual bool drawFrameTabWidget(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorTabClose(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawFrameWindow(const QStyleOption *, QPainter *, const QWidget *) const;

    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const;

    // for menuBar
    virtual bool drawMenuBarEmptyArea(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelMenu(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawMenuBarItem(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawMenuItem(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const;

    // for toolBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTitleBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawToolButtonLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelButtonTool(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawToolButton(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawGroupControlEntry(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorToolBarHandle(const QStyleOption *, QPainter *, const QWidget *) const;
    // for Buttons
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelButtonCommand(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorCheckRadioButton(PrimitiveElement element, const QStyleOption *, QPainter *,
                                               const QWidget *) const;
    virtual bool drawIndicatorViewItemCheck(PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawFrameDefaultButton(const QStyleOption *, QPainter *, const QWidget *) const;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const;
    // for Slider
    virtual bool drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;

    // for ScrollBar
    virtual bool drawScrollBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawScrollBarLine(ControlElement element, const QStyleOption *opt, QPainter *p,
                                   const QWidget *w) const;
    virtual bool drawScrollBarSlider(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawScrollBarPage(ControlElement element, const QStyleOption *, QPainter *, const QWidget *) const;
    // for editors
    virtual bool drawControlEdit(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawFrameLineEdit(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawComboBox(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    // for DockWidget
    virtual bool drawDockWidgetTitle(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorDockWidgetResizeHandle(const QStyleOption *, QPainter *, const QWidget *) const;
    // MDI
    virtual bool drawMdiControls(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    // for TabBar
    virtual bool drawTabBarTab(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTabBarTabLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    // for SpinBox
    virtual bool drawSpinBox(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    // for ComboBox
    virtual bool drawComboBoxLabel(ControlElement element, const QStyleOption *opt, QPainter *p,
                                   const QWidget *widget) const;
    // for ProgressBar
    virtual bool drawProgressBarGroove(const QStyleOption *, QPainter *, const QWidget *) const;
    // for ToolBox
    virtual bool drawToolBoxTab(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawToolBoxTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawToolBoxTabLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    // for ViewItem
    virtual bool drawItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const;
    //
    virtual bool drawHeader(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawHeaderSection(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawHeaderEmptyArea(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorHeaderArrow(const QStyleOption *, QPainter *, const QWidget *) const;

    // shared
    virtual bool drawIndicatorArrow(PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelTipLabel(const QStyleOption *, QPainter *, const QWidget *) const;

    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled, bool checked,
                               bool popuped, BarType barType, BarPosition barPos) const;
    virtual void drawSplitButtonPopup(QPainter *p, const QRect &rect, bool selected, bool enabled, bool popuped) const;

    virtual void drawLabelGallery(QPainter *p, QdfRibbonGalleryItem *item, const QRect &rect);
    virtual QColor getTextColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped, BarType barType,
                                BarPosition barPosition) const;

protected:
    bool isStyle2010() const;

    void createPopupProxy();
    bool isExistPopupProxy() const;
    void unsetPopupProxy();

    virtual bool transitionsEnabled() const;

protected:
    virtual QPixmap cached(const QString &img) const;
    virtual QWidgetList allWidgets() const;
    virtual bool isNativeDialog(const QWidget *wid) const;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    friend class QdfRibbonGalleryItem;
    friend class QdfOfficePopupWindow;
    friend class QdfOfficePopupWindowPrivate;
    friend class QdfManagerPopup;
    friend class QdfOfficePaintManager;
    Q_DISABLE_COPY(QdfOfficeStyle)
    QDF_DECLARE_EX_PRIVATE(QdfOfficeStyle)
};

class QDF_EXPORT QdfOfficeStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
//      Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "officestyle.json")
#endif
public:
    QStringList keys() const;
    QStyle *create(const QString &key);
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFOFFICESTYLE_H
