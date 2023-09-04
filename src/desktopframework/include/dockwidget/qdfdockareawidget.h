#ifndef DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_H
#define DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_H

#include "qdfdockwidget.h"
#include <QAbstractButton>
#include <QFrame>
#include <QRect>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockManager;
class QdfDockContainerWidget;
class QdfAutoHideDockContainer;
class QdfDockAreaTitleBar;

class QdfDockAreaWidgetPrivate;
class QDF_EXPORT QdfDockAreaWidget : public QFrame
{
    Q_OBJECT
public:
    enum eDockAreaFlag
    {
        HideSingleWidgetTitleBar = 0x0001,
        DefaultFlags = 0x0000
    };
    Q_DECLARE_FLAGS(DockAreaFlags, eDockAreaFlag)

    QdfDockAreaWidget(QdfDockManager *manager, QdfDockContainerWidget *widget = nullptr);
    ~QdfDockAreaWidget() override;

    QdfDockManager *dockManager() const;
    QdfDockContainerWidget *dockContainer() const;
    QdfAutoHideDockContainer *autoHideDockContainer() const;

    bool autoHide() const;
    void setAutoHideDockContainer(QdfAutoHideDockContainer *container);
    QRect titleBarGeometry() const;
    QRect contentAreaGeometry() const;
    int dockWidgetsCount() const;
    QList<QdfDockWidget *> dockWidgets() const;
    int openedDockWidgetsCount() const;
    QList<QdfDockWidget *> openedDockWidgets() const;

    QdfDockWidget *dockWidget(int index) const;
    int currentIndex() const;

    int indexOfFirstOpenDockWidget() const;
    QdfDockWidget *currentDockWidget() const;
    void setCurrentDockWidget(QdfDockWidget *dockWidget);

    QdfDockWidget::DockWidgetFeatures features(BitwiseOperator mode = BitwiseOperator::BitwiseAnd) const;
    QAbstractButton *titleBarButton(TitleBarButton which) const;
    virtual void setVisible(bool visible) override;
    void setAllowedAreas(DockWidgetAreas areas);
    DockWidgetAreas allowedAreas() const;
    QdfDockAreaTitleBar *titleBar() const;
    DockAreaFlags dockAreaFlags() const;
    void setDockAreaFlags(DockAreaFlags flags);
    void setDockAreaFlag(eDockAreaFlag flag, bool on);
    bool isCentralWidgetArea() const;
    bool containsCentralWidget() const;
    bool isTopLevelArea() const;

    virtual QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void closeArea();
    void setAutoHide(bool Enable, SideBarLocation location = SideBarLocation::SL_None);
    void toggleAutoHide(SideBarLocation location = SideBarLocation::SL_None);
    void closeOtherAreas();

Q_SIGNALS:
    void tabBarClicked(int index);
    void currentChanging(int index);
    void currentChanged(int index);
    void viewToggled(bool open);

protected:
#ifdef Q_OS_WIN
    virtual bool event(QEvent *event) override;
#endif
    void insertDockWidget(int index, QdfDockWidget *dockWidget, bool activate = true);
    void addDockWidget(QdfDockWidget *dockWidget);
    void removeDockWidget(QdfDockWidget *dockWidget);
    void toggleDockWidgetView(QdfDockWidget *dockWidget, bool open);
    QdfDockWidget *nextOpenDockWidget(QdfDockWidget *dockWidget) const;
    int index(QdfDockWidget *dockWidget);
    void hideAreaWithNoVisibleContent();
    void updateTitleBarVisibility();
    void internalSetCurrentDockWidget(QdfDockWidget *dockWidget);
    void markTitleBarMenuOutdated();
    void updateTitleBarButtonVisibility(bool isTopLevel);

protected Q_SLOTS:
    void toggleView(bool open);

private Q_SLOTS:
    void onTabCloseRequested(int index);
    void reorderDockWidget(int fromIndex, int toIndex);
    void updateAutoHideButtonCheckState();
    void updateTitleBarButtonsToolTips();
    SideBarLocation calculateSideTabBarArea() const;

private:
    void onDockWidgetFeaturesChanged();

private:
    QDF_DECLARE_PRIVATE(QdfDockAreaWidget)
    Q_DISABLE_COPY(QdfDockAreaWidget)
    friend class QdfDockManagerPrivate;
    friend class QdfDockManager;
    friend class QdfDockWidgetPrivate;
    friend class QdfDockWidget;
    friend class QdfDockContainerWidgetPrivate;
    friend class QdfDockContainerWidget;
    friend class QdfAutoHideDockContainer;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREAWIDGET_H