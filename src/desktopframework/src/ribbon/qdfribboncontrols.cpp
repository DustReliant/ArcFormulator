#include "private/qdfribbonbutton_p.h"
#include "private/qdfribboncontrols_p.h"
#include "qdfribbon_def.h"
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QToolBar>
#include <QWidgetAction>
#include <ribbon/qdfofficepopupmenu.h>
#include <ribbon/qdfribboncontrols.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonstyle.h>

QDF_USE_NAMESPACE


QdfRibbonControlSizeDefinitionPrivate::QdfRibbonControlSizeDefinitionPrivate() : m_parentControl(nullptr) {}

void QdfRibbonControlSizeDefinitionPrivate::init(QdfRibbonControl *parentControl, QdfRibbonControlSizeDefinition::GroupSize size)
{
    m_parentControl = parentControl;
    if (size == QdfRibbonControlSizeDefinition::GroupLarge || size == QdfRibbonControlSizeDefinition::GroupPopup)
    {
        m_imageSize = QdfRibbonControlSizeDefinition::ImageLarge;
        m_labelVisible = true;
    }
    else
    {
        m_imageSize = QdfRibbonControlSizeDefinition::ImageSmall;
        if (size == QdfRibbonControlSizeDefinition::GroupMedium)
        {
            m_labelVisible = true;
        }
        else
        {
            m_labelVisible = false;
        }
    }
    m_popup = size == QdfRibbonControlSizeDefinition::GroupPopup;
    m_showSeparator = true;
    m_visible = true;
    m_enabled = true;
    m_stretchable = true;
    m_visualIndex = -1;
    m_minItemCount = -1;
    m_maxItemCount = -1;
}

void QdfRibbonControlSizeDefinitionPrivate::updateSizeDefinition()
{
    m_parentControl->sizeChanged(m_parentControl->currentSize());
}

QdfRibbonControlSizeDefinitionPrivate::~QdfRibbonControlSizeDefinitionPrivate() {}


QdfRibbonControlSizeDefinition::QdfRibbonControlSizeDefinition(QdfRibbonControl *parentControl,
                                                               QdfRibbonControlSizeDefinition::GroupSize size)
    : QObject()
{
    QDF_INIT_PRIVATE(QdfRibbonControlSizeDefinition);
    QDF_D(QdfRibbonControlSizeDefinition);
    d->init(parentControl, size);
}

QdfRibbonControlSizeDefinition::~QdfRibbonControlSizeDefinition() { QDF_FINI_PRIVATE(); }

QdfRibbonControlSizeDefinition::ControlImageSize QdfRibbonControlSizeDefinition::imageSize() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_imageSize;
}

void QdfRibbonControlSizeDefinition::setImageSize(QdfRibbonControlSizeDefinition::ControlImageSize size)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    if (d->m_imageSize != size)
    {
        d->m_imageSize = size;
        d->updateSizeDefinition();
    }
}

bool QdfRibbonControlSizeDefinition::isLabelVisible() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_labelVisible;
}

void QdfRibbonControlSizeDefinition::setLabelVisible(bool visible)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    if (d->m_labelVisible != visible)
    {
        d->m_labelVisible = visible;
        d->updateSizeDefinition();
    }
}

bool QdfRibbonControlSizeDefinition::isPopup() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_popup;
}

void QdfRibbonControlSizeDefinition::setPopup(bool popup)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_popup = popup;
}

int QdfRibbonControlSizeDefinition::visualIndex() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_visualIndex;
}

void QdfRibbonControlSizeDefinition::setVisualIndex(int index)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_visualIndex = index;
}

bool QdfRibbonControlSizeDefinition::showSeparator() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_showSeparator;
}

void QdfRibbonControlSizeDefinition::setShowSeparator(bool showSeparator)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_showSeparator = showSeparator;
}

bool QdfRibbonControlSizeDefinition::isVisible() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_visible;
}

void QdfRibbonControlSizeDefinition::setVisible(bool visible)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_visible = visible;
}

bool QdfRibbonControlSizeDefinition::isEnabled() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_enabled;
}

void QdfRibbonControlSizeDefinition::setEnabled(bool enabled)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_enabled = enabled;
}

bool QdfRibbonControlSizeDefinition::isStretchable() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_stretchable;
}

void QdfRibbonControlSizeDefinition::setStretchable(bool stretchable)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_stretchable = stretchable;
}

int QdfRibbonControlSizeDefinition::minimumItemCount() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_minItemCount;
}

void QdfRibbonControlSizeDefinition::setMinimumItemCount(int count)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_minItemCount = count;
}

int QdfRibbonControlSizeDefinition::maximumItemCount() const
{
    QDF_D(const QdfRibbonControlSizeDefinition);
    return d->m_maxItemCount;
}

void QdfRibbonControlSizeDefinition::setMaximumItemCount(int count)
{
    QDF_D(QdfRibbonControlSizeDefinition);
    d->m_maxItemCount = count;
}


QdfRibbonControlPrivate::QdfRibbonControlPrivate()
    : m_parentGroup(nullptr), m_size(QdfRibbonControlSizeDefinition::GroupLarge), m_action(nullptr)
{
}

QdfRibbonControlPrivate::~QdfRibbonControlPrivate()
{
    delete m_allSizes[0];
    delete m_allSizes[1];
    delete m_allSizes[2];
    delete m_allSizes[3];
    m_allSizes.clear();
}

void QdfRibbonControlPrivate::init(QdfRibbonGroup *parentGroup)
{
    QDF_Q(QdfRibbonControl)
    m_parentGroup = parentGroup;
    m_allSizes.append(new QdfRibbonControlSizeDefinition(q, QdfRibbonControlSizeDefinition::GroupLarge)); // Large
    m_allSizes.append(new QdfRibbonControlSizeDefinition(q, QdfRibbonControlSizeDefinition::GroupMedium));// Medium
    m_allSizes.append(new QdfRibbonControlSizeDefinition(q, QdfRibbonControlSizeDefinition::GroupSmall)); // Small
    m_allSizes.append(new QdfRibbonControlSizeDefinition(q, QdfRibbonControlSizeDefinition::GroupPopup)); // Popup
}

