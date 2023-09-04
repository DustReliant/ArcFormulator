#ifndef DESKTOPFRAMEWORK_QDFRIBBONBUTTONCONTROLS_H
#define DESKTOPFRAMEWORK_QDFRIBBONBUTTONCONTROLS_H

#include "qdfribboncontrols.h"
#include <QCheckBox>
#include <QMenu>
#include <QRadioButton>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonButtonControlPrivate;
class QDF_EXPORT QdfRibbonButtonControl : public QdfRibbonWidgetControl
{
    Q_OBJECT
public:
    explicit QdfRibbonButtonControl(QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonButtonControl();

public:
    const QPixmap &largeIcon() const;
    void setLargeIcon(const QPixmap &icon);

    const QPixmap &smallIcon() const;
    void setSmallIcon(const QPixmap &icon);

    const QString &label() const;
    void setLabel(const QString &label);

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setPopupMode(QToolButton::ToolButtonPopupMode mode);
    QToolButton::ToolButtonPopupMode popupMode() const;

    void setToolButtonStyle(Qt::ToolButtonStyle style);
    Qt::ToolButtonStyle toolButtonStyle() const;

    QToolButton *widget() const;

public:
    virtual QSize sizeHint() const;
    virtual void sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size);
    virtual void actionChanged();

private:
    QDF_DECLARE_PRIVATE(QdfRibbonButtonControl)
    Q_DISABLE_COPY(QdfRibbonButtonControl)
};

class QDF_EXPORT QdfRibbonCheckBoxControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonCheckBoxControl(const QString &text = QString(), QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonCheckBoxControl() override;

public:
    QCheckBox *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonCheckBoxControl)
};

class QDF_EXPORT QdfRibbonRadioButtonControl : public QdfRibbonWidgetControl
{
public:
    explicit QdfRibbonRadioButtonControl(const QString &text = QString(), QdfRibbonGroup *parentGroup = nullptr);
    virtual ~QdfRibbonRadioButtonControl() override;

public:
    QRadioButton *widget() const;

private:
    Q_DISABLE_COPY(QdfRibbonRadioButtonControl)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONBUTTONCONTROLS_H
