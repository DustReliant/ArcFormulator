#ifndef DESKTOPFRAMEWORK_QDFRIBBONPAGE_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONPAGE_P_H

#include "qdfribbon_p.h"
#include <QBasicTimer>
#include <QObject>
#include <qdf_global.h>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribboncontrols.h>
#include <ribbon/qdfribbonpage.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonPagePrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonPage)
public:
    explicit QdfRibbonPagePrivate();
    virtual ~QdfRibbonPagePrivate();

public:
    void init();
    bool validateGroupIndex(int index) const
    {
        return index >= 0 && index < m_listGroups.count();
    }
    void removeGroup(int index, bool deleteGroup);
    int groupIndex(QdfRibbonGroup *page) const;

    int calcReducedGroupsWidth() const;
    void enableGroupScroll(bool scrollLeft, bool scrollRight);
    void showGroupScroll(bool onlyCalc);
    void scrollGroupAnimate();
    void startScrollGropsAnimate(int groupScrollPos, int scrollPosTarget);

    bool calcMinimumWidth(QdfRibbonControlSizeDefinition::GroupSize size) const;

    virtual bool eventFilter(QObject *watched, QEvent *event);
    void listPageWidth(int totalWidth, int realWidth, QList<int> &pagesWidth);

public Q_SLOTS:
    void pressLeftScrollButton();
    void pressRightScrollButton();
    void forcedStopScrollTimer();

public:
    void updateLayout(bool updateScroll = true);
    int calculateGroupsWidth();
    int adjustGroups(int leftOffset, int actualWidth);
    bool isCollapseOrder();
    bool collapseGroups(int &leftOffset, int actualWidth, QdfRibbonControlSizeDefinition::GroupSize size, bool adjust);
    bool expandGroups(int &leftOffset, int actualWidth, QdfRibbonControlSizeDefinition::GroupSize size, bool adjust);
    QdfRibbonControlSizeDefinition::GroupSize getMinGroupSize() const;
    QdfRibbonControlSizeDefinition::GroupSize getMaxGroupSize() const;
    bool canReduce() const;

public:
    QdfRibbonGroupScroll *m_buttonScrollGroupLeft;
    QdfRibbonGroupScroll *m_buttonScrollGroupRight;

    QList<QdfRibbonGroup *> m_listGroups;
    QList<QAction *> m_listShortcuts;
    QdfRibbonTab *m_associativeTab;
    QBasicTimer m_timer;
    QBasicTimer m_scrollTimer;
    QString m_title;
    QString m_contextTitle;
    QString m_contextGroupName;
    QdfRibbonPage::ContextColor m_contextColor;
    Qt::LayoutDirection m_layoutDirection;

    int m_groupsHeight;
    int m_pageScrollPos;
    int m_groupScrollPos;
    int m_scrollPosTarget;
    double m_animationStep;
    int m_timerElapse;

    bool m_minimazeRibbon : 1;
    bool m_doVisiblePage : 1;
    bool m_allowPress : 1;
    bool m_doPopupPage : 1;
    bool m_animation : 1;

private:
    Q_DISABLE_COPY(QdfRibbonPagePrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONPAGE_P_H