#include "private/qdfribbonbutton_p.h"
#include "qdfribbon_def.h"
#include "qeffects.h"
#include <QApplication>
#include <QBitmap>
#include <QDesktopWidget>
#include <QHash>
#include <QMouseEvent>
#include <QStyleOption>
#include <QStylePainter>
#include <QTextDocument>
#include <QToolTip>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfribbontooltip.h>
#include <ribbon/qdfstylehelpers.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonToolTipPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonToolTip)
public:
    explicit QdfRibbonToolTipPrivate();

public:
    void updateTool();

public:
    int m_margin;
    int m_indent;
    int m_maxTipWidth;
    QBasicTimer m_hideTimer, m_expireTimer;
    bool m_fadingOut;
    QWidget *m_widget;
    QRect m_rect;
    QString m_text;
    QString m_textTitle;
    QIcon m_icon;
    static QdfRibbonToolTip *m_instance;
    static QdfRibbonToolTip::WrapMode m_wrapMode;
    static bool m_showIcon;
    QLabel *m_label;
};


class QdfRibbonKeyTipPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonKeyTip)
public:
    explicit QdfRibbonKeyTipPrivate();

public:
    QString strTip_;
    QString strCaption_;
    QString strPrefix_;
    QWidget *owner_;
    QAction *action_;
    QPoint pt_;
    bool enabled_;
    bool visible_;
    QWidget *bars_;
    uint align_;
    bool explicit_;
};


QdfRibbonToolTip *QdfRibbonToolTipPrivate::m_instance = nullptr;
QdfRibbonToolTip::WrapMode QdfRibbonToolTipPrivate::m_wrapMode = QdfRibbonToolTip::NoWrap;
bool QdfRibbonToolTipPrivate::m_showIcon = true;
QdfRibbonToolTipPrivate::QdfRibbonToolTipPrivate()
{
    m_margin = 0;
    m_fadingOut = false;
    m_widget = nullptr;
    m_indent = -1;
    m_maxTipWidth = 160;
    m_label = nullptr;
}

void QdfRibbonToolTipPrivate::updateTool()
{
    QDF_Q(QdfRibbonToolTip);
    if (m_label)
    {
        m_label->setText(m_text);
        m_label->setWordWrap(/*Qt::mightBeRichText(m_text)*/ true);
        m_label->adjustSize();
    }

    q->updateGeometry();
    q->update(q->contentsRect());
}

QdfRibbonToolTip::QdfRibbonToolTip(const QString &title, const QString &text, const QIcon &icon, QWidget *parent)
    : QFrame(parent, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    delete QdfRibbonToolTipPrivate::m_instance;
    QdfRibbonToolTipPrivate::m_instance = this;

    QDF_INIT_PRIVATE(QdfRibbonToolTip);
    QDF_D(QdfRibbonToolTip);

    if (d->m_label)
    {
        d->m_label->setForegroundRole(QPalette::ToolTipText);
        d->m_label->setBackgroundRole(QPalette::ToolTipBase);
        d->m_label->setPalette(QToolTip::palette());
        d->m_label->setFont(QToolTip::font());
        d->m_label->ensurePolished();
        d->m_label->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
        d->m_label->setFrameStyle(QFrame::NoFrame);
    }

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    setFont(QToolTip::font());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
    setMouseTracking(true);
    d->m_fadingOut = false;
    setIcon(isIconVisible() ? icon : QIcon());
    reuseTip(title, text);
}

QdfRibbonToolTip::~QdfRibbonToolTip()
{
    QdfRibbonToolTipPrivate::m_instance = nullptr;
    QDF_FINI_PRIVATE();
}

QdfRibbonToolTip *QdfRibbonToolTip::instance() { return QdfRibbonToolTipPrivate::m_instance; }

void QdfRibbonToolTip::showToolTip(const QPoint &pos, const QString &title, const QString &text, const QIcon &icon,
                                   QWidget *w)
{
    QRect rect;
    // a tip does already exist
    if (QdfRibbonToolTipPrivate::m_instance && QdfRibbonToolTipPrivate::m_instance->isVisible())
    {
        // empty text means hide current tip
        if (text.isEmpty())
        {
            QdfRibbonToolTipPrivate::m_instance->hideTip();
            return;
        }
        else if (!QdfRibbonToolTipPrivate::m_instance->fadingOut())
        {
            // If the tip has changed, reuse the one
            // that is showing (removes flickering)
            QPoint localPos = pos;
            if (w)
            {
                localPos = w->mapFromGlobal(pos);
            }
            if (QdfRibbonToolTipPrivate::m_instance->tipChanged(localPos, text, title, w))
            {
                QdfRibbonToolTipPrivate::m_instance->setIcon(isIconVisible() ? icon : QIcon());
                QdfRibbonToolTipPrivate::m_instance->reuseTip(title, text);
                QdfRibbonToolTipPrivate::m_instance->setTipRect(w, rect);
                QdfRibbonToolTipPrivate::m_instance->placeTip(pos, w);
            }
            return;
        }
    }
    // no tip can be reused, create new tip:
    if (!text.isEmpty())
    {
#ifndef Q_WS_WIN
        new QdfRibbonToolTip(title, text, icon, w);// sets TipLabel::m_pInstanceto itself
#else
        // On windows, we can't use the widget as parent otherwise the window will be
        // raised when the tooltip will be shown
        new QdfRibbonToolTip(title, text, icon, QApplication::desktop()->screen(QdfRibbonToolTip::getTipScreen(pos, w)));
#endif
        QdfRibbonToolTipPrivate::m_instance->setTipRect(w, rect);
        QdfRibbonToolTipPrivate::m_instance->placeTip(pos, w);
        QdfRibbonToolTipPrivate::m_instance->setObjectName("qtctooltip_label");

#if !defined(QT_NO_EFFECTS) && !defined(Q_WS_MAC)
        if (QApplication::isEffectEnabled(Qt::UI_FadeTooltip))
        {
            qFadeEffect(QdfRibbonToolTipPrivate::m_instance);
        }
        else if (QApplication::isEffectEnabled(Qt::UI_AnimateTooltip))
        {
            qScrollEffect(QdfRibbonToolTipPrivate::m_instance);
        }
        else
        {
            QdfRibbonToolTipPrivate::m_instance->show();
        }
#else
        QdfRibbonToolTipPrivate::m_instance->show();
#endif
    }
}

void QdfRibbonToolTip::hideToolTip() { showToolTip(QPoint(), QString(), QString(), QIcon()); }

bool QdfRibbonToolTip::isToolTipVisible()
{
    return (QdfRibbonToolTipPrivate::m_instance != 0 && QdfRibbonToolTipPrivate::m_instance->isVisible());
}

void QdfRibbonToolTip::setWrapMode(WrapMode mode) { QdfRibbonToolTipPrivate::m_wrapMode = mode; }

QdfRibbonToolTip::WrapMode QdfRibbonToolTip::wordWrap() { return QdfRibbonToolTipPrivate::m_wrapMode; }

void QdfRibbonToolTip::setIconVisible(bool visible) { QdfRibbonToolTipPrivate::m_showIcon = visible; }

bool QdfRibbonToolTip::isIconVisible() { return QdfRibbonToolTipPrivate::m_showIcon; }

int QdfRibbonToolTip::margin() const
{
    QDF_D(const QdfRibbonToolTip);
    return d->m_margin;
}

void QdfRibbonToolTip::setMargin(int margin)
{
    QDF_D(QdfRibbonToolTip);
    if (d->m_margin == margin)
    {
        return;
    }
    d->m_margin = margin;
    d->updateTool();
}

void QdfRibbonToolTip::setTitle(const QString &text)
{
    QDF_D(QdfRibbonToolTip);
    if (d->m_textTitle == text)
    {
        return;
    }
    d->m_textTitle = text;
    d->updateTool();
}

const QString &QdfRibbonToolTip::title() const
{
    QDF_D(const QdfRibbonToolTip);
    return d->m_textTitle;
}

void QdfRibbonToolTip::setText(const QString &text)
{
    QDF_D(QdfRibbonToolTip);
    if (d->m_text == text)
    {
        return;
    }
    d->m_text = text;
    d->updateTool();
}

const QString &QdfRibbonToolTip::text() const
{
    QDF_D(const QdfRibbonToolTip);
    return d->m_text;
}

void QdfRibbonToolTip::setIcon(const QIcon &icon)
{
    QDF_D(QdfRibbonToolTip);
    d->m_icon = icon;
    d->updateTool();
}

const QIcon &QdfRibbonToolTip::icon() const
{
    QDF_D(const QdfRibbonToolTip);
    return d->m_icon;
}

void QdfRibbonToolTip::restartExpireTimer()
{
    QDF_D(QdfRibbonToolTip);
    int time = 10000 + 40 * qMax(0, text().length() - 100);
    d->m_expireTimer.start(time, this);
    d->m_hideTimer.stop();
}

void QdfRibbonToolTip::reuseTip(const QString &textTitle, const QString &text)
{
    setText(text);
    setTitle(textTitle);
    QFontMetrics fm(font());
    QSize extra(1, 0);
    if (fm.descent() == 2 && fm.ascent() >= 11)
    {
        ++extra.rheight();
    }

    resize(sizeHint() + extra);
    restartExpireTimer();
}

QSize QdfRibbonToolTip::sizeHint() const
{
    QDF_D(const QdfRibbonToolTip);

    QFontMetrics fm = fontMetrics();

    QRect rcMargin(margin(), margin(), margin(), margin());
    QSize szMargin(3 + rcMargin.left() + rcMargin.right() + 3, 3 + rcMargin.top() + rcMargin.bottom() + 3);

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d->m_textTitle.isEmpty();
    bool drawImage = !d->m_icon.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);
    QSize szTitle(0, 0);

    if (drawTitle)
    {
        QFont fontBold = font();
        fontBold.setBold(true);
        QFontMetrics fmBold(fontBold);
        int w = fmBold.averageCharWidth() * 80;
        QRect rcTitle = fmBold.boundingRect(0, 0, w, 2000, flags | Qt::TextSingleLine, d->m_textTitle);
        szTitle = QSize(rcTitle.width(), rcTitle.height() + 15);
    }

    if (drawImage)
    {
        QSize szIcon(0, 0);
        if (!d->m_icon.availableSizes().isEmpty())
        {
            szIcon = d->m_icon.availableSizes().first();
        }

        drawImageTop = (szIcon.height() <= 16);

        if (drawImageTop)
        {
            if (!drawTitle)
            {
                szImage.setWidth(szIcon.height() + 3);
            }
            else
            {
                szTitle.setWidth(szTitle.width() + szIcon.width() + 1);
            }
        }
        else
        {
            szImage.setWidth(szIcon.width() + 5);
        }
        szImage.setHeight(szIcon.height());
    }

    QSize szText;
    if (!d->m_label)
    {
        int nMaxTipWidth = d->m_maxTipWidth;
        QRect rcText(QPoint(0, 0), QSize(nMaxTipWidth - szMargin.width(), 0));
        rcText = fm.boundingRect(rcText.left(), rcText.top(), rcText.width(), rcText.height(), flags | Qt::TextWordWrap,
                                 d->m_text);
        szText = rcText.size();
    }
    else
    {
        szText = d->m_label->sizeHint();
    }

    QSize sz(0, 0);
    sz.setHeight(qMax(szImage.height(), szText.height()));
    sz.setWidth(szImage.width() + szText.width());

    if (drawTitle)
    {
        sz.setWidth(qMax(sz.width(), szTitle.width()));
        sz.setHeight(sz.height() + szTitle.height());
    }

    sz += szMargin;
    return sz;
}

void QdfRibbonToolTip::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfRibbonToolTip);

    QStylePainter p(this);
    drawFrame(&p);

    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);

    QRect rc = contentsRect();
    rc.adjust(margin(), margin(), -margin(), -margin());
    rc.adjust(3, 3, -3, -3);

    QRect rcTitle(rc.left() + 2, rc.top() + 2, rc.right() - 2, rc.bottom());

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d->m_textTitle.isEmpty();
    bool drawImage = !d->m_icon.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);

    if (drawImage)
    {
        if (!d->m_icon.availableSizes().isEmpty())
        {
            szImage = d->m_icon.availableSizes().first();
        }

        drawImageTop = (szImage.height() <= 16);

        if (drawImageTop)
        {
            QPoint ptIcon = rc.topLeft();
            QPixmap pm = d->m_icon.pixmap(szImage, QIcon::Normal, QIcon::On);
            p.drawPixmap(ptIcon, pm);

            if (drawTitle)
            {
                rcTitle.setLeft(rcTitle.left() + szImage.width() + 1);
            }
            else
            {
                rc.setLeft(rc.left() + szImage.width() + 3);
            }
        }
    }
    if (drawTitle)
    {
        QFont oldFont = p.font();
        QFont fnt = oldFont;
        fnt.setBold(true);
        p.setFont(fnt);
        style()->drawItemText(&p, rcTitle, flags | Qt::TextSingleLine, opt.palette, isEnabled(), d->m_textTitle,
                              QPalette::ToolTipText);
        p.setFont(oldFont);

        rc.setTop(rc.top() + p.boundingRect(rcTitle, flags | Qt::TextSingleLine, d->m_textTitle).height());
        rc.adjust(0, 12, 0, 0);
    }

    if (drawImage && !drawImageTop)
    {
        QPoint ptIcon = rc.topLeft();
        QPixmap pm = d->m_icon.pixmap(szImage, QIcon::Normal, QIcon::On);
        p.drawPixmap(ptIcon, pm);
        rc.setLeft(rc.left() + szImage.width() + 4);
    }

    if (d->m_label)
    {
        d->m_label->move(rc.topLeft());
    }
    else
    {
        style()->drawItemText(&p, rc, flags | Qt::TextWordWrap, opt.palette, isEnabled(), d->m_text,
                              QPalette::ToolTipText);
    }
    p.end();
}

void QdfRibbonToolTip::resizeEvent(QResizeEvent *event)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
    {
        setMask(frameMask.region);
    }

    QFrame::resizeEvent(event);
}

void QdfRibbonToolTip::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfRibbonToolTip);
    if (d->m_rect.isNull())
    {
        return;
    }
    QPoint pos = event->globalPos();
    if (d->m_widget)
    {
        pos = d->m_widget->mapFromGlobal(pos);
    }
    if (!d->m_rect.contains(pos))
    {
        hideTip();
    }
    QFrame::mouseMoveEvent(event);
}

void QdfRibbonToolTip::hideTip()
{
    QDF_D(QdfRibbonToolTip);
    if (!d->m_hideTimer.isActive())
    {
        d->m_hideTimer.start(300, this);
    }
}

void QdfRibbonToolTip::closeToolTip()
{
    close();
    deleteLater();
}

void QdfRibbonToolTip::setTipRect(QWidget *w, const QRect &r)
{
    QDF_D(QdfRibbonToolTip);
    if (!d->m_rect.isNull() && !w)
    {
        qWarning("QdfRibbonToolTip::setTipRect: Cannot pass null widget if rect is set");
    }
    else
    {
        d->m_widget = w;
        d->m_rect = r;
    }
}

void QdfRibbonToolTip::timerEvent(QTimerEvent *event)
{
    QDF_D(QdfRibbonToolTip);
    if (event->timerId() == d->m_hideTimer.timerId() || event->timerId() == d->m_expireTimer.timerId())
    {
        d->m_hideTimer.stop();
        d->m_expireTimer.stop();
        closeToolTip();
    }
}

bool QdfRibbonToolTip::eventFilter(QObject *o, QEvent *event)
{
    QDF_D(QdfRibbonToolTip);
    switch (event->type())
    {
        case QEvent::Leave:
            hideTip();
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
            closeToolTip();
            break;
        case QEvent::MouseMove:
            if (o == d->m_widget && !d->m_rect.isNull() && !d->m_rect.contains(static_cast<QMouseEvent *>(event)->pos()))
            {
                hideTip();
            }
        default:
            break;
    }
    return false;
}

int QdfRibbonToolTip::getTipScreen(const QPoint &pos, QWidget *w)
{
    if (QApplication::desktop()->isVirtualDesktop())
    {
        return QApplication::desktop()->screenNumber(pos);
    }
    else
    {
        return QApplication::desktop()->screenNumber(w);
    }
}

void QdfRibbonToolTip::placeTip(const QPoint &pos, QWidget *w)
{
    QRect screen = QApplication::desktop()->screenGeometry(getTipScreen(pos, w));

    QPoint p = pos;
    if (p.x() + this->width() > screen.x() + screen.width())
    {
        p.rx() -= 4 + this->width();
    }
    if (p.y() + this->height() > screen.y() + screen.height())
    {
        p.ry() -= 24 + this->height();
    }
    if (p.y() < screen.y())
    {
        p.setY(screen.y());
    }
    if (p.x() + this->width() > screen.x() + screen.width())
    {
        p.setX(screen.x() + screen.width() - this->width());
    }
    if (p.x() < screen.x())
    {
        p.setX(screen.x());
    }
    if (p.y() + this->height() > screen.y() + screen.height())
    {
        p.setY(screen.y() + screen.height() - this->height());
    }

    this->move(p);
}

bool QdfRibbonToolTip::tipChanged(const QPoint &pos, const QString &text, const QString &textTitle, QObject *o)
{
    QDF_D(QdfRibbonToolTip);
    if (QdfRibbonToolTipPrivate::m_instance->text() != text)
    {
        return true;
    }

    if (QdfRibbonToolTipPrivate::m_instance->title() != textTitle)
    {
        return true;
    }

    if (o != d->m_widget)
    {
        return true;
    }

    if (!d->m_rect.isNull())
    {
        return !d->m_rect.contains(pos);
    }
    else
    {
        return false;
    }
}

bool QdfRibbonToolTip::fadingOut() const
{
    QDF_D(const QdfRibbonToolTip);
    return d->m_fadingOut;
}

QdfRibbonKeyTipPrivate::QdfRibbonKeyTipPrivate()
{
    owner_ = nullptr;
    action_ = nullptr;
    bars_ = nullptr;
    enabled_ = false;
    align_ = 0;
    visible_ = true;
    explicit_ = false;
}


QdfRibbonKeyTip::QdfRibbonKeyTip(QWidget *bars, QWidget *owner, const QString &caption, const QPoint &pt, uint align,
                                 bool enabled, QAction *action)
    : QFrame(bars, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    QDF_INIT_PRIVATE(QdfRibbonKeyTip);
    QDF_D(QdfRibbonKeyTip);
    d->action_ = action;
    d->owner_ = owner;
    d->strCaption_ = caption;
    d->bars_ = bars;
    d->pt_ = pt;
    d->enabled_ = enabled;
    d->align_ = align;

    setEnabled(enabled);

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    QPalette pal = QToolTip::palette();
    pal.setColor(QPalette::Light, pal.color(QPalette::ToolTipText));

    setPalette(pal);
    setFont(QToolTip::font());
    ensurePolished();

    if (!enabled)
    {
        setWindowOpacity(0.5);
    }
}

QdfRibbonKeyTip::~QdfRibbonKeyTip() { QDF_FINI_PRIVATE(); }

QPoint QdfRibbonKeyTip::posTip() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->pt_;
}

void QdfRibbonKeyTip::setVisibleTip(bool visible)
{
    QDF_D(QdfRibbonKeyTip);
    d->visible_ = visible;
}

bool QdfRibbonKeyTip::isVisibleTip() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->visible_;
}

bool QdfRibbonKeyTip::isEnabledTip() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->enabled_;
}

QWidget *QdfRibbonKeyTip::getBars() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->bars_;
}

uint QdfRibbonKeyTip::getAlign() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->align_;
}

void QdfRibbonKeyTip::setExplicit(bool exp)
{
    QDF_D(QdfRibbonKeyTip);
    d->explicit_ = exp;
}

bool QdfRibbonKeyTip::isExplicit() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->explicit_;
}

QString QdfRibbonKeyTip::getStringTip() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->strTip_;
}

void QdfRibbonKeyTip::setStringTip(const QString &str)
{
    QDF_D(QdfRibbonKeyTip);
    d->strTip_ = str;
}

QString QdfRibbonKeyTip::getCaption() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->strCaption_;
}

void QdfRibbonKeyTip::setCaption(const QString &str)
{
    QDF_D(QdfRibbonKeyTip);
    d->strCaption_ = str;
}

QString QdfRibbonKeyTip::getPrefix() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->strPrefix_;
}

void QdfRibbonKeyTip::setPrefix(const QString &pref)
{
    QDF_D(QdfRibbonKeyTip);
    d->strPrefix_ = pref;
}

QWidget *QdfRibbonKeyTip::getOwner() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->owner_;
}

QAction *QdfRibbonKeyTip::getAction() const
{
    QDF_D(const QdfRibbonKeyTip);
    return d->action_;
}

void QdfRibbonKeyTip::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfRibbonKeyTip);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    if (d->enabled_)
    {
        opt.state |= QStyle::State_Enabled;
    }
    else
    {
        opt.state &= ~QStyle::State_Enabled;
    }

    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonKeyTip, &opt, &p, this);
    style()->drawItemText(&p, opt.rect, Qt::AlignVCenter | Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap,
                          opt.palette, d->enabled_, d->strTip_, QPalette::ToolTipText);
}

void QdfRibbonKeyTip::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QBitmap maskBitmap(width(), height());
    maskBitmap.clear();

    QPainter p(&maskBitmap);
    p.setBrush(Qt::black);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    p.drawRoundedRect(0, 0, width(), height(), 2, 2, Qt::AbsoluteSize);
#else
    p.drawRoundedRect(0, 0, width() - 1, height() - 1, 2, 2, Qt::AbsoluteSize);
#endif
    p.end();
    setMask(maskBitmap);
}


QDF_END_NAMESPACE