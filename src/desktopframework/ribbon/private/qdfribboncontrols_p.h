#ifndef DESKTOPFRAMEWORK_QDFRIBBONCONTROL_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONCONTROL_P_H

#include <QObject>
#include <QPixmap>
#include <QStyleOptionGroupBox>
#include <qdf_global.h>
#include <ribbon/qdfribbonbuttoncontrols.h>
#include <ribbon/qdfribboncontrols.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonControlSizeDefinitionPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonControlSizeDefinition)

public:
    explicit QdfRibbonControlSizeDefinitionPrivate();
    virtual ~QdfRibbonControlSizeDefinitionPrivate();
    void init(QdfRibbonControl *parent, QdfRibbonControlSizeDefinition::GroupSize size);
    void updateSizeDefinition();

public:
    QdfRibbonControl *m_parentControl;
    QdfRibbonControlSizeDefinition::ControlImageSize m_imageSize;
    bool m_labelVisible : 1;
    bool m_popup : 1;
    bool m_showSeparator : 1;
    bool m_visible : 1;
    bool m_enabled : 1;
    bool m_stretchable : 1;
    int m_visualIndex;
    int m_minItemCount;
    int m_maxItemCount;

private:
    Q_DISABLE_COPY(QdfRibbonControlSizeDefinitionPrivate)
};


class QdfRibbonControlPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonControl)
public:
    explicit QdfRibbonControlPrivate();
    virtual ~QdfRibbonControlPrivate();
    void init(QdfRibbonGroup *parentGroup);
    void changed();

public:
    QdfRibbonGroup *m_parentGroup;
    QList<QdfRibbonControlSizeDefinition *> m_allSizes;
    QdfRibbonControlSizeDefinition::GroupSize m_size;
    QAction *m_action;

private:
    Q_DISABLE_COPY(QdfRibbonControlPrivate)
};


class QdfRibbonWidgetControlPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonWidgetControl)
public:
    explicit QdfRibbonWidgetControlPrivate();
    virtual ~QdfRibbonWidgetControlPrivate();
    void init(bool ignoreActionSettings);
    void initStyleOption(QStyleOptionButton *option) const;
    void clearWidget();

public:
    QWidget *m_widget;
    bool m_visibleText;
    bool m_ignoreActionSettings;
    int m_margin;

private:
    Q_DISABLE_COPY(QdfRibbonWidgetControlPrivate)
};


class QdfRibbonColumnBreakControlPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonColumnBreakControl)
public:
    explicit QdfRibbonColumnBreakControlPrivate();
    virtual ~QdfRibbonColumnBreakControlPrivate();
    void init();
    void initStyleOption(QStyleOptionGroupBox *option) const;

public:
    Qt::Alignment m_textAlignment;

private:
    Q_DISABLE_COPY(QdfRibbonColumnBreakControlPrivate)
};


class QdfRibbonButtonControlPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonButtonControl)
public:
    explicit QdfRibbonButtonControlPrivate();
    virtual ~QdfRibbonButtonControlPrivate();
    void init();

public:
    QToolButton *m_toolButton;
    QPixmap m_largeIcon;
    QPixmap m_smallIcon;
    QString m_label;

private:
    Q_DISABLE_COPY(QdfRibbonButtonControlPrivate)
};


class QdfRibbonToolBarSeparatorControlPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit QdfRibbonToolBarSeparatorControlPrivate(QdfRibbonToolBarControl *parent);
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonToolBarSeparatorControlPrivate)
};


class QdfRibbonToolBarControlPrivate : public QObject
{
    Q_OBJECT
public:
    QDF_DECLARE_PUBLIC(QdfRibbonToolBarControl)
    struct RibbonToolBarActionGroupItem
    {
        QAction *action;
        QWidget *widget;
        RibbonToolBarActionGroupItem(QAction *act, QWidget *w) : action(act), widget(w)
        {
        }
        RibbonToolBarActionGroupItem(const RibbonToolBarActionGroupItem &other)
            : action(other.action), widget(other.widget)
        {
        }
        bool isEmpty() const
        {
            return action == 0 || !action->isVisible();
        }
    };
    typedef QList<RibbonToolBarActionGroupItem> RibbonToolBarActionGroupItems;

    struct RibbonToolBarActionsGroup
    {
        int row;
        QRect rect;
        RibbonToolBarActionGroupItems items;
        RibbonToolBarActionsGroup() : row(0)
        {
        }
    };

    typedef QList<RibbonToolBarActionsGroup> RibbonToolBarActionGroupList;

public:
    explicit QdfRibbonToolBarControlPrivate();
    virtual ~QdfRibbonToolBarControlPrivate();

public:
    void recalcWidths(int height);
    void showDefaultButton();
    void addActionToGroup(QAction *action, QWidget *widget);
    void removeActionFromGroup(QAction *action);
    void updateGroupsGeometry(const QRect &rect);
    void updateGroupProperties(RibbonToolBarActionGroupItems &group);
    void updateGeometryWidget();
    void updateLayoutParent(bool calcLayoutParent = true);
    QSize calculateSizeHint(const RibbonToolBarActionGroupItems &group) const;
    bool isMixedWidgets() const;

public:
    RibbonToolBarActionGroupList m_groups;
    int m_margin;
    int m_rowsCount;
    QSize m_sizeHint;

    QVector<int> m_vecWidths;
    QMap<int, int> m_mapNonOptWidths;
    bool m_isCalcWidth;
    bool m_dirty;
    int m_calcRows;
    int m_fullWidth;
    QRect m_rect;

private:
    Q_DISABLE_COPY(QdfRibbonToolBarControlPrivate)
};


QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONCONTROL_P_H