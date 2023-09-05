#ifndef DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_P_H
#define DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_P_H

#include <QBoxLayout>
#include <QFrame>
#include <QMenu>
#include <QPoint>
#include <QPointer>
#include <QToolButton>
#include <dockwidget/qdfdockmanager.h>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfTitleBarButton : public QToolButton
{
    Q_OBJECT
public:
    QdfTitleBarButton(bool visible = true, QWidget *parent = nullptr);
    virtual void setVisible(bool visible) override;

protected:
    bool event(QEvent *event) override;

private:
    bool Visible = true;
    bool HideWhenDisabled = false;
};

class QdfSpacerWidget : public QWidget
{
    Q_OBJECT
public:
    QdfSpacerWidget(QWidget *parent = 0);
    virtual QSize sizeHint() const override
    {
        return QSize(0, 0);
    }
    virtual QSize minimumSizeHint() const override
    {
        return QSize(0, 0);
    }
};

class QdfFloatingWidget;
class QdfDockAreaTitleBar;
class QdfDockAreaWidget;
class QdfDockAreaTabBar;
class QdfElidingLabel;

class QdfDockAreaTitleBarPrivate
{
public:
    QDF_DECLARE_PUBLIC(QdfDockAreaTitleBar)

    QdfDockAreaTitleBarPrivate() = default;
    void createButtons();
    void createAutoHideTitleLabel();
    void createTabBar();
    QdfDockManager *dockManager() const;
    static bool testConfigFlag(QdfDockManager::ConfigFlag flag);
    static bool testAutoHideConfigFlag(QdfDockManager::AutoHideFlag flag);
    bool isDraggingState(DragState state) const;


    void startFloating(const QPoint &offset);
    QdfFloatingWidget *makeAreaFloating(const QPoint &offset, DragState state);
    QAction *createAutoHideToAction(const QString &title, SideBarLocation location, QMenu *menu);

private:
    QPointer<QToolButton> tabsMenuButton;
    QPointer<QToolButton> autoHideButton;
    QPointer<QToolButton> undockButton;
    QPointer<QToolButton> closeButton;
    QBoxLayout *pLayout;
    QdfDockAreaWidget *dockArea;
    QdfDockAreaTabBar *tabBar;
    QdfElidingLabel *autoHideTitleLabel;
    bool menuOutdated = true;
    QMenu *tabsMenu;
    QList<QToolButton *> dockWidgetActionsButtons;

    QPoint dragStartMousePos;
    DragState dragState = DragState::DS_Inactive;
    QdfFloatingWidget *pFloatingWidget = nullptr;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_P_H