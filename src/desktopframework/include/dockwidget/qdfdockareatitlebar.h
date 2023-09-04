#ifndef DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_H
#define DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_H

#include <QAbstractButton>
#include <QFrame>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockAreaWidget;
class QdfDockAreaTabBar;
class QdfElidingLabel;

class QdfDockAreaTitleBarPrivate;
class QDF_EXPORT QdfDockAreaTitleBar : public QFrame
{
    Q_OBJECT
public:
    QdfDockAreaTitleBar(QdfDockAreaWidget *parent = nullptr);
    ~QdfDockAreaTitleBar() override;

    QdfDockAreaTabBar *tabBar() const;
    QAbstractButton *button(TitleBarButton which) const;
    QdfElidingLabel *autoHideTitleLabel() const;
    void updateDockWidgetActionsButtons();
    virtual void setVisible(bool Visible) override;
    void insertWidget(int index, QWidget *widget);
    int indexOf(QWidget *widget) const;
    QString titleBarButtonToolTip(TitleBarButton button) const;

public Q_SLOTS:
    void markTabsMenuOutdated();

Q_SIGNALS:
    void tabBarClicked(int index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private Q_SLOTS:
    void onTabsMenuAboutToShow();
    void onCloseButtonClicked();
    void onUndockButtonClicked();
    void onTabsMenuActionTriggered(QAction *action);
    void onCurrentTabChanged(int index);
    void onAutoHideButtonClicked();
    void onAutoHideDockAreaActionClicked();
    void onAutoHideToActionClicked();

private:
    QDF_DECLARE_PRIVATE(QdfDockAreaTitleBar)
    Q_DISABLE_COPY(QdfDockAreaTitleBar)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREATITLEBAR_H