#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QComboBox>
#include <QCommonStyle>
#include <QDockWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QMdiArea>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QTextEdit>
#include <QToolTip>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qdrawutil.h>
#endif

#include "../private/qdfribbon_p.h"
#include "../private/qdfribbonbutton_p.h"
#include "../private/qdfribbongroup_p.h"
#include "../qdfribbontabbar.h"
#include "qdfcommonstyle_p.h"
#include "qdfofficeframehelper.h"
#include "qdfribbonstyle_p.h"
#include <qdfmainwindow.h>
#include <ribbon/qdfcommonstyle.h>
#include <ribbon/qdfribbonbackstageview.h>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribbongallerycontrol.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonquickaccessbar.h>
#include <ribbon/qdfribbonsliderpane.h>
#include <ribbon/qdfribbonstatusbar.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfribbonsystempopupbar.h>
#include <ribbon/qdfribbontooltip.h>
#include <ribbon/qdfstylehelpers.h>

QDF_USE_NAMESPACE


static QColor qdf_getColorToRGB(QdfRibbonPage::ContextColor color)
{
    switch (color)
    {
        case QdfRibbonPage::ContextColorRed:
            return QColor(255, 160, 160);

        case QdfRibbonPage::ContextColorOrange:
            return QColor(239, 189, 55);

        case QdfRibbonPage::ContextColorYellow:
            return QColor(253, 229, 27);

        case QdfRibbonPage::ContextColorGreen:
            return QColor(113, 190, 89);

        case QdfRibbonPage::ContextColorBlue:
            return QColor(128, 181, 196);

        case QdfRibbonPage::ContextColorCyan:
            return QColor(114, 163, 224);

        case QdfRibbonPage::ContextColorPurple:
            return QColor(214, 178, 209);
        default:
            break;
    }
    return QColor();
}

QdfRibbonPaintManager::QdfRibbonPaintManager(QdfCommonStyle *baseStyle)
    : QdfOfficePaintManager(baseStyle)
{
}

QdfRibbonPaintManager::~QdfRibbonPaintManager()
{
}

QIcon QdfRibbonPaintManager::standardIconEx(QStyle::StandardPixmap px, const QStyleOption *option,
                                            const QWidget *wd, bool &ret) const
{
    Q_UNUSED(px);
    Q_UNUSED(option);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

void QdfRibbonPaintManager::drawRibbonBar(const QStyleOption *option, QPainter *painter,
                                          const QWidget *widget) const
{
    Q_UNUSED(widget)
    if (const QdfStyleOptionRibbon *optRibbonBar =
                qstyleoption_cast<const QdfStyleOptionRibbon *>(option))
    {
        painter->fillRect(optRibbonBar->rect, ribbonBarColor(optRibbonBar->isBackstageVisible));

        if (!optRibbonBar->pixTitleBackground.isNull())
        {
            QRect rect = optRibbonBar->rect;
            QRect rectSrc = optRibbonBar->pixTitleBackground.rect();
            QRect rectDst = rect;
            rectDst.setLeft(rectDst.right() - rectSrc.width() + 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.width() && rectDst.width() != rectSrc.width())
                {
                    rectSrc.setLeft(rectSrc.left() + (rectDst.left() - rt.left()));
                    rectSrc.setRight(rectSrc.left() + qMin(rectDst.width(), rectSrc.width()));
                }
            }

            rectDst.setBottom(rectDst.top() + rectSrc.height() - 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.height() && rectDst.height() != rectSrc.height())
                {
                    rectSrc.setTop(rectSrc.top() + (rectDst.top() - rt.top()));
                    rectSrc.setBottom(rectSrc.top() + qMin(rectDst.height(), rectSrc.height()));
                }
            }
            drawPixTitleBackground(painter, rectDst, optRibbonBar->pixTitleBackground, rectSrc, widget);
        }
    }
}

void QdfRibbonPaintManager::drawPixTitleBackground(QPainter *painter, const QRect &rectDst,
                                                   const QPixmap &pixTitleBackground,
                                                   const QRect &rectSrc, const QWidget *widget) const
{
    Q_UNUSED(widget);

    if (theme() == QdfOfficeStyle::OfficeDark)
    {
        QImage image = pixTitleBackground.toImage();
        QColor color(0, 0, 0);
        QColor clrDest(240, 240, 240);
        QImage imageMask = QdfDrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
        imageMask = QdfDrawHelpers::updateImage(imageMask, color.rgb(), clrDest.rgb());
        image.setAlphaChannel(imageMask);
        painter->drawImage(rectDst, image, rectSrc);
    }
    else
    {
        painter->drawPixmap(rectDst, pixTitleBackground, rectSrc);
    }
}

void QdfRibbonPaintManager::drawRibbonTabBar(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(QdfRibbonStyle)

    if (d->m_clrRibbonTabBarBackground != d->m_clrRibbonFace)
    {
        painter->fillRect(option->rect, d->m_clrRibbonTabBarBackground.isValid()
                                                ? d->m_clrRibbonTabBarBackground
                                                : d->m_clrRibbonFace);
    }
}

void QdfRibbonPaintManager::drawRibbonGroups(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(QdfRibbonStyle)
    if (const QdfStyleOptionRibbon *optGroups = qstyleoption_cast<const QdfStyleOptionRibbon *>(option))
    {
        painter->fillRect(optGroups->rect, m_clrRibbonGroupsFill);

        QRect rectGroups = optGroups->rect;
        QRect rectActiveTab;
        if (const QdfRibbonBar *rb = qobject_cast<const QdfRibbonBar *>(widget))
        {
            QList<QdfRibbonTabBar *> l = widget->findChildren<QdfRibbonTabBar *>();
            if (l.size() > 0)
            {
                QdfRibbonTabBar *tabBar = l[0];
                if (QdfRibbonTab *tab = tabBar->getTab(tabBar->currentIndex()))
                {
                    rectActiveTab = tab->rect();
                    QPoint pntrb(tab->mapTo((QWidget *) rb, rectActiveTab.topLeft()));
                    rectActiveTab.translate(pntrb);
                }
            }
        }

        if (d->m_isActiveTabAccented)
        {
            QPen saveTopPen = painter->pen();
            painter->setPen(QPen(d->m_clrAccent, d->m_lineWidthTop));

            QPainterPath pathTop;
            pathTop.moveTo(rectGroups.left(), rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.left() + 1, rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.right() - 2, rectGroups.top() + 1);
            pathTop.lineTo(rectGroups.right() - 1, rectGroups.top() + 1);
            painter->drawPath(pathTop);
            painter->setPen(saveTopPen);

            QPainterPath path;
            path.moveTo(rectGroups.right() - 1, rectGroups.top() + 1);
            path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.top());

            QPen savePen = painter->pen();
            painter->setPen(d->m_clrRibbonSeparator);
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        else
        {
            QPainterPath path;
            if (optGroups->tabBarPosition == QdfRibbonBar::TabBarTopPosition)
            {
                path.moveTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectActiveTab.left() + 1, rectGroups.top());
                path.lineTo(rectActiveTab.right() - 2, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.top());
            }
            else if (optGroups->tabBarPosition == QdfRibbonBar::TabBarBottomPosition)
            {
                path.moveTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectActiveTab.left(), rectGroups.bottom() - 1);
                path.moveTo(rectActiveTab.right(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.bottom());
            }
            QPen savePen = painter->pen();
            painter->setPen(d->m_clrRibbonSeparator);
            painter->drawPath(path);
            painter->setPen(savePen);
        }
    }
}

