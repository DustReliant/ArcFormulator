#ifndef DESKTOPFRAMEWORK_QDFRIBBONGALLERY_P_H
#define DESKTOPFRAMEWORK_QDFRIBBONGALLERY_P_H

#include <QAction>
#include <QBasicTimer>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QToolButton>
#include <QVariant>
#include <QVector>
#include <qdf_global.h>
#include <ribbon/qdfribbongallery.h>

QDF_BEGIN_NAMESPACE

class WidgetItemData
{
public:
    inline WidgetItemData() : role(-1)
    {
    }
    inline WidgetItemData(int r, QVariant v) : role(r), value(v)
    {
    }
    int role;
    QVariant value;
    inline bool operator==(const WidgetItemData &other)
    {
        return role == other.role && value == other.value;
    }
};

class QdfRibbonGalleryItem;
class QdfRibbonGalleryItemPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonGalleryItem)
public:
    explicit QdfRibbonGalleryItemPrivate();
    void init();

public:
    int m_index;
    QString m_descriptionText;
    QdfRibbonGalleryGroup *m_items;
    bool m_separator : 1;
    bool m_enabled : 1;
    bool m_visible : 1;

    QVector<WidgetItemData> m_values;
};

class QdfRibbonGalleryGroup;
class QdfRibbonGalleryGroupPrivate : public QObject
{
public:
    QDF_DECLARE_PUBLIC(QdfRibbonGalleryGroup)
public:
    explicit QdfRibbonGalleryGroupPrivate();
    virtual ~QdfRibbonGalleryGroupPrivate();

    void init();
    void updateIndexes(int start);
    void itemsChanged();
    void redrawWidget();

    void clear();

public:
    bool m_clipItems;
    QSize m_sizeItem;
    QVector<QdfRibbonGalleryItem *> m_arrItems;
    QList<QWidget *> m_viewWidgets;
};

class QdfRibbonGallery;
struct qdf_galleryitem_rect
{
    QdfRibbonGalleryItem *item;
    QRect rect;
    bool beginRow;
};


class QdfOfficePopupMenu;
class QdfRibbonGalleryPrivate : public QObject
{
public:
    Q_OBJECT
    QDF_DECLARE_PUBLIC(QdfRibbonGallery)
public:
    explicit QdfRibbonGalleryPrivate();
    virtual ~QdfRibbonGalleryPrivate();

public:
    void init();
    void layoutItems();
    void layoutScrollBar();
    void setScrollBarValue();
    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    Qt::ScrollBarPolicy scrollBarPolicy() const;
    QAction *setPopupMenu(QdfOfficePopupMenu *popup);

    void drawItems(QPainter *painter);
    void repaintItems(QRect *pRect = nullptr, bool bAnimate = true);

    bool isItemChecked(QdfRibbonGalleryItem *pItem) const;
    bool isScrollButtonEnabled(bool buttonUp);

    int scrollWidth() const;

    void startAnimation(int scrollPos);
    void startAnimate();

public Q_SLOTS:
    void setScrollPos(int nScrollPos);
    void actionTriggered(int action);
    void pressedScrollUp();
    void pressedScrollDown();

protected:
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *object, QEvent *event);

public:
    QScrollBar *m_scrollBar;
    QToolButton *m_buttonScrollUp;
    QToolButton *m_buttonScrollDown;
    QToolButton *m_buttonPopup;
    QdfOfficePopupMenu *m_menuBar;
    QVector<qdf_galleryitem_rect> m_arrRects;

    bool m_showBorders : 1;
    bool m_showLabels : 1;
    bool m_hideSelection : 1;
    bool m_keyboardSelected : 1;
    bool m_preview : 1;
    bool m_pressed : 1;
    bool m_animation : 1;
    bool m_autoWidth : 1;

    int m_totalHeight;
    int m_scrollPos;
    int m_scrollPosTarget;
    double m_animationStep;
    int m_timerElapse;

    int m_selected;
    QdfRibbonGalleryItem *m_checkedItem;
    QdfRibbonGalleryGroup *m_items;

    QPoint m_ptPressed;
    QBasicTimer m_scrollTimer;

    int m_minimumColumnCount;
    int m_maximumColumnCount;
    int m_currentColumnCount;
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONGALLERY_P_H