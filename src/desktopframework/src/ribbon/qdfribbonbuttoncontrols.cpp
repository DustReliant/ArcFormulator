#include "private/qdfribbonbutton_p.h"
#include "private/qdfribboncontrols_p.h"
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribbonbuttoncontrols.h>
#include <ribbon/qdfribbongroup.h>

QDF_USE_NAMESPACE


QdfRibbonButtonControlPrivate::QdfRibbonButtonControlPrivate() : m_toolButton(nullptr) {}

QdfRibbonButtonControlPrivate::~QdfRibbonButtonControlPrivate() {}

void QdfRibbonButtonControlPrivate::init()
{
    QDF_Q(QdfRibbonButtonControl);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setStretchable(false);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setStretchable(false);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setStretchable(false);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setStretchable(false);

    m_toolButton = new QdfRibbonButton(q);
    q->setContentWidget(m_toolButton);
}

QdfRibbonButtonControl::QdfRibbonButtonControl(QdfRibbonGroup *parentGroup) : QdfRibbonWidgetControl(parentGroup, true)
{
    QDF_INIT_PRIVATE(QdfRibbonButtonControl);
    QDF_D(QdfRibbonButtonControl);
    d->init();
}

QdfRibbonButtonControl::~QdfRibbonButtonControl() { QDF_FINI_PRIVATE(); }

const QPixmap &QdfRibbonButtonControl::largeIcon() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_largeIcon;
}

void QdfRibbonButtonControl::setLargeIcon(const QPixmap &icon)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_largeIcon = icon;
}

const QPixmap &QdfRibbonButtonControl::smallIcon() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_smallIcon;
}

void QdfRibbonButtonControl::setSmallIcon(const QPixmap &icon)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_smallIcon = icon;
}

const QString &QdfRibbonButtonControl::label() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_label;
}

void QdfRibbonButtonControl::setLabel(const QString &label)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_label = label;
}

void QdfRibbonButtonControl::setMenu(QMenu *menu)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_toolButton->setMenu(menu);
}

QMenu *QdfRibbonButtonControl::menu() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_toolButton->menu();
}

void QdfRibbonButtonControl::setPopupMode(QToolButton::ToolButtonPopupMode mode)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_toolButton->setPopupMode(mode);
}

QToolButton::ToolButtonPopupMode QdfRibbonButtonControl::popupMode() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_toolButton->popupMode();
}

void QdfRibbonButtonControl::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    QDF_D(QdfRibbonButtonControl);
    d->m_toolButton->setToolButtonStyle(style);
    if (style == Qt::ToolButtonIconOnly)
    {
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setLabelVisible(false);
    }
    else if (style == Qt::ToolButtonTextOnly)
    {
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setImageSize(QdfRibbonControlSizeDefinition::ImageNone);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setImageSize(QdfRibbonControlSizeDefinition::ImageNone);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setImageSize(QdfRibbonControlSizeDefinition::ImageNone);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setImageSize(QdfRibbonControlSizeDefinition::ImageNone);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);
    }
    else if (style == Qt::ToolButtonTextBesideIcon)
    {
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setImageSize(QdfRibbonControlSizeDefinition::ImageSmall);
    }
}

Qt::ToolButtonStyle QdfRibbonButtonControl::toolButtonStyle() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_toolButton->toolButtonStyle();
}

QToolButton *QdfRibbonButtonControl::widget() const
{
    QDF_D(const QdfRibbonButtonControl);
    return d->m_toolButton;
}

QSize QdfRibbonButtonControl::sizeHint() const
{
    QDF_D(const QdfRibbonButtonControl);
    QSize sz = QdfRibbonWidgetControl::sizeHint();

    if (d->m_toolButton && parentGroup())
    {
        if (QdfRibbonBar *ribbonBar = parentGroup()->ribbonBar())
        {
            Qt::ToolButtonStyle style = toolButtonStyle();
            if (style == Qt::ToolButtonTextBesideIcon || style == Qt::ToolButtonIconOnly ||
                style == Qt::ToolButtonTextOnly)
            {
                sz.setHeight(ribbonBar->rowItemHeight());
            }
            else if (style == Qt::ToolButtonTextUnderIcon)
            {
                sz.setHeight(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount());
            }
        }
    }

    return sz;
}

void QdfRibbonButtonControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    QDF_D(QdfRibbonButtonControl);
    QdfRibbonControl::sizeChanged(size);

    d->m_toolButton->setDefaultAction(defaultAction());

    if (sizeDefinition(size)->imageSize() == QdfRibbonControlSizeDefinition::ImageLarge)
    {
        const QPixmap &px = largeIcon();
        d->m_toolButton->setIconSize(px.size());
        d->m_toolButton->setIcon(px);
        d->m_toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else if (sizeDefinition(size)->imageSize() == QdfRibbonControlSizeDefinition::ImageSmall)
    {
        const QPixmap &px = smallIcon();
        d->m_toolButton->setIconSize(px.size());
        d->m_toolButton->setIcon(px);
        d->m_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        if (sizeDefinition(size)->isLabelVisible())
        {
            d->m_toolButton->setText(label());
        }
        else
        {
            d->m_toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }
    }
}

void QdfRibbonButtonControl::actionChanged()
{
    const QSize bugImageSize = QSize(32, 32);
    const QSize smallImageSize = QSize(16, 16);

    if (defaultAction() != nullptr)
    {
        if (!defaultAction()->iconText().isEmpty())
        {
            setLabel(defaultAction()->iconText());
        }
        else
        {
            setLabel(defaultAction()->text());
        }

        setLargeIcon(defaultAction()->icon().pixmap(bugImageSize));
        setSmallIcon(defaultAction()->icon().pixmap(smallImageSize));
    }
    else
    {
        setLabel("Empty");
    }

    QdfRibbonControl::actionChanged();
}

QdfRibbonCheckBoxControl::QdfRibbonCheckBoxControl(const QString &text, QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true)
{
    QCheckBox *checkBox = new QCheckBox(text, this);
    setContentWidget(checkBox);
}

QdfRibbonCheckBoxControl::~QdfRibbonCheckBoxControl() {}

QCheckBox *QdfRibbonCheckBoxControl::widget() const { return qobject_cast<QCheckBox *>(contentWidget()); }

QdfRibbonRadioButtonControl::QdfRibbonRadioButtonControl(const QString &text, QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true)
{
    QRadioButton *radioButton = new QRadioButton(text, this);
    setContentWidget(radioButton);
}

QdfRibbonRadioButtonControl::~QdfRibbonRadioButtonControl() {}

QRadioButton *QdfRibbonRadioButtonControl::widget() const { return qobject_cast<QRadioButton *>(contentWidget()); }
