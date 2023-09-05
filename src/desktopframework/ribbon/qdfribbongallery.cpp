#include "private/qdfribbongallery_p.h"
#include "qdfribbon_def.h"
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QDesktopWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QToolButton>
#include <QToolTip>
#include <QWidgetAction>
#include <qmath.h>
#include <qstyleoption.h>
#include <ribbon/qdfofficepopupmenu.h>
#include <ribbon/qdfribbongallery.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

QDF_USE_NAMESPACE


QdfRibbonGalleryItemPrivate::QdfRibbonGalleryItemPrivate()
{
    m_index = -1;
    m_separator = false;
    m_enabled = true;
    m_visible = true;
}

void QdfRibbonGalleryItemPrivate::init()
{
    QDF_Q(QdfRibbonGalleryItem);
    q->setSizeHint(QSize(0, 0));
}

QdfRibbonGalleryGroupPrivate::QdfRibbonGalleryGroupPrivate()
{
    m_sizeItem = QSize(0, 0);
    m_clipItems = true;
}

QdfRibbonGalleryGroupPrivate::~QdfRibbonGalleryGroupPrivate()
{
    for (int i = 0, count = m_viewWidgets.size(); count > i; ++i)
    {
        if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(m_viewWidgets.at(i)))
        {
            gallery->qdf_d()->m_items = nullptr;
        }
    }
}

void QdfRibbonGalleryGroupPrivate::init()
{
}

void QdfRibbonGalleryGroupPrivate::updateIndexes(int start /*= 0*/)
{
    QDF_Q(QdfRibbonGalleryGroup);
    for (int i = start; i < m_arrItems.size(); ++i)
    {
        q->item(i)->qdf_d()->m_index = i;
    }
}

void QdfRibbonGalleryGroupPrivate::itemsChanged()
{
    for (int i = 0; i < m_viewWidgets.size(); ++i)
    {
        if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(m_viewWidgets.at(i)))
        {
            gallery->qdf_d()->m_checkedItem = nullptr;
            gallery->qdf_d()->m_selected = -1;
            gallery->qdf_d()->m_scrollPos = 0;
            gallery->qdf_d()->layoutItems();

            if (gallery->qdf_d()->m_preview)
            {
                gallery->qdf_d()->m_preview = false;
            }
            gallery->update();
        }
    }
}

void QdfRibbonGalleryGroupPrivate::redrawWidget()
{
    for (int i = 0, count = m_viewWidgets.size(); count > i; ++i)
    {
        m_viewWidgets.at(i)->update();
    }
}

void QdfRibbonGalleryGroupPrivate::clear()
{
    QdfRibbonGalleryItem *item = nullptr;
    foreach (item, m_arrItems)
    {
        delete item;
    }
    m_arrItems.clear();
}

QdfRibbonGalleryPrivate::QdfRibbonGalleryPrivate()
{
    m_showBorders = false;
    m_showLabels = true;
    m_hideSelection = false;
    m_keyboardSelected = false;
    m_preview = false;
    m_pressed = false;
    m_animation = false;
    m_autoWidth = false;
    m_scrollPos = 0;
    m_scrollPosTarget = 0;
    m_totalHeight = 0;
    m_selected = -1;
    m_checkedItem = nullptr;
    m_items = nullptr;
    m_ptPressed = QPoint(0, 0);
    m_scrollBar = nullptr;
    m_menuBar = nullptr;
    m_buttonPopup = nullptr;
    m_buttonScrollUp = nullptr;
    m_buttonScrollDown = nullptr;
    m_animationStep = 0.0;
    m_timerElapse = 0;
    m_minimumColumnCount = -1;
    m_maximumColumnCount = -1;
    m_currentColumnCount = -1;
}

QdfRibbonGalleryPrivate::~QdfRibbonGalleryPrivate()
{
    QDF_Q(QdfRibbonGallery);
    if (m_items && m_items->qdf_d()->m_viewWidgets.size() > 0)
    {
        m_items->qdf_d()->m_viewWidgets.removeOne(q);
    }
}

