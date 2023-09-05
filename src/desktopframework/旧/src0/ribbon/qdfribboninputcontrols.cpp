#include "private/qdfribbongroup_p.h"
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribboninputcontrols.h>

QDF_USE_NAMESPACE

QdfRibbonFontComboBoxControl::QdfRibbonFontComboBoxControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QFontComboBox(this));
}

QdfRibbonFontComboBoxControl::~QdfRibbonFontComboBoxControl() {}

QFontComboBox *QdfRibbonFontComboBoxControl::widget() const { return qobject_cast<QFontComboBox *>(contentWidget()); }

QdfRibbonLineEditControl::QdfRibbonLineEditControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QLineEdit(this));
}

QdfRibbonLineEditControl::~QdfRibbonLineEditControl() {}

QLineEdit *QdfRibbonLineEditControl::widget() const { return qobject_cast<QLineEdit *>(contentWidget()); }

QdfRibbonComboBoxControl::QdfRibbonComboBoxControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QComboBox(this));
}

QdfRibbonComboBoxControl::~QdfRibbonComboBoxControl() {}

QComboBox *QdfRibbonComboBoxControl::widget() const { return qobject_cast<QComboBox *>(contentWidget()); }

QdfRibbonSpinBoxControl::QdfRibbonSpinBoxControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QSpinBox(this));
}

QdfRibbonSpinBoxControl::~QdfRibbonSpinBoxControl() {}

QSpinBox *QdfRibbonSpinBoxControl::widget() const { return qobject_cast<QSpinBox *>(contentWidget()); }

QdfRibbonDoubleSpinBoxControl::QdfRibbonDoubleSpinBoxControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QDoubleSpinBox(this));
}

QdfRibbonDoubleSpinBoxControl::~QdfRibbonDoubleSpinBoxControl() {}

QDoubleSpinBox *QdfRibbonDoubleSpinBoxControl::widget() const { return qobject_cast<QDoubleSpinBox *>(contentWidget()); }

QdfRibbonSliderControl::QdfRibbonSliderControl(Qt::Orientation orientation, QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true)
{
    setContentWidget(new QSlider(orientation, this));
}

QdfRibbonSliderControl::~QdfRibbonSliderControl() {}

QSlider *QdfRibbonSliderControl::widget() const { return qobject_cast<QSlider *>(contentWidget()); }

QdfRibbonSliderPaneControl::QdfRibbonSliderPaneControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true)
{
    setContentWidget(new QdfRibbonSliderPane(this));
}

QdfRibbonSliderPaneControl::~QdfRibbonSliderPaneControl() {}

QdfRibbonSliderPane *QdfRibbonSliderPaneControl::widget() const { return qobject_cast<QdfRibbonSliderPane *>(contentWidget()); }

QdfRibbonDateTimeEditControl::QdfRibbonDateTimeEditControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QDateTimeEdit(this));
}

QdfRibbonDateTimeEditControl::~QdfRibbonDateTimeEditControl() {}

QDateTimeEdit *QdfRibbonDateTimeEditControl::widget() const { return qobject_cast<QDateTimeEdit *>(contentWidget()); }

bool QdfRibbonDateTimeEditControl::calendarPopup() const
{
    QDateTimeEdit *dateTimeEdit = widget();
    Q_ASSERT(dateTimeEdit != nullptr);
    return dateTimeEdit->calendarPopup();
}

void QdfRibbonDateTimeEditControl::setCalendarPopup(bool enable)
{
    QDateTimeEdit *dateTimeEdit = widget();
    Q_ASSERT(dateTimeEdit != nullptr);
    dateTimeEdit->setCalendarPopup(enable);
}

QdfRibbonTimeEditControl::QdfRibbonTimeEditControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QTimeEdit(this));
}

QdfRibbonTimeEditControl::~QdfRibbonTimeEditControl() {}

QTimeEdit *QdfRibbonTimeEditControl::widget() const { return qobject_cast<QTimeEdit *>(contentWidget()); }

QdfRibbonDateEditControl::QdfRibbonDateEditControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, false)
{
    setContentWidget(new QDateEdit(this));
}

QdfRibbonDateEditControl::~QdfRibbonDateEditControl() {}

QDateEdit *QdfRibbonDateEditControl::widget() const { return qobject_cast<QDateEdit *>(contentWidget()); }

bool QdfRibbonDateEditControl::calendarPopup() const
{
    QDateEdit *dateEdit = widget();
    Q_ASSERT(dateEdit != nullptr);
    return dateEdit->calendarPopup();
}

void QdfRibbonDateEditControl::setCalendarPopup(bool enable)
{
    QDateEdit *dateEdit = widget();
    Q_ASSERT(dateEdit != nullptr);
    dateEdit->setCalendarPopup(enable);
}