void QdfRibbonPaintManager::drawGroup(const QStyleOption *option, QPainter *painter,
                                      const QWidget *widget) const
{
    Q_UNUSED(widget)
    QDF_D_STYLE(QdfRibbonStyle)
    QRect rect = option->rect;
    QPen savePen = painter->pen();
    painter->setPen(d->m_clrRibbonSeparator);
    painter->drawLine(QPoint(rect.right() - 1, rect.top()), QPoint(rect.right() - 1, rect.bottom()));
    painter->setPen(savePen);
}

void QdfRibbonPaintManager::drawReducedGroup(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    QDF_D_STYLE(QdfRibbonStyle)
    if (const QStyleOptionToolButton *optGroup =
                qstyleoption_cast<const QStyleOptionToolButton *>(option))
    {
        QRect rcEntry(option->rect);
        if (optGroup->features & QStyleOptionToolButton::HasMenu)
        {
            painter->fillRect(rcEntry.adjusted(0, 0, -1, 0), m_clrBarShadow);
        }
        else if (option->state & QStyle::State_MouseOver)
        {
            painter->fillRect(rcEntry, QdfDrawHelpers::pixelAlpha(m_clrBarShadow.rgb(), 103));
        }

        QPen savePen = painter->pen();
        painter->setPen(d->m_clrRibbonSeparator);
        painter->drawLine(QPoint(rcEntry.right() - 1, rcEntry.top()),
                          QPoint(rcEntry.right() - 1, rcEntry.bottom()));
        painter->setPen(savePen);

        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
        {
            flags |= Qt::TextHideMnemonic;
        }

        if (!optGroup->icon.isNull())
        {
            QPixmap pm;
            QSize pmSize = optGroup->iconSize;

            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
            {
                mode = QIcon::Disabled;
            }
            else if ((option->state & QStyle::State_MouseOver) &&
                     (option->state & QStyle::State_AutoRaise))
            {
                mode = QIcon::Active;
            }
            else
            {
                mode = QIcon::Normal;
            }

            pm = optGroup->icon.pixmap(optGroup->rect.size().boundedTo(optGroup->iconSize), mode,
                                       stateIcon);
            pmSize = pm.size();

            QRect rectImage = option->rect;
            if (pmSize.width() < 20)
            {
                rectImage.setTop(rectImage.top() + 10);
                rectImage.setBottom(rectImage.top() + pmSize.height());

                rectImage.setLeft(rectImage.center().x() - pmSize.width() / 2);
                rectImage.setRight(rectImage.left() + pmSize.width());
                rectImage.adjust(-5, -5, 5, 5);
                painter->fillRect(rectImage, m_clrBarFace);

                QPen savePen = painter->pen();
                painter->setPen(d->m_clrMenuPopupSeparator);
                painter->drawRect(rectImage);
                painter->setPen(savePen);
            }
            baseStyle()->proxy()->drawItemPixmap(painter, rectImage, Qt::AlignCenter, pm);
        }

        QString strFirstRow, strSecondRow;
        QdfCommonStyle::splitString(optGroup->text, strFirstRow, strSecondRow);

        int textHeightTotal = optGroup->rect.height() * 0.5;

        QRect rcText = rcEntry;
        rcText.adjust(4, textHeightTotal - 3, -4, 0);

        QRect rectFirstRow = rcText;
        if (!strFirstRow.isEmpty())
        {
            int textHeight = optGroup->fontMetrics.boundingRect(strFirstRow).height() +
                             optGroup->fontMetrics.descent();
            rectFirstRow.setHeight(textHeight + 3);
            baseStyle()->proxy()->drawItemText(
                    painter, rectFirstRow, flags | Qt::AlignHCenter, optGroup->palette,
                    optGroup->state & QStyle::State_Enabled, strFirstRow, QPalette::WindowText);
        }

        QRect rectSecondRow = rectFirstRow;
        if (!strSecondRow.isEmpty())
        {
            rectSecondRow.setTop(rectFirstRow.bottom());
            int textHeight = optGroup->fontMetrics.boundingRect(strSecondRow).height() +
                             optGroup->fontMetrics.descent();
            rectSecondRow.setHeight(textHeight);
            baseStyle()->proxy()->drawItemText(
                    painter, rectSecondRow, flags | Qt::AlignLeft, optGroup->palette,
                    optGroup->state & QStyle::State_Enabled, strSecondRow, QPalette::WindowText);
        }
        QdfOfficePaintManager::ImageIcons index = QdfOfficePaintManager::Icon_ArowDown;
        QdfOfficePaintManager::ImageState state = QdfOfficePaintManager::Black2;

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        int width = option->fontMetrics.width(strSecondRow);
#else
        int width = option->fontMetrics.horizontalAdvance(strSecondRow);
#endif
        QPoint pt = QPoint(
                strSecondRow.isEmpty() ? rcText.center().x() - 4 : rectSecondRow.left() + width + 1,
                strSecondRow.isEmpty() ? rectFirstRow.bottom()
                                       : (rectSecondRow.top() + (rectSecondRow.height() - 9) / 2) + 1);

        bool enabled = option->state & QStyle::State_Enabled;
        if (!enabled)
        {
            state = QdfOfficePaintManager::LtGray;
        }
        else if (optGroup->features & QStyleOptionToolButton::HasMenu)
        {
            drawIconByColor(painter, index, pt, m_clrHighlightDn);
            return;
        }
        drawIcon(painter, pt, index, state, QSize(9, 9));
    }
}

bool QdfRibbonPaintManager::drawToolBar(const QStyleOption *option, QPainter *painter,
                                        const QWidget *widget) const
{
    if (qstyleoption_cast<const QStyleOptionToolBar *>(option))
    {
        return QdfOfficePaintManager::drawToolBar(option, painter, widget);
    }
    else if (qstyleoption_cast<const QdfStyleRibbonQuickAccessBar *>(option))
    {
        return true;
    }
    return QdfOfficePaintManager::drawToolBar(option, painter, widget);
}

