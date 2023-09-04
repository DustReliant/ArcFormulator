#ifndef DESKTOPFRAMEWORK_QDFRIBBONTABBAR_H
#define DESKTOPFRAMEWORK_QDFRIBBONTABBAR_H

#include <QMenu>
#include <QWidget>
#include <qdf_global.h>
#include <ribbon/qdfribbonpage.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonTabPrivate;
class QdfRibbonTab;

class QdfContextHeader
{
public:
    QdfContextHeader(QdfRibbonTab *tab);
    virtual ~QdfContextHeader();

public:
    QdfRibbonTab *firstTab;
    QdfRibbonTab *lastTab;
    QdfRibbonPage::ContextColor color;
    QString strTitle;
    QString strGroupName;
    QRect rcRect;
};

class QdfRibbonTab : public QWidget
{
    Q_OBJECT
protected:
    QdfRibbonTab(const QString &text, QWidget *parent);
    virtual ~QdfRibbonTab();

public:
    void setPage(QdfRibbonPage *page);

    void setSelect(bool);
    bool select() const;

    int indent() const;
    void setIndent(int);

    int margin() const;
    void setMargin(int);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment);

    void setTextTab(const QString &);
    const QString &textTab() const;

    void setContextTextTab(const QString &);
    const QString &contextTextTab() const;

    void setContextGroupName(const QString &);
    const QString &contextGroupName() const;

    void setContextTab(QdfRibbonPage::ContextColor color);
    QdfRibbonPage::ContextColor contextColor() const;

    void setContextHeader(QdfContextHeader *contextHeaders);
    QdfContextHeader *getContextHeader() const;

    void setTrackingMode(bool tracking);
    bool isTrackingMode() const;

    void setTabWidth(int width);
    int tabWidth() const;

    QAction *defaultAction() const;

protected:
    int shortcut() const;
    bool validRect() const;

public:
    virtual QSize sizeHint() const;

signals:
    void pressTab(QWidget *widget);

protected:
    virtual void paintEvent(QPaintEvent *event);

    virtual QSize minimumSizeHint() const;

    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);

protected:
    int lastTab;

private:
    friend class QdfRibbonStyle;
    friend class QdfRibbonPage;
    friend class QdfRibbonTabBar;
    friend class QdfRibbonQuickAccessBar;
    friend class QdfRibbonSystemButton;
    friend class QdfRibbonTabBarPrivate;
    QDF_DECLARE_PRIVATE(QdfRibbonTab)
    Q_DISABLE_COPY(QdfRibbonTab)
};

class QdfRibbonPage;
class QdfRibbonSystemPopupBar;
class QdfRibbonTabBarPrivate;
/* QdfRibbonTabBar */
class QdfRibbonTabBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(
            SelectionBehavior selectionBehaviorOnRemove READ selectionBehaviorOnRemove WRITE setSelectionBehaviorOnRemove)
protected:
    QdfRibbonTabBar(QWidget *parent);
    virtual ~QdfRibbonTabBar();

    enum SelectionBehavior
    {
        SelectLeftTab,
        SelectRightTab,
        SelectPreviousTab
    };

protected:
    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon &icon, const QString &text);

    void moveTab(int index, int newIndex);
    void removeTab(int index);

    int currentIndex() const;

    QdfRibbonTab *getTab(int nIndex) const;
    int getTabCount() const;
    int getIndex(const QdfRibbonTab *tab) const;

    SelectionBehavior selectionBehaviorOnRemove() const;
    void setSelectionBehaviorOnRemove(SelectionBehavior behavior);

    void setEnabledWidgets(bool enabled);

protected:
    QAction *addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu = nullptr);
    QAction *addAction(QAction *action, Qt::ToolButtonStyle style);
    QMenu *addMenu(const QString &title);

public Q_SLOTS:
    void setCurrentIndex(int index);
    void currentNextTab(bool next);

private Q_SLOTS:
    void activateTab(QWidget *widget);

Q_SIGNALS:
    void currentChanged(int index);
    void activating(bool &allow);

protected:
    void layoutWidgets();
    void refresh();
    bool validWidth() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    friend class QdfRibbonTab;
    friend class QdfRibbonBar;
    friend class QdfRibbonBarPrivate;
    friend class QdfRibbonPaintManager;
    QDF_DECLARE_PRIVATE(QdfRibbonTabBar)
    Q_DISABLE_COPY(QdfRibbonTabBar)
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONTABBAR_H