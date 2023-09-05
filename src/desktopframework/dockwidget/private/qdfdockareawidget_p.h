#ifndef DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_P_H
#define DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_P_H

#include <QBoxLayout>
#include <QList>
#include <QRect>
#include <QWidget>
#include <dockwidget/qdfdockareawidget.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

static const DockWidgetAreas DefaultAllowedAreas = AllDockAreas;

class QdfDockAreaLayout;

class QdfDockWidget;
class QdfDockWidgetTab;
class QdfDockManager;
class QdfDockAreaTitleBar;
class QdfDockAreaTabBar;
class QdfAutoHideDockContainer;
class QdfDockAreaWidgetPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockAreaWidget)

    QdfDockAreaWidgetPrivate();
    ~QdfDockAreaWidgetPrivate();
    void createTitleBar();
    QdfDockWidget *dockWidgetAt(int index);
    QdfDockWidgetTab *tabWidgetAt(int index);
    QAction *dockWidgetTabAction(QdfDockWidget *dockWidget) const;
    int dockWidgetIndex(QdfDockWidget *dockWidget) const;
    QdfDockAreaTabBar *tabBar() const;
    void updateTitleBarButtonStates();
    void updateTitleBarButtonVisibility(bool isTopLevel);
    void updateMinimumSizeHint();

private:
    QBoxLayout *pLayout = nullptr;
    QdfDockAreaLayout *contentsLayout = nullptr;
    QdfDockAreaTitleBar *titleBar = nullptr;
    QdfDockManager *dockManager = nullptr;
    QdfAutoHideDockContainer *autoHideDockContainer = nullptr;
    bool updateTitleBarButtons = false;
    DockWidgetAreas allowedAreas = DefaultAllowedAreas;
    QSize minSizeHint;
    QdfDockAreaWidget::DockAreaFlags flags{QdfDockAreaWidget::DefaultFlags};
};


class QdfDockAreaLayout
{
public:
    QdfDockAreaLayout(QBoxLayout *parent);
    int count() const;
    void insertWidget(int index, QWidget *widget);
    void removeWidget(QWidget *widget);
    QWidget *currentWidget() const;
    void setCurrentIndex(int index);
    int currentIndex() const;
    bool isEmpty() const;
    int indexOf(QWidget *widget) const;
    QWidget *widget(int index) const;
    QRect geometry() const;

private:
    QBoxLayout *m_parentLayout;
    QList<QWidget *> m_widgets;
    int m_currentIndex = -1;
    QWidget *m_currentWidget = nullptr;
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_P_H