bool QdfRibbonPaintManager::drawIndicatorToolBarSeparator(const QStyleOption *option, QPainter *painter,
                                                          const QWidget *widget) const
{
    if (qobject_cast<const QdfRibbonGroup *>(widget))
    {
        if (const QStyleOptionGroupBox *optGroup =
                    qstyleoption_cast<const QStyleOptionGroupBox *>(option))
        {
            QDF_D_STYLE(QdfRibbonStyle)
            QRect rect = option->rect;

            if (optGroup->text.isEmpty())
            {
                QPainterPath path;
                path.moveTo(rect.center().x(), rect.top());
                path.lineTo(rect.center().x(), rect.bottom());

                QPen savePen = painter->pen();
                painter->setPen(d->m_clrRibbonSeparator);
                painter->drawPath(path);
                painter->setPen(savePen);
            }
            else
            {
                QString titleText = optGroup->text;
                QRect rectText = optGroup->fontMetrics.boundingRect(titleText);
                if (optGroup->textAlignment == Qt::AlignBottom)
                {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top());
                    path.lineTo(rect.center().x(), rect.bottom() - rectText.width() - 4);

                    painter->save();
                    painter->setPen(d->m_clrRibbonSeparator);
                    painter->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(
                            painter, rect.adjusted(indent + 1, 1, -indent - 1, -1),
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                            optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignTop)
                {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top() + rectText.width());
                    path.lineTo(rect.center().x(), rect.bottom());

                    painter->save();
                    painter->setPen(d->m_clrRibbonSeparator);
                    painter->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(
                            painter,
                            rect.adjusted(indent + rect.width() - rectText.width() - 2, 1, -indent - 1,
                                          -1),
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                            optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignVCenter)
                {
                    painter->save();
                    painter->setPen(d->m_clrRibbonSeparator);
                    painter->drawLine(rect.center().x(),
                                      rect.top() + rectText.width() + rectText.width() + 4,
                                      rect.center().x(), rect.bottom());
                    painter->drawLine(rect.center().x(), rect.top(), rect.center().x(),
                                      rect.bottom() - (rectText.width() + rectText.width() + 4));

                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    painter->translate(rect.left() - 1, rect.top() + rect.width());
                    painter->rotate(-90);
                    painter->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(
                            painter,
                            rect.adjusted(indent + (rect.width() - rectText.width()) / 2 - 2, 1,
                                          -indent - 1, -1),
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                            optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    painter->restore();
                }
            }
            return true;
        }
    }
    else if (qobject_cast<const QdfRibbonStatusBar *>(widget))
    {
        return true;
    }
    return false;
}

bool QdfRibbonPaintManager::drawPanelButtonTool(const QStyleOption *option, QPainter *painter,
                                                const QWidget *widget) const
{
    if (!widget)
    {
        return false;
    }

    if (widget && qobject_cast<const QdfRibbonBackstageButton *>(widget))
    {
        drawPanelBackstageButton(option, painter, widget);
        return true;
    }

    if (widget && !qobject_cast<const QdfRibbonTitleButton *>(widget))
    {
        if (const QStyleOptionToolButton *toolbutton =
                    qstyleoption_cast<const QStyleOptionToolButton *>(option))
        {
            bool smallSize = option->rect.height() < 33;

            QdfRibbonToolBarControl *toolBarControl =
                    qobject_cast<QdfRibbonToolBarControl *>(widget->parentWidget());
            if (toolBarControl)
            {
                smallSize = true;
            }

            if (widget && qobject_cast<const QdfRibbonButton *>(widget))
            {
                if (!option->rect.isEmpty())
                {
                    int minIcon = qMin(option->rect.height(), option->rect.width());
                    int actualArea = minIcon * minIcon;

                    int index = -1;
                    int curArea = 0;
                    QList<QSize> lSz = toolbutton->icon.availableSizes();
                    if (lSz.count() > 1)
                    {
                        for (int i = 0, count = lSz.count(); count > i; i++)
                        {
                            QSize curSz = lSz[i];
                            int area = curSz.height() * curSz.width();
                            if (actualArea > area)
                            {
                                if (area > curArea)
                                {
                                    index = i;
                                }
                                curArea = area;
                            }
                        }
                        int iconExtent = index != -1 ? toolbutton->icon.actualSize(lSz[index]).height()
                                                     : smallSize;
                        smallSize = toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon
                                            ? iconExtent < 32
                                            : iconExtent;
                    }
                }
            }

            bool enabled = option->state & QStyle::State_Enabled;
            bool checked = option->state & QStyle::State_On;
            bool highlighted = option->state & QStyle::State_MouseOver;
            bool mouseInSplit = option->state & QStyle::State_MouseOver &&
                                toolbutton->activeSubControls & QStyle::SC_ToolButton;
            bool mouseInSplitDropDown = option->state & QStyle::State_MouseOver &&
                                        toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu;
            bool pressed = option->state & QStyle::State_Sunken;
            bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) &&
                           (option->state & QStyle::State_Sunken);

            if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
            {
                bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
                if (!autoRaise && !qobject_cast<const QStatusBar *>(widget->parentWidget()))
                {
                    QStyleOptionButton tmpBtn;// = *btn;
                    tmpBtn.state = toolbutton->state;
                    tmpBtn.rect = toolbutton->rect;
                    baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, painter,
                                                        widget);
                }
                else
                {
                    QColor color;
                    if (!enabled)
                    {
                        color = QColor();
                    }

                    else if (popuped)
                    {
                        color = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (checked && !highlighted && !pressed)
                    {
                        color = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (pressed)
                    {
                        color = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (highlighted)
                    {
                        color = buttonHiliteColor(widget, false /*highlight*/);
                    }

                    if (color.isValid())
                    {
                        painter->fillRect(option->rect, color);
                    }

                    if (checked && highlighted)
                    {
                        QColor colorDn = buttonHiliteColor(widget, true /*highlightDn*/);
                        QdfDrawHelpers::draw3dRectEx(*painter, option->rect, colorDn, colorDn);
                    }
                }
                return true;
            }

            QRect rcButton = option->rect;
            QRect popupr = baseStyle()->subControlRect(QStyle::CC_ToolButton, toolbutton,
                                                       QStyle::SC_ToolButtonMenu, widget);

            if (highlighted)
            {
                QPen savePen = painter->pen();
                painter->setPen(buttonHiliteColor(widget, pressed /*highlightDn*/));
                painter->drawRect(rcButton.adjusted(0, 0, -1, -1));
                painter->setPen(savePen);
            }

            if (mouseInSplit || checked)
            {
                QColor colorSplit;
                if (!enabled)
                {
                    colorSplit = QColor();
                }
                else if (checked)
                {
                    if (popuped)
                    {
                        colorSplit = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (highlighted && !pressed && !checked)
                    {
                        colorSplit = buttonHiliteColor(widget, false /*highlight*/);
                    }
                    else if (highlighted && pressed)
                    {
                        colorSplit = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (pressed || checked)
                    {
                        colorSplit = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (highlighted || pressed)
                    {
                        colorSplit = !mouseInSplit ? buttonHiliteColor(widget, true /*highlightDn*/)
                                                   : QColor();
                    }
                }
                else
                {
                    if (popuped)
                    {
                        colorSplit = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (pressed)
                    {
                        colorSplit = buttonHiliteColor(widget, true /*highlightDn*/);
                    }
                    else if (highlighted)
                    {
                        colorSplit = buttonHiliteColor(widget, false /*highlight*/);
                    }
                }
                if (colorSplit.isValid())
                {
                    QRect rcSplitUp = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()),
                                                        QPoint(rcButton.right() - popupr.width() - 2,
                                                               rcButton.bottom()))
                                                : QRect(QPoint(rcButton.left(), rcButton.top()),
                                                        QPoint(rcButton.right(),
                                                               rcButton.bottom() - popupr.height() - 2));
                    painter->fillRect(rcSplitUp, colorSplit);
                }
            }

            if (mouseInSplitDropDown || popuped || checked)
            {
                if (!popuped)
                {
                    QRect rcSplitDown = smallSize
                                                ? QRect(QPoint(rcButton.right() - popupr.width() - 1,
                                                               rcButton.top()),
                                                        QPoint(rcButton.right(), rcButton.bottom()))
                                                : QRect(QPoint(rcButton.left(),
                                                               rcButton.bottom() - popupr.height() - 1),
                                                        QPoint(rcButton.right(), rcButton.bottom()));

                    QColor colorSplit = buttonHiliteColor(widget, pressed /*highlightDn*/);
                    painter->fillRect(rcSplitDown, colorSplit);
                }
                else if (popuped || checked)
                {
                    painter->fillRect(rcButton, buttonHiliteColor(widget, true /*highlightDn*/));
                }
            }
            return true;
        }
    }
    else if (widget && qobject_cast<const QdfRibbonTitleButton *>(widget))
    {
        if (const QdfStyleCaptionButton *toolbutton =
                    qstyleoption_cast<const QdfStyleCaptionButton *>(option))
        {
            bool down = toolbutton->state & QStyle::State_Sunken;
            bool highlighted = toolbutton->state & QStyle::State_MouseOver;

            if (down || highlighted)
            {
                painter->fillRect(toolbutton->rect, buttonHiliteColor(widget, down /*highlightDn*/));
            }

            QRect rectImage = toolbutton->rect;
            QSize sizeImage = QSize(9, 9);
            QPoint ptImage(rectImage.left() + (rectImage.width() - sizeImage.width()) / 2 +
                                   ((rectImage.width() - sizeImage.width()) % 2),
                           rectImage.top() + (rectImage.height() - sizeImage.height()) / 2 +
                                   ((rectImage.height() - sizeImage.height()) % 2));

            QColor colText =
                    titleButtonHiliteColor(!(toolbutton->state & QStyle::State_Enabled), widget);

            ImageState state = highlighted ? Black2 : Black;
            if (!(toolbutton->state & QStyle::State_Enabled))
            {
                state = Gray;
            }
            else if (colText.isValid())
            {
                if (colText.red() <= 128 || colText.green() <= 128 || colText.blue() <= 128)
                {
                    state = highlighted ? Black2 : Black;
                }
                else
                {
                    state = White;
                }
            }

            ImageIcons index;
            if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
            {
                index = Icon_Close;
            }
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMaxButton)
            {
                index = Icon_Maximize;
            }
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMinButton)
            {
                index = Icon_Minimize;
            }
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarNormalButton)
            {
                index = Icon_Restore;
            }
            else
            {
                Q_ASSERT(false);
                return false;
            }

            drawIcon(painter, ptImage, index, state, sizeImage);

            return true;
        }
    }

    return false;
}

void QdfRibbonPaintManager::drawContextHeaders(const QStyleOption *option, QPainter *painter) const
{
    if (const QdfStyleOptionTitleBar *optTitleBar =
                qstyleoption_cast<const QdfStyleOptionTitleBar *>(option))
    {
        const QList<QdfContextHeader *> &listContextHeaders = *optTitleBar->listContextHeaders;
        Q_FOREACH (const QdfContextHeader *header, listContextHeaders)
        {
            QRect rc = header->rcRect;
            rc.translate(0, 2);
            QColor color = qdf_getColorToRGB(header->color);

            QColor clrDark = QdfDrawHelpers::colorMakeDarker(color.rgb());
            QColor clrLight = QdfDrawHelpers::colorMakePale(color.rgb());

            painter->fillRect(rc, clrLight);

            QRect rectTop = header->rcRect;
            rectTop.setBottom(rectTop.top() + 6);

            painter->fillRect(rectTop, clrDark);

            rc.setLeft(rc.left() + 4);

            int frameBorder = optTitleBar->frameHelper ? optTitleBar->frameHelper->frameBorder() : 0;

            rc.setTop(rc.top() + (frameBorder - 2));

            QString text = header->strTitle.toUpper();

            QFont saveFont = painter->font();
            painter->setFont(header->firstTab->font());
            QPen savePen = painter->pen();
            painter->setPen(QdfDrawHelpers::colorMakeDarker(color.rgb(), .4));
            painter->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine,
                              painter->fontMetrics().elidedText(text, Qt::ElideRight, rc.width()));
            painter->setPen(savePen);
            painter->setFont(saveFont);
        }
    }
}

