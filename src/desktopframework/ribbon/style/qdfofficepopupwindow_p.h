#ifndef DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_P_H
#define DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_P_H

#include "qdfpopuphelpers.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QRect>
#include <QTimer>
#include <QWidget>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfTitleBar : public QWidget
{
public:
    QdfTitleBar(QWidget *parent = nullptr);
    virtual ~QdfTitleBar();

public:
    QHBoxLayout *horizontalMainLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelIcon;
    QLabel *labelTitle;
};

class QdfManagerPopup
{
public:
    static QdfManagerPopup &getMngPopup()
    {
        static QdfManagerPopup mngPopup;
        return mngPopup;
    }

protected:
    QdfManagerPopup()
    {
        m_ptPopup = QPoint(-1, -1);
    }
    virtual ~QdfManagerPopup()
    {
    }

public:
    void appendPopup(QdfOfficePopupWindow *popup);
    void removePopup(QdfOfficePopupWindow *popup);
    QPoint findBestPosition(QdfOfficePopupWindow *popup);

protected:
    QList<QPointer<QdfOfficePopupWindow>> m_listPopup;

public:
    QPoint m_ptPopup;
};

enum PopupState
{
    PopupStateClosed,
    PopupStateExpanding,
    PopupStateShow,
    PopupStateCollapsing
};

class QdfOfficePopupWindowPrivate : public QObject
{
public:
    struct InfoState
    {
        QRect rcPopup;
        int transparency;
    };

public:
    QDF_DECLARE_PUBLIC(QdfOfficePopupWindow)
public:
    explicit QdfOfficePopupWindowPrivate();
    virtual ~QdfOfficePopupWindowPrivate();
    void init();

public:
    void onCollapsing();
    void onExpanding(bool updateCurrent);

    void updateState(bool repaint);
    bool closePopup();
    void showPopup();
    void animate(int step);

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);

    void initTitleBarStyleOption(QdfStyleOptionPopupTitleBar *option) const;
    void initFormStyleOption(QStyleOptionFrame *option) const;

    void setPopupState(PopupState popupState);
    PopupState getPopupState() const;
    void createStyle();
    void calclayout();

public:
    int m_transparency;
    uint m_showDelay;
    uint m_animationSpeed;
    uint m_animationInterval;
    int m_step;

    bool m_capture;
    bool m_allowMove;
    bool m_dragging;

    QPoint m_positionPopup;
    QPoint m_dragPressPosition;

    PopupLocation m_popupLocation;
    PopupAnimation m_popupAnimation;
    PopupState m_popupState;

    InfoState m_stateTarget;
    InfoState m_stateCurrent;

    QTimer m_showDelayTimer;
    QTimer m_collapsingTimer;
    QTimer m_expandingTimer;

    QWidget *m_form;
    QdfTitleBar *m_title;
    QFont m_titleFont;
    QIcon m_titleIcon;
    QString m_titleTxt;
    QString m_titleTxtRef;
    QString m_bodyTxtRef;
    QPixmap m_closePixmap;
    QPalette m_titlePalette;
    QStyle::SubControl m_buttonDown;
    bool m_pressed;
    bool m_closeButton;
    bool m_notCloseTimer;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFOFFICEPOPUPWINDOW_P_H