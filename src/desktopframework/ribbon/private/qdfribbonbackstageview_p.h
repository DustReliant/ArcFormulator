#ifndef DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_P_H

#include <QStyleOptionMenuItem>
#include <QToolButton>
#include <QWidget>
#include <QWidgetAction>
#include <qdf_global.h>
#include <ribbon/qdfribbonbackstageview.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonBackstageButtonPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonBackstageButton)
public:
    explicit QdfRibbonBackstageButtonPrivate();
    virtual ~QdfRibbonBackstageButtonPrivate();

public:
    bool m_tabStyle;
    bool m_flatStyle;
};


class QdfRibbonBackstageCloseButton : public QToolButton
{
public:
    QdfRibbonBackstageCloseButton(QWidget *parent);

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonBackstageCloseButton)
};

class QdfStyleOptionRibbonBackstage;
class QdfRibbonBackstageViewMenu : public QWidget
{
public:
    QdfRibbonBackstageViewMenu(QdfRibbonBackstageView *backstageView, QWidget *parent = nullptr);

public:
    void createBackstageCloseButton();
    QAction *actionAt(const QPoint &p) const;
    QRect actionRect(QAction *act) const;
    void resetItemsDirty();

public:
    virtual QSize sizeHint() const;

protected:
    void initStyleOption(QdfStyleOptionRibbonBackstage *opt) const;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;
    void updateActionRects() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    QdfRibbonBackstageView *m_backstageView;
    QdfRibbonBackstageCloseButton *m_backStageCloseButton;
    bool m_mouseDown;

    mutable bool m_itemsDirty;
    mutable int m_menuWidth;
    mutable int m_maxIconWidth;
    mutable QVector<QRect> m_actionRects;

private:
    Q_DISABLE_COPY(QdfRibbonBackstageViewMenu)
};

class QdfRibbonBar;
class QdfRibbonBackstageViewPrivate : public QObject
{
public:
    Q_OBJECT
    QDF_DECLARE_PUBLIC(QdfRibbonBackstageView)
public:
    explicit QdfRibbonBackstageViewPrivate();
    virtual ~QdfRibbonBackstageViewPrivate();

public:
    void init(QdfRibbonBar *ribbonBar);
    void layoutBackstage();
    void updateLayout();
    void updateGeometryPage(QWidget *widget);

    void setCurrentAction(QAction *action);
    QAction *currentAction() const;

    QWidgetAction *getAction(QWidget *w) const;

public:
    QdfRibbonBar *m_ribbon;
    QdfRibbonBackstageViewMenu *m_backstageMenu;
    QdfRibbonBar::TabBarPosition m_tabBarPosition;
    QWidget *m_activePage;
    QAction *m_currentAction;

    bool m_closePrevented;
    mutable QVector<QRect> m_actionRects;
    mutable QHash<QAction *, QWidget *> m_widgetItems;

private:
    Q_DISABLE_COPY(QdfRibbonBackstageViewPrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_P_H