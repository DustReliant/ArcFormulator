#include "../qdfribbon_def.h"
#include "qdfcommonstyle_p.h"
#include <QApplication>
#include <QBitmap>
#include <QBrush>
#include <QDialog>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QPushButton>
#include <QSettings>
#include <QStyleFactory>
#include <QStyleOption>
#include <QVariant>
#include <qevent.h>
#include <ribbon/qdfcommonstyle.h>
#include <ribbon/qdfstylehelpers.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wswitch"
#endif

QDF_USE_NAMESPACE

static QPixmap cachedPixmap(const QString &img)
{
    QPixmap pm;
    if (QPixmapCache::find(img, &pm))
    {
        return pm;
    }

    pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);

    QPixmapCache::insert(img, pm);
    return pm;
}

QdfStyleHelper::QdfStyleHelper(QObject *parent) : QObject(parent), m_pSettings(nullptr)
{
}

QdfStyleHelper::~QdfStyleHelper()
{
    delete m_pSettings;
}

void QdfStyleHelper::openIniFile(const QString &fileName)
{
    Q_ASSERT(!fileName.isEmpty());

    if (fileName.isEmpty())
    {
        return;
    }

    delete m_pSettings;
    m_pSettings = new QSettings(fileName, QSettings::IniFormat);

    Q_ASSERT(m_pSettings->status() == QSettings::NoError);

    m_pSettings->sync();
    m_pSettings->setFallbacksEnabled(true);
}

QString QdfStyleHelper::getValue(const QString &prefix, const QString &key) const
{
    Q_ASSERT(m_pSettings != nullptr);

    QString strValue;
    m_pSettings->beginGroup(prefix);
    QVariant value = m_pSettings->value(key);

    if (value.isValid() && value.type() == QVariant::String)
    {
        strValue = value.toString();
    }

    m_pSettings->endGroup();

    return strValue;
}

QColor QdfStyleHelper::getColor(const QString &prefix, const QString &key, QColor color) const
{
    Q_ASSERT(m_pSettings != nullptr);

    int r = 0, g = 0, b = 0;
    QString strValue = getValue(prefix, key);

    if (!strValue.isEmpty())
    {
        QStringList strList = strValue.split(' ');
        if (strList.size() == 3)
        {
            r = strList[0].toUInt();
            g = strList[1].toUInt();
            b = strList[2].toUInt();
        }
        return QColor(r, g, b);
    }
    return color;
}

int QdfStyleHelper::getInteger(const QString &prefix, const QString &key, int val) const
{
    Q_ASSERT(m_pSettings != nullptr);

    QString strValue = getValue(prefix, key);
    if (!strValue.isEmpty())
    {
        val = strValue.toInt();
    }
    return val;
}

QdfCommonAnimation::~QdfCommonAnimation()
{
    setWidget(nullptr);
}

void QdfCommonAnimation::setWidget(QWidget *widget)
{
    if (widget)
    {
        widget->installEventFilter(this);
    }
    else if (m_widget)
    {
        m_widget->removeEventFilter(this);
    }

    m_widget = widget;
}

void QdfCommonAnimation::paint(QPainter *painter, const QStyleOption *option)
{
    Q_UNUSED(option);
    Q_UNUSED(painter);
}

void QdfCommonAnimation::drawBlendedImage(QPainter *painter, QRect rect, float alpha)
{
    if (m_secondaryImage.isNull() || m_primaryImage.isNull())
    {
        return;
    }

    if (m_tempImage.isNull())
    {
        m_tempImage = m_secondaryImage;
    }

    const int a = qRound(alpha * 256);
    const int ia = 256 - a;
    const int sw = m_primaryImage.width();
    const int sh = m_primaryImage.height();
    const int bpl = m_primaryImage.bytesPerLine();
    switch (m_primaryImage.depth())
    {
        case 32:
            {
                uchar *mixed_data = m_tempImage.bits();
                const uchar *back_data = m_primaryImage.bits();
                const uchar *front_data = m_secondaryImage.bits();
                for (int sy = 0; sy < sh; sy++)
                {
                    quint32 *mixed = (quint32 *) mixed_data;
                    const quint32 *back = (const quint32 *) back_data;
                    const quint32 *front = (const quint32 *) front_data;
                    for (int sx = 0; sx < sw; sx++)
                    {
                        quint32 bp = back[sx];
                        quint32 fp = front[sx];
                        mixed[sx] = qRgba((qRed(bp) * ia + qRed(fp) * a) >> 8,
                                          (qGreen(bp) * ia + qGreen(fp) * a) >> 8,
                                          (qBlue(bp) * ia + qBlue(fp) * a) >> 8,
                                          (qAlpha(bp) * ia + qAlpha(fp) * a) >> 8);
                    }
                    mixed_data += bpl;
                    back_data += bpl;
                    front_data += bpl;
                }
            }
        default:
            break;
    }
    painter->drawPixmap(rect, QPixmap::fromImage(m_tempImage, Qt::AutoColor));
}

bool QdfCommonAnimation::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_widget && event->type() == QEvent::Hide)
    {
        setWidget(nullptr);
    }

    return QObject::eventFilter(obj, event);
}

QdfCommonTransition::QdfCommonTransition() : QdfCommonAnimation()
{
}

QdfCommonTransition::~QdfCommonTransition()
{
}