bool QdfRibbonPaintManager::drawSlider(const QStyleOptionComplex *option, QPainter *painter,
                                       const QWidget *widget) const
{
    if (!qobject_cast<QdfRibbonSliderPane *>(widget->parentWidget()))
    {
        return false;
    }

    if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
    {
        if (slider->orientation == Qt::Horizontal)
        {
            QRect groove = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, option,
                                                                QStyle::SC_SliderGroove, widget);
            QRect handle = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, option,
                                                                QStyle::SC_SliderHandle, widget);

            QRect rcTrackDest(QPoint(groove.left(), groove.center().y()), QSize(groove.width(), 2));
            rcTrackDest.adjust(3, 0, -2, 0);
            painter->fillRect(rcTrackDest, m_clrBarFace);
            QdfDrawHelpers::draw3dRectEx(*painter, rcTrackDest, m_clrBarDkShadow, m_clrBarDkShadow);

            if ((option->subControls & QStyle::SC_SliderHandle))
            {
                bool enabled = option->state & QStyle::State_Enabled;
                bool selected = slider->activeSubControls & QStyle::SC_SliderHandle &&
                                option->state & QStyle::State_MouseOver;
                bool pressetHt = slider->state & QStyle::State_Sunken;

                QColor clrLine = enabled ? m_clrBarDkShadow : m_clrBarShadow;
                QColor clrFill = m_clrBarFace;

                if (enabled && (pressetHt || selected))
                {
                    clrFill = m_clrAccentLight;
                    clrLine = m_clrHighlightDn;
                }

                handle.adjust(2, 0, -2, -2);
                handle.setLeft(handle.center().x() - 2);
                handle.setRight(handle.center().x() + 2);

                painter->fillRect(handle, clrFill);

                QPen savePen = painter->pen();
                painter->setPen(clrLine);
                painter->drawRect(handle);
                painter->setPen(savePen);
            }
            return true;
        }
    }
    return false;
}

void QdfRibbonPaintManager::drawGroupScrollButton(const QStyleOption *option, QPainter *painter,
                                                  const QWidget *widget) const
{
    Q_UNUSED(widget)
    if (const QStyleOptionToolButton *toolbutton =
                qstyleoption_cast<const QStyleOptionToolButton *>(option))
    {
        QDF_D_STYLE(const QdfRibbonStyle)
        QRect rc = toolbutton->rect;

        bool enabled = toolbutton->state & QStyle::State_Enabled;
        bool highlighted = toolbutton->state & QStyle::State_MouseOver;
        bool pressed = toolbutton->state & QStyle::State_Sunken;

        if (pressed)
        {
            painter->fillRect(rc, m_clrHighlightDn);
        }
        else if (pressed || highlighted)
        {
            painter->fillRect(rc, d->m_clrHighlight);
        }
        else
        {
            painter->fillRect(rc, QColor(255, 255, 255));
        }

        QColor clrBorder = !enabled                   ? m_clrBarShadow
                           : (highlighted || pressed) ? m_clrHighlightDn
                                                      : m_clrBarDkShadow;

        QPen oldPen = painter->pen();
        painter->setPen(clrBorder);
        painter->drawRect(rc.adjusted(0, -1, -1, -2));
        painter->setPen(oldPen);

        ImageIcons iImage = toolbutton->arrowType == Qt::LeftArrow
                                    ? QdfOfficePaintManager::Icon_ArowLeft
                                    : QdfOfficePaintManager::Icon_ArowRight;

        QdfOfficePaintManager::ImageState imageState = QdfOfficePaintManager::Gray;
        if (!enabled)
        {
            imageState = QdfOfficePaintManager::LtGray;
        }
        else if (highlighted)
        {
            imageState = QdfOfficePaintManager::DkGray;
        }

        QRect rcIndicator(QPoint(rc.x() + (rc.width() - 9) / 2, rc.y() + (rc.height() - 9) / 2),
                          QSize(9, 9));
        drawIcon(painter, rcIndicator, iImage, imageState, QSize(9, 9));
    }
}

bool QdfRibbonPaintManager::drawFrameMenu(const QStyleOption *option, QPainter *painter,
                                          const QWidget *widget) const
{
    QDF_D_STYLE(const QdfRibbonStyle)
    if (qobject_cast<const QdfRibbonSystemPopupBar *>(widget) ||
        qobject_cast<const QdfRibbonPageSystemPopup *>(widget))
    {
        QRect rect = option->rect;
        QdfDrawHelpers::draw3dRectEx(*painter, rect, d->m_clrMenubarBorder, d->m_clrMenubarBorder);
        rect.adjust(1, 1, -1, -1);
        QdfDrawHelpers::draw3dRectEx(*painter, rect, d->m_clrBackgroundLight, d->m_clrBackgroundLight);
        QRect rectLeft(1, 1, 2, rect.bottom() - 1);
        painter->fillRect(rectLeft, m_clrBarHilite);

        QRect rectFill = option->rect;
        rectFill.adjust(1, 1, -1, -1);
        painter->fillRect(rectFill, m_clrBarLight);
        return true;
    }
    return QdfOfficePaintManager::drawFrameMenu(option, painter, widget);
}

void QdfRibbonPaintManager::drawTabShape(const QStyleOption *option, QPainter *painter,
                                         const QWidget *widget) const
{
    QDF_D_STYLE(QdfRibbonStyle)
    if (const QdfStyleRibbonOptionHeader *optTab =
                qstyleoption_cast<const QdfStyleRibbonOptionHeader *>(option))
    {
        if (optTab->state & QStyle::State_Selected)
        {
            QRect rectTab = optTab->rect;
            painter->fillRect(rectTab,
                              d->m_isActiveTabAccented
                                      ? (optTab->state & QStyle::State_MouseOver ? m_clrAccentHilight
                                                                                 : d->m_clrAccent)
                                      : m_clrRibbonGroupsFill);

            QPainterPath path;
            if (optTab->tabBarPosition == QdfRibbonBar::TabBarTopPosition)
            {
                path.moveTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.bottom());
            }
            else if (optTab->tabBarPosition == QdfRibbonBar::TabBarBottomPosition)
            {
                path.moveTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.top());
            }
            else
            {
                return;
            }

            QPen savePen = painter->pen();
            painter->setPen(d->m_clrRibbonSeparator);
            painter->drawPath(path);
            painter->setPen(savePen);
        }
        else
        {
            fillRibbonTabControl(optTab, painter, widget);
        }

        if (optTab->state & QStyle::State_Small)
        {
            QRect rectTab = optTab->rect;
            QPainterPath path;
            path.moveTo(rectTab.right(), rectTab.top());
            path.lineTo(rectTab.right(), rectTab.bottom());
            QPen savePen = painter->pen();
            painter->setPen(d->m_clrRibbonSeparator);
            painter->drawPath(path);
            painter->setPen(savePen);
        }
    }
}

void QdfRibbonPaintManager::drawTabShapeLabel(const QStyleOption *option, QPainter *painter,
                                              const QWidget *widget) const
{
    QDF_D_STYLE(QdfRibbonStyle)
    if (const QStyleOptionHeader *optTab = qstyleoption_cast<const QStyleOptionHeader *>(option))
    {
        bool selected = option->state & QStyle::State_Selected;
        bool highlighted = option->state & QStyle::State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, option, widget))
        {
            flags |= Qt::TextHideMnemonic;
        }

        QdfRibbonPage::ContextColor pageColor = QdfRibbonPage::ContextColorNone;
        if (const QdfRibbonTab *tab = qobject_cast<const QdfRibbonTab *>(widget))
        {
            pageColor = tab->contextColor();
        }

        QColor textColor = d->m_clrTabNormalText;
        if (d->m_clrRibbonTabBarText.isValid())
        {
            textColor = d->m_clrRibbonTabBarText;
        }

        if (selected && pageColor != QdfRibbonPage::ContextColorNone)
        {
            textColor = ::qdf_getColorToRGB(pageColor);
            textColor = QdfDrawHelpers::colorMakeDarker(textColor.rgb(), .4);
        }
        else if (selected || highlighted)
        {
            textColor = d->m_clrTabSelectedText;
        }

        if (d->m_isActiveTabAccented && selected)
        {
            textColor = m_clrRibbonGroupsFill;
        }

        QString text = optTab->text;
        if (d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(widget))
        {
            text = text.toUpper();
        }

        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        baseStyle()->proxy()->drawItemText(painter, optTab->rect, flags, pal,
                                           option->state & QStyle::State_Enabled, text,
                                           QPalette::WindowText);
    }
}

