#ifndef DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_H
#define DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_H

#include "qdfribbonbar.h"
#include <QFrame>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QDF_EXPORT QdfRibbonBackstageSeparator : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    QdfRibbonBackstageSeparator(QWidget *parent);
    virtual ~QdfRibbonBackstageSeparator();

public:
    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const;

private:
    Q_DISABLE_COPY(QdfRibbonBackstageSeparator)
};

class QdfRibbonBackstageButtonPrivate;
class QDF_EXPORT QdfRibbonBackstageButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(bool tabStyle READ tabStyle WRITE setTabStyle)
    Q_PROPERTY(bool flatStyle READ flatStyle WRITE setFlatStyle)
public:
    QdfRibbonBackstageButton(QWidget *parent);
    virtual ~QdfRibbonBackstageButton();

public:
    bool tabStyle() const;
    void setTabStyle(bool style);

    bool flatStyle();
    void setFlatStyle(bool flat);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QDF_DECLARE_PRIVATE(QdfRibbonBackstageButton)
    Q_DISABLE_COPY(QdfRibbonBackstageButton)
};

class QDF_EXPORT QdfRibbonBackstagePage : public QWidget
{
    Q_OBJECT
public:
    QdfRibbonBackstagePage(QWidget *parent);
    virtual ~QdfRibbonBackstagePage();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonBackstagePage)
};

class QdfRibbonBackstageViewPrivate;
class QDF_EXPORT QdfRibbonBackstageView : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonBackstageView(QdfRibbonBar *ribbonBar);
    virtual ~QdfRibbonBackstageView();

public:
    bool isClosePrevented() const;
    void setClosePrevented(bool prevent);

    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addSeparator();
    QAction *addPage(QWidget *widget);

    void setActivePage(QWidget *widget);
    QWidget *getActivePage() const;

    QRect actionGeometry(QAction *) const;
    QRect menuGeometry() const;

    using QWidget::addAction;

public Q_SLOTS:
    void open();
Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual bool eventFilter(QObject *object, QEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    friend class QdfRibbonBackstageViewMenu;
    friend class QdfRibbonBar;
    QDF_DECLARE_PRIVATE(QdfRibbonBackstageView)
    Q_DISABLE_COPY(QdfRibbonBackstageView)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONBACKSTAGEVIEW_H