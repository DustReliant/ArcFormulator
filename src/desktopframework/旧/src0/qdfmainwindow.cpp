#include "ribbon/style/qdfofficeframehelper.h"
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <dockwidget/qdfdockmanager.h>
#include <qdfmainwindow.h>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribbonstyle.h>

QDF_BEGIN_NAMESPACE

class QdfMainWindowPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfMainWindow)
public:
    explicit QdfMainWindowPrivate()
    {
        m_frameHelper = nullptr;
        m_dockManager = nullptr;
    }

    void initDockArea()
    {
        QDF_Q(QdfMainWindow)
        m_dockManager = new QdfDockManager(q);
    }

public:
    QdfDockManager *m_dockManager;
    QdfOfficeFrameHelper *m_frameHelper;
    bool m_attrOpaquePaintEvent;
    bool m_attrNoSystemBackground;
};


QdfMainWindow::QdfMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    QdfRibbonBar::loadTranslation();
    QDF_INIT_PRIVATE(QdfMainWindow);
    setObjectName("QdfMainWindow");
    QDF_D(QdfMainWindow);
    d->m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
    d->m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
}

QdfMainWindow::~QdfMainWindow() { QDF_FINI_PRIVATE(); }

QdfRibbonBar *QdfMainWindow::ribbonBar() const
{
    QdfRibbonBar *ribbonBar = nullptr;

    QWidget *menu = menuWidget();
    if (menu)
    {
        ribbonBar = qobject_cast<QdfRibbonBar *>(menu);
    }

    if (!menu && !ribbonBar)
    {
        QdfMainWindow *self = const_cast<QdfMainWindow *>(this);
        ribbonBar = new QdfRibbonBar(self);
        self->setMenuWidget(ribbonBar);
    }
    return ribbonBar;
}

void QdfMainWindow::setRibbonBar(QdfRibbonBar *ribbonBar) { setMenuWidget(ribbonBar); }

QdfDockManager *QdfMainWindow::dockManager() const
{
    QDF_D(const QdfMainWindow)
    return d->m_dockManager;
}

void QdfMainWindow::setDockManager(QdfDockManager *manager)
{
    QDF_D(QdfMainWindow)
    d->m_dockManager = manager;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void QdfMainWindow::setCentralWidget(QStyle *style)
{
    Q_UNUSED(style);
}
#endif

void QdfMainWindow::setFrameHelper(QdfOfficeFrameHelper *helper)
{
    QDF_D(QdfMainWindow);
    d->m_frameHelper = helper;

    if (helper)
    {
        d->m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
        d->m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
    }
    else
    {
        setAttribute(Qt::WA_OpaquePaintEvent, d->m_attrOpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground, d->m_attrNoSystemBackground);
    }
}

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool QdfMainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
    #else
bool QdfMainWindow::winEvent(MSG *message, long *result)
    #endif
{
    QDF_D(QdfMainWindow);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d->m_frameHelper)
    {
        MSG *msg = static_cast<MSG *>(message);
        if (d->m_frameHelper->winEvent(msg, result))
        {
            return true;
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
    #else
    if (d->m_frameHelper)
    {
        return d->m_frameHelper->winEvent(message, result);
    }

    return QMainWindow::winEvent(message, result);
    #endif
}
#endif


QDF_END_NAMESPACE