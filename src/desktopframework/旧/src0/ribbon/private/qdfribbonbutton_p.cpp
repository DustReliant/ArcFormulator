#include "qdfribbonbutton_p.h"
#include <QActionEvent>
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionToolButton>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribboncontrols.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

QDF_USE_NAMESPACE

QdfRibbonButton::QdfRibbonButton(QWidget *parent)
    : QToolButton(parent), m_saveButtonStyle(Qt::ToolButtonIconOnly), m_saveArrowType(Qt::NoArrow)
{
    setAutoRaise(true);
}

QdfRibbonButton::~QdfRibbonButton() {}

QSize QdfRibbonButton::sizeHint() const
{
    QdfRibbonButton *thisButton = (QdfRibbonButton *) this;

    if (m_saveButtonStyle != toolButtonStyle())
    {
        thisButton->m_sizeHint = QSize();
    }

    if (m_saveArrowType != arrowType())
    {
        thisButton->m_sizeHint = QSize();
    }

    if (m_sizeHint.isValid())
    {
        return m_sizeHint;
    }

    thisButton->m_saveButtonStyle = toolButtonStyle();
    thisButton->m_saveArrowType = arrowType();

    QSize sz = QToolButton::sizeHint();
    if (QdfRibbonControl *control = qobject_cast<QdfRibbonControl *>(parentWidget()))
    {
        if (QdfRibbonBar *ribbonBar = control->parentGroup()->ribbonBar())
        {
            sz.setHeight(ribbonBar->rowItemHeight());
        }
    }

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QString textButton = text();

    if (!textButton.isEmpty() &&
        (bool) style()->styleHint((QStyle::StyleHint) QdfRibbonStyle::SH_RibbonItemUpperCase, 0, this))
    {
        textButton = textButton.toUpper();
    }

    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
    {
        QSize icon = opt.iconSize;
        QAction *action = defaultAction();
        if (action && !action->icon().isNull())
        {
            icon = action->icon().actualSize(opt.iconSize);
        }
        w = icon.width();
        h = icon.height();
    }

    bool indicatorCenter = false;
    QToolButton::ToolButtonPopupMode mode = QToolButton::popupMode();

    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
    {
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
        {
            QString strFirstRow, strSecondRow;
            QdfCommonStyle::splitString(textButton, strFirstRow, strSecondRow);

            QSize textFirstSize;
            if (!strFirstRow.isEmpty())
            {
                textFirstSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);
            }

            if (!strSecondRow.isEmpty())
            {
                QSize textSecondSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strSecondRow);
                textFirstSize.setWidth(qMax(textFirstSize.width(), textSecondSize.width()));
            }

            indicatorCenter = strSecondRow.isEmpty() && opt.features & QStyleOptionToolButton::HasMenu;

            h = sz.height();

            if (QdfRibbonControl *control = qobject_cast<QdfRibbonControl *>(parentWidget()))
            {
                if (QdfRibbonBar *ribbonBar = control->parentGroup()->ribbonBar())
                {
                    h = ribbonBar->rowItemHeight() * ribbonBar->rowItemCount();
                }
            }

            // if the text is more than icon
            if (textFirstSize.width() > w)
            {
                w = textFirstSize.width();
            }
            // add pixel
            w += mode == MenuButtonPopup || mode == QToolButton::InstantPopup ? 4 : 10;
        }
        else
        {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, textButton);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
            textSize.setWidth(textSize.width() + opt.fontMetrics.width(QLatin1Char(' ')) * 2);
#else
            textSize.setWidth(textSize.width() + opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
#endif
            h = sz.height();

            if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon)
            {
                w += textSize.width() + 6;

                if (textSize.height() > h)
                {
                    h = textSize.height();
                }
            }
            else
            {
                w = textSize.width() + 4;
            }
        }
    }
    else
    {
        h = sz.height();
        w = h;
        if ((opt.features & QStyleOptionToolButton::HasMenu) || (opt.subControls & QStyle::SC_ToolButtonMenu))
        {
            w += 1;
        }
    }

    opt.rect.setSize(QSize(w, h));// PM_MenuButtonIndicator depends on the height

    if ((mode == MenuButtonPopup || mode == QToolButton::InstantPopup) && !indicatorCenter)
    {
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    }

    thisButton->m_sizeHint = QSize(w, h).expandedTo(QApplication::globalStrut());
    return m_sizeHint;
}

bool QdfRibbonButton::event(QEvent *event)
{
    bool result = QToolButton::event(event);
    if (event->type() == QEvent::LayoutRequest)
    {
        m_sizeHint = QSize();
    }
    return result;
}

void QdfRibbonButton::actionEvent(QActionEvent *event)
{
    QToolButton::actionEvent(event);
    switch (event->type())
    {
        case QEvent::ActionChanged:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionAdded:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionRemoved:
            m_sizeHint = QSize();
            break;
        default:;
    }
}

QdfRibbonTitleButton::QdfRibbonTitleButton(QWidget *parent, QStyle::SubControl subControl) : QToolButton(parent)
{
    m_subControl = subControl;
    setAttribute(Qt::WA_Hover, true);
}

QdfRibbonTitleButton::~QdfRibbonTitleButton() {}

QStyle::SubControl QdfRibbonTitleButton::subControl() const { return m_subControl; }

void QdfRibbonTitleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    QdfStyleCaptionButton opt;
    initStyleOption(&opt);
    opt.activeSubControls = m_subControl;
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
}
