#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_P_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_P_H

#include <QBoxLayout>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockContainerWidget;
class QdfTabsWidget;
class QdfAutoHideSideBar;
class QdfAutoHideSideBarPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfAutoHideSideBar)

    QdfAutoHideSideBarPrivate();
    QdfDockContainerWidget *containerWidget;
    QdfTabsWidget *tabsContainerWidget;
    QBoxLayout *tabsLayout;
    Qt::Orientation orientation;
    SideBarLocation sideTabArea = SideBarLocation::SL_Left;
    bool isHorizontal() const
    {
        return Qt::Horizontal == orientation;
    }
    void handleViewportEvent(QEvent *event);
};

class QdfTabsWidget : public QWidget
{
public:
    QdfTabsWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }

    QdfAutoHideSideBarPrivate *eventHandler;

    virtual QSize minimumSizeHint() const override
    {
        return QWidget::sizeHint();
    }

    virtual bool event(QEvent *e) override
    {
        eventHandler->handleViewportEvent(e);
        return QWidget::event(e);
    }
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_P_H