bool QdfRibbonPaintManager::drawTabBarTabShape(const QStyleOption *option, QPainter *painter,
                                               const QWidget *widget) const
{
    return QdfOfficePaintManager::drawTabBarTabShape(option, painter, widget);
}

bool QdfRibbonPaintManager::drawShapedFrame(const QStyleOption *option, QPainter *painter,
                                            const QWidget *widget) const
{
    return QdfOfficePaintManager::drawShapedFrame(option, painter, widget);
}

void QdfRibbonPaintManager::drawSystemButton(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    QDF_D_STYLE(const QdfRibbonStyle)
    if (const QdfStyleSystemToolButton *toolbutton =
                qstyleoption_cast<const QdfStyleSystemToolButton *>(option))
    {
        if (qobject_cast<const QdfRibbonBar *>(widget->parentWidget()))
        {
            if (toolbutton->toolButtonStyle != Qt::ToolButtonFollowStyle)
            {
                bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) &&
                               (option->state & QStyle::State_Sunken);
                bool isHighlighted = option->state & QStyle::State_MouseOver;
                bool enabled = widget->isEnabled();

                bool usePalette = false;
                uint resolve_mask = toolbutton->palette.resolve();
                if (resolve_mask & (1 << QPalette::Button))
                {
                    usePalette = true;
                }

                if (usePalette)
                {
                    QBrush brButton = toolbutton->palette.brush(QPalette::Active, QPalette::Button);
                    QBrush brButtonDisabled =
                            toolbutton->palette.brush(QPalette::Disabled, QPalette::Button);
                    QBrush brHighlightButton =
                            (resolve_mask & (1 << QPalette::Highlight))
                                    ? toolbutton->palette.brush(QPalette::Active, QPalette::Highlight)
                                    : brButton;
                    QBrush brPopupButton =
                            (resolve_mask & (1 << QPalette::Dark))
                                    ? toolbutton->palette.brush(QPalette::Active, QPalette::Dark)
                                    : brButton;

                    QBrush brush = !enabled        ? brButtonDisabled
                                   : popuped       ? (isHighlighted ? brHighlightButton : brPopupButton)
                                   : false         ? brButton
                                   : isHighlighted ? brHighlightButton
                                                   : brButton;
                    painter->fillRect(toolbutton->rect, brush);
                }
                else
                {
                    painter->fillRect(toolbutton->rect,
                                      isHighlighted ? m_clrAccentHilight : d->m_clrAccent);
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly && !toolbutton->icon.isNull())
                {
                    bool enabled = toolbutton->state & QStyle::State_Enabled;
                    QRect rc = toolbutton->rect;
                    QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize,
                                                             enabled ? QIcon::Normal : QIcon::Disabled,
                                                             isHighlighted ? QIcon::On : QIcon::Off);
                    QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2,
                                  (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                    painter->drawPixmap(ptIcon, pxIcon);
                }
                return;
            }
        }

        bool isHighlighted = option->state & QStyle::State_MouseOver;
        if ((toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) &&
            (toolbutton->state & QStyle::State_Sunken))
        {
            isHighlighted = true;
        }

        painter->fillRect(toolbutton->rect.adjusted(0, 0, -1, -1),
                          isHighlighted ? m_clrAccentHilight : d->m_clrAccent);
        if (!toolbutton->icon.isNull())
        {
            QRect rc = toolbutton->rect;
            bool enabled = toolbutton->state & QStyle::State_Enabled;
            bool selected = toolbutton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize,
                                                     enabled ? QIcon::Normal : QIcon::Disabled,
                                                     selected ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2,
                          (rc.top() + rc.bottom() - pxIcon.height()) / 2);
            painter->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
        }
    }
}

void QdfRibbonPaintManager::drawQuickAccessButton(const QStyleOption *option, QPainter *painter,
                                                  const QWidget *widget) const
{
    Q_UNUSED(widget);
    fillRibbonButton(option, painter, widget);
    ImageIcons index = Icon_CustomizeArowDown;
    QRect rect = option->rect;
    QRect rectWhite = rect;
    rectWhite.adjust(0, 1, 0, 1);
    drawIcon(painter, rectWhite, index, LtGray);
    drawIcon(painter, rect, index, Black2);
}

void QdfRibbonPaintManager::drawOptionButton(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    Q_UNUSED(widget);

    fillRibbonButton(option, painter, widget);

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;
    bool enabled = option->state & QStyle::State_Enabled;
    QRect rect = option->rect;

    QdfOfficePaintManager::ImageState state = QdfOfficePaintManager::Black2;
    QdfOfficePaintManager::ImageIcons index = QdfOfficePaintManager::Icon_LaunchArrow;

    if (!enabled)
    {
        state = QdfOfficePaintManager::LtGray;
    }
    else if (highlighted && pressed)
    {
        drawIconByColor(painter, index, rect, m_clrHighlightDn);
        return;
    }
    drawIcon(painter, rect, index, state);
}

void QdfRibbonPaintManager::drawPopupResizeGripper(const QStyleOption *option, QPainter *painter,
                                                   const QWidget *widget) const
{
    Q_UNUSED(widget);

    QDF_D_STYLE(const QdfRibbonStyle)
    if (const QStyleOptionSizeGrip *sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip *>(option))
    {
        painter->fillRect(sizeGrip->rect, d->m_clrBackgroundLight);

        QPoint pt(sizeGrip->rect.right() - 3, sizeGrip->rect.bottom() - 3);
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3 - y; x++)
            {
                painter->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)),
                                  QColor(255, 255, 255));
                painter->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)),
                                  d->m_clrToolbarSeparator);
            }
        }
    }
}

bool QdfRibbonPaintManager::drawPanelStatusBar(const QStyleOption *option, QPainter *painter,
                                               const QWidget *widget) const
{
    return QdfOfficePaintManager::drawPanelStatusBar(option, painter, widget);
}

bool QdfRibbonPaintManager::drawSizeGrip(const QStyleOption *option, QPainter *painter,
                                         const QWidget *widget) const
{
    return QdfOfficePaintManager::drawSizeGrip(option, painter, widget);
}

