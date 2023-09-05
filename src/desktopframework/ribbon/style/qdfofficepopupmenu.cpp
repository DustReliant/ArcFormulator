#include "../private/qdfribbonbar_p.h"
#include "../qdfribbon_def.h"
#include <QApplication>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QWidgetAction>
#include <qevent.h>
#include <ribbon/qdfofficepopupmenu.h>
#include <ribbon/qdfribbongallery.h>
#include <ribbon/qdfribbonstyle.h>

QDF_USE_NAMESPACE


QDF_BEGIN_NAMESPACE

class QdfOfficePopupMenuPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfOfficePopupMenu)
public:
    explicit QdfOfficePopupMenuPrivate();

public:
    void init();
    int calcMinimumHeight(bool without = false);
    QWidget *findWidget(const char *nameWidget) const;
    void udateSizeGallery(const QSize &sz);

public:
    QWidget *m_widgetPopup;
    bool m_resizable : 1;
    bool m_pressSizeGrip : 1;
    bool m_showGrip : 1;
    int m_lastWidth;
};

QDF_END_NAMESPACE

QdfOfficePopupMenuPrivate::QdfOfficePopupMenuPrivate()
{
    m_widgetPopup = nullptr;
    m_resizable = false;
    m_pressSizeGrip = false;
    m_showGrip = true;
    m_lastWidth = -1;
}

void QdfOfficePopupMenuPrivate::init()
{
    QDF_Q(QdfOfficePopupMenu);
    q->setProperty(_qdf_PopupBar, true);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    q->setMouseTracking(true);
}

int QdfOfficePopupMenuPrivate::calcMinimumHeight(bool without)
{
    QDF_Q(QdfOfficePopupMenu);

    int height = 11;

    QList<QAction *> listActions = q->actions();
    for (int i = 0; i < listActions.count(); i++)
    {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(listActions.at(i)))
        {
            if (QWidget *w = widgetAction->defaultWidget())
            {
                if (!without)
                {
                    int minHeight = w->minimumSizeHint().height();
                    if (minHeight == -1)
                    {
                        minHeight = w->sizeHint().height();
                    }
                    height += minHeight;
                }
            }
        }
        else
        {
            height += q->actionGeometry(listActions.at(i)).height();
        }
    }
    return height;
}

QWidget *QdfOfficePopupMenuPrivate::findWidget(const char *nameWidget) const
{
    QDF_Q(const QdfOfficePopupMenu);
    const QObjectList &listChildren = q->children();
    for (int i = 0; i < listChildren.size(); ++i)
    {
        QWidget *w = qobject_cast<QWidget *>(listChildren.at(i));

        if (w && w->property(nameWidget).toBool())
        {
            return w;
        }
    }
    return nullptr;
}

void QdfOfficePopupMenuPrivate::udateSizeGallery(const QSize &sz)
{
    QDF_Q(const QdfOfficePopupMenu);
    if (QWidget *w = findWidget(_qdf_WidgetGallery))
    {
        if (!w->isWindow() && !w->isHidden())
        {
            QSize szOld = q->sizeHint();
            QSize sizeWidget = w->geometry().size();
            int offset = szOld.height() - sz.height();
            sizeWidget.setHeight(sizeWidget.height() - offset);
            QdfRibbonGallery *gallery = (QdfRibbonGallery *) w;
            gallery->updatelayout();
            QRect rect(QPoint(0, 0), sizeWidget);
            w->setGeometry(rect.adjusted(1, 1, -1, 1));
        }
    }
}


QdfOfficePopupMenu::QdfOfficePopupMenu(QWidget *parent) : QMenu(parent)
{
    QDF_INIT_PRIVATE(QdfOfficePopupMenu);
    QDF_D(QdfOfficePopupMenu);
    d->init();

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShowBar()));
}

QdfOfficePopupMenu::~QdfOfficePopupMenu()
{
    QDF_FINI_PRIVATE();
}

QdfOfficePopupMenu *QdfOfficePopupMenu::createPopupMenu(QWidget *parent)
{
    QdfOfficePopupMenu *popupBar = new QdfOfficePopupMenu(parent);
    return popupBar;
}

QAction *QdfOfficePopupMenu::addWidget(QWidget *widget)
{
    QDF_D(QdfOfficePopupMenu);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    addAction(action);

    if (widget && widget->property("isResizable").toBool())
    {
        d->m_resizable = true;
    }

    if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(d->findWidget(_qdf_WidgetGallery)))
    {
        gallery->adjustSize();
        gallery->updatelayout();
    }
    return action;
}

void QdfOfficePopupMenu::setGripVisible(bool visible)
{
    QDF_D(QdfOfficePopupMenu);
    d->m_showGrip = visible;
}

bool QdfOfficePopupMenu::isGripVisible() const
{
    QDF_D(const QdfOfficePopupMenu);
    return d->m_showGrip;
}

void QdfOfficePopupMenu::setWidgetBar(QWidget *widget)
{
    QDF_D(QdfOfficePopupMenu);
    Q_ASSERT(widget != nullptr && d->m_widgetPopup == nullptr);
    d->m_widgetPopup = widget;
}

void QdfOfficePopupMenu::aboutToShowBar()
{
    QDF_D(QdfOfficePopupMenu);

    if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(d->findWidget(_qdf_WidgetGallery)))
    {
        if (!gallery->isWindow() && !gallery->isHidden())
        {
            gallery->updatelayout();
        }
    }
}

QSize QdfOfficePopupMenu::sizeHint() const
{
    QSize size = QMenu::sizeHint();

    QDF_D(const QdfOfficePopupMenu);

    int height = 0;
    if (d->m_resizable)
    {
        height = 11;
    }

    QSize sz;
    if (d->m_widgetPopup)
    {
        sz = d->m_widgetPopup->geometry().size();
    }
    else if (QWidget *w = d->findWidget(_qdf_WidgetGallery))
    {
        sz = w->geometry().size();
        if (sz.isNull())
        {
            sz = size;
            sz = QSize(qMin(size.width(), sz.width()), size.height());
        }
    }
    size.setWidth(d->m_lastWidth != -1 ? d->m_lastWidth : sz.width() + 1);
    size.setHeight(size.height() + height);
    return size;
}

void QdfOfficePopupMenu::paintEvent(QPaintEvent *event)
{
    QMenu::paintEvent(event);
    QDF_D(QdfOfficePopupMenu);

    if (d->m_resizable)
    {
        QPainter p(this);
        QStyleOptionSizeGrip opt;
        opt.init(this);
        opt.rect.adjust(1, 0, -1, -1);
        opt.rect.setTop(opt.rect.bottom() - 11);
        style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_PopupSizeGrip, &opt, &p, this);
    }
}

void QdfOfficePopupMenu::mousePressEvent(QMouseEvent *event)
{
    QDF_D(QdfOfficePopupMenu);
    if (d->m_resizable)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            QRect rcResizeGripper(rect());
            rcResizeGripper.setTop(rcResizeGripper.bottom() - 11);
            QRect rcResizeGripperAll = rcResizeGripper;
            rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());
            if (rcResizeGripper.contains(event->pos()))
            {
                if (QWidget *w = d->findWidget(_qdf_WidgetGallery))
                {
                    if (w && !w->isWindow() && !w->isHidden())
                    {
                        w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                    }
                }
                d->m_pressSizeGrip = true;
                return;
            }
            else if (rcResizeGripperAll.contains(event->pos()))
            {
                return;
            }
        }
    }
    QMenu::mousePressEvent(event);
}

void QdfOfficePopupMenu::mouseMoveEvent(QMouseEvent *event)
{
    QDF_D(QdfOfficePopupMenu);

    if (QWidget *w = d->findWidget(_qdf_WidgetGallery))
    {
        QMouseEvent evPress(event->type(), event->pos(), event->globalPos(), event->button(),
                            event->buttons(), event->modifiers());
        QApplication::sendEvent(w, &evPress);
    }

    if (d->m_resizable)
    {
        QRect rcResizeGripper(rect());
        rcResizeGripper.setTop(rcResizeGripper.bottom() - 11);
        rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());

        if (d->m_pressSizeGrip || rcResizeGripper.contains(event->pos()))
        {
            setCursor(Qt::SizeFDiagCursor);
        }
        else
        {
            unsetCursor();
        }

        if (d->m_pressSizeGrip)
        {
            QPoint np(event->globalPos());

            QRect rect = geometry();
            rect.setRight(np.x());
            rect.setBottom(np.y());

            d->m_lastWidth = rect.width();
            setGeometry(rect);
            return;
        }
    }

    if (!d->m_pressSizeGrip)
    {
        QMenu::mouseMoveEvent(event);
    }
}

void QdfOfficePopupMenu::mouseReleaseEvent(QMouseEvent *event)
{
    QDF_D(QdfOfficePopupMenu);
    d->m_pressSizeGrip = false;

    if (QWidget *w = d->findWidget(_qdf_WidgetGallery))
    {
        if (w && !w->isWindow() && !w->isHidden())
        {
            w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        }
    }
    QMenu::mouseReleaseEvent(event);
}

void QdfOfficePopupMenu::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event);
    QDF_D(QdfOfficePopupMenu);
    if (d->m_pressSizeGrip)
    {
        setCursor(Qt::SizeFDiagCursor);
    }
}

void QdfOfficePopupMenu::showEvent(QShowEvent *event)
{
    QDF_D(QdfOfficePopupMenu);
    QMenu::showEvent(event);

    if (d->m_widgetPopup && d->m_resizable)
    {
        setMinimumWidth(d->m_widgetPopup->geometry().width());
        setMinimumHeight(d->calcMinimumHeight());
    }
}

void QdfOfficePopupMenu::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfOfficePopupMenu);
    d->udateSizeGallery(event->size());
    QMenu::resizeEvent(event);
}