void QdfCommonTransition::paint(QPainter *painter, const QStyleOption *option)
{
    float alpha = 1.0;
    if (m_duration > 0)
    {
        QTime current = QTime::currentTime();

        if (m_startTime > current)
        {
            m_startTime = current;
        }

        int timeDiff = m_startTime.msecsTo(current);
        alpha = timeDiff / (float) m_duration;
        if (timeDiff > m_duration)
        {
            m_running = false;
            alpha = 1.0;
        }
    }
    else
    {
        m_running = false;
    }
    drawBlendedImage(painter, option->rect, alpha);
}

QdfCommonPaintManager::QdfCommonPaintManager(QdfCommonStyle *baseStyle) : m_base(baseStyle)
{
}

QdfCommonPaintManager::~QdfCommonPaintManager()
{
}

QdfCommonStyle *QdfCommonPaintManager::baseStyle() const
{
    return m_base;
}

QdfStyleHelper &QdfCommonPaintManager::helper() const
{
    return baseStyle()->helper();
}

QPixmap QdfCommonPaintManager::cached(const QString &img) const
{
    return baseStyle()->cached(img);
}

QPixmap QdfCommonPaintManager::cachedPath(const QString &img) const
{
    return baseStyle()->QdfCommonStyle::cached(img);
}

QRect QdfCommonPaintManager::sourceRectImage(QRect rcSrc, int state, int count) const
{
    return baseStyle()->sourceRectImage(rcSrc, state, count);
}

void QdfCommonPaintManager::drawImage(const QPixmap &soSrc, QPainter &p, const QRect &rcDest,
                                      const QRect &rcSrc, QRect rcSizingMargins, QColor clrTransparent,
                                      bool alphaBlend) const
{
    baseStyle()->drawImage(soSrc, p, rcDest, rcSrc, rcSizingMargins, clrTransparent, alphaBlend);
}

void QdfCommonPaintManager::drawImage(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest,
                                      const QRect &rcSrc, QRect rcSizingMargins, bool alphaBlend) const
{
    baseStyle()->drawImage(dcSrc, p, rcDest, rcSrc, rcSizingMargins, alphaBlend);
}

