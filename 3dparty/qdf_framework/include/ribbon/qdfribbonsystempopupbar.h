#ifndef DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_H
#define DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_H

#include <QMenu>
#include <QToolButton>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonBackstageView;
class QdfRibbonSystemButtonPrivate;
class QDF_EXPORT QdfRibbonSystemButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
public:
    explicit QdfRibbonSystemButton(QWidget *parent);
    virtual ~QdfRibbonSystemButton();

public:
    void setBackstage(QdfRibbonBackstageView *backstage);
    QdfRibbonBackstageView *backstage() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonSystemButton)
    QDF_DECLARE_PRIVATE(QdfRibbonSystemButton)
};

class QdfRibbonPageSystemRecentFileList;
class QdfRibbonPageSystemPopup;
class QdfRibbonSystemPopupBarPrivate;
class QDF_EXPORT QdfRibbonSystemPopupBar : public QMenu
{
    Q_OBJECT
public:
    QdfRibbonSystemPopupBar(QWidget *parent);
    virtual ~QdfRibbonSystemPopupBar();

public:
    QAction *addPopupBarAction(const QString &text);
    void addPopupBarAction(QAction *action, Qt::ToolButtonStyle style = Qt::ToolButtonTextOnly);

    QdfRibbonPageSystemRecentFileList *addPageRecentFile(const QString &caption);
    QdfRibbonPageSystemPopup *addPageSystemPopup(const QString &caption, QAction *defaultAction,
                                                 bool splitAction);

public:
    virtual QSize sizeHint() const;

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    friend class QdfRibbonPageSystemPopup;
    Q_DISABLE_COPY(QdfRibbonSystemPopupBar)
    QDF_DECLARE_PRIVATE(QdfRibbonSystemPopupBar)
};

class QDF_EXPORT QdfRibbonPageSystemPopupListCaption : public QWidget
{
    Q_OBJECT
public:
    QdfRibbonPageSystemPopupListCaption(const QString &caption);
    virtual ~QdfRibbonPageSystemPopupListCaption();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void drawEvent(QPaintEvent *event, QPainter *p);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    friend class QdfRibbonSystemPopupBar;
    Q_DISABLE_COPY(QdfRibbonPageSystemPopupListCaption)
};

class QdfRibbonPageSystemRecentFileListPrivate;
class QDF_EXPORT QdfRibbonPageSystemRecentFileList : public QdfRibbonPageSystemPopupListCaption
{
    Q_OBJECT
public:
    QdfRibbonPageSystemRecentFileList(const QString &caption);
    virtual ~QdfRibbonPageSystemRecentFileList();

public:
    void setSize(int size);
    int getSize() const;

    QAction *getCurrentAction() const;

public Q_SLOTS:
    void updateRecentFileActions(const QStringList &files);

Q_SIGNALS:
    void openRecentFile(const QString &file);

protected:
    virtual void drawEvent(QPaintEvent *event, QPainter *p);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void resizeEvent(QResizeEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonPageSystemRecentFileList)
    QDF_DECLARE_PRIVATE(QdfRibbonPageSystemRecentFileList)
};

class QdfRibbonPageSystemPopupPrivate;
class QDF_EXPORT QdfRibbonPageSystemPopup : public QMenu
{
    Q_OBJECT
public:
    QdfRibbonPageSystemPopup(const QString &caption, QWidget *parent = nullptr);
    virtual ~QdfRibbonPageSystemPopup();

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    Q_DISABLE_COPY(QdfRibbonPageSystemPopup)
    QDF_DECLARE_PRIVATE(QdfRibbonPageSystemPopup)
};

QDF_END_NAMESPACE


#endif//DESKTOPFRAMEWORK_QDFRIBBONSYSTEMPOPUPBAR_H