#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_H

#include <QPushButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfAutoHideTab;
class QdfAutoHideSideBar;
class QdfDockAreaWidget;
class QdfDockContainerWidget;
class QdfAutoHideDockContainerPrivate;
class QDF_EXPORT QdfAutoHideDockContainer : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int sideBarLocation READ sideBarLocation)
public:
    QdfAutoHideDockContainer(QdfDockWidget *DockWidget, SideBarLocation area,
                             QdfDockContainerWidget *parent = nullptr);
    virtual ~QdfAutoHideDockContainer();
    QdfAutoHideSideBar *sideBar() const;
    QdfAutoHideTab *autoHideTab() const;
    QdfDockWidget *dockWidget() const;
    void addDockWidget(QdfDockWidget *DockWidget);
    SideBarLocation sideBarLocation() const;
    void setSideBarLocation(SideBarLocation SideBarLocation);
    QdfDockAreaWidget *dockAreaWidget() const;
    QdfDockContainerWidget *dockContainer() const;
    void moveContentsToParent();
    void cleanupAndDelete();
    void toggleView(bool Enable);
    void collapseView(bool Enable);
    void toggleCollapseState();
    void setSize(int Size);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual bool event(QEvent *event) override;
    void updateSize();

private:
    QDF_DECLARE_PRIVATE(QdfAutoHideDockContainer)
    Q_DISABLE_COPY(QdfAutoHideDockContainer)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDEDOCKCONTAINER_H