void QdfCommonPaintManager::drawPixmap(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest,
                                       const QRect &rcSrc, bool alpha, QRect rcSizingMargins,
                                       bool alphaBlend) const
{
    baseStyle()->drawPixmap(dcSrc, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

QdfCommonStylePrivate::QdfCommonStylePrivate()
{
    if (qApp->style() != nullptr)
    {
        QString styleName = qApp->property(_qdf_StyleName).toString();
        if (styleName.isEmpty())
        {
            styleName = qApp->style()->objectName();
            qApp->setProperty(_qdf_StyleName, styleName);
        }
    }

    m_paintManager = nullptr;
    m_defaultStyle = nullptr;
    m_helper = nullptr;
}

QdfCommonStylePrivate::~QdfCommonStylePrivate()
{
    for (int i = m_animations.size() - 1; i >= 0; --i)
    {
        delete m_animations.takeAt(i);
    }

    delete m_paintManager;
    m_paintManager = nullptr;

    delete m_defaultStyle;
    m_defaultStyle = nullptr;
}

void QdfCommonStylePrivate::initialization()
{
    QDF_Q(QdfCommonStyle);
    m_helper = new QdfStyleHelper(q);

    QString styleName = qApp->property(_qdf_StyleName).toString();
    if (!styleName.isEmpty())
    {
        m_defaultStyle = QStyleFactory::create(styleName);
    }
}

void QdfCommonStylePrivate::doTimer()
{
    for (int i = m_animations.size() - 1; i >= 0; --i)
    {
        QdfCommonAnimation *pItem = m_animations[i];
        QWidget *pWidget = pItem->widget();
        if (pWidget)
        {
            pWidget->update();
        }

        if (!pWidget || !pWidget->isEnabled() || !pWidget->isVisible() ||
            pWidget->window()->isMinimized() || !pItem->running())
        {
            QdfCommonAnimation *a = m_animations.takeAt(i);
            delete a;
        }
    }

    if (m_animations.size() == 0 && m_animationTimer.isActive())
    {
        m_animationTimer.stop();
    }
}

void QdfCommonStylePrivate::stopAnimation(const QWidget *w)
{
    for (int i = m_animations.size() - 1; i >= 0; --i)
    {
        if (m_animations[i]->widget() == w)
        {
            QdfCommonAnimation *a = m_animations.takeAt(i);
            delete a;
            break;
        }
    }
}

void QdfCommonStylePrivate::startAnimation(QdfCommonAnimation *t)
{
    QDF_Q(QdfCommonStyle);
    stopAnimation(t->widget());
    m_animations.append(t);
    if (m_animations.size() > 0 && !m_animationTimer.isActive())
    {
        m_animationTimer.start(45, q);
    }
}

QdfCommonAnimation *QdfCommonStylePrivate::widgetAnimation(const QWidget *widget) const
{
    if (!widget)
    {
        return 0;
    }
    foreach (QdfCommonAnimation *a, m_animations)
    {
        if (a->widget() == widget)
        {
            return a;
        }
    }
    return 0;
}

void QdfCommonStylePrivate::setPalette(QWidget *widget)
{
    m_customPaletteWidgets.insert(widget, widget->palette());
}

void QdfCommonStylePrivate::unsetPalette(QWidget *widget)
{
    if (m_customPaletteWidgets.contains(widget))
    {
        QPalette p = m_customPaletteWidgets.value(widget);
        widget->setPalette(p);
        m_customPaletteWidgets.remove(widget);
    }
}

void QdfCommonStylePrivate::setAutoFillBackground(QWidget *widget)
{
    m_customAutoFillBackground.insert(widget, widget->autoFillBackground());
}

void QdfCommonStylePrivate::unsetAutoFillBackground(QWidget *widget)
{
    if (m_customAutoFillBackground.contains(widget))
    {
        bool flag = m_customAutoFillBackground.value(widget);
        widget->setAutoFillBackground(flag);
        m_customAutoFillBackground.remove(widget);
    }
}

void QdfCommonStylePrivate::setPaintManager(QdfCommonPaintManager &paintManager)
{
    if (m_paintManager)
    {
        delete m_paintManager;
        m_paintManager = nullptr;
    }
    m_paintManager = &paintManager;
}

QdfCommonStyle::QdfCommonStyle(QdfCommonStylePrivate *d)
{
    QDF_INIT_EX_PRIVATE(QdfCommonStyle, d);
    d->initialization();
    d->makePaintManager();
}

QdfCommonStyle::~QdfCommonStyle()
{
    QDF_FINI_PRIVATE();
}

QColor QdfCommonStyle::getThemeColor(const QString &sectionName, const QString &keyName,
                                     QColor color) const
{
    return helper().getColor(sectionName, keyName, color);
}

void QdfCommonStyle::polish(QApplication *pApp)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(pApp);
#else
    QWindowsStyle::polish(pApp);
#endif
}

void QdfCommonStyle::polish(QWidget *widget)
{
    QDF_D(QdfCommonStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(widget);
#else
    QWindowsStyle::polish(widget);
#endif
    d->unsetPalette(widget);
    d->setPalette(widget);
}


void QdfCommonStyle::unpolish(QApplication *app)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::unpolish(app);
#else
    QWindowsStyle::unpolish(app);
#endif
}


void QdfCommonStyle::unpolish(QWidget *widget)
{
    QDF_D(QdfCommonStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::unpolish(widget);
#else
    QWindowsStyle::unpolish(widget);
#endif

    d->unsetPalette(widget);
    d->stopAnimation(widget);
}


void QdfCommonStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *option, QPainter *p,
                                   const QWidget *widget) const
{
    bool ret = false;
    bool qtcStyle = false;
    if (!isNativeDialog(widget))
    {
        switch (pe)
        {
            case PE_Workspace:
                ret = drawWorkspace(option, p, widget);
                break;
            case PE_Frame:
                ret = drawFrame(option, p, widget);
                break;
            case PE_FrameGroupBox:
                ret = false;
                qtcStyle = true;
                break;
            case PE_FrameDefaultButton:
                ret = drawFrameDefaultButton(option, p, widget);
                break;
            case PE_IndicatorToolBarSeparator:
                ret = drawIndicatorToolBarSeparator(option, p, widget);
                break;
            case PE_IndicatorRadioButton:
            case PE_IndicatorCheckBox:
                ret = drawIndicatorCheckRadioButton(pe, option, p, widget);
                break;
            case PE_IndicatorViewItemCheck:
                ret = drawIndicatorViewItemCheck(pe, option, p, widget);
                break;
            case PE_PanelLineEdit:
                ret = drawControlEdit(option, p, widget);
                break;
            case PE_FrameLineEdit:
                ret = drawFrameLineEdit(option, p, widget);
                break;
            case PE_PanelStatusBar:
                ret = drawPanelStatusBar(option, p, widget);
                break;
            case PE_PanelMenu:
                ret = drawPanelMenu(option, p, widget);
                break;
            case PE_FrameMenu:
                ret = drawFrameMenu(option, p, widget);
                break;
            case PE_IndicatorMenuCheckMark:
                ret = drawIndicatorMenuCheckMark(option, p, widget);
                break;
            case PE_IndicatorToolBarHandle:
                ret = drawIndicatorToolBarHandle(option, p, widget);
                break;
            case PE_PanelButtonCommand:
                ret = drawPanelButtonCommand(option, p, widget);
                break;
            case PE_PanelButtonTool:
                {
                    if (paintAnimation(tp_PrimitiveElement, (int) pe, option, p, widget))
                    {
                        return;
                    }
                    ret = drawGroupControlEntry(option, p, widget);
                    if (!ret)
                    {
                        ret = drawPanelButtonTool(option, p, widget);
                    }
                    break;
                }
            case PE_FrameStatusBarItem:
                ret = true;
                break;
            case PE_IndicatorArrowLeft:
            case PE_IndicatorArrowRight:
            case PE_IndicatorArrowDown:
            case PE_IndicatorArrowUp:
                ret = drawIndicatorArrow(pe, option, p, widget);
                break;
            case PE_PanelTipLabel:
                ret = drawPanelTipLabel(option, p, widget);
                break;
            case PE_IndicatorDockWidgetResizeHandle:
                ret = drawIndicatorDockWidgetResizeHandle(option, p, widget);
                break;
            case PE_PanelItemViewItem:
                ret = drawPanelItemViewItem(option, p, widget);
                break;
            case PE_IndicatorHeaderArrow:
                ret = drawIndicatorHeaderArrow(option, p, widget);
                break;
            case PE_FrameTabWidget:
                ret = drawFrameTabWidget(option, p, widget);
                break;
            case PE_FrameWindow:
                ret = drawFrameWindow(option, p, widget);
                break;
            case PE_FrameTabBarBase:// draw a line along the tabs
                ret = true;
                break;
            case PE_IndicatorTabClose:
                ret = drawIndicatorTabClose(option, p, widget);
                break;
            default:
                break;
        }
    }

    if (!ret)
    {
        QDF_D(const QdfCommonStyle);
        if (d->m_defaultStyle && !qtcStyle)
        {
            d->m_defaultStyle->drawPrimitive(pe, option, p, widget);
        }
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawPrimitive(pe, option, p, widget);
#else
            QWindowsStyle::drawPrimitive(pe, option, p, widget);
#endif
    }
}


void QdfCommonStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                                 const QWidget *widget) const
{
    bool ret = false;
    bool qtcStyle = false;
    switch (element)
    {
        case CE_ShapedFrame:
            ret = !isNativeDialog(widget) && drawShapedFrame(opt, p, widget);
            break;
        case CE_PushButton:
        case CE_PushButtonBevel:
            ret = false;
            qtcStyle = !isNativeDialog(widget) && true;
            break;
        case CE_RadioButton:
        case CE_CheckBox:
            ret = false;
            qtcStyle = !isNativeDialog(widget) && true;
            break;
        case CE_MenuBarEmptyArea:
            ret = !isNativeDialog(widget) && drawMenuBarEmptyArea(opt, p, widget);
            break;
        case CE_MenuItem:
            ret = !isNativeDialog(widget) && drawMenuItem(opt, p, widget);
            break;
        case CE_MenuBarItem:
            ret = !isNativeDialog(widget) && drawMenuBarItem(opt, p, widget);
            break;
        case CE_SizeGrip:
            ret = !isNativeDialog(widget) && drawSizeGrip(opt, p, widget);
            break;
        case CE_ScrollBarSubLine:
        case CE_ScrollBarAddLine:
            ret = !isNativeDialog(widget) && drawScrollBarLine(element, opt, p, widget);
            break;
        case CE_ScrollBarAddPage:
        case CE_ScrollBarSubPage:
            ret = !isNativeDialog(widget) && drawScrollBarPage(element, opt, p, widget);
            break;
        case CE_ScrollBarSlider:
            ret = !isNativeDialog(widget) && drawScrollBarSlider(opt, p, widget);
            break;
        case CE_ToolBar:
            ret = !isNativeDialog(widget) && drawToolBar(opt, p, widget);
            break;
        case CE_ToolButtonLabel:
            ret = !isNativeDialog(widget) && drawToolButtonLabel(opt, p, widget);
            break;
        case CE_DockWidgetTitle:
            ret = !isNativeDialog(widget) && drawDockWidgetTitle(opt, p, widget);
            break;
        case CE_TabBarTab:
            ret = !isNativeDialog(widget) && drawTabBarTab(opt, p, widget);
            break;
        case CE_TabBarTabShape:
            ret = !isNativeDialog(widget) && drawTabBarTabShape(opt, p, widget);
            break;
        case CE_ProgressBarGroove:
            ret = !isNativeDialog(widget) && drawProgressBarGroove(opt, p, widget);
            break;
        case CE_ToolBoxTab:
            ret = !isNativeDialog(widget) && drawToolBoxTab(opt, p, widget);
            break;
        case CE_ToolBoxTabShape:
            ret = !isNativeDialog(widget) && drawToolBoxTabShape(opt, p, widget);
            ;
            break;
        case CE_ToolBoxTabLabel:
            ret = !isNativeDialog(widget) && drawToolBoxTabLabel(opt, p, widget);
            break;
        case CE_ItemViewItem:
            ret = !isNativeDialog(widget) && drawItemViewItem(opt, p, widget);
            break;
        case CE_HeaderEmptyArea:
            ret = !isNativeDialog(widget) && drawHeaderEmptyArea(opt, p, widget);
            break;
        case CE_Header:
            ret = !isNativeDialog(widget) && drawHeader(opt, p, widget);
            break;
        case CE_HeaderSection:
            ret = !isNativeDialog(widget) && drawHeaderSection(opt, p, widget);
            break;
        case CE_TabBarTabLabel:
            ret = !isNativeDialog(widget) && drawTabBarTabLabel(opt, p, widget);
            break;
        case CE_ComboBoxLabel:
            ret = !isNativeDialog(widget) && drawComboBoxLabel(element, opt, p, widget);
            break;
        default:
            break;
    }

    if (!ret)
    {
        QDF_D(const QdfCommonStyle);
        if (d->m_defaultStyle && !qtcStyle)
        {
            d->m_defaultStyle->drawControl(element, opt, p, widget);
        }
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawControl(element, opt, p, widget);
#else
            QWindowsStyle::drawControl(element, opt, p, widget);
#endif
    }
}


void QdfCommonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                        const QWidget *widget) const
{
    bool ret = false;
    bool qtcStyle = false;
    if (!isNativeDialog(widget))
    {
        switch (cc)
        {
            case CC_GroupBox:
                ret = false;
                qtcStyle = true;
                break;
            case CC_Slider:
                ret = drawSlider(opt, p, widget);
                break;
            case CC_SpinBox:
                ret = drawSpinBox(opt, p, widget);
                break;
            case CC_ScrollBar:
                ret = drawScrollBar(opt, p, widget);
                break;
            case CC_ToolButton:
                ret = drawToolButton(opt, p, widget);
                break;
            case CC_ComboBox:
                {
                    if (paintAnimation(tp_ComplexControl, (int) cc, opt, p, widget))
                    {
                        return;
                    }
                    ret = drawComboBox(opt, p, widget);
                }
                break;
            case CC_MdiControls:
                ret = drawMdiControls(opt, p, widget);
                break;
            case CC_TitleBar:
                ret = drawTitleBar(opt, p, widget);
                break;

            default:
                break;
        }
    }

    if (!ret)
    {
        QDF_D(const QdfCommonStyle);
        if (d->m_defaultStyle && !qtcStyle)
        {
            d->m_defaultStyle->drawComplexControl(cc, opt, p, widget);
        }
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawComplexControl(cc, opt, p, widget);
#else
            QWindowsStyle::drawComplexControl(cc, opt, p, widget);
#endif
    }
}