void QdfRibbonControlPrivate::changed() {}


QdfRibbonControl::QdfRibbonControl(QdfRibbonGroup *parentGroup) : QWidget(nullptr)
{
    QDF_INIT_PRIVATE(QdfRibbonControl);
    QDF_D(QdfRibbonControl);
    d->init(parentGroup);
}

QdfRibbonControl::~QdfRibbonControl()
{
    if (parentGroup())
    {
        parentGroup()->removeControl(this);
    }
    QDF_FINI_PRIVATE();
}

QdfRibbonGroup *QdfRibbonControl::parentGroup() const
{
    QDF_D(const QdfRibbonControl);
    return d->m_parentGroup;
}

QAction *QdfRibbonControl::defaultAction() const
{
    QDF_D(const QdfRibbonControl);
    return d->m_action;
}

void QdfRibbonControl::setDefaultAction(QAction *action)
{
    QDF_D(QdfRibbonControl);
    if (d->m_action == action)
    {
        return;
    }
    d->m_action = action;
    actionChanged();
}

QdfRibbonControlSizeDefinition *QdfRibbonControl::sizeDefinition(QdfRibbonControlSizeDefinition::GroupSize size) const
{
    QDF_D(const QdfRibbonControl);
    return d->m_allSizes[(int) size];
}

void QdfRibbonControl::adjustCurrentSize(bool expand) { Q_UNUSED(expand); }

void QdfRibbonControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    QDF_D(QdfRibbonControl);
    d->m_size = size;
}

void QdfRibbonControl::actionChanged()
{
    QDF_D(QdfRibbonControl);
    d->changed();
}

void QdfRibbonControl::updateLayout() {}

QdfRibbonControlSizeDefinition::GroupSize QdfRibbonControl::currentSize() const
{
    QDF_D(const QdfRibbonControl);
    return d->m_size;
}


QdfRibbonWidgetControlPrivate::QdfRibbonWidgetControlPrivate()
    : m_widget(nullptr), m_visibleText(false), m_ignoreActionSettings(false), m_margin(4)
{
}

QdfRibbonWidgetControlPrivate::~QdfRibbonWidgetControlPrivate() {}

void QdfRibbonWidgetControlPrivate::init(bool ignoreActionSettings)
{
    QDF_Q(QdfRibbonWidgetControl);
    m_ignoreActionSettings = ignoreActionSettings;

    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
    q->sizeDefinition(QdfRibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);

    if (!ignoreActionSettings)
    {
        q->setAttribute(Qt::WA_Hover);
    }
}

void QdfRibbonWidgetControlPrivate::initStyleOption(QStyleOptionButton *option) const
{
    QDF_Q(const QdfRibbonWidgetControl);
    option->init(q);
    QAction *action = q->defaultAction();
    if (action && !m_ignoreActionSettings)
    {
        option->icon = action->icon();
        option->text = m_visibleText ? action->text() : QString("");
        QSize szIcon = option->icon.actualSize(option->rect.size());
        option->iconSize = szIcon;
    }
}

void QdfRibbonWidgetControlPrivate::clearWidget()
{
}


QdfRibbonWidgetControl::QdfRibbonWidgetControl(QdfRibbonGroup *parentGroup, bool ignoreActionSettings)
    : QdfRibbonControl(parentGroup)
{
    QDF_INIT_PRIVATE(QdfRibbonWidgetControl);
    QDF_D(QdfRibbonWidgetControl);
    d->init(ignoreActionSettings);
}

QdfRibbonWidgetControl::~QdfRibbonWidgetControl() { QDF_FINI_PRIVATE(); }

int QdfRibbonWidgetControl::margin() const
{
    QDF_D(const QdfRibbonWidgetControl);
    return d->m_margin;
}

void QdfRibbonWidgetControl::setMargin(int margin)
{
    QDF_D(QdfRibbonWidgetControl);
    d->m_margin = margin;
}

QWidget *QdfRibbonWidgetControl::contentWidget() const
{
    QDF_D(const QdfRibbonWidgetControl);
    return d->m_widget;
}

void QdfRibbonWidgetControl::setContentWidget(QWidget *widget)
{
    QDF_D(QdfRibbonWidgetControl);
    if (d->m_widget != nullptr)
    {
        delete d->m_widget;
    }

    d->m_widget = widget;

    if (d->m_widget != nullptr)
    {
        d->m_widget->setParent(this);
        d->m_widget->setVisible(true);
        d->m_widget->setAttribute(Qt::WA_MacSmallSize);
    }
}

void QdfRibbonWidgetControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    QDF_D(QdfRibbonWidgetControl);
    QdfRibbonControl::sizeChanged(size);
    d->m_visibleText = sizeDefinition(size)->isLabelVisible();
}

static QSize getBoundedSizeHint(const QWidget *widget)
{
    QSize ret = widget->sizeHint();
    ret.rwidth() = qMin(widget->maximumWidth(), ret.width());
    ret.rheight() = qMin(widget->maximumHeight(), ret.height());
    ret.rwidth() = qMax(widget->minimumWidth(), ret.width());
    ret.rheight() = qMax(widget->minimumHeight(), ret.height());
    return ret;
}

