#ifndef DESKTOPFRAMEWORK_QDFDOCKCOMPONENTSFACTORY_H
#define DESKTOPFRAMEWORK_QDFDOCKCOMPONENTSFACTORY_H

#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidgetTab;
class QdfDockAreaTitleBar;
class QdfDockAreaTabBar;
class QdfDockAreaWidget;
class QdfDockWidget;
class QdfAutoHideTab;

class QdfDockComponentsFactory
{
public:
    virtual ~QdfDockComponentsFactory()
    {
    }
    virtual QdfDockWidgetTab *createDockWidgetTab(QdfDockWidget *parent) const;
    virtual QdfAutoHideTab *createDockWidgetSideTab(QdfDockWidget *parent) const;
    virtual QdfDockAreaTabBar *createDockAreaTabBar(QdfDockAreaWidget *parent) const;
    virtual QdfDockAreaTitleBar *createDockAreaTitleBar(QdfDockAreaWidget *parent) const;
    static const QdfDockComponentsFactory *instance();
    static void setInstance(QdfDockComponentsFactory *factory);
    static void resetDefaultFactory();
};

QDF_END_NAMESPACE

#endif// DESKTOPFRAMEWORK_QDFDOCKCOMPONENTSFACTORY_H