void QdfRibbonGalleryPrivate::init()
{
    QDF_Q(QdfRibbonGallery);
    q->setAttribute(Qt::WA_MouseTracking);
    q->setObjectName("QdfRibbonGallery");
    q->setProperty(_qdf_WidgetGallery, true);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void QdfRibbonGalleryPrivate::layoutItems()
{
    QDF_Q(QdfRibbonGallery);

    if (q->isShowAsButton())
    {
        return;
    }

    QRect rcItems = q->getItemsRect();

    if (q->autoWidth() && q->itemCount() > 0 && m_currentColumnCount != -1)
    {
        QdfRibbonGalleryItem *item = q->item(0);
        int width = m_currentColumnCount * item->sizeHint().width() + q->borders().width();
        rcItems.setWidth(width);
    }

    int x = rcItems.left();
    int y = rcItems.top();
    bool firstItem = true;

    int count = q->itemCount();

    m_arrRects.resize(count);
    int rowHeight = 0;

    for (int i = 0; i < count; ++i)
    {
        QdfRibbonGalleryItem *item = q->item(i);
        m_arrRects[i].item = item;
        m_arrRects[i].beginRow = false;

        if (!item->isVisible())
        {
            m_arrRects[i].rect = QRect(QPoint(0, 0), QPoint(0, 0));
            continue;
        }

        if (item->isSeparator())
        {
            if (!m_showLabels)
            {
                m_arrRects[i].rect = QRect(QPoint(0, 0), QPoint(0, 0));
                continue;
            }
            QPainter pp(q);
            QFontMetrics qfm(pp.font());
            const int marginH = (q->style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1) * 2;
            const int heightLabel = qfm.height() + marginH;

            if (!firstItem)
            {
                y += rowHeight;
            }

            QRect rcItem(QPoint(rcItems.left(), y), QPoint(rcItems.right(), y + heightLabel));
            m_arrRects[i].rect = rcItem;
            m_arrRects[i].beginRow = true;

            y += heightLabel;
            rowHeight = 0;
            x = rcItems.left();
            firstItem = true;
        }
        else
        {
            QSize szItem = item->sizeHint();
            if (szItem.width() == 0)
            {
                szItem.setWidth(rcItems.width());
            }

            if (firstItem)
            {
                m_arrRects[i].beginRow = true;
            }

            if (!firstItem && x + szItem.width() > rcItems.right())
            {
                y += rowHeight;
                x = rcItems.left();
                m_arrRects[i].beginRow = true;
                rowHeight = 0;
            }

            QRect rcItem(QPoint(x, y), szItem);
            m_arrRects[i].rect = rcItem;

            rowHeight = qMax(rowHeight, szItem.height());

            firstItem = false;
            x += szItem.width();
        }
    }

    if (!firstItem)
    {
        y += rowHeight;
    }

    m_totalHeight = y - rcItems.top();

    if (m_scrollPos > m_totalHeight - rcItems.height())
    {
        m_scrollPos = qMax(0, m_totalHeight - rcItems.height());
    }

    setScrollBarValue();
}

void QdfRibbonGalleryPrivate::layoutScrollBar()
{
    QDF_Q(QdfRibbonGallery);

    if (m_scrollBar)
    {
        QSize size = m_scrollBar->sizeHint();

        QRect rectScroll(q->rect());
        rectScroll.setLeft(rectScroll.right() - size.width());

        if (qobject_cast<QdfOfficePopupMenu *>(q->parentWidget()))
        {
            rectScroll.adjust(0, 0, 0, 0);
        }
        else if (m_showBorders)
        {
            rectScroll.adjust(0, 1, -1, -1);
        }

        m_scrollBar->setGeometry(rectScroll);
    }
    else if (m_buttonPopup && m_buttonScrollUp && m_buttonScrollDown)
    {
        QRect rectScroll(q->rect());

        if (m_showBorders)
        {
            rectScroll.adjust(1, 0, 0, 0);
        }

        rectScroll.setLeft(rectScroll.right() - 14);

        QRect rectUp(QPoint(rectScroll.left(), rectScroll.top()),
                     QPoint(rectScroll.right(), rectScroll.top() + 20));
        m_buttonScrollUp->setGeometry(rectUp);
        QRect rectDown(QPoint(rectScroll.left(), rectUp.bottom() + 1),
                       QPoint(rectScroll.right(), rectUp.bottom() + 19));
        m_buttonScrollDown->setGeometry(rectDown);
        QRect rectScrollPopup(QPoint(rectScroll.left(), rectDown.bottom() + 1),
                              QPoint(rectScroll.right(), rectScroll.bottom()));
        m_buttonPopup->setGeometry(rectScrollPopup);
    }
}

void QdfRibbonGalleryPrivate::setScrollBarValue()
{
    if (m_scrollBar)
    {
        QDF_Q(QdfRibbonGallery);
        QRect rcItems = q->getItemsRect();

        int nRangeMax = qMax(0, (m_totalHeight + 2) - m_scrollBar->rect().height());
        m_scrollBar->setRange(0, nRangeMax);
        m_scrollBar->setPageStep(rcItems.height());
        m_scrollBar->setSingleStep(20);
        m_scrollBar->setValue(m_scrollPos);
    }
    else if (m_buttonPopup && m_buttonScrollUp && m_buttonScrollDown)
    {
        m_buttonScrollUp->setEnabled(isScrollButtonEnabled(true));
        m_buttonScrollDown->setEnabled(isScrollButtonEnabled(false));
    }
}

void QdfRibbonGalleryPrivate::setScrollPos(int scrollPos)
{
    QDF_Q(QdfRibbonGallery);
    QRect rcItems = q->getItemsRect();

    if (scrollPos > m_totalHeight - rcItems.height())
    {
        scrollPos = m_totalHeight - rcItems.height();
    }

    if (scrollPos < 0)
    {
        scrollPos = 0;
    }

    if (m_animation)
    {
        m_animation = false;
        m_timerElapse = 0;
        m_scrollTimer.stop();
        m_scrollPos = -1;
    }

    if (m_scrollPos == scrollPos)
    {
        return;
    }

    m_scrollPos = scrollPos;
    layoutItems();

    repaintItems(NULL, false);
}

void QdfRibbonGalleryPrivate::actionTriggered(int action)
{
    QDF_Q(QdfRibbonGallery);
    int y = m_animation ? m_scrollPosTarget : m_scrollPos;
    QRect rcItems = q->getItemsRect();

    QdfRibbonGalleryGroup *items = q->galleryGroup();
    if (!items)
    {
        return;
    }

    switch (action)
    {
        case QAbstractSlider::SliderSingleStepAdd:
            y += items->size().height();
            break;
        case QAbstractSlider::SliderSingleStepSub:
            y -= items->size().height();
            break;
        case QAbstractSlider::SliderPageStepAdd:
            y += rcItems.height();
            break;
        case QAbstractSlider::SliderPageStepSub:
            y -= rcItems.height();
            break;
        case QAbstractSlider::SliderToMinimum:
            y = 0;
            break;
        case QAbstractSlider::SliderToMaximum:
            y = m_totalHeight;
            break;
        case QAbstractSlider::SliderMove:
            y = m_scrollBar->sliderPosition();
            break;
        default:
            break;
    }

    if (m_menuBar && (action == QAbstractSlider::SliderSingleStepSub ||
                      action == QAbstractSlider::SliderSingleStepAdd))
    {
        startAnimation(y);
    }
    else
    {
        setScrollPos(y);
    }
}

void QdfRibbonGalleryPrivate::pressedScrollUp()
{
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepSub);
}

void QdfRibbonGalleryPrivate::pressedScrollDown()
{
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepAdd);
}