QSize QdfRibbonWidgetControl::sizeHint() const
{
    QDF_D(const QdfRibbonWidgetControl);
    if (d->m_widget != nullptr)
    {
        QSize sz = getBoundedSizeHint(d->m_widget);

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        if (!opt.icon.isNull())
        {
            sz.setWidth(sz.width() + opt.iconSize.width() + d->m_margin);
        }

        if (!opt.text.isEmpty())
        {
            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            sz.setWidth(sz.width() + szText.width() + d->m_margin);
        }

        return sz;
    }
    return QdfRibbonControl::sizeHint();
}

void QdfRibbonWidgetControl::childEvent(QChildEvent *event)
{
    QDF_D(QdfRibbonWidgetControl);
    QdfRibbonControl::childEvent(event);

    if (event->type() == QEvent::ChildRemoved)
    {
        if (QdfRibbonGroup *group = parentGroup())
        {
            if (QAction *action = defaultAction())
            {
                QChildEvent *childEvent = (QChildEvent *) event;
                if (childEvent->child()->isWidgetType())
                {
                    if (childEvent->child() == contentWidget())
                    {
                        d->m_widget = nullptr;
                        group->removeAction(action);
                    }
                }
            }
        }
    }
}

void QdfRibbonWidgetControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfRibbonWidgetControl);
    if (!d->m_ignoreActionSettings && d->m_widget != nullptr)
    {
        QPainter painter(this);

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        bool enabled = d->m_widget->isEnabled();
        int posText = 0;

        if (!opt.icon.isNull())
        {
            QRect rcIcon = opt.rect;
            rcIcon.setWidth(opt.iconSize.width());
            opt.icon.paint(&painter, rcIcon, Qt::AlignLeft | Qt::AlignVCenter,
                           enabled ? QIcon::Normal : QIcon::Disabled);
            posText = rcIcon.width() + d->m_margin;
        }

        if (!opt.text.isEmpty())
        {
            QRect rectText = opt.rect;
            rectText.setLeft(posText);

            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            rectText.setWidth(szText.width());

            int flags = 0;// opt.textAlignment;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter;
            style()->drawItemText(&painter, rectText, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::WindowText);
        }
    }
}

void QdfRibbonWidgetControl::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfRibbonWidgetControl);
    if (d->m_widget != nullptr)
    {
        int x = 0;
        int y = 0;

        QSize sz = event->size();

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        if (!opt.icon.isNull())
        {
            x = opt.iconSize.width();
            x += d->m_margin;
        }
        if (!opt.text.isEmpty())
        {
            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            x += szText.width();
            x += d->m_margin;
        }

        sz.rwidth() -= x;
        QRect r = QRect(x, y, sz.width(), sz.height());
        sz.rwidth() = qMin(sz.width(), d->m_widget->maximumWidth());
        QRect controlRect = QStyle::alignedRect(layoutDirection(), Qt::AlignRight, sz, r);
        d->m_widget->setGeometry(controlRect);
    }

    QdfRibbonControl::resizeEvent(event);
}

QdfRibbonColumnBreakControlPrivate::QdfRibbonColumnBreakControlPrivate() : m_textAlignment(Qt::AlignVCenter) {}

QdfRibbonColumnBreakControlPrivate::~QdfRibbonColumnBreakControlPrivate() {}

void QdfRibbonColumnBreakControlPrivate::init()
{
    QDF_Q(QdfRibbonColumnBreakControl);
    QFont fnt = QApplication::font("QMenuBar");
    qreal pointSize = 0.8 * fnt.pointSizeF();
    fnt.setPointSizeF(pointSize);
    q->setFont(fnt);
}

void QdfRibbonColumnBreakControlPrivate::initStyleOption(QStyleOptionGroupBox *option) const
{
    QDF_Q(const QdfRibbonColumnBreakControl);
    option->initFrom(q);
    option->state |= QStyle::State_Horizontal;
    option->textAlignment = m_textAlignment;
    if (QAction *action = q->defaultAction())
    {
        option->text = action->text();
    }
}

QdfRibbonColumnBreakControl::QdfRibbonColumnBreakControl(QdfRibbonGroup *parentGroup) : QdfRibbonWidgetControl(parentGroup, true)
{
    QDF_INIT_PRIVATE(QdfRibbonColumnBreakControl);
    QDF_D(QdfRibbonColumnBreakControl);
    d->init();
}

QdfRibbonColumnBreakControl::~QdfRibbonColumnBreakControl() { QDF_FINI_PRIVATE(); }

Qt::Alignment QdfRibbonColumnBreakControl::alignmentText() const
{
    QDF_D(const QdfRibbonColumnBreakControl);
    return d->m_textAlignment;
}

void QdfRibbonColumnBreakControl::setAlignmentText(Qt::Alignment alignment)
{
    QDF_D(QdfRibbonColumnBreakControl);
    if (d->m_textAlignment != alignment)
    {
        switch (alignment)
        {
            case Qt::AlignTop:
                d->m_textAlignment = alignment;
                break;
            case Qt::AlignBottom:
                d->m_textAlignment = alignment;
                break;
            case Qt::AlignVCenter:
                d->m_textAlignment = alignment;
                break;
            default:
                Q_ASSERT(false);
                break;
        }
        update();
    }
}

void QdfRibbonColumnBreakControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    QdfRibbonControl::sizeChanged(size);
    if (sizeDefinition(size)->showSeparator())
    {
    }
    else
    {
    }
}

QSize QdfRibbonColumnBreakControl::sizeHint() const
{
    QDF_D(const QdfRibbonColumnBreakControl);
    if (!parentGroup() || !parentGroup()->ribbonBar())
    {
        return QWidget::sizeHint();
    }

    QStyleOptionGroupBox opt;
    d->initStyleOption(&opt);
    const int height = parentGroup()->ribbonBar()->rowItemHeight() * parentGroup()->ribbonBar()->rowItemCount();
    int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentGroup());

    if (!opt.text.isEmpty())
    {
        QFontMetrics fm = opt.fontMetrics;
        extent += fm.height();
    }

    return QSize(extent, height);
}

void QdfRibbonColumnBreakControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfRibbonColumnBreakControl);
    QPainter p(this);
    QStyleOptionGroupBox opt;
    d->initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentGroup());
}

/* QdfRibbonLabelControl */
QdfRibbonLabelControl::QdfRibbonLabelControl(QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    setContentWidget(new QLabel(this));
}

QdfRibbonLabelControl::QdfRibbonLabelControl(const QString &text, QdfRibbonGroup *parentGroup)
    : QdfRibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    setContentWidget(new QLabel(text, this));
}

QdfRibbonLabelControl::~QdfRibbonLabelControl() {}

QLabel *QdfRibbonLabelControl::widget() const { return qobject_cast<QLabel *>(contentWidget()); }

/*QdfRibbonToolBarSeparatorControlPrivate*/
QdfRibbonToolBarSeparatorControlPrivate::QdfRibbonToolBarSeparatorControlPrivate(QdfRibbonToolBarControl *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

QSize QdfRibbonToolBarSeparatorControlPrivate::sizeHint() const
{
    QSize sz = QWidget::sizeHint();
    QStyleOption opt;
    opt.init(this);
    opt.state |= QStyle::State_Horizontal;
    const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
    return QSize(extent, sz.width());
}

void QdfRibbonToolBarSeparatorControlPrivate::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    opt.state |= QStyle::State_Horizontal;
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
}

/* QdfRibbonColumnBreakControlPrivate */
QdfRibbonToolBarControlPrivate::QdfRibbonToolBarControlPrivate()
    : m_margin(1), m_rowsCount(2), m_isCalcWidth(false), m_dirty(false), m_calcRows(-1), m_fullWidth(0)
{
}

QdfRibbonToolBarControlPrivate::~QdfRibbonToolBarControlPrivate() {}

void QdfRibbonToolBarControlPrivate::recalcWidths(int height)
{
    QDF_Q(QdfRibbonToolBarControl);
    if (q->parentGroup() == nullptr)
    {
        return;
    }

    m_vecWidths.clear();
    m_mapNonOptWidths.clear();

    QStyleOptionToolButton opt;
    opt.init(q->parentGroup());
    opt.text = q->parentGroup()->title();

    q->setRowsCount(2);
    m_isCalcWidth = true;
    QDesktopWidget desktop;
    QRect rect = desktop.screenGeometry();
    QRect rectScreen(QPoint(0, 0), rect.size());

    bool m_floatyMode = false;
    const int maxRows = 3;
    const int widthDefaultButton = q->parentGroup()->style()->pixelMetric(
            (QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonReducedGroupWidth, &opt, q->parentGroup());
    int lastWidth = -1;
    int dx = 16;

    for (int rows = 1; rows <= maxRows; rows++)
    {
        QRect rect(QPoint(0, 0), QSize(widthDefaultButton + 1, height));

        for (;; rect.setRight(rect.right() + dx))
        {
            if (rect.width() >= rectScreen.width())
            {
                if (m_vecWidths.size() == 0)
                {
                    m_vecWidths.append(32767);
                }
                break;
            }

            updateGroupsGeometry(rect);

            if (lastWidth != -1 && m_fullWidth > lastWidth)
            {
                break;
            }

            if (m_calcRows == rows && m_fullWidth > 0)
            {
                if (m_calcRows == maxRows - 1 && !m_floatyMode)
                {
                    m_vecWidths.clear();
                }

                m_vecWidths.append(m_fullWidth);
                m_mapNonOptWidths.insert(m_fullWidth + m_margin, rect.width());
                lastWidth = m_fullWidth;
                break;
            }
        }
    }
    if (m_vecWidths.count() == 1)
    {
        m_vecWidths.append(m_vecWidths[0]);
    }
    m_vecWidths.append(widthDefaultButton);
    m_isCalcWidth = false;
}

void QdfRibbonToolBarControlPrivate::showDefaultButton()
{
    QDF_Q(QdfRibbonToolBarControl);
    if (q->parentGroup() == nullptr)
    {
        return;
    }
    QStyleOptionToolButton opt;
    opt.init(q->parentGroup());
    opt.text = q->parentGroup()->title();
    const int widthDefaultButton = q->parentGroup()->style()->pixelMetric(
            (QStyle::PixelMetric) QdfRibbonStyle::PM_RibbonReducedGroupWidth, &opt, q->parentGroup());
    RibbonToolBarActionGroupList &groupsRef = m_groups;
    for (RibbonToolBarActionGroupList::iterator it = groupsRef.begin(); it != groupsRef.end(); ++it)
    {
        RibbonToolBarActionsGroup &actionGroup = *it;
        actionGroup.rect = QRect(0, 0, 0, 0);
    }
    m_rect.setRight(m_rect.left() + widthDefaultButton);
    m_calcRows = 0;
}

void QdfRibbonToolBarControlPrivate::addActionToGroup(QAction *action, QWidget *widget)
{
    bool endgroup = m_groups.size() == 0 ||
                    (m_groups.last().items.size() > 0 && m_groups.last().items.last().action->isSeparator());
    if (endgroup)
    {
        m_groups.append(RibbonToolBarActionsGroup());
    }
    RibbonToolBarActionGroupItems &lastGroup = m_groups.last().items;
    RibbonToolBarActionGroupItem item(action, widget);
    lastGroup.append(item);
}

static int indexOfAction(const QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupItems &group, QAction *action)
{
    int index = 0;
    for (QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::const_iterator it = group.constBegin();
         it != group.constEnd(); ++it)
    {
        if (it->action == action)
        {
            return index;
        }
        index++;
    }
    return -1;
}

void QdfRibbonToolBarControlPrivate::removeActionFromGroup(QAction *action)
{
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        RibbonToolBarActionsGroup group1 = *it;
        RibbonToolBarActionGroupItems &items = group1.items;
        int index = indexOfAction(items, action);
        if (index != -1)
        {
            delete items.at(index).widget;
            items.removeAt(index);
            if (items.size() == 0)
            {
                m_groups.erase(it);
            }
            else if (action->isSeparator() && (it + 1) != m_groups.end())
            {
                RibbonToolBarActionsGroup group2 = *(it + 1);
                items += group2.items;//*(it + 1);
                //  updateGroupProperties(items);
                m_groups.erase(it + 1);
            }
            return;
        }
    }
}

