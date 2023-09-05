#ifndef DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_P_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonSliderButton : public QPushButton
{
public:
    QdfRibbonSliderButton(QWidget *parent,
                          QStyle::PrimitiveElement typeBut = QStyle::PE_IndicatorArrowUp);
    ~QdfRibbonSliderButton();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual QSize sizeHint() const;

protected:
    QStyle::PrimitiveElement m_typeBut;

private:
    Q_DISABLE_COPY(QdfRibbonSliderButton)
};

class QdfRibbonSliderPanePrivate : QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonSliderPane)
public:
    explicit QdfRibbonSliderPanePrivate();

public:
    void initSlider();

public:
    QSlider *m_slider;
    QdfRibbonSliderButton *m_buttonUp;
    QdfRibbonSliderButton *m_buttonDown;
    QHBoxLayout *m_layout;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_P_H