void QdfRibbonGalleryPrivate::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QDF_Q(QdfRibbonGallery);

    if (policy == Qt::ScrollBarAlwaysOn)
    {
        delete m_buttonScrollUp;
        m_buttonScrollUp = nullptr;
        delete m_buttonScrollDown;
        m_buttonScrollDown = nullptr;
        delete m_buttonPopup;
        m_buttonPopup = nullptr;
        if (m_menuBar)
        {
            m_menuBar->removeEventFilter(this);
        }
        m_menuBar = nullptr;

        if (!m_scrollBar)
        {
            m_scrollBar = new QScrollBar(q);
            setScrollBarValue();
            connect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
        }
    }
    else
    {
        if (m_scrollBar)
        {
            disconnect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
            delete m_scrollBar;
            m_scrollBar = nullptr;
        }
    }
}

Qt::ScrollBarPolicy QdfRibbonGalleryPrivate::scrollBarPolicy() const
{
    return m_scrollBar ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff;
}

QAction *QdfRibbonGalleryPrivate::setPopupMenu(QdfOfficePopupMenu *popup)
{
    QDF_Q(QdfRibbonGallery);
    QAction *act = new QAction(q);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_menuBar = popup;
    m_menuBar->installEventFilter(this);

    m_buttonScrollUp = new QToolButton(q);
    m_buttonScrollUp->setProperty(_qdf_ScrollUpButtonGallery, true);
    m_buttonScrollDown = new QToolButton(q);
    m_buttonScrollDown->setProperty(_qdf_ScrollDownButtonGallery, true);
    m_buttonPopup = new QToolButton(q);
    m_buttonPopup->setProperty(_qdf_PopupButtonGallery, true);

    layoutScrollBar();
    m_buttonPopup->setPopupMode(QToolButton::InstantPopup);
    m_buttonPopup->setMenu(m_menuBar);
    m_menuBar->setWidgetBar(q);
    m_menuBar->setDefaultAction(act);

    connect(m_buttonScrollUp, SIGNAL(pressed()), this, SLOT(pressedScrollUp()));
    connect(m_buttonScrollDown, SIGNAL(pressed()), this, SLOT(pressedScrollDown()));
    return act;
}

void QdfRibbonGalleryPrivate::drawItems(QPainter *painter)
{
    QDF_Q(QdfRibbonGallery);

    QdfRibbonGalleryGroup *items = q->galleryGroup();
    if (!items)
    {
        return;
    }

    Q_ASSERT(m_arrRects.size() == items->itemCount());

    QRect rcItems = q->getItemsRect();

    painter->setClipRect(rcItems);

    int selected = m_hideSelection ? -1 : m_selected;
    for (int i = 0; i < m_arrRects.size(); ++i)
    {
        const qdf_galleryitem_rect &pos = m_arrRects[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -m_scrollPos);

        if (rcItem.bottom() < rcItems.top())
        {
            continue;
        }

        bool enabled = pos.item->isEnabled() && q->isEnabled();

        if (!rcItem.isEmpty())
        {
            pos.item->draw(painter, q, rcItem, enabled, selected == i, selected == i && m_pressed,
                           isItemChecked(pos.item));
        }

        if (rcItem.top() > rcItems.bottom())
        {
            break;
        }
    }
}

void QdfRibbonGalleryPrivate::repaintItems(QRect *pRect, bool bAnimate)
{
    Q_UNUSED(bAnimate);
    QDF_Q(QdfRibbonGallery);
#ifdef Q_OS_MAC
    Q_UNUSED(pRect);
    q->update();
#else
    q->update(pRect ? *pRect : q->rect());
#endif
}

bool QdfRibbonGalleryPrivate::isItemChecked(QdfRibbonGalleryItem *pItem) const
{
    return m_checkedItem == pItem;
}

bool QdfRibbonGalleryPrivate::isScrollButtonEnabled(bool buttonUp)
{
    QDF_Q(QdfRibbonGallery);

    if (!q->isEnabled())
    {
        return false;
    }
    return buttonUp ? m_scrollPos > 0 : m_scrollPos < m_totalHeight - q->getItemsRect().height();
}

int QdfRibbonGalleryPrivate::scrollWidth() const
{
    return m_scrollBar        ? m_scrollBar->sizeHint().width()
           : m_buttonScrollUp ? m_buttonScrollUp->width()
                              : 0;
}

void QdfRibbonGalleryPrivate::startAnimation(int scrollPos)
{
    QDF_Q(QdfRibbonGallery);
    QRect rcItems = q->getItemsRect();

    if (scrollPos > m_totalHeight - rcItems.height())
    {
        scrollPos = m_totalHeight - rcItems.height();
    }

    if (scrollPos < 0)
    {
        scrollPos = 0;
    }

    if (m_animation && m_scrollPosTarget == scrollPos)
    {
        return;
    }

    if (!m_animation && m_scrollPos == scrollPos)
    {
        return;
    }

    m_animation = true;
    m_scrollPosTarget = scrollPos;
    m_animationStep = double(m_scrollPosTarget - m_scrollPos) / (m_timerElapse > 200 ? 8.0 : 3.0);

    if (m_animationStep > 0 && m_animationStep < 1)
    {
        m_animationStep = 1;
    }
    if (m_animationStep < 0 && m_animationStep > -1)
    {
        m_animationStep = -1;
    }

    int nms = 40;
    m_scrollTimer.start(nms, this);
    startAnimate();
}

void QdfRibbonGalleryPrivate::startAnimate()
{
    if (qAbs(m_scrollPos - m_scrollPosTarget) > qFabs(m_animationStep))
    {
        m_scrollPos = int((double) m_scrollPos + m_animationStep);
    }
    else
    {
        m_animation = false;
        m_scrollPos = m_scrollPosTarget;
        m_timerElapse = 0;
        m_scrollTimer.stop();
    }
    layoutItems();
    setScrollBarValue();
    repaintItems(nullptr, false);
}

