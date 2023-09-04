#include <QChildEvent>
#include <QDebug>
#include <QVariant>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdocksplitter.h>

QDF_BEGIN_NAMESPACE

class QdfDockSplitterPrivate
{
    int VisibleContentCount = 0;

public:
    QDF_DECLARE_PUBLIC(QdfDockSplitter)

    QdfDockSplitterPrivate() = default;
};

QdfDockSplitter::QdfDockSplitter(QWidget *parent)
    : QSplitter(parent)
{
    setProperty("splitter", QVariant(true));
    setChildrenCollapsible(false);
}

QdfDockSplitter::QdfDockSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent){
              QDF_INIT_PRIVATE(QdfDockSplitter)}

      QdfDockSplitter::~QdfDockSplitter()
{
    QDF_FINI_PRIVATE()
}

bool QdfDockSplitter::hasVisibleContent() const
{
    for (int i = 0; i < count(); ++i)
    {
        if (!widget(i)->isHidden())
        {
            return true;
        }
    }

    return false;
}

QWidget *QdfDockSplitter::firstWidget() const
{
    return (count() > 0) ? widget(0) : nullptr;
}

QWidget *QdfDockSplitter::lastWidget() const
{
    return (count() > 0) ? widget(count() - 1) : nullptr;
}

bool QdfDockSplitter::isResizingWithContainer() const
{
    for (auto &area: findChildren<QdfDockAreaWidget *>())
    {
        if (area->isCentralWidgetArea())
        {
            return true;
        }
    }

    return false;
}

QDF_END_NAMESPACE