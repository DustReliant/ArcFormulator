#ifndef DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_P_H

#include <QSize>
#include <QSizeGrip>
#include <QStyleOptionToolButton>
#include <QToolButton>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfStatusButton : public QToolButton
{
public:
    QdfStatusButton(QWidget *parent = nullptr);

public:
    virtual QSize sizeHint() const;

public:
    void initStyleOptionButton(QStyleOptionToolButton &option) const;

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *);
};

class QdfStatusSeparator : public QToolButton
{
public:
    QdfStatusSeparator(QWidget *parent = nullptr);

public:
    virtual QSize sizeHint() const;

protected:
    void initStyleOption(QStyleOption *option) const;

protected:
    virtual void paintEvent(QPaintEvent *);
};

class QdfRibbonStatusBarSwitchGroupPrivate : public QObject
{
    QDF_DECLARE_PUBLIC(QdfRibbonStatusBarSwitchGroup)
public:
    explicit QdfRibbonStatusBarSwitchGroupPrivate();

public:
    void init();
    QdfStatusButton *createButton(QAction *action) const;
    void updateIndexesButtons();

public:
    QHBoxLayout *m_layout;
};

class QdfRibbonStatusBarPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonStatusBar)
public:
    explicit QdfRibbonStatusBarPrivate();

public:
    QLayout *findLayout();
    void updateLayout();
    QRect childrenPermanentRect() const;
    QdfStatusButton *createButton(QAction *action) const;
    QdfStatusSeparator *createSeparator(QAction *action) const;

public:
    bool m_dirty;
    bool m_addPermanentAction;
    int m_indexGroupAction;
    QSizeGrip *m_sizeGripWidget;
    QList<QWidget *> m_widgets;
    QList<QWidget *> m_permanentWidgets;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_P_H