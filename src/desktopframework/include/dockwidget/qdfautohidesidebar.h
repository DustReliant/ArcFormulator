#ifndef DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_H
#define DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_H

#include <QScrollArea>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfAutoHideTab;
class QdfDockContainerWidget;
class QdfDockWidget;
class QdfAutoHideDockContainer;
class QdfDockContainerWidget;

class QdfAutoHideSideBarPrivate;
class QDF_EXPORT QdfAutoHideSideBar : public QScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int sideBarLocation READ sideBarLocation)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)

public:
    QdfAutoHideSideBar(QdfDockContainerWidget *parent, SideBarLocation area);
    virtual ~QdfAutoHideSideBar();
    void removeTab(QdfAutoHideTab *SideTab);
    QdfAutoHideDockContainer *insertDockWidget(int index, QdfDockWidget *DockWidget);
    void removeAutoHideWidget(QdfAutoHideDockContainer *AutoHideWidget);
    void addAutoHideWidget(QdfAutoHideDockContainer *AutoHideWidget);
    Qt::Orientation orientation() const;
    QdfAutoHideTab *tabAt(int index) const;
    int tabCount() const;
    SideBarLocation sideBarLocation() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
    int spacing() const;
    void setSpacing(int Spacing);
    QdfDockContainerWidget *dockContainer() const;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void insertTab(int index, QdfAutoHideTab *SideTab);

private:
    QDF_DECLARE_PRIVATE(QdfAutoHideSideBar)
    Q_DISABLE_COPY(QdfAutoHideSideBar)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFAUTOHIDESIDEBAR_H