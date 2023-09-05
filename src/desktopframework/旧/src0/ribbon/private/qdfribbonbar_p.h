#ifndef DESKTOPFRAMEWORK_QDFRIBBONBAR_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONBAR_P_H

#include "../qdfribbontabbar.h"
#include "ribbon/qdfribbonbar.h"
#include <QLabel>
#include <QStack>
#include <QStyle>
#include <qdf_global.h>
#include <ribbon/qdfribboncustomizepage.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonquickaccessbar.h>

QDF_BEGIN_NAMESPACE

class QdfLogotypeLabel : public QLabel
{
public:
    QdfLogotypeLabel(QWidget *parent);

public:
    Qt::AlignmentFlag alignmentLogotype() const { return m_alignment; }
    void setAlignmentLogotype(Qt::AlignmentFlag alignment) { m_alignment = alignment; }

protected:
    virtual void paintEvent(QPaintEvent *p);

private:
    Qt::AlignmentFlag m_alignment;
};

class QdfRibbonTabBar;
class QdfRibbonTitleButton;
class QdfOfficeFrameHelper;
class QdfStyleOptionTitleBar;
class QdfRibbonSystemButton;
class QdfRibbonBackstageCloseButton;
class QdfMenuMinimizedGroups;
class QdfRibbonKeyTip;
class QdfRibbonCustomizeManager;

class QdfRibbonBarPrivate : public QObject
{
public:
    Q_OBJECT
    QDF_DECLARE_PUBLIC(QdfRibbonBar)
public:
    explicit QdfRibbonBarPrivate();
    virtual ~QdfRibbonBarPrivate();

public:
    void init();
    void calcHeightItem();

    void layoutRibbon();
    void layoutTitleButton();
    bool isExistAssociativeTabWithPage(const QdfRibbonTab *tab);
    void layoutContextHeaders();
    void layoutCorner();

    void removeAllContextHeaders();
    bool reduceContextHeaders();

    void updateMinimizedModePage(QdfRibbonPage *page);
    void swapVisiblePages(int index);
    void setVisibleBackstage(bool visible);

    QAction *createSystemButton(const QIcon &icon, const QString &text);

    QdfRibbonPage *insertPage(int indTab, int index);
    void insertPage(int indTab, int index, QdfRibbonPage *page);
    void removePage(int index, bool deletePage = true);
    int indexPage(QdfRibbonPage *page) const;

    bool validIndex(int index) const { return index >= 0 && index < m_listPages.count(); }
    int heightTabs() const;
    int topBorder() const;

    void titleBarOptions(QdfStyleOptionTitleBar &opt) const;
    QStyle::SubControl getSubControl(const QPoint &pos) const;

    QdfRibbonTitleButton *findTitleButton(QStyle::SubControl subControl) const;
    QAction *addTitleButton(QStyle::SubControl subControl, bool add, QRect &rcControl);

    QdfContextHeader *hitTestContextHeaders(const QPoint &point) const;
    QMenu *createContextMenu();

    void showKeyTips(QWidget *w);
    bool hideKeyTips();
    void createKeyTips();
    void createGroupKeyTips();
    void createWidgetKeyTips(QdfRibbonGroup *group, QWidget *widget, const QString &prefix, const QRect &rcGroups,
                             bool visible);
    bool createPopupKeyTips(QMenu *levalMenu);
    void destroyKeyTips();
    void calcPositionKeyTips();
    int rowItemHeight() const;
    int rowItemCount() const;
    int maxGroupHeight() const;
    void createCustomizeDialog();

private Q_SLOTS:
    void processClickedSubControl();
    void toggledCustomizeBar();
    void toggledQuickAccessBarPos();
    void toggledMinimized();
    void currentChanged(int index);

protected:
    bool pressTipCharEvent(const QKeyEvent *key);
    virtual bool eventFilter(QObject *, QEvent *event);

public:
    QRect m_rcPageArea;
    QRect m_rcTabBar;
    int m_currentIndexPage;
    int m_heightTabs;
    QdfRibbonTabBar *m_tabBar;
    QdfRibbonQuickAccessBar *m_quickAccessBar;
    QdfRibbonSystemButton *m_controlSystemButton;
    QVector<QWidget *> m_visibleWidgets;

    QdfOfficeFrameHelper *m_frameHelper;
    QRect m_rcTitle;
    QRect m_rcHeader;
    QRect m_rcTitleText;
    QRect m_rcQuickAccess;

    bool m_showTitleAlways;
    bool m_ribbonBarVisible;
    bool m_minimizationEnabled;
    bool m_minimized;
    bool m_keyTipsComplement;
    bool m_keyTipsEnabled;
    bool m_keyTipsShowing;
    bool m_titleGroupsVisible;
    QdfRibbonBar::QuickAccessBarPosition m_quickAccessBarPosition;
    QdfRibbonBar::TabBarPosition m_tabBarPosition;

    QAction *m_actCloseButton;
    QAction *m_actNormalButton;
    QAction *m_actMaxButton;
    QAction *m_actMinButton;

    QList<QdfContextHeader *> m_listContextHeaders;
    QList<QdfRibbonKeyTip *> m_keyTips;
    QStack<QWidget *> m_levels;
    int m_countKey;

    QStyle::SubControl m_hoveredSubControl;
    QStyle::SubControl m_activeSubControl;
    QString m_windowTitle;

    QdfLogotypeLabel *m_logotypeLabel;

    QPixmap m_pixTitleBackground;
    QdfRibbonCustomizeManager *m_customizeManager;
    QdfRibbonCustomizeDialog *m_customizeDialog;

protected:
    QList<QdfRibbonPage *> m_listPages;
    int m_rowItemHeight;
    int m_rowItemCount;

private:
    Q_DISABLE_COPY(QdfRibbonBarPrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONBAR_P_H