bool QdfRibbonGalleryPrivate::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::Timer:
            {
                QTimerEvent *timerEvent = (QTimerEvent *) event;
                if (m_scrollTimer.timerId() == timerEvent->timerId())
                {
                    startAnimate();
                    event->accept();
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return QObject::event(event);
}

bool QdfRibbonGalleryPrivate::eventFilter(QObject *object, QEvent *event)
{
    bool bResult = QObject::eventFilter(object, event);

    if (event->type() == QEvent::Show)
    {
        if (QdfOfficePopupMenu *popup = qobject_cast<QdfOfficePopupMenu *>(object))
        {
            QDF_Q(QdfRibbonGallery);
            QPoint pos;
            QRect screen = QApplication::desktop()->availableGeometry(m_buttonPopup);
            QSize sh = popup->sizeHint();
            QRect rect = q->rect();
            if (q->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height())
            {
                pos = q->mapToGlobal(rect.topLeft());
            }
            else
            {
                pos = q->mapToGlobal(rect.topLeft() - QPoint(0, sh.height()));
            }
            popup->move(pos.x(), pos.y());
        }
    }
    else if (event->type() == QEvent::Hide)
    {
    }
    return bResult;
}

QdfRibbonGalleryItem::QdfRibbonGalleryItem()
{
    QDF_INIT_PRIVATE(QdfRibbonGalleryItem);
    QDF_D(QdfRibbonGalleryItem);
    d->init();
}

QdfRibbonGalleryItem::~QdfRibbonGalleryItem()
{
    QDF_FINI_PRIVATE();
}

QSize QdfRibbonGalleryItem::sizeHint() const
{
    QDF_D(const QdfRibbonGalleryItem);

    QSize size = qvariant_cast<QSize>(data(Qt::SizeHintRole));

    if (!size.isNull() && size.isValid())
    {
        return size;
    }

    return d->m_items->qdf_d()->m_sizeItem;
}

void QdfRibbonGalleryItem::setSizeHint(const QSize &size)
{
    setData(Qt::SizeHintRole, size);
}

QIcon QdfRibbonGalleryItem::icon() const
{
    return qvariant_cast<QIcon>(data(Qt::DecorationRole));
}

void QdfRibbonGalleryItem::setIcon(const QIcon &icon)
{
    setData(Qt::DecorationRole, icon);
}

QString QdfRibbonGalleryItem::caption() const
{
    return data(Qt::DisplayRole).toString();
}

void QdfRibbonGalleryItem::setCaption(const QString &caption)
{
    setData(Qt::DisplayRole, caption);
}

QString QdfRibbonGalleryItem::toolTip() const
{
    return data(Qt::ToolTipRole).toString();
}

void QdfRibbonGalleryItem::setToolTip(const QString &toolTip)
{
    setData(Qt::ToolTipRole, toolTip);
}

QString QdfRibbonGalleryItem::statusTip() const
{
    return data(Qt::StatusTipRole).toString();
}

void QdfRibbonGalleryItem::setStatusTip(const QString &statusTip)
{
    setData(Qt::StatusTipRole, statusTip);
}

int QdfRibbonGalleryItem::getIndex() const
{
    QDF_D(const QdfRibbonGalleryItem);
    return d->m_index;
}

bool QdfRibbonGalleryItem::isSeparator() const
{
    QDF_D(const QdfRibbonGalleryItem);
    return d->m_separator;
}

void QdfRibbonGalleryItem::setSeparator(bool b)
{
    QDF_D(QdfRibbonGalleryItem);
    d->m_separator = b;
}

void QdfRibbonGalleryItem::setEnabled(bool enabled)
{
    QDF_D(QdfRibbonGalleryItem);
    if (d->m_enabled == enabled)
    {
        return;
    }

    d->m_enabled = enabled;

    if (d->m_items)
    {
        d->m_items->qdf_d()->redrawWidget();
    }
}

bool QdfRibbonGalleryItem::isEnabled() const
{
    QDF_D(const QdfRibbonGalleryItem);
    return d->m_enabled;
}

void QdfRibbonGalleryItem::setVisible(bool visible)
{
    QDF_D(QdfRibbonGalleryItem);
    if (d->m_visible == visible)
    {
        return;
    }

    d->m_visible = visible;

    if (d->m_items)
    {
        d->m_items->qdf_d()->itemsChanged();
    }
}

bool QdfRibbonGalleryItem::isVisible() const
{
    QDF_D(const QdfRibbonGalleryItem);
    return d->m_visible;
}

void QdfRibbonGalleryItem::setData(int role, const QVariant &value)
{
    QDF_D(QdfRibbonGalleryItem);
    bool found = false;
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d->m_values.count(); ++i)
    {
        if (d->m_values.at(i).role == role)
        {
            if (d->m_values.at(i).value == value)
            {
                return;
            }
            d->m_values[i].value = value;
            found = true;
            break;
        }
    }
    if (!found)
    {
        d->m_values.append(WidgetItemData(role, value));
    }
}

QVariant QdfRibbonGalleryItem::data(int role) const
{
    QDF_D(const QdfRibbonGalleryItem);
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d->m_values.count(); ++i)
    {
        if (d->m_values.at(i).role == role)
        {
            return d->m_values.at(i).value;
        }
    }
    return QVariant();
}