void QdfRibbonToolBarControlPrivate::updateGroupsGeometry(const QRect &rect)
{
    QDF_Q(QdfRibbonToolBarControl);

    if (q->parentGroup() == nullptr)
    {
        return;
    }

    QdfRibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == nullptr)
    {
        return;
    }

    bool m_floatyMode = false;
    m_rect = rect;
    m_rect.setWidth(rect.width());

    m_fullWidth = 0;
    m_calcRows = 0;

    QFontMetrics fm(q->parentGroup()->font());
    const QSize sizeCaption = fm.boundingRect(q->parentGroup()->title()).size();

    bool m_bTrancateCaption = false;
    if (!m_bTrancateCaption)
    {
        m_rect.setRight(m_rect.left() + qMax(rect.width(), sizeCaption.width()));
    }

    QSize size = rect.size();
    size.setWidth(size.width() - m_margin);

    int rowHeight = ribbonBar->rowItemHeight();

    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());
        actionGroup.rect = QRect(QPoint(0, 0), sizeElem);
        actionGroup.row = -1;
    }

    int x = 0;
    int y = 0;

    int xStart = 0;
    bool isFullHeight = m_rect.bottom() == 32767;
    int cx = size.width();

    if (!m_isCalcWidth && m_mapNonOptWidths.size() > 0 && m_mapNonOptWidths.contains(cx))
    {
        cx = m_mapNonOptWidths.value(cx);
    }

    if (isMixedWidgets())
    {
        for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
        {
            RibbonToolBarActionsGroup &actionGroup = *it;
            QSize sizeElem = calculateSizeHint(actionGroup.items);

            const bool isLargeButton = sizeElem.height() > rowHeight;

            if (isLargeButton)
            {
                QRect rectElem(QPoint(m_rect.left() + x + m_margin, m_rect.top() /*+ m_margin / 2*/),
                               QSize(sizeElem.width(), isFullHeight ? sizeElem.height() : size.height()));

                actionGroup.rect = rectElem;
                actionGroup.row = 999;

                x += sizeElem.width() + m_margin;
                xStart = x;
                y = 0;
            }
        }
    }

    int bottom = 0;
    QVector<int> vecRowWidths;
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());

        if (sizeElem.isNull())
        {
            actionGroup.rect = QRect(0, 0, 0, 0);
            continue;
        }

        if (actionGroup.row != -1)
        {
            continue;
        }

        if (x + sizeElem.width() + m_margin - 1 > cx)
        {
            if (x == xStart)
            {
                showDefaultButton();
                return;
            }

            y += rowHeight;

            if (m_floatyMode)
            {
                y += m_margin / 2;
            }

            vecRowWidths.append(x);

            m_calcRows++;

            x = xStart;
            rowHeight = 0;
        }

        if (y + sizeElem.height() > size.height())
        {
            showDefaultButton();
            return;
        }

        QRect rectElem(QPoint(m_rect.left() + x + m_margin, m_rect.top() + y /*+ m_margin / 2*/), sizeElem);

        actionGroup.rect = rectElem;
        actionGroup.row = m_calcRows;

        rowHeight = qMax(rowHeight, sizeElem.height());
        x += sizeElem.width() + m_margin - 1;

        m_fullWidth = qMax(m_fullWidth, x - 1);

        bottom = qMax(bottom, rectElem.bottom());
    }

    vecRowWidths.append(x);
    m_calcRows++;

    if (isFullHeight)
    {
        m_rect.setBottom(bottom + sizeCaption.height() + m_margin / 2);
        size.setHeight(m_rect.height() - sizeCaption.height() - m_margin / 2);
    }

    int i = 0;
    bool m_preserveElementOrder = false;
    if (m_calcRows > 1 && !m_preserveElementOrder)
    {
        bool recalcFullWidth = false;

        while (true)
        {
            int maxRowWidth = 0;
            int maxRowIndex = -1;

            for (i = 0; i < vecRowWidths.size(); i++)
            {
                if (vecRowWidths[i] > maxRowWidth)
                {
                    maxRowWidth = vecRowWidths[i];
                    maxRowIndex = i;
                }
            }

            if (maxRowIndex < 0)
            {
                break;
            }

            int minWidth = 9999;
            RibbonToolBarActionsGroup *pMinElem = nullptr;

            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
            {
                RibbonToolBarActionsGroup &actionGroup = *it;

                if (actionGroup.row == maxRowIndex)
                {
                    QRect rectElem = actionGroup.rect;

                    if (!rectElem.isEmpty() && rectElem.width() < minWidth)
                    {
                        minWidth = rectElem.width();
                        pMinElem = &actionGroup;
                    }
                }
            }

            if (pMinElem == nullptr)
            {
                break;
            }

            bool bMoved = false;

            for (i = maxRowIndex + 1; i < vecRowWidths.size(); i++)
            {
                if (vecRowWidths[i] + minWidth < maxRowWidth)
                {
                    int x = 0;
                    int y = 0;

                    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
                    {
                        RibbonToolBarActionsGroup &actionGroup = *it;

                        if (actionGroup.row == i)
                        {
                            x = qMax(actionGroup.rect.right() + m_margin, x);
                            y = actionGroup.rect.top();
                        }
                        else if (actionGroup.row == maxRowIndex)
                        {
                            QRect rectElem = actionGroup.rect;

                            if (rectElem.left() > pMinElem->rect.left())
                            {
                                int dx = -(minWidth + m_margin);
                                rectElem.adjust(dx, 0, dx, 0);
                                actionGroup.rect = rectElem;
                            }
                        }
                    }

                    pMinElem->rect = QRect(QPoint(x, y), pMinElem->rect.size());
                    pMinElem->row = i;

                    vecRowWidths[i] += minWidth;
                    vecRowWidths[maxRowIndex] -= minWidth;

                    recalcFullWidth = true;
                    bMoved = true;
                    break;
                }
            }

            if (!bMoved)
            {
                break;
            }
        }

        if (recalcFullWidth)
        {
            m_fullWidth = 0;
            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
            {
                m_fullWidth = qMax(m_fullWidth, (*it).rect.right());
            }
            m_fullWidth -= m_rect.left() + m_margin;
        }
    }

    if (!isFullHeight && !m_floatyMode && m_calcRows > 1)
    {
        int yOffset = (size.height() - m_calcRows * rowHeight) / (m_calcRows + 1);
        if (yOffset > 0)
        {
            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
            {
                RibbonToolBarActionsGroup &actionGroup = *it;

                int row = actionGroup.row;
                QRect rectElem = actionGroup.rect;

                if (row != 999 && !rectElem.isEmpty())
                {
                    int dy = (yOffset * (row + 1) - row);
                    rectElem.adjust(0, dy - 1 /* - 3*/, 0, dy);
                    actionGroup.rect = rectElem;
                }
            }
        }
    }

    if (!m_isCalcWidth)
    {
        updateGeometryWidget();
    }
}