QSize QdfCommonStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *w) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDF_D(const QdfCommonStyle);
    if (d->m_defaultStyle)
    {
        return d->m_defaultStyle->sizeFromContents(ct, opt, contentsSize, w);
    }

    return QCommonStyle::sizeFromContents(ct, opt, contentsSize, w);
#else
    return QWindowsStyle::sizeFromContents(ct, opt, contentsSize, w);
#endif
}


QRect QdfCommonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                     const QWidget *widget) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #ifdef Q_OS_WIN
    // Get sub control settings from Windows style
    QDF_D(const QdfCommonStyle);
    if (d->m_defaultStyle)
    {
        return d->m_defaultStyle->subControlRect(cc, opt, sc, widget);
    }
    #endif

    return QCommonStyle::subControlRect(cc, opt, sc, widget);
#else
    return QWindowsStyle::subControlRect(cc, opt, sc, widget);
#endif
}


int QdfCommonStyle::pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const
{
    QDF_D(const QdfCommonStyle);
    if (d->m_defaultStyle && (pm == PM_DockWidgetFrameWidth || pm == PM_TitleBarHeight))
    {
        return d->m_defaultStyle->pixelMetric(pm, option, widget);
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d->m_defaultStyle)
    {
        return d->m_defaultStyle->pixelMetric(pm, option, widget);
    }
    else
    {
        return QCommonStyle::pixelMetric(pm, option, widget);
    }
#else
    return QWindowsStyle::pixelMetric(pm, option, widget);
#endif
}


int QdfCommonStyle::styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                              QStyleHintReturn *returnData) const
{
    if (hint == QStyle::SH_CustomBase)
    {
        if (QdfStyleHintReturnThemeColor *vret =
                    qstyleoption_cast<QdfStyleHintReturnThemeColor *>(returnData))
        {
            if (!vret->sectionName.isEmpty() && !vret->keyName.isEmpty())
            {
                QColor returnColor(getThemeColor(vret->sectionName, vret->keyName, vret->color));
                vret->isValid = returnColor.isValid();
                return returnColor.rgb();
            }
        }
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDF_D(const QdfCommonStyle);
    if (d->m_defaultStyle)
    {
        return d->m_defaultStyle->styleHint(hint, opt, widget, returnData);
    }
    else
    {
        return QCommonStyle::styleHint(hint, opt, widget, returnData);
    }
#else
    return QWindowsStyle::styleHint(hint, opt, widget, returnData);
#endif
}

QPixmap QdfCommonStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                       const QWidget *widget) const
{
    QDF_D(const QdfCommonStyle);
    if (d->m_defaultStyle)
    {
        return d->m_defaultStyle->standardPixmap(standardPixmap, opt, widget);
    }
    else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        return QCommonStyle::standardPixmap(standardPixmap, opt, widget);
#else
        return QWindowsStyle::standardPixmap(standardPixmap, opt, widget);
#endif
}

QIcon QdfCommonStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *opt,
                                   const QWidget *widget) const
{
    QDF_D(const QdfCommonStyle);
    bool ret = false;
    if (!isNativeDialog(widget))
    {
        QIcon icon = standardIconEx(standardIcon, opt, widget, ret);
        if (ret)
        {
            return icon;
        }
    }

    if (!ret)
    {
        if (d->m_defaultStyle)
        {
            return d->m_defaultStyle->standardIcon(standardIcon, opt, widget);
        }
        else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            return QCommonStyle::standardIcon(standardIcon, opt, widget);
#else
            return QWindowsStyle::standardIcon(standardIcon, opt, widget);
#endif
    }
    return QIcon();
}

void QdfCommonStyle::splitString(const QString &str, QString &strFirstRow, QString &strSecondRow)
{
    strFirstRow = str;
    strSecondRow.clear();

    int nIndex = str.indexOf(QLatin1Char(' '));
    if (nIndex < 1)
    {
        return;
    }

    int nCenter = str.length() / 2;
    for (int i = 0; i < nCenter - 1; i++)
    {
        Q_ASSERT(nCenter - i >= 0 && nCenter + i + 1 < str.length());

        if (str[nCenter - i] == QLatin1Char(' '))
        {
            strFirstRow = str.left(nCenter - i);
            strSecondRow = str.mid(nCenter - i + 1);
            return;
        }

        if (str[nCenter + i + 1] == QLatin1Char(' '))
        {
            strFirstRow = str.left(nCenter + i + 1);
            strSecondRow = str.mid(nCenter + i + 2);
            return;
        }
    }
}