bool QdfRibbonPaintManager::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption *option,
                                               QPainter *painter, const QWidget *widget) const
{
    if (getParentWidget<QdfRibbonGroup>(widget) || getParentWidget<QdfRibbonBar>(widget) ||
        qobject_cast<const QdfRibbonBackstageButton *>(widget))
    {
        switch (pe)
        {
            case QStyle::PE_IndicatorArrowDown:
                if (qobject_cast<const QToolButton *>(widget))
                {
                    if (!qobject_cast<const QTabBar *>(widget->parentWidget()))
                    {
                        bool enabled = option->state & QStyle::State_Enabled;
                        bool highlighted = option->state & QStyle::State_Selected;
                        bool pressed = option->state & QStyle::State_Sunken;

                        QdfOfficePaintManager::ImageIcons index = QdfOfficePaintManager::Icon_ArowDown;
                        QdfOfficePaintManager::ImageState state = QdfOfficePaintManager::Black2;

                        if (!enabled)
                        {
                            state = QdfOfficePaintManager::LtGray;
                        }
                        else if (highlighted && pressed)
                        {
                            drawIconByColor(painter, index, option->rect, m_clrHighlightDn);
                            return true;
                        }
                        drawIcon(painter, option->rect, index, state);
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }

    return QdfOfficePaintManager::drawIndicatorArrow(pe, option, painter, widget);
}

void QdfRibbonPaintManager::drawRectangle(QPainter *painter, const QRect &rect, bool selected,
                                          bool pressed, bool enabled, bool checked, bool popuped,
                                          Qdf::BarType barType, Qdf::BarPosition barPos) const
{
    QdfOfficePaintManager::drawRectangle(painter, rect, selected, pressed, enabled, checked, popuped,
                                         barType, barPos);
}

void QdfRibbonPaintManager::drawKeyTip(const QStyleOption *option, QPainter *painter,
                                       const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(const QdfRibbonStyle)
    painter->fillRect(option->rect, m_clrBarHilite);
    QdfDrawHelpers::draw3dRectEx(*painter, option->rect, d->m_clrRibbonSeparator,
                                 d->m_clrRibbonSeparator);
}

void QdfRibbonPaintManager::drawBackstage(const QStyleOption *option, QPainter *painter,
                                          const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(const QdfRibbonStyle)
    if (const QdfStyleOptionRibbonBackstage *optBackstage =
                qstyleoption_cast<const QdfStyleOptionRibbonBackstage *>(option))
    {
        painter->fillRect(option->rect, m_clrBarLight);
        int top = (bool) baseStyle()->proxy()->styleHint(
                          (QStyle::StyleHint) QdfRibbonStyle::SH_RibbonBackstageHideTabs)
                          ? 0
                          : 2;
        QRect rc = QRect(QPoint(0, top), QPoint(optBackstage->menuWidth, option->rect.bottom()));
        painter->fillRect(rc, d->m_clrAccent);
    }
}

void QdfRibbonPaintManager::drawRibbonBackstageMenu(const QStyleOption *option, QPainter *painter,
                                                    const QWidget *widget) const
{
    QDF_D_STYLE(const QdfRibbonStyle)
    if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(option))
    {
        bool act = menuitem->state & QStyle::State_Selected;
        bool focus = menuitem->state & QStyle::State_HasFocus;
        bool dis = !(menuitem->state & QStyle::State_Enabled);

        QPalette pal = menuitem->palette;

        QColor clrText = dis ? QColor(192, 192, 192) : QColor(255, 255, 255);
        pal.setColor(QPalette::WindowText, clrText);

        QRect rcText(option->rect);
        rcText.adjust(15, 2, 2, 2);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            const int iconSize =
                    baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, option, widget);
            int yoff = (y - 1 + h / 2);
            QPen penSave = painter->pen();
            QColor clr = d->m_clrAccent.lighter(130);
            painter->setPen(clr);
            painter->drawLine(x + 2 + iconSize, yoff, x + w - 4, yoff);
            painter->setPen(penSave);
            return;
        }
        if (menuitem->menuItemType == QStyleOptionMenuItem::Normal)
        {
            if (act || focus)
            {
                painter->fillRect(option->rect, m_clrAccentText);
            }
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            if (act || focus)
            {
                painter->fillRect(option->rect, m_clrAccentText);
                if (focus)
                {
                    painter->fillRect(option->rect, m_clrAccentHilight);
                }
            }
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull())
        {
            int iconSize = baseStyle()->pixelMetric(QStyle::PM_LargeIconSize, option, widget);
            if (iconSize > qMin(option->rect.height(), option->rect.width()))
            {
                iconSize = baseStyle()->pixelMetric(QStyle::PM_SmallIconSize, option, widget);
            }

            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = menuitem->icon.pixmap(iconSize, mode);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - pixmap.height()) / 2);
            painter->drawPixmap(pmr, pixmap);
            iconWidth = pixmap.width();
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);

        if (!menuitem->text.isEmpty())
        {
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;
            int index = menuitem->text.indexOf(QLatin1String("&"));
            if (index != -1)
            {
                index = menuitem->text.indexOf(QLatin1String(" "), index);
                if (index != -1)
                {
                    flags &= ~Qt::TextHideMnemonic;
                }
            }
            // draw text
            baseStyle()->proxy()->drawItemText(painter, rcText, flags, pal,
                                               option->state & QStyle::State_Enabled, menuitem->text,
                                               QPalette::WindowText);
        }
    }
}

void QdfRibbonPaintManager::drawRibbonBackstageCloseButton(const QStyleOption *option, QPainter *painter,
                                                           const QWidget *widget) const
{
    Q_UNUSED(widget);

    painter->fillRect(option->rect, backstageCloseButton());

    if (const QStyleOptionToolButton *optButton =
                qstyleoption_cast<const QStyleOptionToolButton *>(option))
    {
        QList<QSize> sz = optButton->icon.availableSizes();
        if (!optButton->icon.isNull() && sz.count() > 0)
        {
            bool enabled = optButton->state & QStyle::State_Enabled;
            bool highlighted = optButton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = optButton->icon.pixmap(
                    sz.at(0),
                    enabled ? (highlighted ? QIcon::Selected : QIcon::Active) : QIcon::Disabled,
                    QIcon::On);
            painter->drawPixmap(0, 0, pxIcon);
        }
    }
}

bool QdfRibbonPaintManager::drawRibbonBackstageSeparator(const QStyleOption *option, QPainter *painter,
                                                         const QWidget *widget) const
{
    Q_UNUSED(widget);
    bool ret = false;
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionFrameV3 *f = qstyleoption_cast<const QStyleOptionFrameV3 *>(option))
#else
    if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(option))
