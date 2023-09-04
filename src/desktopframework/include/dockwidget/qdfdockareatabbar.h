#ifndef DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_H
#define DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_H

#include <QScrollArea>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockAreaWidget;
class QdfDockWidgetTab;

class QdfDockAreaTabBarPrivate;
class QDF_EXPORT QdfDockAreaTabBar : public QScrollArea
{
    Q_OBJECT
public:
    QdfDockAreaTabBar(QdfDockAreaWidget *parent = nullptr);
    virtual ~QdfDockAreaTabBar() override;

    void insertTab(int index, QdfDockWidgetTab *tab);
    void removeTab(QdfDockWidgetTab *tab);

    int count() const;
    int currentIndex() const;

    QdfDockWidgetTab *currentTab() const;
    QdfDockWidgetTab *tab(int index) const;
    bool isTabOpen(int index) const;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void closeTab(int index);

protected:
    void wheelEvent(QWheelEvent *event);

Q_SIGNALS:
    void currentChanging(int index);
    void currentChanged(int index);
    void tabBarClicked(int index);
    void tabCloseRequested(int index);
    void tabClosed(int index);
    void tabOpened(int index);
    void tabMoved(int from, int to);
    void removingTab(int index);
    void tabInserted(int index);
    void elidedChanged(bool elided);

private Q_SLOTS:
    void onTabClicked();
    void onTabCloseRequested();
    void onCloseOtherTabsRequested();
    void onTabWidgetMoved(const QPoint &globalPos);

private:
    friend class QdfDockAreaTitleBar;
    QDF_DECLARE_PRIVATE(QdfDockAreaTabBar)
    Q_DISABLE_COPY(QdfDockAreaTabBar)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKAREATABBAR_H