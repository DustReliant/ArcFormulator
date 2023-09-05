#ifndef DESKTOPFRAMEWORK_QDFDOCKWIDGET_P_H
#define DESKTOPFRAMEWORK_QDFDOCKWIDGET_P_H

#include <QAction>
#include <QBoxLayout>
#include <QPointer>
#include <QScrollArea>
#include <QWidget>
#include <dockwidget/qdfdockwidget.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidgetTab;
class QdfAutoHideTab;
class QdfDockWidgetPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockWidget)

    QdfDockWidgetPrivate();
    void showDockWidget();
    void hideDockWidget();
    void updateParentDockArea();
    void closeAutoHideDockWidgetsIfNeeded();
    void setupToolBar();
    void setupScrollArea();
    bool createWidgetFromFactory();


private:
    struct WidgetFactory
    {
        QdfDockWidget::FactoryFunc createWidget;
        QdfDockWidget::InsertMode insertMode;
    };

    QBoxLayout *layout = nullptr;
    QWidget *widget = nullptr;
    QdfDockWidgetTab *tabWidget = nullptr;
    QdfDockWidget::DockWidgetFeatures features = QdfDockWidget::DefaultDockWidgetFeatures;
    QdfDockManager *dockManager = nullptr;
    QdfDockAreaWidget *dockArea = nullptr;
    QAction *toggleViewAction = nullptr;
    bool closed = false;
    QScrollArea *scrollArea = nullptr;
    QToolBar *toolBar = nullptr;
    Qt::ToolButtonStyle toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize toolBarIconSizeDocked = QSize(16, 16);
    QSize toolBarIconSizeFloating = QSize(24, 24);
    bool isFloatingTopLevel = false;
    QList<QAction *> titleBarActions;
    QdfDockWidget::MinimumSizeHintMode minimumSizeHintMode = QdfDockWidget::MinimumSizeHintFromDockWidget;
    WidgetFactory *factory = nullptr;
    QPointer<QdfAutoHideTab> sideTabWidget;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKWIDGET_P_H