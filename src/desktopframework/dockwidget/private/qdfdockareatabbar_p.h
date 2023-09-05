#ifndef DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_P_H
#define DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_P_H

#include <QBoxLayout>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockAreaWidget;
class QdfDockWidgetTab;
class QdfDockAreaTabBarPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockAreaTabBar)
    QdfDockAreaTabBarPrivate() = default;
    void updateTabs();
    QdfDockWidgetTab *firstTab() const;
    QdfDockWidgetTab *lastTab() const;

private:
    QdfDockAreaWidget *dockArea;
    QWidget *tabsContainerWidget;
    QBoxLayout *tabsLayout;
    int currentIndex = -1;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_P_H