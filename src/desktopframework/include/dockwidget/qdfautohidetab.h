#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDETAB_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDETAB_H

#include <QPushButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfAutoHideSideBar;

class QdfAutoHideTabPrivate;
class QDF_EXPORT QdfAutoHideTab : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int sideBarLocation READ sideBarLocation)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)
    Q_PROPERTY(bool activeTab READ isActiveTab)
    Q_PROPERTY(bool iconOnly READ iconOnly)

public:
    enum Orientation
    {
        Horizontal,
        VerticalTopToBottom,
        VerticalBottomToTop
    };

    QdfAutoHideTab(QWidget *parent = nullptr);
    virtual ~QdfAutoHideTab();
    void updateStyle();
    SideBarLocation sideBarLocation() const;
    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    bool isActiveTab() const;
    QdfDockWidget *dockWidget() const;
    void setDockWidget(QdfDockWidget *dockWidget);
    bool iconOnly() const;
    QdfAutoHideSideBar *sideBar() const;

    virtual QSize sizeHint() const override;
    Orientation buttonOrientation() const;
    void setButtonOrientation(Orientation orientation);

protected:
    void setSideBar(QdfAutoHideSideBar *sideTabBar);
    void removeFromSideBar();
    virtual bool event(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    friend class QdfDockWidget;
    friend class QdfAutoHideDockContainer;
    friend class QdfAutoHideSideBar;
    friend class QdfDockAreaWidget;
    friend class QdfDockContainerWidget;
    QDF_DECLARE_PRIVATE(QdfAutoHideTab)
    Q_DISABLE_COPY(QdfAutoHideTab)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDETAB_H