void QdfRibbonGalleryItem::draw(QPainter *p, QdfRibbonGallery *gallery, QRect rectItem, bool enabled,
                                bool selected, bool pressed, bool checked)
{
    Q_UNUSED(gallery);
    if (QdfOfficeStyle *style = qobject_cast<QdfOfficeStyle *>(gallery->style()))
    {
        if (isSeparator())
        {
            style->drawLabelGallery(p, this, rectItem);
            return;
        }

        if (!icon().isNull())
        {
            style->drawRectangle(p, rectItem, selected, pressed, enabled, checked, false, TypeNormal,
                                 BarPopup);
            icon().paint(p, rectItem, Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
        }
        else
        {
            style->drawRectangle(p, rectItem, selected, false, enabled, checked, false, TypePopup,
                                 BarPopup);
            QRect rcText(rectItem);
            rcText.setLeft(rcText.left() + 3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            QPalette palette;
            palette.setColor(QPalette::WindowText, style->getTextColor(selected, false, enabled, checked,
                                                                       false, TypePopup, BarPopup));
            style->drawItemText(p, rcText, alignment, palette, enabled, caption(), QPalette::WindowText);
        }
    }
}

QdfRibbonGalleryGroup::QdfRibbonGalleryGroup(QObject *parent) : QObject(parent)
{
    setObjectName("QdfRibbonGalleryGroup");
    QDF_INIT_PRIVATE(QdfRibbonGalleryGroup);
    QDF_D(QdfRibbonGalleryGroup);

    d->init();
}

QdfRibbonGalleryGroup::~QdfRibbonGalleryGroup()
{
    clear();
    QDF_FINI_PRIVATE();
}

QdfRibbonGalleryGroup *QdfRibbonGalleryGroup::createGalleryGroup()
{
    QdfRibbonGalleryGroup *group = new QdfRibbonGalleryGroup();
    return group;
}

QdfRibbonGalleryItem *QdfRibbonGalleryGroup::addItem(const QString &caption, const QPixmap &pixmap,
                                                     const QColor &transparentColor)
{
    QdfRibbonGalleryItem *item = new QdfRibbonGalleryItem();

    if (!pixmap.isNull())
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QImage copyImage = pixmap.copy(0, 0, pixmap.width(), pixmap.height()).toImage();
        if (transparentColor.isValid())
        {
            copyImage.setAlphaChannel(
                    copyImage.createMaskFromColor(transparentColor.rgb(), Qt::MaskOutColor));
        }
        item->setIcon(QPixmap::fromImage(copyImage));
#else
        QPixmap copyPix = pixmap.copy(0, 0, pixmap.width(), pixmap.height());
        if (transparentColor.isValid())
        {
            copyPix.setAlphaChannel(
                    QBitmap(copyPix.createMaskFromColor(transparentColor, Qt::MaskOutColor)));
        }
        item->setIcon(QIcon(copyPix));
#endif
    }

    if (!caption.isEmpty())
    {
        item->setCaption(caption);
        item->setToolTip(caption);
    }
    appendItem(item);
    return item;
}

QdfRibbonGalleryItem *QdfRibbonGalleryGroup::addItemFromMap(const QString &caption, int mapIndex,
                                                            const QPixmap &map, const QSize &mapItemSize,
                                                            const QColor &clrTransparent)
{
    QRect rcImage(mapIndex * mapItemSize.width(), 0, mapItemSize.width(), mapItemSize.height());
    QPixmap copyPix = map.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());

    Q_ASSERT(!copyPix.isNull());

    if (copyPix.isNull())
    {
        return nullptr;
    }

    return addItem(caption, copyPix, clrTransparent);
}

void QdfRibbonGalleryGroup::appendItem(QdfRibbonGalleryItem *item)
{
    insertItem(itemCount(), item);
}

void QdfRibbonGalleryGroup::insertItem(int index, QdfRibbonGalleryItem *item)
{
    QDF_D(QdfRibbonGalleryGroup);

    if (!item)
    {
        Q_ASSERT(false);
        return;
    }

    if (index < 0 || index > itemCount())
    {
        index = itemCount();
    }

    d->m_arrItems.insert(index, item);
    d->updateIndexes(index);

    item->qdf_d()->m_items = this;
    d->itemsChanged();
}

QdfRibbonGalleryItem *QdfRibbonGalleryGroup::addSeparator(const QString &caption)
{
    QdfRibbonGalleryItem *item = new QdfRibbonGalleryItem();
    appendItem(item);
    item->setCaption(caption);
    item->setSeparator(true);
    return item;
}

void QdfRibbonGalleryGroup::clear()
{
    QDF_D(QdfRibbonGalleryGroup);
    d->clear();
    d->itemsChanged();
}

void QdfRibbonGalleryGroup::remove(int index)
{
    QDF_D(QdfRibbonGalleryGroup);

    QdfRibbonGalleryItem *galleryItem = item(index);
    if (!galleryItem)
    {
        return;
    }

    d->m_arrItems.remove(index);
    delete galleryItem;

    d->itemsChanged();
}

int QdfRibbonGalleryGroup::itemCount() const
{
    QDF_D(const QdfRibbonGalleryGroup);
    return d->m_arrItems.size();
}

QdfRibbonGalleryItem *QdfRibbonGalleryGroup::item(int index) const
{
    QDF_D(const QdfRibbonGalleryGroup);
    return index >= 0 && index < itemCount() ? d->m_arrItems.at(index) : nullptr;
}

QdfRibbonGalleryItem *QdfRibbonGalleryGroup::takeItem(int index)
{
    QDF_D(QdfRibbonGalleryGroup);

    QdfRibbonGalleryItem *galleryItem = item(index);
    if (!galleryItem)
    {
        return NULL;
    }

    d->m_arrItems.remove(index);
    d->updateIndexes(0);

    d->itemsChanged();
    return galleryItem;
}

QSize QdfRibbonGalleryGroup::size() const
{
    QDF_D(const QdfRibbonGalleryGroup);
    return d->m_sizeItem;
}

void QdfRibbonGalleryGroup::setSize(const QSize &size)
{
    QDF_D(QdfRibbonGalleryGroup);
    d->m_sizeItem = size;
}

void QdfRibbonGalleryGroup::setClipItems(bool clipItems)
{
    QDF_D(QdfRibbonGalleryGroup);
    d->m_clipItems = clipItems;
}

/* QdfRibbonGallery */
QdfRibbonGallery::QdfRibbonGallery(QWidget *parent) : QWidget(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonGallery);
    QDF_D(QdfRibbonGallery);
    d->init();
}