QdfCommonPaintManager *QdfCommonStyle::paintManager() const
{
    QDF_D(const QdfCommonStyle)
    return d->m_paintManager;
}

QRect QdfCommonStyle::sourceRectImage(QRect rcSrc, int state, int count) const
{
    QRect rcImage(0, 0, rcSrc.width(), rcSrc.height() / count);
    rcImage.translate(0, state * rcImage.height());
    return rcImage;
}

QPixmap QdfCommonStyle::cached(const QString &img) const
{
    return ::cachedPixmap(img);
}

void QdfCommonStyle::clearCache()
{
    QPixmapCache::clear();
}

void QdfCommonStyle::drawImagePart(const QPixmap &soSrc, QPainter &p, const QRect &rcDest,
                                   const QRect &rcSrc, bool alpha) const
{
    Q_UNUSED(alpha);
    if (rcDest.width() <= 0 || rcDest.height() <= 0 || rcSrc.width() <= 0 || rcSrc.height() <= 0)
    {
        return;
    }
    p.drawPixmap(rcDest, soSrc, rcSrc);
}

void QdfCommonStyle::drawImage(const QPixmap &soSrc, QPainter &p, const QRect &rcDest,
                               const QRect &rcSrc, QRect rcSizingMargins, QColor clrTransparent,
                               bool alphaBlend) const
{
    // if ((rcSizingMargins.top() + rcSizingMargins.bottom()) > /*rcSrc.height()*/(rcSrc.bottom() - rcSrc.top()))
    //     return;

    if ((rcSizingMargins.top() + rcSizingMargins.bottom()) >
        rcSrc.height() /*(rcSrc.bottom() - rcSrc.top())*/)
    {
        return;
    }

    bool alpha = soSrc.hasAlphaChannel();

    QPixmap copyPix = soSrc.copy();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = copyPix.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    copyPix = QPixmap::fromImage(img);
#else
    copyPix.setAlphaChannel(copyPix.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif

    drawPixmap(copyPix, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

void QdfCommonStyle::drawImage(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest,
                               const QRect &rcSrc, QRect rcSizingMargins, bool alphaBlend) const
{
    bool alpha = dcSrc.hasAlphaChannel();
    QPixmap copyPix = dcSrc.copy();
    drawPixmap(copyPix, p, rcDest, rcSrc, alpha, rcSizingMargins, alphaBlend);
}

static bool qdf_isRectNull(const QRect &rect)
{
    return (rect.left() == 0 && rect.right() == 0 && rect.top() == 0 && rect.bottom() == 0);
}

void QdfCommonStyle::drawPixmap(const QPixmap &soSrc, QPainter &p, const QRect &rcDest,
                                const QRect &rcSrc, bool alpha, QRect rcSizingMargins,
                                bool alphaBlend) const
{
    Q_UNUSED(alphaBlend)
    QPixmap dcSrc = soSrc;
    /*
    // Draw the pixmap with alpha
    if (alpha) {
        dcSrc = helper().alphaBlend(dcSrc);
        if (alphaBlend) {
            if (dcSrc.hasAlphaChannel())
                dcSrc.setAlphaChannel(dcSrc.alphaChannel());
        }
    }
    */
    if (rcSizingMargins.top() + rcSizingMargins.bottom() > rcSrc.height())
    {
        return;
    }

    if (dcSrc.isNull())
    {
        return;
    }

    if (rcDest.width() == rcSrc.width() - 1)
    {
        rcSizingMargins.setLeft(0);
        rcSizingMargins.setRight(0);
    }

    if (rcDest.height() == rcSrc.height() - 1)
    {
        rcSizingMargins.setTop(0);
        rcSizingMargins.setBottom(0);
    }

    QRect rcDestSizingMargins = rcSizingMargins;

    if (rcDest.left() >= rcDest.right() || rcDest.top() >= rcDest.bottom())
    {
        return;
    }

    if (::qdf_isRectNull(rcSizingMargins))
    {
        p.drawPixmap(rcDest, dcSrc, rcSrc);
    }
    else
    {
        class qdf_Rect : public QRect
        {
        public:
            qdf_Rect(int l, int t, int r, int b)
            {
                setLeft(l);
                setRight(r - 1);
                setTop(t);
                setBottom(b - 1);
            }
        };
        QRect rc = rcDest;
        rc.adjust(0, 0, 1, 1);
        QRect rcImage = rcSrc;
        rcImage.adjust(0, 0, 1, 1);

        // left-top
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.left(), rc.top(), rc.left() + rcDestSizingMargins.left(),
                               rc.top() + rcDestSizingMargins.top()),
                      qdf_Rect(rcImage.left(), rcImage.top(), rcImage.left() + rcSizingMargins.left(),
                               rcImage.top() + rcSizingMargins.top()),
                      alpha);
        // top-center
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.left() + rcDestSizingMargins.left(), rc.top(),
                               rc.right() - rcDestSizingMargins.right(),
                               rc.top() + rcDestSizingMargins.top()),
                      qdf_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top(),
                               rcImage.right() - rcSizingMargins.right(),
                               rcImage.top() + rcSizingMargins.top()),
                      alpha);
        // right-top
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.right() - rcDestSizingMargins.right(), rc.top(), rc.right(),
                               rc.top() + rcDestSizingMargins.top()),
                      qdf_Rect(rcImage.right() - rcSizingMargins.right(), rcImage.top(), rcImage.right(),
                               rcImage.top() + rcSizingMargins.top()),
                      alpha);
        // left-center
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.left(), rc.top() + rcDestSizingMargins.top(),
                               rc.left() + rcDestSizingMargins.left(),
                               rc.bottom() - rcDestSizingMargins.bottom()),
                      qdf_Rect(rcImage.left(), rcImage.top() + rcSizingMargins.top(),
                               rcImage.left() + rcSizingMargins.left(),
                               rcImage.bottom() - rcSizingMargins.bottom()),
                      alpha);
        // center
        drawImagePart(
                dcSrc, p,
                qdf_Rect(rc.left() + rcDestSizingMargins.left(), rc.top() + rcDestSizingMargins.top(),
                         rc.right() - rcDestSizingMargins.right(),
                         rc.bottom() - rcDestSizingMargins.bottom()),
                qdf_Rect(rcImage.left() + rcSizingMargins.left(), rcImage.top() + rcSizingMargins.top(),
                         rcImage.right() - rcSizingMargins.right(),
                         rcImage.bottom() - rcSizingMargins.bottom()),
                alpha);
        // right-center
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.right() - rcDestSizingMargins.right(),
                               rc.top() + rcDestSizingMargins.top(), rc.right(),
                               rc.bottom() - rcDestSizingMargins.bottom()),
                      qdf_Rect(rcImage.right() - rcSizingMargins.right(),
                               rcImage.top() + rcSizingMargins.top(), rcImage.right(),
                               rcImage.bottom() - rcSizingMargins.bottom()),
                      alpha);
        // left-bottom
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.left(), rc.bottom() - rcDestSizingMargins.bottom(),
                               rc.left() + rcDestSizingMargins.left(), rc.bottom()),
                      qdf_Rect(rcImage.left(), rcImage.bottom() - rcSizingMargins.bottom(),
                               rcImage.left() + rcSizingMargins.left(), rcImage.bottom()),
                      alpha);
        // bottom-center
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.left() + rcDestSizingMargins.left(),
                               rc.bottom() - rcDestSizingMargins.bottom(),
                               rc.right() - rcDestSizingMargins.right(), rc.bottom()),
                      qdf_Rect(rcImage.left() + rcSizingMargins.left(),
                               rcImage.bottom() - rcSizingMargins.bottom(),
                               rcImage.right() - rcSizingMargins.right(), rcImage.bottom()),
                      alpha);
        // right-bottom
        drawImagePart(dcSrc, p,
                      qdf_Rect(rc.right() - rcDestSizingMargins.right(),
                               rc.bottom() - rcDestSizingMargins.bottom(), rc.right(), rc.bottom()),
                      qdf_Rect(rcImage.right() - rcSizingMargins.right(),
                               rcImage.bottom() - rcSizingMargins.bottom(), rcImage.right(),
                               rcImage.bottom()),
                      alpha);
    }
}