void QdfRibbonToolBarControlPrivate::updateGeometryWidget()
{
    QDF_Q(QdfRibbonToolBarControl);

    if (q->parentGroup() == nullptr)
    {
        return;
    }

    QdfRibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == nullptr)
    {
        return;
    }

    int rowHeight = ribbonBar->rowItemHeight();
    QList<QWidget *> showWidgets, hideWidgets;

    int currentRow = 0;
    QWidget *lastWidget = nullptr;
    QdfRibbonToolBarSeparatorControlPrivate *currentSeparator = nullptr;
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        RibbonToolBarActionsGroup &actionGroup = *it;
        if (currentRow != actionGroup.row && actionGroup.row != 999)
        {
            currentRow = actionGroup.row;
            if (currentSeparator)
            {
                hideWidgets << currentSeparator;
            }
        }
        QList<RibbonToolBarActionGroupItem> items = actionGroup.items;

        QRect rect = actionGroup.rect;
        int x = rect.left();
        for (QList<RibbonToolBarActionGroupItem>::iterator itItem = items.begin(); itItem != items.end(); ++itItem)
        {
            RibbonToolBarActionGroupItem &item = *itItem;
            if (item.widget)
            {
                QSize sizeElem = getBoundedSizeHint(item.widget);
                if (lastWidget)
                {
                    if (QdfRibbonToolBarSeparatorControlPrivate *separatorControl =
                                qobject_cast<QdfRibbonToolBarSeparatorControlPrivate *>(item.widget))
                    {
                        QSize sz = lastWidget->sizeHint();
                        sizeElem.setHeight(sz.height());
                        currentSeparator = separatorControl;
                        showWidgets << separatorControl;
                    }
                    else
                    {
                        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());
                    }
                }
                else
                {
                    sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());
                }

                if (item.widget)
                {
                    lastWidget = item.widget;
                }

                if (item.isEmpty())
                {
                    hideWidgets << item.widget;
                }
                else
                {
                    item.widget->setGeometry(QRect(QPoint(x, rect.top()), sizeElem));
                    showWidgets << item.widget;
                    x += sizeElem.width();
                }
            }
        }
    }

    if (currentSeparator != nullptr && currentSeparator == lastWidget)
    {
        hideWidgets << currentSeparator;
    }

    // we have to do the show/hide here, because it triggers more calls to setGeometry
    for (int i = 0, count = showWidgets.count(); i < count; ++i)
    {
        showWidgets.at(i)->show();
    }

    for (int i = 0, count = hideWidgets.count(); i < count; ++i)
    {
        hideWidgets.at(i)->hide();
    }
}

void QdfRibbonToolBarControlPrivate::updateGroupProperties(RibbonToolBarActionGroupItems &group)
{
    QWidget *firstWidget = nullptr;
    QWidget *lastWidget = nullptr;
    for (RibbonToolBarActionGroupItems::iterator it = group.begin(); it != group.end(); ++it)
    {
        if (it->isEmpty())
        {
            continue;
        }

        if (it->widget == nullptr)
        {
            continue;
        }

        if (it->action->isSeparator())
        {
            continue;
        }

        if (firstWidget == nullptr)
        {
            firstWidget = it->widget;
        }

        lastWidget = it->widget;
        lastWidget->setProperty(_qdf_Begin_Group, firstWidget == it->widget);
        lastWidget->setProperty(_qdf_Group, true);
        lastWidget->setProperty(_qdf_End_Group, false);
    }

    if (lastWidget != nullptr)
    {
        lastWidget->setProperty(_qdf_End_Group, true);
    }
}

QSize QdfRibbonToolBarControlPrivate::calculateSizeHint(const RibbonToolBarActionGroupItems &group) const
{
    QDF_Q(const QdfRibbonToolBarControl);

    if (q->parentGroup() == nullptr)
    {
        return QSize();
    }

    QdfRibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    Q_ASSERT(ribbonBar != nullptr);

    QSize ret = QSize(0, 0);
    for (QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::const_iterator it = group.constBegin();
         it != group.constEnd(); ++it)
    {
        if (it->action->isSeparator())
        {
            if (it->action->isVisible())
            {
                QStyleOption opt;
                opt.init(q);
                const int widthSeparator =
                        q->parentGroup()->style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, q);
                ret.rwidth() += widthSeparator;
            }
            continue;
        }

        Q_ASSERT(it->widget != nullptr);
        if (it->widget == nullptr)
        {
            continue;
        }

        if (/*it->action->isVisible()*/ !it->isEmpty())
        {
            QSize s = getBoundedSizeHint(it->widget);
            s.setHeight(s.height() < ribbonBar->rowItemHeight() ? ribbonBar->rowItemHeight() : s.height());
            ret.rwidth() += s.width();
            ret.rheight() = qMax(ret.height(), s.height());
        }
    }
    return ret;
}

void QdfRibbonToolBarControlPrivate::updateLayoutParent(bool calcLayoutParent)
{
    QDF_Q(QdfRibbonToolBarControl);

    QdfRibbonGroup *parentGroup = q->parentGroup();
    if (parentGroup == nullptr)
    {
        return;
    }

    QdfRibbonBar *ribbonBar = parentGroup->ribbonBar();
    if (ribbonBar == nullptr)
    {
        return;
    }

    recalcWidths(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount());

    for (RibbonToolBarActionGroupList::iterator itGroup = m_groups.begin(); itGroup != m_groups.end(); ++itGroup)
    {
        RibbonToolBarActionsGroup &group = *itGroup;
        for (QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::iterator itItem = group.items.begin();
             itItem != group.items.end(); ++itItem)
        {
            updateGroupProperties(group.items);
        }
    }
    if (calcLayoutParent)
    {
        if (QdfRibbonPage *page = qobject_cast<QdfRibbonPage *>(parentGroup->parentWidget()))
        {
            page->updateLayout();
        }
    }
}

bool QdfRibbonToolBarControlPrivate::isMixedWidgets() const
{
    QDF_Q(const QdfRibbonToolBarControl);
    if (q->parentGroup() == nullptr)
    {
        return false;
    }

    QdfRibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == nullptr)
    {
        return false;
    }

    bool largeButton = false;
    bool smallButton = false;
    for (RibbonToolBarActionGroupList::const_iterator it = m_groups.begin(); it != m_groups.end(); ++it)
    {
        const RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        bool isLargeButton = sizeElem.height() > ribbonBar->rowItemHeight();
        if (isLargeButton)
        {
            largeButton = true;
        }
        else
        {
            smallButton = true;
        }
        if (largeButton && smallButton)
        {
            return true;
        }
    }
    return false;
}

/* QdfRibbonToolBarControl*/
QdfRibbonToolBarControl::QdfRibbonToolBarControl(QdfRibbonGroup *parentGroup) : QdfRibbonControl(parentGroup)
{
    QDF_INIT_PRIVATE(QdfRibbonToolBarControl);
}

QdfRibbonToolBarControl::~QdfRibbonToolBarControl() { QDF_FINI_PRIVATE(); }

void QdfRibbonToolBarControl::sizeChanged(QdfRibbonControlSizeDefinition::GroupSize size)
{
    QdfRibbonControl::sizeChanged(size);
    QDF_D(QdfRibbonToolBarControl);
    if (d->m_vecWidths.isEmpty())
    {
        return;
    }

    switch (size)
    {
        case QdfRibbonControlSizeDefinition::GroupLarge:
        case QdfRibbonControlSizeDefinition::GroupMedium:
            {
                setRowsCount(2);
            }
            break;
        case QdfRibbonControlSizeDefinition::GroupPopup:
            {
                setRowsCount(2);
            }
            break;
        case QdfRibbonControlSizeDefinition::GroupSmall:
            {
                setRowsCount(3);
            }
            break;
        default:
            break;
    }
}

int QdfRibbonToolBarControl::rowsCount()
{
    QDF_D(QdfRibbonToolBarControl);
    return d->m_rowsCount;
}

void QdfRibbonToolBarControl::setRowsCount(int count)
{
    QDF_D(QdfRibbonToolBarControl);
    d->m_rowsCount = count;
}

void QdfRibbonToolBarControl::clear()
{
    QList<QAction *> actions = this->actions();
    for (int i = 0; i < actions.size(); i++)
    {
        removeAction(actions.at(i));
    }
}

QAction *QdfRibbonToolBarControl::addWidget(QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addWidget(const QIcon &icon, const QString &text, QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);
    action->setDefaultWidget(widget);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addMenu(const QIcon &icon, const QString &text, QMenu *menu,
                                          QToolButton::ToolButtonPopupMode mode)
{
    Q_ASSERT(menu != nullptr);
    Q_ASSERT_X(mode != QToolButton::DelayedPopup, "QdfRibbonToolBarControl::addMenu", "The use of mode is not valid");

    if (mode == QToolButton::DelayedPopup)
    {
        return nullptr;
    }

    QAction *action = menu->menuAction();
    action->setIcon(icon);
    menu->setTitle(text);

    QdfRibbonControl::addAction(action);

    QdfRibbonButton *button = qobject_cast<QdfRibbonButton *>(widgetByAction(action));
    if (button != nullptr)
    {
        button->setPopupMode(mode);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                            QToolButton::ToolButtonPopupMode mode)
{
    addAction(action);
    QdfRibbonButton *button = qobject_cast<QdfRibbonButton *>(widgetByAction(action));
    Q_ASSERT(button != nullptr);
    button->setToolButtonStyle(style);
    if (menu || action->menu())
    {
        button->setPopupMode(mode);
    }
    if (menu)
    {
        button->setMenu(menu);
    }
    return action;
}

QAction *QdfRibbonToolBarControl::insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                               QToolButton::ToolButtonPopupMode mode)
{
    insertAction(before, action);
    QdfRibbonButton *button = qobject_cast<QdfRibbonButton *>(widgetByAction(action));
    Q_ASSERT(button != nullptr);
    button->setToolButtonStyle(style);
    if (menu || action->menu())
    {
        button->setPopupMode(mode);
    }
    if (menu)
    {
        button->setMenu(menu);
    }
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(const QString &text)
{
    QAction *action = new QAction(text, this);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(icon, text, this);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(const QString &text, const QObject *receiver, const char *member)
{
    QAction *action = new QAction(text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(const QIcon &icon, const QString &text, const QObject *receiver,
                                            const char *member)
{
    QAction *action = new QAction(icon, text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    QdfRibbonControl::addAction(action);
    return action;
}

QAction *QdfRibbonToolBarControl::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu,
                                            QToolButton::ToolButtonPopupMode mode)
{
    QAction *action = nullptr;
    if (mode == QToolButton::InstantPopup && menu)
    {
        action = menu->menuAction();
        action->setIcon(icon);
        menu->setTitle(text);
    }
    else
    {
        action = new QAction(icon, text, this);
    }

    QdfRibbonControl::addAction(action);

    QdfRibbonButton *button = qobject_cast<QdfRibbonButton *>(widgetByAction(action));
    if (button != nullptr)
    {
        button->setToolButtonStyle(style);
    }

    return action;
}

QAction *QdfRibbonToolBarControl::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    QdfRibbonControl::addAction(action);
    return action;
}

QSize QdfRibbonToolBarControl::sizeHint() const
{
    QDF_D(const QdfRibbonToolBarControl);
    return d->m_sizeHint;
}

QWidget *QdfRibbonToolBarControl::widgetByAction(QAction *action)
{
    QDF_D(QdfRibbonToolBarControl);
    for (QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupList::iterator it = d->m_groups.begin();
         it != d->m_groups.end(); ++it)
    {
        QdfRibbonToolBarControlPrivate::RibbonToolBarActionsGroup &group = *it;
        QdfRibbonToolBarControlPrivate::RibbonToolBarActionGroupItems &items = group.items;
        int index = indexOfAction(items, action);
        if (index != -1)
        {
            return items[index].widget;
        }
    }
    return nullptr;
}

void QdfRibbonToolBarControl::updateLayout()
{
    QDF_D(QdfRibbonToolBarControl);
    if (parentGroup() == NULL)
    {
        return;
    }
    if (d->m_vecWidths.isEmpty())
    {
        return;
    }

    if (QdfRibbonBar *ribbonBar = parentGroup()->ribbonBar())
    {
        QSize sz;
        const int currWidthIndex = rowsCount() == 2 ? 0 : 1;
        const int currPanelWidth = d->m_vecWidths[currWidthIndex] + 2 * d->m_margin;
        sz.setWidth(currPanelWidth);
        const int vertMargin = 3;
        const int height =
                (ribbonBar->rowItemHeight() * ribbonBar->rowItemCount()) + 2 * (d->m_margin / 2) + vertMargin;
        sz.setHeight(height);
        d->updateGroupsGeometry(QRect(QPoint(0, 0), sz));
        d->m_sizeHint = sz;
        d->m_sizeHint.setWidth(sz.width() - d->m_margin / 2);
    }
}

bool QdfRibbonToolBarControl::event(QEvent *event)
{
    QDF_D(QdfRibbonToolBarControl);
    if (d->m_dirty && event->type() == QEvent::LayoutRequest)
    {
        d->updateLayoutParent();
        d->m_dirty = false;
    }
    return QdfRibbonControl::event(event);
}

void QdfRibbonToolBarControl::actionEvent(QActionEvent *event)
{
    QDF_D(QdfRibbonToolBarControl);
    QAction *action = event->action();

    switch (event->type())
    {
        case QEvent::ActionAdded:
            {
                QWidget *widget = nullptr;
                if (!action->isSeparator())
                {
                    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);
                    if (widgetAction)
                    {
                        widget = widgetAction->requestWidget(this);
                    }

                    if (widget == nullptr)
                    {
                        QdfRibbonButton *button = new QdfRibbonButton(this);
                        button->setAutoRaise(true);
                        button->setFocusPolicy(Qt::NoFocus);
                        button->setDefaultAction(action);
                        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
                        // QObject::connect(button, SIGNAL(triggered(QAction*)), parentWidget(),
                        // SIGNAL(actionTriggered(QAction*)));
                        widget = button;
                    }
                    widget->hide();
                }
                else
                {
                    QdfRibbonToolBarSeparatorControlPrivate *separator = new QdfRibbonToolBarSeparatorControlPrivate(this);
                    widget = separator;
                }

                d->addActionToGroup(action, widget);
                d->m_dirty = true;
                break;
            }

        case QEvent::ActionChanged:
            d->m_dirty = true;
            break;
        case QEvent::ActionRemoved:
            {
                d->removeActionFromGroup(action);
                d->m_dirty = true;
            }
            break;
        default:
            break;
    }
}

void QdfRibbonToolBarControl::changeEvent(QEvent *event)
{
    QDF_D(QdfRibbonToolBarControl);
    switch (event->type())
    {
        case QEvent::StyleChange:
            d->updateLayoutParent(false);
            updateLayout();
            break;
        case QEvent::FontChange:
            d->updateLayoutParent(false);
            updateLayout();
            break;
        default:
            break;
    }
    QdfRibbonControl::changeEvent(event);
}

void QdfRibbonToolBarControl::setMargin(int margin)
{
    QDF_D(QdfRibbonToolBarControl);
    d->m_margin = margin;
}

int QdfRibbonToolBarControl::margin() const
{
    QDF_D(const QdfRibbonToolBarControl);
    return d->m_margin;
}