QdfRibbonGallery::~QdfRibbonGallery()
{
    QDF_FINI_PRIVATE();
}

void QdfRibbonGallery::setGalleryGroup(QdfRibbonGalleryGroup *items)
{
    QDF_D(QdfRibbonGallery);
    if (d->m_items)
    {
        d->m_arrRects.clear();
        d->m_items->qdf_d()->m_viewWidgets.removeOne(this);
        d->m_items = nullptr;
    }

    if (items)
    {
        d->m_items = items;
        d->m_items->qdf_d()->m_viewWidgets.append(this);
    }
    d->layoutItems();
    update();
}

void QdfRibbonGallery::setBorderVisible(bool bShowBorders)
{
    QDF_D(QdfRibbonGallery);
    d->m_showBorders = bShowBorders;
    update();
}

bool QdfRibbonGallery::isBorderVisible() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_showBorders;
}

void QdfRibbonGallery::setLabelsVisible(bool showLabels)
{
    QDF_D(QdfRibbonGallery);
    d->m_showLabels = showLabels;
}

bool QdfRibbonGallery::isLabelsVisible() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_showLabels;
}

void QdfRibbonGallery::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QDF_D(QdfRibbonGallery);
    d->setScrollBarPolicy(policy);
}

Qt::ScrollBarPolicy QdfRibbonGallery::scrollBarPolicy() const
{
    QDF_D(const QdfRibbonGallery);
    return d->scrollBarPolicy();
}

void QdfRibbonGallery::ensureVisible(int index)
{
    QDF_D(QdfRibbonGallery);

    if (itemCount() == 0 || index == -1 || index >= itemCount() || index >= d->m_arrRects.size())
    {
        return;
    }

    const qdf_galleryitem_rect &pos = d->m_arrRects.at(index);

    QRect rcItem = pos.rect;
    rcItem.translate(0, -d->m_scrollPos);

    QRect rcItems = getItemsRect();

    if (rcItem.top() < rcItems.top())
    {
        int scrollPos = pos.rect.top() - rcItems.top();
        if (d->m_showLabels)
        {
            for (int i = index - 1; i >= 0; i--)
            {
                const qdf_galleryitem_rect &posLabel = d->m_arrRects.at(i);
                if (posLabel.item->isSeparator() &&
                    pos.rect.top() - posLabel.rect.top() <= rcItems.height() - rcItem.height())
                {
                    scrollPos = posLabel.rect.top() - rcItems.top();
                    break;
                }
                if (pos.rect.top() - posLabel.rect.top() > rcItems.height() - rcItem.height())
                {
                    break;
                }
            }
        }
        d->setScrollPos(scrollPos);
    }
    else if (rcItem.bottom() > rcItems.bottom())
    {
        d->setScrollPos(pos.rect.bottom() - rcItems.bottom());
    }
}

QAction *QdfRibbonGallery::setPopupMenu(QdfOfficePopupMenu *popupMenu)
{
    QDF_D(QdfRibbonGallery);
    return d->setPopupMenu(popupMenu);
}

QdfOfficePopupMenu *QdfRibbonGallery::popupMenu() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_menuBar;
}

int QdfRibbonGallery::itemCount() const
{
    QdfRibbonGalleryGroup *pItems = galleryGroup();
    return pItems ? pItems->itemCount() : 0;
}

QdfRibbonGalleryItem *QdfRibbonGallery::item(int index) const
{
    QdfRibbonGalleryGroup *items = galleryGroup();
    return items ? items->item(index) : NULL;
}

QdfRibbonGalleryGroup *QdfRibbonGallery::galleryGroup() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_items;
}

void QdfRibbonGallery::setSelectedItem(int indexItem)
{
    QDF_D(QdfRibbonGallery);

    int indSelected = d->m_selected;

    d->m_hideSelection = false;
    d->m_selected = indexItem;
    d->m_pressed = false;
    d->m_keyboardSelected = true;

    QdfRibbonGalleryGroup *items = galleryGroup();

    if (items && items->qdf_d()->m_clipItems && indSelected != -1)
    {
        QRect rect(getDrawItemRect(indSelected));
        d->repaintItems(&rect, true);
    }

    ensureVisible(d->m_selected);

    if (items && items->qdf_d()->m_clipItems && d->m_selected != -1)
    {
        QRect rect(getDrawItemRect(d->m_selected));
        d->repaintItems(&rect, false);
    }

    if (!items || !items->qdf_d()->m_clipItems)
    {
        d->repaintItems();
    }

    if (d->m_selected != -1)
    {
    }
    if (d->m_selected != -1 && !d->m_preview)
    {
        d->m_preview = true;
    }

    if (d->m_preview)
    {
    }

    if (d->m_selected == -1 && d->m_preview)
    {
        d->m_preview = false;
    }
    selectedItemChanged();
}

int QdfRibbonGallery::selectedItem() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_selected;
}

void QdfRibbonGallery::setCheckedIndex(int index)
{
    QDF_D(QdfRibbonGallery);

    QdfRibbonGalleryItem *previous = NULL;
    int previousInd = checkedIndex();
    if (previousInd != -1)
    {
        previous = item(previousInd);
    }

    QdfRibbonGalleryItem *currentItem = item(index);
    if (currentItem && d->m_checkedItem != currentItem)
    {
        d->m_checkedItem = currentItem;
        d->repaintItems();
        emit currentItemChanged(currentItem, previous);
    }
    if (currentItem)
    {
        ensureVisible(index);
    }
}

int QdfRibbonGallery::checkedIndex() const
{
    QDF_D(const QdfRibbonGallery);
    if (d->m_checkedItem)
    {
        return d->m_checkedItem->getIndex();
    }
    return -1;
}

