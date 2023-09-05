#ifndef DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_H
#define DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_H

#include <QStatusBar>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonStatusBarSwitchGroupPrivate;
class QDF_EXPORT QdfRibbonStatusBarSwitchGroup : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonStatusBarSwitchGroup();
    virtual ~QdfRibbonStatusBarSwitchGroup();

public:
    void clear();

protected:
    virtual void actionEvent(QActionEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonStatusBarSwitchGroup)
    Q_DISABLE_COPY(QdfRibbonStatusBarSwitchGroup)
};

class QdfRibbonStatusBarPrivate;
class QDF_EXPORT QdfRibbonStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit QdfRibbonStatusBar(QWidget *parent = nullptr);
    virtual ~QdfRibbonStatusBar();

public:
    QAction *addAction(const QString &text);
    QAction *addSeparator();

    void addPermanentAction(QAction *action);

    QRect childrenPermanentRect() const;

public:
    using QWidget::addAction;

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void resizeEvent(QResizeEvent *);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonStatusBar)
    Q_DISABLE_COPY(QdfRibbonStatusBar)
};


QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONSTATUSBAR_H