void QdfCommonStyle::setDefaultStyle(QStyle *pStyle)
{
    QDF_D(QdfCommonStyle);
    d->m_defaultStyle = pStyle;
}

QStyle *QdfCommonStyle::defaultStyle() const
{
    QDF_D(const QdfCommonStyle);
    return d->m_defaultStyle;
}

QdfStyleHelper &QdfCommonStyle::helper() const
{
    QDF_D(const QdfCommonStyle);
    return *d->m_helper;
}

bool QdfCommonStyle::transitionsEnabled() const
{
    return false;
}

bool QdfCommonStyle::paintAnimation(TypePaint type, int nPrim, const QStyleOption *option, QPainter *p,
                                    const QWidget *widget, int iDurationOver) const
{
    QDF_D(const QdfCommonStyle);

    if (!transitionsEnabled() && widget->isVisible())
    {
        return false;
    }

    bool bAnimation = qstyleoption_cast<const QStyleOptionToolButton *>(option) ||
                      qstyleoption_cast<const QStyleOptionButton *>(option) ||
                      qstyleoption_cast<const QStyleOptionGroupBox *>(option) ||
                      qstyleoption_cast<const QStyleOptionHeader *>(option) ||
                      qstyleoption_cast<const QStyleOptionComplex *>(option) ||
                      qstyleoption_cast<const QdfStyleOptionTitleBar *>(option) ||
                      qstyleoption_cast<const QdfStyleRibbonOptionHeader *>(option) ||
                      qstyleoption_cast<const QStyleOptionFrame *>(option);

    int state = option->state;

    QRect oldRect;
    QRect newRect;

    bool wasDetermined = widget && widget->property("_qdf_isDetermined").toBool();
    if (widget && !wasDetermined && bAnimation)
    {
        QWidget *w = const_cast<QWidget *>(widget);
        int oldState = w->property("_q_stylestate").toInt();
        oldRect = w->property("_q_stylerect").toRect();
        newRect = w->rect();

        w->setProperty("_q_stylestate", (int) option->state);
        w->setProperty("_q_stylerect", w->rect());
        w->setProperty("_qdf_isDetermined", true);

        // Determine the animated transition
        bool doTransition = ((state & State_Sunken) != (oldState & State_Sunken) ||
                             (state & State_On) != (oldState & State_On) ||
                             (state & State_MouseOver) != (oldState & State_MouseOver));

        QdfCommonStyle *thisStyle = (QdfCommonStyle *) this;

        if (oldRect != newRect || (state & State_Enabled) != (oldState & State_Enabled) ||
            (state & State_Active) != (oldState & State_Active))
        {
            thisStyle->qdf_d()->stopAnimation(widget);
        }

        if (doTransition)
        {
            QRect rect(option->rect);
            QImage startImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
            QImage endImage(rect.size(), QImage::Format_ARGB32_Premultiplied);

            QdfCommonAnimation *anim = d->widgetAnimation(widget);

            startImage.fill(0);
            QdfCommonTransition *transition = new QdfCommonTransition;
            transition->setWidget(w);

            bool bTransition = qstyleoption_cast<const QStyleOptionComboBox *>(option) ||
                               qstyleoption_cast<const QdfStyleSystemToolButton *>(option) ||
                               qstyleoption_cast<const QStyleOptionToolButton *>(option) ||
                               qstyleoption_cast<const QdfStyleOptionTitleBar *>(option) ||
                               qstyleoption_cast<const QdfStyleRibbonOptionHeader *>(option) ||
                               qstyleoption_cast<const QStyleOptionButton *>(option) ||
                               qstyleoption_cast<const QStyleOptionGroupBox *>(option) ||
                               qstyleoption_cast<const QStyleOptionHeader *>(option) ||
                               qstyleoption_cast<const QStyleOptionComplex *>(option) ||
                               qstyleoption_cast<const QStyleOptionFrame *>(option);

            if (!bTransition)
            {
                delete transition;
                return false;
            }

            QStyleOption *opt = const_cast<QStyleOption *>(option);

            opt->rect.setRect(0, 0, rect.width(), rect.height());

            QPainter startPainter(&startImage);
            if (!anim)
            {
                opt->state = (QStyle::State) oldState;
                switch (type)
                {
                    case tp_PrimitiveElement:
                        drawPrimitive((PrimitiveElement) nPrim, opt, &startPainter, widget);
                        break;
                    case tp_ControlElement:
                        drawControl((ControlElement) nPrim, opt, &startPainter, widget);
                        break;
                    case tp_ComplexControl:
                        {
                            if (const QStyleOptionComplex *pOptionComplex =
                                        qstyleoption_cast<const QStyleOptionComplex *>(opt))
                            {
                                drawComplexControl((ComplexControl) nPrim, pOptionComplex, &startPainter,
                                                   widget);
                            }
                            break;
                        }
                    default:
                        break;
                }
            }
            else
            {
                anim->paint(&startPainter, opt);
                thisStyle->qdf_d()->stopAnimation(widget);
            }

            transition->setStartImage(startImage);
            endImage.fill(0);

            opt->state = (QStyle::State) option->state;

            QPainter endPainter(&endImage);
            switch (type)
            {
                case tp_PrimitiveElement:
                    drawPrimitive((PrimitiveElement) nPrim, opt, &endPainter, widget);
                    break;
                case tp_ControlElement:
                    drawControl((ControlElement) nPrim, opt, &endPainter, widget);
                    break;
                case tp_ComplexControl:
                    {
                        if (const QStyleOptionComplex *pOptionComplex =
                                    qstyleoption_cast<const QStyleOptionComplex *>(opt))
                        {
                            drawComplexControl((ComplexControl) nPrim, pOptionComplex, &endPainter,
                                               widget);
                        }
                        break;
                    }
                default:
                    break;
            }

            transition->setEndImage(endImage);

            if (option->state & State_MouseOver || option->state & State_Sunken)
            {
                transition->setDuration(iDurationOver);
            }
            else
            {
                transition->setDuration(300);
            }

            thisStyle->qdf_d()->startAnimation(transition);
            transition->setStartTime(QTime::currentTime());
        }
        w->setProperty("_qdf_isDetermined", false);
    }

    if (QdfCommonAnimation *anim = d->widgetAnimation(widget))
    {
        anim->paint(p, option);
        return true;
    }
    return false;
}