void QdfRibbonGallery::setCheckedItem(const QdfRibbonGalleryItem *item)
{
    Q_ASSERT(item != nullptr);
    if (item->getIndex() != -1)
    {
        setCheckedIndex(item->getIndex());
    }
}

QdfRibbonGalleryItem *QdfRibbonGallery::checkedItem() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_checkedItem;
}

bool QdfRibbonGallery::isShowAsButton() const
{
    return false;
}

bool QdfRibbonGallery::isItemSelected() const
{
    QDF_D(const QdfRibbonGallery);
    return !d->m_hideSelection && selectedItem() != -1;
}

QRect QdfRibbonGallery::borders() const
{
    QDF_D(const QdfRibbonGallery);
    QRect rcBorders(0, 0, 0, 0);

    if (d->m_showBorders)
    {
        rcBorders.setTopLeft(QPoint(1, 1));
        rcBorders.setBottomRight(QPoint(1, 1));
    }

    rcBorders.setRight(rcBorders.right() + d->scrollWidth());

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    int left = 0, top = 0, right = 0, bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);
    rcBorders.adjust(left, top, right, bottom);
#else
    QMargins m = contentsMargins();
    rcBorders.adjust(m.left(), m.top(), m.right(), m.bottom());
#endif

    return rcBorders;
}

QRect QdfRibbonGallery::getDrawItemRect(int index)
{
    QDF_D(QdfRibbonGallery);
    if (index < 0 || index >= d->m_arrRects.size())
    {
        return QRect(0, 0, 0, 0);
    }

    QRect rc(d->m_arrRects[index].rect);
    rc.translate(0, -d->m_scrollPos);

    rc = rc.intersected(getItemsRect());
    return rc;
}

bool QdfRibbonGallery::isResizable() const
{
    bool isResizable = sizePolicy().horizontalPolicy() == QSizePolicy::Expanding ||
                       sizePolicy().horizontalPolicy() == QSizePolicy::MinimumExpanding;
    return !isShowAsButton() && galleryGroup() ? isResizable : false;
}

bool QdfRibbonGallery::autoWidth() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_autoWidth;
}

void QdfRibbonGallery::setAutoWidth(bool width)
{
    QDF_D(QdfRibbonGallery);
    d->m_autoWidth = width;
}

int QdfRibbonGallery::hitTestItem(QPoint point, QRect *pRect) const
{
    if (!isEnabled())
    {
        return -1;
    }

    if (isShowAsButton())
    {
        return -1;
    }

    QDF_D(const QdfRibbonGallery);
    Q_ASSERT(d->m_arrRects.size() == itemCount());

    QRect rcItems = getItemsRect();

    for (int i = 0; i < d->m_arrRects.size(); ++i)
    {
        const qdf_galleryitem_rect &pos = d->m_arrRects[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -d->m_scrollPos);

        if (!pos.item->isEnabled())
        {
            continue;
        }

        if (rcItem.top() >= rcItems.bottom())
        {
            break;
        }

        if (rcItem.contains(point))
        {
            if (pRect)
            {
                *pRect = rcItem;
            }
            return pos.item->isSeparator() ? -1 : i;
        }
    }

    return -1;
}

QRect QdfRibbonGallery::getItemsRect() const
{
    QRect rc = rect();

    if (rc.isNull())
    {
        rc.setWidth(32000);
        rc.setHeight(32000);
    }

    QRect rcBorders = borders();
    rc.adjust(rcBorders.left(), rcBorders.top(), -rcBorders.right(), -rcBorders.bottom());
    return rc;
}

void QdfRibbonGallery::hideSelection()
{
    QDF_D(QdfRibbonGallery);
    d->m_hideSelection = true;
    d->m_keyboardSelected = false;

    if (d->m_selected != -1)
    {
        QdfRibbonGalleryGroup *pItems = galleryGroup();
        if (pItems && pItems->qdf_d()->m_clipItems)
        {
            QRect rect(getDrawItemRect(d->m_selected));
            d->repaintItems(&rect, false);
        }
        else
        {
            d->repaintItems();
        }
    }

    if (d->m_preview && d->m_selected != -1)
    {
    }

    if (d->m_preview)
    {
        d->m_preview = false;
    }
    selectedItemChanged();
}

void QdfRibbonGallery::updatelayout()
{
    QDF_D(QdfRibbonGallery);
    d->layoutItems();
    d->layoutScrollBar();
}

void QdfRibbonGallery::bestFit()
{
    if (itemCount() > 0)
    {
        QdfRibbonGalleryItem *galleryItem = item(0);
        QSize szItem = galleryItem->sizeHint();
        int totalWidth = width() - borders().width();
        if (szItem.width() < totalWidth)
        {
            int columnCount = totalWidth / szItem.width();
            setColumnCount(columnCount);
        }
    }
}

void QdfRibbonGallery::selectedItemChanged()
{
}

void QdfRibbonGallery::setMinimumColumnCount(int count)
{
    QDF_D(QdfRibbonGallery);
    if (d->m_minimumColumnCount != count)
    {
        d->m_minimumColumnCount = count;

        if (d->m_minimumColumnCount > d->m_currentColumnCount)
        {
            d->m_currentColumnCount = d->m_minimumColumnCount;
        }

        updatelayout();
    }
}

int QdfRibbonGallery::minimumColumnCount() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_minimumColumnCount;
}

void QdfRibbonGallery::setMaximumColumnCount(int count)
{
    QDF_D(QdfRibbonGallery);
    if (d->m_maximumColumnCount != count)
    {
        d->m_maximumColumnCount = count;

        if (d->m_maximumColumnCount < d->m_currentColumnCount || d->m_currentColumnCount == -1)
        {
            d->m_currentColumnCount = d->m_maximumColumnCount;
        }

        updatelayout();
    }
}

int QdfRibbonGallery::maximumColumnCount() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_maximumColumnCount;
}