#endif
    {
        painter->fillRect(option->rect, QColor(255, 255, 255));

        if (f->frameShape == QFrame::HLine)
        {
            QRect rc(QPoint(option->rect.left(), option->rect.height() / 2),
                     QSize(option->rect.width(), 1));
            painter->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        }
        else if (f->frameShape == QFrame::VLine)
        {
            QRect rc(QPoint(option->rect.width() / 2, option->rect.top()),
                     QSize(1, option->rect.height()));
            painter->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        }
    }
    return ret;
}

void QdfRibbonPaintManager::setupPalette(QWidget *widget) const
{
    if (qobject_cast<QdfRibbonKeyTip *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ToolTipText, m_clrBarText);
        palette.setColor(QPalette::Disabled, QPalette::ToolTipText, m_clrBarShadow);
        widget->setPalette(palette);
    }
}

void QdfRibbonPaintManager::modifyColors()
{
    QdfOfficePaintManager::modifyColors();
    QDF_D_STYLE(QdfRibbonStyle)
    d->m_clrRibbonFace = m_clrBarFace;
    m_clrRibbonGroupsFill = m_clrBarLight;
    d->m_clrRibbonGroupCaptionText = m_clrBarText;
    d->m_clrControlGallerySelected = QColor(255, 255, 255);
    d->m_clrControlGalleryNormal = QColor(255, 255, 255);
    d->m_clrControlGalleryBorder = m_clrBarShadow;
}

QColor QdfRibbonPaintManager::titleButtonHiliteColor(bool disabled, const QWidget *widget) const
{
    Q_UNUSED(disabled);
    Q_UNUSED(widget);
    return QColor();
}

QColor QdfRibbonPaintManager::buttonHiliteColor(const QWidget *widget, bool highlightDn) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(QdfRibbonStyle)
    return highlightDn ? m_clrHighlightDn : d->m_clrHighlight;
}

QColor QdfRibbonPaintManager::ribbonBarColor(bool isBackstageVisible) const
{
    QDF_D_STYLE(QdfRibbonStyle)
    return isBackstageVisible ? QColor(255, 255, 255) : d->m_clrRibbonFace;
}

QColor QdfRibbonPaintManager::backstageCloseButton() const
{
    QDF_D_STYLE(QdfRibbonStyle)
    return d->m_clrAccent;
}

void QdfRibbonPaintManager::drawRibbonSliderButton(const QStyleOption *option, QPainter *painter,
                                                   const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(const QdfRibbonStyle)
    bool left = option->direction != Qt::RightToLeft;

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;
    bool enabled = option->state & QStyle::State_Enabled;

    QPoint ptCenter = option->rect.center();
    int radius = 7;
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
    {
        clrLine = m_clrBarShadow;
    }
    else if (pressed && highlighted)
    {
        clrLine = d->m_clrMenubarBorder;
    }
    else if (highlighted)
    {
        clrLine = d->m_clrMenubarBorder;
    }
    else
    {
        clrLine = widget && getParentWidget<const QStatusBar>(widget) ? QColor(255, 255, 255)
                                                                      : m_clrBarText;
    }

    QPen savePen = painter->pen();
    painter->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(), ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left)
    {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    painter->drawPath(path);
    painter->setPen(savePen);
}

bool QdfRibbonPaintManager::isTopLevelMenuItemUpperCase(const QWidget *widget) const
{
    if (getParentWidget<QdfRibbonTabBar>(widget))
    {
        return true;
    }
    else if (qobject_cast<const QdfRibbonSystemButton *>(widget))
    {
        return true;
    }
    return false;
}

void QdfRibbonPaintManager::fillRibbonButton(const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    bool popuped = false;
    if (const QStyleOptionToolButton *toolbutton =
                qstyleoption_cast<const QStyleOptionToolButton *>(option))
    {
        popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) &&
                  (toolbutton->state & QStyle::State_Sunken);
    }

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;

    if (pressed || highlighted)
    {
        bool isHighlightDown = (pressed || popuped);
        QColor color = buttonHiliteColor(widget, isHighlightDown /*highlightDn*/);
        QRect rectFill = option->rect;
        painter->fillRect(rectFill, color);
    }
}

void QdfRibbonPaintManager::fillRibbonTabControl(const QStyleOption *option, QPainter *painter,
                                                 const QWidget *widget) const
{
    if (const QdfRibbonTab *tab = qobject_cast<const QdfRibbonTab *>(widget))
    {
        if (QdfContextHeader *contextHeader = tab->getContextHeader())
        {
            if (!contextHeader || !contextHeader->firstTab || !contextHeader->lastTab ||
                contextHeader->color == QdfRibbonPage::ContextColorNone)
            {
                return;
            }

            QColor color = qdf_getColorToRGB(contextHeader->color);

            QColor clrLight = QdfDrawHelpers::colorMakePale(color.rgb());

            QRect rcFirst(contextHeader->firstTab->rect());
            QRect rcLast(contextHeader->lastTab->rect());

            QRect rc(rcFirst.left(), option->rect.top(), rcLast.right() + 1, rcLast.bottom());
            painter->fillRect(rc, clrLight);
        }
    }
}

void QdfRibbonPaintManager::drawPanelBackstageButton(const QStyleOption *option, QPainter *painter,
                                                     const QWidget *widget) const
{
    Q_UNUSED(widget);
    QDF_D_STYLE(QdfRibbonStyle)
    if (const QdfStyleOptionBackstageButton *btnStyle =
                qstyleoption_cast<const QdfStyleOptionBackstageButton *>(option))
    {
        // QRect rc = btnStyle->rect;
        bool enabled = option->state & QStyle::State_Enabled;
        bool checked = option->state & QStyle::State_On;
        bool highlighted = option->state & QStyle::State_MouseOver;
        bool pressed = option->state & QStyle::State_Sunken;
        bool popuped = (btnStyle->activeSubControls & QStyle::SC_ToolButtonMenu) &&
                       (option->state & QStyle::State_Sunken);

        if (btnStyle->flatStyle && !highlighted && !pressed && !checked)
        {
            return;
        }

        QColor color(Qt::white);
        if (!enabled)
        {
            color = QColor();
        }

        else if (popuped)
        {
            color = m_clrHighlightDn;
        }
        else if (checked && !highlighted && !pressed)
        {
            color = m_clrHighlightDn;
        }
        else if (pressed)
        {
            color = m_clrHighlightDn;
        }
        else if (highlighted)
        {
            color = d->m_clrHighlight;
        }

        if (color.isValid())
        {
            painter->fillRect(option->rect, color);
        }

        if (!btnStyle->tabStyle)
        {
            const QPen oldPen = painter->pen();
            painter->setPen(QColor(170, 170, 170));
            painter->drawRect(btnStyle->rect.adjusted(0, 0, -1, -1));
            painter->setPen(oldPen);
        }
    }
}
