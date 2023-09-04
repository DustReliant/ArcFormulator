#ifndef DESKTOPFRAMEWORK_QDFRIBBON_P_H
#define DESKTOPFRAMEWORK_QDFRIBBON_P_H

#include <QEvent>
#include <QEventLoop>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>
#include <QStyleOption>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonGroup;
class QdfRibbonGroupPopup;

class QdfRibbonGroupPopupButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QdfRibbonGroupPopupButton(QWidget *parent, QdfRibbonGroup *group);
    virtual ~QdfRibbonGroupPopupButton();

public:
    bool isShowPopup() const;
    virtual void setVisible(bool visible);
    QdfRibbonGroupPopup *popupWidget();

protected:
    void resetReducedGroup();

public Q_SLOTS:
    void resetPopopGroup();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    bool m_hasPopup;
    QEventLoop *m_eventLoop;
    QdfRibbonGroup *m_ribbonGroup;
    QdfRibbonGroupPopup *m_popupWidget;

private:
    Q_DISABLE_COPY(QdfRibbonGroupPopupButton)
};

class QdfRibbonGroupPopup : public QMenu
{
public:
    explicit QdfRibbonGroupPopup(QdfRibbonGroup *group);
    virtual ~QdfRibbonGroupPopup();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    QdfRibbonGroup *m_group;

private:
    Q_DISABLE_COPY(QdfRibbonGroupPopup)
};

class QdfRibbonGroupOption : public QToolButton
{
    Q_OBJECT
public:
    explicit QdfRibbonGroupOption(QWidget *parent);
    virtual ~QdfRibbonGroupOption();

public:
    QString text() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void actionEvent(QActionEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonGroupOption)
};


class QdfRibbonGroupScroll : public QToolButton
{
    Q_OBJECT
public:
    explicit QdfRibbonGroupScroll(QWidget *parent, bool scrollLeft);
    virtual ~QdfRibbonGroupScroll();

Q_SIGNALS:
    void stopScrollTimer();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void leaveEvent(QEvent *event);

protected:
    bool m_scrollLeft;

private:
    Q_DISABLE_COPY(QdfRibbonGroupScroll)
};

class QdfRibbonKeyTip;
class QdfKeyTipEvent : public QEvent
{
public:
    explicit QdfKeyTipEvent(QdfRibbonKeyTip *kTip);

public:
    QdfRibbonKeyTip *getKeyTip() const;

public:
    static QEvent::Type eventNumber();

protected:
    QdfRibbonKeyTip *keyTip;
};

class QdfShowKeyTipEvent : public QEvent
{
public:
    QdfShowKeyTipEvent(QWidget *w);
    QWidget *buddy() const;
    static QEvent::Type eventNumber();

protected:
    QWidget *m_buddy;
};

class QdfHideKeyTipEvent : public QEvent
{
public:
    QdfHideKeyTipEvent();
    static QEvent::Type eventNumber();
};

class QdfMinimizedEvent : public QEvent
{
public:
    QdfMinimizedEvent();
    static QEvent::Type eventNumber();
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBON_P_H