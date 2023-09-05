#ifndef DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_P_H

#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonQuickAccessButton : public QToolButton
{
    Q_OBJECT
public:
    QdfRibbonQuickAccessButton(QWidget *parent = nullptr);
    ~QdfRibbonQuickAccessButton();
    virtual QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Q_DISABLE_COPY(QdfRibbonQuickAccessButton)
};


class QdfQuickAccessAction : public QAction
{
public:
    QdfQuickAccessAction(QObject *p, QAction *srcAction)
        : QAction(srcAction->text(), p), m_srcAction(srcAction)
    {
        setCheckable(true);
        QToolBar *toolBar = qobject_cast<QToolBar *>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(srcAction));
    }
    void update()
    {
        QToolBar *toolBar = qobject_cast<QToolBar *>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(m_srcAction));
        setText(m_srcAction->text());
    }

public:
    QAction *m_srcAction;

private:
    Q_DISABLE_COPY(QdfQuickAccessAction)
};


class QdfQuickAccessActionInvisible : public QAction
{
public:
    QdfQuickAccessActionInvisible(QObject *p, QActionGroup *data) : QAction(p)
    {
        m_data = data;
        setProperty("__qdf_Action_Invisible", QLatin1String("__qdf_Action_Invisible"));
        setVisible(false);
    }

public:
    QActionGroup *m_data;

private:
    Q_DISABLE_COPY(QdfQuickAccessActionInvisible)
};


class QdfRibbonQuickAccessBarPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonQuickAccessBar)
public:
    explicit QdfRibbonQuickAccessBarPrivate();
    virtual ~QdfRibbonQuickAccessBarPrivate();

public:
    void init();
    QdfQuickAccessAction *findQuickAccessAction(QAction *action) const;
    QAction *findBeforeAction(QAction *action) const;
    void updateAction(QAction *action);
    void updateParentLayout() const;
    void setActionVisible(QAction *action, bool visible);

public:
    QdfRibbonQuickAccessButton *m_accessPopup;
    QMenu *m_menu;
    QAction *m_actionAccessPopup;
    QActionGroup *m_customizeGroupAction;
    QList<QAction *> m_actionList;
    bool m_removeAction : 1;
    bool m_customizeAction : 1;

private:
    Q_DISABLE_COPY(QdfRibbonQuickAccessBarPrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONQUICKACCESSBAR_P_H