#ifndef DESKTOPFRAMEWORK_QEvent_H
#define DESKTOPFRAMEWORK_QEvent_H

#include <QColor>
#include <QDateTime>
#include <QEvent>
#include <qdf_global.h>
#include <ribbon/qdfribbongallery.h>

QDF_BEGIN_NAMESPACE

class QdfApplication;
class QdfRibbonControl;
class QDF_EXPORT QdfRibbonEvent : public QEvent
{
public:
    enum RibbonEventType
    {
        ButtonClick = 1001,
        OptionButtonClick = ButtonClick,// Trigger a click event to activate a tool
        CheckBoxToggle,
        SliderValueChange,
        FontChange,
        ColorChange,
        ComboBoxChange,
        GalleryItemClick,
        SpinBoxValueChange,
        LineEditTextChange,
        LineEditReturnPressed = LineEditTextChange,
        DateTimeEditChange,
        WindowMinimized,
        WindowMaximized,
        WindowNormalized,
        WindowFullScreen
    };

public:
    explicit QdfRibbonEvent(RibbonEventType type) : QEvent(static_cast<QEvent::Type>(type))
    {
    }
    virtual ~QdfRibbonEvent() override
    {
        control = nullptr;
    }

    QString command;
    QdfRibbonControl *control;
};

class QDF_EXPORT QdfCheckBoxToggleEvent : public QdfRibbonEvent
{
public:
    explicit QdfCheckBoxToggleEvent() : QdfRibbonEvent(CheckBoxToggle)
    {
        checked = false;
    }

    bool checked;
};

class QDF_EXPORT QdfSliderValueChangeEvent : public QdfRibbonEvent
{
public:
    explicit QdfSliderValueChangeEvent() : QdfRibbonEvent(SliderValueChange)
    {
        value = 0;
    }

    int value;
};

class QDF_EXPORT QdfFontEvent : public QdfRibbonEvent
{
public:
    explicit QdfFontEvent() : QdfRibbonEvent(FontChange)
    {
    }

    QString fontFamily;
};

class QDF_EXPORT QdfColorEvent : public QdfRibbonEvent
{
public:
    explicit QdfColorEvent() : QdfRibbonEvent(ColorChange)
    {
    }

    QColor color;
};

class QDF_EXPORT QdfComboBoxEvent : public QdfRibbonEvent
{
public:
    explicit QdfComboBoxEvent() : QdfRibbonEvent(ComboBoxChange)
    {
        index = 0;
    }

    int index;
    QString currentText;
};

class QDF_EXPORT QdfGalleryItemEvent : public QdfRibbonEvent
{
public:
    explicit QdfGalleryItemEvent() : QdfRibbonEvent(GalleryItemClick)
    {
    }

    QdfRibbonGalleryItem item;
};

class QDF_EXPORT QdfSpinBoxEvent : public QdfRibbonEvent
{
public:
    explicit QdfSpinBoxEvent() : QdfRibbonEvent(SpinBoxValueChange)
    {
        doubleControl = false;
        value = 0.0;
    }

    bool doubleControl;
    double value;
};

class QDF_EXPORT QdfLineEditEvent : public QdfRibbonEvent
{
public:
    QdfLineEditEvent() : QdfRibbonEvent(LineEditTextChange)
    {
        returnPressed = false;
    }

    bool returnPressed;
    QString text;
};

class QDF_EXPORT QdfDateTimeEvent : public QdfRibbonEvent
{
public:
    QdfDateTimeEvent() : QdfRibbonEvent(DateTimeEditChange)
    {
    }

    int controlType;
    QDateTime dateTime;
};

class QDF_EXPORT QdfWindowEvent : public QdfRibbonEvent
{
public:
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QEvent_H