#ifndef DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_H
#define DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_H

#include <QFrame>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockWidget;
class QdfDockAreaWidget;
class QdfDockManager;

class QdfDockWidgetTabPrivate;
class QDF_EXPORT QdfDockWidgetTab : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

public:
    QdfDockWidgetTab(QdfDockWidget *DockWidget, QWidget *parent = 0);
    virtual ~QdfDockWidgetTab() override;

    bool isActiveTab() const;
    void setActiveTab(bool active);
    void setDockAreaWidget(QdfDockAreaWidget *dockArea);
    QdfDockAreaWidget *dockAreaWidget() const;
    QdfDockWidget *dockWidget() const;

    void setIcon(const QIcon &icon);
    const QIcon &icon() const;

    QString text() const;
    void setText(const QString &title);

    bool isTitleElided() const;
    bool isClosable() const;

    virtual bool event(QEvent *event) override;
    void setElideMode(Qt::TextElideMode mode);
    void updateStyle();
    QSize iconSize() const;
    void setIconSize(const QSize &Size);

public Q_SLOTS:
    virtual void setVisible(bool visible) override;

Q_SIGNALS:
    void activeTabChanged();
    void clicked();
    void closeRequested();
    void closeOtherTabsRequested();
    void moved(const QPoint &pos);
    void elidedChanged(bool elided);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private Q_SLOTS:
    void detachDockWidget();
    void autoHideDockWidget();
    void onAutoHideToActionClicked();

private:
    void onDockWidgetFeaturesChanged();

private:
    QDF_DECLARE_PRIVATE(QdfDockWidgetTab)
    Q_DISABLE_COPY(QdfDockWidgetTab)
    friend class QdfDockWidget;
    friend class QdfDockManager;
    friend class QdfAutoHideDockContainer;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKWIDGETTAB_H