void QdfRibbonGallery::setColumnCount(int count)
{
    QDF_D(QdfRibbonGallery);

    if (d->m_maximumColumnCount != -1 && d->m_maximumColumnCount < count)
    {
        count = d->m_maximumColumnCount;
    }
    else if (d->m_minimumColumnCount != -1 && d->m_minimumColumnCount > count)
    {
        count = d->m_minimumColumnCount;
    }

    d->m_currentColumnCount = count;

    if (autoWidth())
    {
        updatelayout();
    }
}

int QdfRibbonGallery::columnCount() const
{
    QDF_D(const QdfRibbonGallery);
    return d->m_currentColumnCount;
}

QSize QdfRibbonGallery::sizeHint() const
{
    QDF_D(const QdfRibbonGallery);

    QSize size = minimumSizeHint();

    if (columnCount() != -1)
    {
        if (QdfRibbonGalleryGroup *group = galleryGroup())
        {
            size.setWidth(columnCount() * group->size().width());
            size.setHeight(d->m_totalHeight);
            return size;
        }
    }

    int widthGallery = qMax(isResizable() ? size.width() : 0, geometry().width());
    int heightGallery = geometry().height();

    if (QdfOfficePopupMenu *popupBar = qobject_cast<QdfOfficePopupMenu *>(parentWidget()))
    {
        widthGallery = popupBar->width() - 2;
    }

    return QSize(widthGallery, heightGallery);
}

QSize QdfRibbonGallery::minimumSizeHint() const
{
    QDF_D(const QdfRibbonGallery);

    if (!parentWidget() || !galleryGroup())
    {
        return QSize(0, 0);
    }

    QRect rcBorders(borders());
    int width =
            qMax(16, galleryGroup()->qdf_d()->m_sizeItem.width()) + rcBorders.left() + rcBorders.right();

    if (QdfRibbonGalleryGroup *group = galleryGroup())
    {
        width = d->m_minimumColumnCount != -1 ? d->m_minimumColumnCount * group->size().width() : width;
    }

    int height = qMax(
            galleryGroup()->qdf_d()->m_sizeItem.height() + rcBorders.top() + rcBorders.bottom(), 32);

    return QSize(width, height);
}

void QdfRibbonGallery::paintEvent(QPaintEvent *)
{
    QDF_D(QdfRibbonGallery);
    QPainter p(this);

    QStyleOption opt;
    opt.init(this);

    style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonFillRect, &opt, &p, this);

    if (!parentWidget()->property(_qdf_PopupBar).toBool() && isBorderVisible())
    {
        opt.rect.adjust(0, 0, -1, -1);
        style()->drawPrimitive((QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonRect, &opt, &p, this);
    }
    d->drawItems(&p);
}


void QdfRibbonGallery::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfRibbonGallery);

    if (isShowAsButton())
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (d->m_selected != -1 && !d->m_keyboardSelected)
    {
        d->m_pressed = true;
        d->m_ptPressed = event->pos();
        d->repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1)
        {
            emit itemPressed(item(index));
        }
    }
}


void QdfRibbonGallery::mouseReleaseEvent(QMouseEvent *event)
{
    QDF_D(QdfRibbonGallery);
    if (d->m_pressed || isItemSelected())
    {
        d->m_pressed = false;
        d->repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1)
        {
            d->repaintItems();
            emit itemSelectionChanged();
            QdfRibbonGalleryItem *galleryItem = item(index);

            emit itemClicked(galleryItem);

            bool handled = true;
            emit itemClicking(galleryItem, handled);

            if (handled)
            {
                if (QdfOfficePopupMenu *popupBar = qobject_cast<QdfOfficePopupMenu *>(parentWidget()))
                {
                    popupBar->close();
                    for (QWidget *widget = QApplication::activePopupWidget(); widget;)
                    {
                        if (QMenu *qmenu = qobject_cast<QMenu *>(widget))
                        {
                            qmenu->hide();
                            break;
                        }
                    }
                }
            }
        }
    }
}


void QdfRibbonGallery::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfRibbonGallery);

    if (isShowAsButton())
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    int nItem = hitTestItem(event->pos());

    if (nItem == -1 && d->m_keyboardSelected)
    {
        return;
    }

    if (nItem == -1 && event->pos() == QPoint(-1, -1) && d->m_selected != -1)
    {
        hideSelection();
        return;
    }

    int nSelected = d->m_selected;
    if (nItem != d->m_selected || d->m_hideSelection || d->m_keyboardSelected)
    {
        d->m_selected = nItem;
        d->m_hideSelection = false;
        d->m_keyboardSelected = false;
        d->m_pressed = false;

        QdfRibbonGalleryGroup *pItems = galleryGroup();

        if (pItems && pItems->qdf_d()->m_clipItems)
        {
            if (nSelected != -1)
            {
                QRect rect(getDrawItemRect(nSelected));
                d->repaintItems(&rect, true);
            }

            if (d->m_selected != -1)
            {
                QRect rect(getDrawItemRect(d->m_selected));
                d->repaintItems(&rect, false);
            }
        }
        else
        {
            d->repaintItems();
        }

        if (!d->m_preview && d->m_selected != -1)
        {
        }

        if (d->m_preview)
        {
        }

        if (d->m_selected == -1 && d->m_preview)
        {
            d->m_preview = false;
        }
        selectedItemChanged();
    }
    if (d->m_pressed && (d->m_ptPressed != QPoint(0, 0)) && (d->m_selected != -1) &&
        (abs(d->m_ptPressed.x() - event->pos().x()) > 4 ||
         abs(d->m_ptPressed.y() - event->pos().y()) > 4))
    {
        d->m_ptPressed = QPoint(0, 0);
    }
}

void QdfRibbonGallery::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    QDF_D(QdfRibbonGallery);
    if (d->m_selected != -1)
    {
        hideSelection();
    }
}

void QdfRibbonGallery::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

void QdfRibbonGallery::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (autoWidth())
    {
        bestFit();
    }
}
