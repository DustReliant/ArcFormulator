#ifndef DESKTOPFRAMEWORK_QDFRIBBONINPUTCONTROLS_H
#define DESKTOPFRAMEWORK_QDFRIBBONINPUTCONTROLS_H

#include "qdfribboncontrols.h"
#include "qdfribbonsliderpane.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QFontComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTimeEdit>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfRibbonFontComboBoxControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonFontComboBoxControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonFontComboBoxControl();

public:
    QFontComboBox *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonFontComboBoxControl)
};

class QDF_EXPORT QdfRibbonLineEditControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonLineEditControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonLineEditControl();

public:
    QLineEdit *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonLineEditControl)
};

class QDF_EXPORT QdfRibbonComboBoxControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonComboBoxControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonComboBoxControl();

public:
    QComboBox *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonComboBoxControl)
};

class QDF_EXPORT QdfRibbonSpinBoxControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonSpinBoxControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonSpinBoxControl();

public:
    QSpinBox *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonSpinBoxControl)
};

class QDF_EXPORT QdfRibbonDoubleSpinBoxControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonDoubleSpinBoxControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonDoubleSpinBoxControl();

public:
    QDoubleSpinBox *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonDoubleSpinBoxControl)
};

class QDF_EXPORT QdfRibbonSliderControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonSliderControl(Qt::Orientation, QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonSliderControl();

public:
    QSlider *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonSliderControl)
};

class QDF_EXPORT QdfRibbonSliderPaneControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonSliderPaneControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonSliderPaneControl();

public:
    QdfRibbonSliderPane *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonSliderPaneControl)
};

class QDF_EXPORT QdfRibbonDateTimeEditControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonDateTimeEditControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonDateTimeEditControl();

public:
    QDateTimeEdit *widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);

private:
    Q_DISABLE_COPY(QdfRibbonDateTimeEditControl)
};

class QDF_EXPORT QdfRibbonTimeEditControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonTimeEditControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonTimeEditControl();

public:
    QTimeEdit *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonTimeEditControl)
};

class QDF_EXPORT QdfRibbonDateEditControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonDateEditControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonDateEditControl();

public:
    QDateEdit *widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);

private:
    Q_DISABLE_COPY(QdfRibbonDateEditControl)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONINPUTCONTROLS_H
