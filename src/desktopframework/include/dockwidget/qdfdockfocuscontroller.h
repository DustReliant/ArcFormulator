#ifndef DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_H
#define DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_H

#include <QObject>
#include <QWindow>
#include <qdf_global.h>

QDF_BEGIN_NAMESPACE

class QdfDockManager;
class QdfFloatingDockContainer;
class QdfDockWidget;
class QdfDockWidgetTab;

class QdfDockFocusControllerPrivate;
class QDF_EXPORT QdfDockFocusController : public QObject
{
    Q_OBJECT
public:
    explicit QdfDockFocusController(QdfDockManager *manager);
    virtual ~QdfDockFocusController() override;

    void notifyWidgetOrAreaRelocation(QWidget *widget);
    void notifyFloatingWidgetDrop(QdfFloatingDockContainer *floatingWidget);
    QdfDockWidget *focusedDockWidget() const;
    void setDockWidgetTabFocused(QdfDockWidgetTab *tab);
    void clearDockWidgetFocus(QdfDockWidget *dockWidget);

public Q_SLOTS:
    void setDockWidgetFocused(QdfDockWidget *dockWidget);

private Q_SLOTS:
    void onApplicationFocusChanged(QWidget *old, QWidget *now);
    void onFocusWindowChanged(QWindow *window);
    void onFocusedDockAreaViewToggled(bool open);
    void onStateRestored();
    void onDockWidgetVisibilityChanged(bool visible);

private:
    QDF_DECLARE_PRIVATE(QdfDockFocusController)
    Q_DISABLE_COPY(QdfDockFocusController)
};

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKFOCUSCONTROLLER_H