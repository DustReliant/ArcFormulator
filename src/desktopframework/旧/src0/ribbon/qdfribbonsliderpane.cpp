#include "private/qdfribbonsliderpane_p.h"
#include <QBasicTimer>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <ribbon/qdfribbonsliderpane.h>
#include <ribbon/qdfribbonstyle.h>

QDF_USE_NAMESPACE

QdfRibbonSliderButton::QdfRibbonSliderButton(QWidget *parent, QStyle::PrimitiveElement typeBut) : QPushButton(parent)
{
    m_typeBut = typeBut;
}

QdfRibbonSliderButton::~QdfRibbonSliderButton() {}

void QdfRibbonSliderButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonSliderButton, &option, &painter, this);
}

QSize QdfRibbonSliderButton::sizeHint() const
{
    QSize sz(12, 12);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    sz = style()->sizeFromContents((QStyle::ContentsType) QdfRibbonStyle::CT_RibbonSliderButton, &option, sz, this);
    return sz;
}

/* QdfRibbonSliderPanePrivate */
QdfRibbonSliderPanePrivate::QdfRibbonSliderPanePrivate()
    : m_slider(nullptr), m_buttonUp(nullptr), m_buttonDown(nullptr), m_layout(nullptr)
{
}

void QdfRibbonSliderPanePrivate::initSlider()
{
    QDF_Q(QdfRibbonSliderPane);
    m_slider = new QSlider(Qt::Horizontal, q);
    m_slider->setTickPosition(QSlider::NoTicks);

    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(4);

    m_layout->addWidget(m_slider);
    q->setLayout(m_layout);
}

QdfRibbonSliderPane::QdfRibbonSliderPane(QWidget *parent) : QWidget(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonSliderPane);
    QDF_D(QdfRibbonSliderPane);
    d->initSlider();
    connect(d->m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(d->m_slider, SIGNAL(sliderMoved(int)), this, SIGNAL(sliderMoved(int)));
}

QdfRibbonSliderPane::~QdfRibbonSliderPane() { QDF_FINI_PRIVATE(); }

void QdfRibbonSliderPane::setRange(int min, int max)
{
    QDF_D(QdfRibbonSliderPane);
    d->m_slider->setMinimum(min);
    d->m_slider->setMaximum(max);
}

void QdfRibbonSliderPane::setScrollButtons(bool on)
{
    QDF_D(QdfRibbonSliderPane);
    if (on)
    {
        d->m_buttonDown = new QdfRibbonSliderButton(this, QStyle::PE_IndicatorArrowDown);
        d->m_layout->insertWidget(0, d->m_buttonDown, 0, Qt::AlignLeft);
        d->m_buttonUp = new QdfRibbonSliderButton(this, QStyle::PE_IndicatorArrowUp);
        d->m_layout->addWidget(d->m_buttonUp, 0, Qt::AlignRight);
        connect(d->m_buttonDown, SIGNAL(pressed()), this, SLOT(decrement()));
        connect(d->m_buttonUp, SIGNAL(pressed()), this, SLOT(increment()));
    }
    else
    {
        disconnect(d->m_buttonUp, SIGNAL(pressed()), this, SLOT(increment()));
        disconnect(d->m_buttonDown, SIGNAL(pressed()), this, SLOT(decrement()));
        delete d->m_buttonUp;
        d->m_buttonUp = NULL;
        delete d->m_buttonDown;
        d->m_buttonDown = NULL;
    }
}

void QdfRibbonSliderPane::setSliderPosition(int position)
{
    QDF_D(QdfRibbonSliderPane);
    d->m_slider->setSliderPosition(position);
}

int QdfRibbonSliderPane::sliderPosition() const
{
    QDF_D(const QdfRibbonSliderPane);
    return d->m_slider->sliderPosition();
}

void QdfRibbonSliderPane::setSingleStep(int nPos)
{
    QDF_D(QdfRibbonSliderPane);
    d->m_slider->setSingleStep(nPos);
}

int QdfRibbonSliderPane::singleStep() const
{
    QDF_D(const QdfRibbonSliderPane);
    return d->m_slider->singleStep();
}

int QdfRibbonSliderPane::value() const
{
    QDF_D(const QdfRibbonSliderPane);
    return d->m_slider->value();
}

void QdfRibbonSliderPane::setValue(int value)
{
    QDF_D(QdfRibbonSliderPane);
    d->m_slider->setValue(value);
}

void QdfRibbonSliderPane::increment()
{
    QDF_D(QdfRibbonSliderPane);
    int nPos = d->m_slider->sliderPosition() + d->m_slider->singleStep();
    if (nPos <= d->m_slider->maximum())
    {
        d->m_slider->setSliderPosition(nPos);
    }
}

void QdfRibbonSliderPane::decrement()
{
    QDF_D(QdfRibbonSliderPane);
    int nPos = d->m_slider->sliderPosition() - d->m_slider->singleStep();
    if (nPos >= d->m_slider->minimum())
    {
        d->m_slider->setSliderPosition(nPos);
    }
}
