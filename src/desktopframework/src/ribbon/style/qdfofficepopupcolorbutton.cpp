#include <QColorDialog>
#include <QMouseEvent>
#include <QStyleOption>
#include <QStylePainter>
#include <ribbon/qdfofficepopupcolorbutton.h>

QDF_USE_NAMESPACE

QdfPopupColorButton::QdfPopupColorButton(QWidget *parent) : QToolButton(parent)
{
    setPopupMode(MenuButtonPopup);
    m_color = Qt::color0;
}

QdfPopupColorButton::~QdfPopupColorButton() {}

const QColor &QdfPopupColorButton::color() const { return m_color; }

void QdfPopupColorButton::setColor(const QColor &color)
{
    if (color != m_color)
    {
        m_color = color;
        update();
    }
}

QSize QdfPopupColorButton::sizeHint() const { return QToolButton::sizeHint(); }

void QdfPopupColorButton::paintEvent(QPaintEvent *painter)
{
    QToolButton::paintEvent(painter);

    QStylePainter p(this);

    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    if (opt.icon.isNull())
    {
        return;
    }
    if (!m_color.isValid())
    {
        return;
    }
    QRect rect(opt.rect);
    QSize sizeIcon = opt.iconSize;

    int widthIndicator = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    rect.setHeight(sizeIcon.height() / 5);
    rect.adjust(2, 0, -(widthIndicator + 4), 0);
    rect.translate(0, opt.rect.bottom() - (rect.height() + 2));
    p.fillRect(rect, m_color);
}

void QdfPopupColorButton::mousePressEvent(QMouseEvent *e)
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (e->button() == Qt::LeftButton && (popupMode() == MenuButtonPopup))
    {
        QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
        if (popupr.isValid() && popupr.contains(e->pos()))
        {
            QColor col = QColorDialog::getColor(m_color, this);
            if (!col.isValid())
            {
                return;
            }

            if (col != m_color)
            {
                m_color = col;
                update();
                emit colorChanged(m_color);
            }
            return;
        }
    }
    QToolButton::mousePressEvent(e);
}
