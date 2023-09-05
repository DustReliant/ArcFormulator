#ifndef DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_H
#define DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_H

#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonSliderPanePrivate;
class QDF_EXPORT QdfRibbonSliderPane : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged USER true)
    Q_PROPERTY(int sliderPosition READ sliderPosition WRITE setSliderPosition NOTIFY sliderMoved)
public:
    QdfRibbonSliderPane(QWidget *parent = nullptr);
    virtual ~QdfRibbonSliderPane();

public:
    void setRange(int min, int max);
    void setScrollButtons(bool on);

    void setSliderPosition(int);
    int sliderPosition() const;

    void setSingleStep(int);
    int singleStep() const;

    int value() const;

public Q_SLOTS:
    void setValue(int);

Q_SIGNALS:
    void valueChanged(int value);
    void sliderMoved(int position);

protected Q_SLOTS:
    void increment();
    void decrement();

private:
    QDF_DECLARE_PRIVATE(QdfRibbonSliderPane)
    Q_DISABLE_COPY(QdfRibbonSliderPane)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSLIDERPANE_H