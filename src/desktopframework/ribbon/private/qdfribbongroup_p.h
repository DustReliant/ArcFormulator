#ifndef DESKTOPFRAMEWORK_QDFRIBBONGROUP_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONGROUP_P_H

#include <QAction>
#include <QIcon>
#include <QStyleOptionGroupBox>
#include <QToolButton>
#include <QWidgetAction>
#include <qdf_global.h>
#include <ribbon/qdfribboncontrols.h>
#include <ribbon/qdfribbongroup.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonGroupOption;
class QdfRibbonGroupScroll;
class QdfRibbonControl;
class QdfRibbonGroupPopup;

class QdfRibbonButtonAction : public QWidgetAction
{
public:
    explicit QdfRibbonButtonAction(QObject *parent, const QIcon &icon, const QString &text,
                                   Qt::ToolButtonStyle style, QMenu *menu,
                                   QToolButton::ToolButtonPopupMode mode = QToolButton::DelayedPopup);
    virtual ~QdfRibbonButtonAction();

protected:
    virtual QWidget *createWidget(QWidget *parent);

private:
    Qt::ToolButtonStyle m_style;
    QToolButton::ToolButtonPopupMode m_mode;
};

class QdfRibbonGroupPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonGroup)
public:
    explicit QdfRibbonGroupPrivate();
    virtual ~QdfRibbonGroupPrivate();

public:
    void init();
    void clear(bool deleteControls);
    void enableGroupScroll(bool scrollLeft, bool scrollRight);
    void showGroupScroll();

    void initStyleOption(QStyleOptionGroupBox &opt) const;
    int calcMinimumWidth(QdfRibbonControlSizeDefinition::GroupSize size) const;
    int arrangeRowContent(QList<QdfRibbonControl *> &row, int leftOffset, int rowItemHeight,
                          int rowItemCount, QWidget *parent) const;
    void reposition(const QRect &rect);
    void updateOptionButtonLayout();
    QSize updateControlsLayout(QWidget *parent, int leftOffset = 0) const;
    void adjustCurrentSize(bool expand);
    void updateLayoutParent(bool first);
    QSize sizeHint() const;

    void setReductionSize(QdfRibbonControlSizeDefinition::GroupSize size);
    void reduce();
    void expand();
    bool canReduce();
    bool canExpand();

public Q_SLOTS:
    void pressLeftScrollButton();
    void pressRightScrollButton();
    void hidePopupMode(QAction *action);

public:
    static bool visualIndexLessThan(QdfRibbonControl *first, QdfRibbonControl *second);
    QdfRibbonGroupOption *m_optionButton;
    QdfRibbonGroupScroll *m_buttonScrollGroupLeft; // Button to draw left scroll
    QdfRibbonGroupScroll *m_buttonScrollGroupRight;// Button to draw right scroll
    QdfRibbonGroupPopup *m_groupPopup;
    QdfRibbonToolBarControl *m_toolBar;

    QList<QdfRibbonControl *> m_controls;
    QdfRibbonControlSizeDefinition::GroupSize m_currentSize;
    Qt::Alignment m_contentAlignment;
    Qt::Alignment m_controlsAlignment;

    bool m_butOptionVisible;
    bool m_dirty;

    QAction *m_actOption;
    QString m_title;
    QIcon m_icon;
    QFont *m_font;
    QColor m_color;
    int m_pageScrollPos;
    int m_groupScrollPos;// Scroll position
    Qt::TextElideMode m_titleElideMode;

private:
    Q_DISABLE_COPY(QdfRibbonGroupPrivate)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONGROUP_P_H