bool QdfCommonStyle::isNativeDialog(const QWidget *wid) const
{
    Q_UNUSED(wid);
    return false;
}

bool QdfCommonStyle::showToolTip(const QPoint &pos, QWidget *w)
{
    Q_UNUSED(pos);
    Q_UNUSED(w);
    return false;
}


bool QdfCommonStyle::event(QEvent *e)
{
    QDF_D(QdfCommonStyle);
    switch (e->type())
    {
        case QEvent::Timer:
            {
                QTimerEvent *pTimerEvent = (QTimerEvent *) e;
                if (d->m_animationTimer.timerId() == pTimerEvent->timerId())
                {
                    d->doTimer();
                    e->accept();
                    return true;
                }
            }
            break;
        default:
            break;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::event(e);
#else
    return QWindowsStyle::event(e);
#endif
}


bool QdfCommonStyle::eventFilter(QObject *watched, QEvent *event)
{
    QDF_D(QdfCommonStyle);
    switch (event->type())
    {
        case QEvent::Destroy:
        case QEvent::Create:
            if (QWidget *widget = qobject_cast<QWidget *>(watched))
            {
                d->stopAnimation(widget);
            }
            break;
        case QEvent::ToolTip:
            if (QWidget *pWidget = qobject_cast<QWidget *>(watched))
            {
                if (showToolTip(static_cast<QHelpEvent *>(event)->globalPos(), pWidget))
                {
                    return true;
                }
            }
            break;
        default:
            break;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return QCommonStyle::eventFilter(watched, event);
#else
    return QWindowsStyle::eventFilter(watched, event);
#endif
}
