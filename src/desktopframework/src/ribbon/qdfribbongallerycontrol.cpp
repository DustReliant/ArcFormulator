#include "private/qdfribbonbar_p.h"
#include "private/qdfribbonbutton_p.h"
#include <ribbon/qdfribbongallerycontrol.h>
#include <ribbon/qdfribbongroup.h>

QDF_USE_NAMESPACE

QdfRibbonGalleryControl::QdfRibbonGalleryControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true), m_marginTop(0), m_marginBottom(0)
{
    setContentsMargins(3, 3);
    sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(6);
    sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(6);

    sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(3);
    sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(5);

    sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(2);
    sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(2);

    sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(6);
    sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(6);

    QdfRibbonGallery *gallery = new QdfRibbonGallery(this);
    gallery->setGeometry(0, 0, 0, 0);
    setContentWidget(gallery);
    widget()->updatelayout();
}

QdfRibbonGalleryControl::~QdfRibbonGalleryControl() {}

QdfRibbonGallery *QdfRibbonGalleryControl::widget() const { return qobject_cast<QdfRibbonGallery *>(contentWidget()); }

void QdfRibbonGalleryControl::setContentsMargins(int top, int bottom)
{
    m_marginTop = top;
    m_marginBottom = bottom;
}

QSize QdfRibbonGalleryControl::sizeHint() const
{
    QSize sz = QdfRibbonWidgetControl::sizeHint();

    if (widget() && parentGroup())
    {
        if (QdfRibbonBar *ribbonBar = parentGroup()->ribbonBar())
        {
            sz.setHeight(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount() - (m_marginTop + m_marginBottom));
        }

        int count = widget()->columnCount();
        if (count > 0 && widget()->itemCount() > 0)
        {
            QdfRibbonGalleryItem *item = widget()->item(0);
            QSize szItem = item->sizeHint();
            Q_ASSERT(!szItem.isNull());
            sz.setWidth(count * szItem.width() + widget()->borders().width() + 2);
        }
        else
        {
            sz.rwidth() = qMin(maximumWidth(), sz.width());
            sz.rheight() = qMin(maximumHeight(), sz.height());
            sz.rwidth() = qMax(minimumWidth(), sz.width());
            sz.rheight() = qMax(minimumHeight(), sz.height());
        }
    }
    return sz;
}

void QdfRibbonGalleryControl::adjustCurrentSize(bool expand)
{
    int min = sizeDefinition(currentSize())->minimumItemCount();
    int max = sizeDefinition(currentSize())->maximumItemCount();
    int current = widget()->columnCount();

    if (expand)
    {
        current++;
    }
    else
    {
        current--;
    }
    current = qMin(max, current);
    current = qMax(min, current);
    widget()->setColumnCount(current);
}

void QdfRibbonGalleryControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    int min = sizeDefinition(size)->minimumItemCount();
    int max = sizeDefinition(size)->maximumItemCount();

    if (size > currentSize())
    {
        widget()->setColumnCount(max);
    }
    else
    {
        widget()->setColumnCount(min);
    }

    QdfRibbonWidgetControl::sizeChanged(size);
}

void QdfRibbonGalleryControl::resizeEvent(QResizeEvent *event)
{
    QdfRibbonWidgetControl::resizeEvent(event);
    widget()->updatelayout();
}
