#include "private/qdfelidinglabel_p.h"
#include "qdfdockwidgethelper.h"
#include <QMouseEvent>
#include <dockwidget/qdfelidinglabel.h>

QDF_USE_NAMESPACE

void QdfElidingLabelPrivate::elideText(int width)
{
    QDF_Q(QdfElidingLabel)
    if (isModeElideNone())
    {
        return;
    }
    QFontMetrics fm = q->fontMetrics();
    QString str = fm.elidedText(text, elideMode, width - q->margin() * 2 - q->indent());
    if (str == "...")
    {
        str = text.at(0);
    }
    bool wasElided = isElided;
    isElided = str != text;
    if (isElided != wasElided)
    {
        Q_EMIT q->elidedChanged(isElided);
    }
    q->QLabel::setText(str);
}


QdfElidingLabel::QdfElidingLabel(QWidget *parent, Qt::WindowFlags flag)
    : QLabel(parent, flag)
{
    QDF_INIT_PRIVATE(QdfElidingLabel);
}


QdfElidingLabel::QdfElidingLabel(const QString &text, QWidget *parent, Qt::WindowFlags flag) : QLabel(text, parent, flag)
{
    QDF_INIT_PRIVATE(QdfElidingLabel)
    QDF_D(QdfElidingLabel)
    d->text = text;
    internal::setToolTip(this, text);
}


QdfElidingLabel::~QdfElidingLabel(){
        QDF_FINI_PRIVATE()}


Qt::TextElideMode QdfElidingLabel::elideMode() const
{
    QDF_D(const QdfElidingLabel)
    return d->elideMode;
}


void QdfElidingLabel::setElideMode(Qt::TextElideMode mode)
{
    QDF_D(QdfElidingLabel)
    d->elideMode = mode;
    d->elideText(size().width());
}


bool QdfElidingLabel::isElide() const
{
    QDF_D(const QdfElidingLabel)
    return d->isElided;
}

QSize QdfElidingLabel::minimumSizeHint() const
{
    QDF_D(const QdfElidingLabel)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    bool HasPixmap = !pixmap(Qt::ReturnByValue).isNull();
#else
    bool HasPixmap = (pixmap() != nullptr);
#endif
    if (HasPixmap || d->isModeElideNone())
    {
        return QLabel::minimumSizeHint();
    }
    const QFontMetrics &fm = fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QSize size(fm.horizontalAdvance(d->text.left(2) + "..."), fm.height());
#else
    QSize size(fm.width(d->Text.left(2) + "…"), fm.height());
#endif
    return size;
}

QSize QdfElidingLabel::sizeHint() const
{
    QDF_D(const QdfElidingLabel)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    bool HasPixmap = !pixmap(Qt::ReturnByValue).isNull();
#else
    bool HasPixmap = (pixmap() != nullptr);
#endif
    if (HasPixmap || d->isModeElideNone())
    {
        return QLabel::sizeHint();
    }
    const QFontMetrics &fm = fontMetrics();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QSize size(fm.horizontalAdvance(d->text), QLabel::sizeHint().height());
#else
    QSize size(fm.width(d->text), QLabel::sizeHint().height());
#endif
    return size;
}

void QdfElidingLabel::setText(const QString &text)
{
    QDF_D(QdfElidingLabel)
    d->text = text;
    if (d->isModeElideNone())
    {
        QLabel::setText(text);
    }
    else
    {
        internal::setToolTip(this, text);
        d->elideText(this->size().width());
    }
}

QString QdfElidingLabel::text() const
{
    QDF_D(const QdfElidingLabel)
    return d->text;
}

void QdfElidingLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    Q_EMIT clicked();
}


void QdfElidingLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    QDF_D(QdfElidingLabel)
    Q_UNUSED(event)
    Q_EMIT doubleClicked();
    QLabel::mouseDoubleClickEvent(event);
}


void QdfElidingLabel::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfElidingLabel)
    if (!d->isModeElideNone())
    {
        d->elideText(event->size().width());
    }
    QLabel::resizeEvent(event);
}