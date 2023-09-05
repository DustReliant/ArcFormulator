#ifndef DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_P_H

#include <QAction>
#include <QEvent>
#include <QStyleOptionMenuItem>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfEventParent : public QEvent
{
public:
    QdfEventParent(QWidget *parent) : QEvent(QEvent::ParentChange) { m_parent = parent; }
    QWidget *m_parent;
};

class QdfRibbonSystemButtonPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonSystemButton)
public:
    explicit QdfRibbonSystemButtonPrivate() { m_widget = nullptr; }

public:
    QWidget *m_widget;
};

class QdfSysPopupRibbonButton : public QToolButton
{
public:
    QdfSysPopupRibbonButton(QWidget *parent);
    virtual ~QdfSysPopupRibbonButton();

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    Q_DISABLE_COPY(QdfSysPopupRibbonButton);
};

class QdfRibbonSystemPopupBarPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonSystemPopupBar)
public:
    explicit QdfRibbonSystemPopupBarPrivate();

public:
    QRect m_rcBorders;
    QList<QdfSysPopupRibbonButton *> m_systemButtonList;
    QList<QWidget *> m_pageList;
};

class QdfRibbonPageSystemRecentFileListPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonPageSystemRecentFileList)
public:
    explicit QdfRibbonPageSystemRecentFileListPrivate();

public:
    void updateActionRects() const;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;
    QAction *actionAt(const QPoint &p) const;
    QRect actionRect(QAction *act) const;
    void setCurrentAction(QAction *currentAction);
    void activateAction(QAction *action, QAction::ActionEvent action_e, bool self = true);

public:
    bool m_itemsDirty;
    bool m_mouseDown;
    QAction *m_currentAction;
    QList<QAction *> m_recentFileActs;// contents of the MRU list

    mutable bool m_hasCheckableItems;
    mutable QVector<QRect> m_actionRects;
};

class QdfRibbonPageSystemPopupPrivate : public QObject
{
    QDF_DECLARE_PUBLIC(QdfRibbonPageSystemPopup)
public:
    explicit QdfRibbonPageSystemPopupPrivate() { m_parentMenu = nullptr; }

public:
    QWidget *m_parentMenu;
    QString m_caption;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_P_H