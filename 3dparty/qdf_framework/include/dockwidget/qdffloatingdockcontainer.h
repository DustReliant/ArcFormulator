#ifndef DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_H
#define DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_H

#include "qdfdockmanager.h"
#include "qdffloatingwidget.h"
#include <QFrame>
#include <qdf_global.h>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    #include <QDockWidget>
#else
    #include <QWidget>
#endif

QDF_BEGIN_NAMESPACE

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
class QDF_EXPORT QdfFloatingDockContainer : public QDockWidget, public QdfFloatingWidget
#else
class QDF_EXPORT QdfFloatingDockContainer : public QWidget, public QdfFloatingWidget
#endif
{
    Q_OBJECT

public:
    QdfFloatingDockContainer(QdfDockManager *manager);
    QdfFloatingDockContainer(QdfDockAreaWidget *dockArea);
    QdfFloatingDockContainer(QdfDockWidget *dockWidget);
    virtual ~QdfFloatingDockContainer();

    QdfDockContainerWidget *dockContainer() const;
    bool isClosable() const;
    bool hasTopLevelDockWidget() const;
    QdfDockWidget *topLevelDockWidget() const;
    QList<QdfDockWidget *> dockWidgets() const;

    void hideAndDeleteLater();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    void onMaximizeRequest();
    void showNormal(bool fixGeometry = false);
    void showMaximized();
    bool isMaximized() const;
    void show();
    bool hasNativeTitleBar();
#endif

protected:
    virtual void startFloating(const QPoint &DragStartMousePos, const QSize &Size, DragState DragState,
                               QWidget *MouseEventHandler) override;

    void startDragging(const QPoint &DragStartMousePos, const QSize &Size, QWidget *MouseEventHandler)
    {
        startFloating(DragStartMousePos, Size, DragState::DS_FloatingWidget, MouseEventHandler);
    }

    virtual void finishDragging() override;

    void initFloatingGeometry(const QPoint &DragStartMousePos, const QSize &Size)
    {
        startFloating(DragStartMousePos, Size, DragState::DS_Inactive, nullptr);
    }

    void moveFloating() override;
    void updateWindowTitle();

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

#ifdef Q_OS_MACOS
    bool event(QEvent *e) override;
    void moveEvent(QMoveEvent *event) override;
#elif defined(Q_OS_UNIX)
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *e) override;
#endif

#ifdef Q_OS_WIN
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    #else
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    #endif
#endif

private Q_SLOTS:
    void onDockAreasAddedOrRemoved();
    void onDockAreaCurrentChanged(int index);

private:
    QDF_DECLARE_PRIVATE(QdfFloatingDockContainer)
    Q_DISABLE_COPY(QdfFloatingDockContainer)
    friend class QdfDockManager;
    friend class QdfDockManagerPrivate;
    friend class QdfDockAreaTabBar;
    friend class QdfDockWidgetTabPrivate;
    friend class QdfDockWidgetTab;
    friend class QdfDockAreaTitleBar;
    friend class QdfDockAreaTitleBarPrivate;
    friend class QdfDockWidget;
    friend class QdfDockAreaWidget;
    friend class QdfFloatingWidgetTitleBar;
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFFLOATINGDOCKCONTAINER_H