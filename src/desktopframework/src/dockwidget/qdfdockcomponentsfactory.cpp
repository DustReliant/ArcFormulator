#include "qdfdockcomponentsfactory.h"
#include <dockwidget/qdfautohidetab.h>
#include <dockwidget/qdfdockareatabbar.h>
#include <dockwidget/qdfdockareatitlebar.h>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockwidget.h>
#include <dockwidget/qdfdockwidgettab.h>

QDF_USE_NAMESPACE

static std::unique_ptr<QdfDockComponentsFactory> DefaultFactory(new QdfDockComponentsFactory());

QdfDockWidgetTab *QdfDockComponentsFactory::createDockWidgetTab(QdfDockWidget *parent) const
{
    return new QdfDockWidgetTab(parent);
}

QdfAutoHideTab *QdfDockComponentsFactory::createDockWidgetSideTab(QdfDockWidget *parent) const
{
    return new QdfAutoHideTab(parent);
}

QdfDockAreaTabBar *QdfDockComponentsFactory::createDockAreaTabBar(QdfDockAreaWidget *parent) const
{
    return new QdfDockAreaTabBar(parent);
}

QdfDockAreaTitleBar *QdfDockComponentsFactory::createDockAreaTitleBar(QdfDockAreaWidget *parent) const
{
    return new QdfDockAreaTitleBar(parent);
}

const QdfDockComponentsFactory *QdfDockComponentsFactory::instance()
{
    return DefaultFactory.get();
}

void QdfDockComponentsFactory::setInstance(QdfDockComponentsFactory *factory)
{
    DefaultFactory.reset(factory);
}

void QdfDockComponentsFactory::resetDefaultFactory()
{
    DefaultFactory.reset(new QdfDockComponentsFactory());
}