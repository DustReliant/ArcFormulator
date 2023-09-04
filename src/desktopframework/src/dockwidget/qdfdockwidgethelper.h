#ifndef DESKTOPFRAMEWORK_QDFDOCKWIDGETHELPER_H
#define DESKTOPFRAMEWORK_QDFDOCKWIDGETHELPER_H

#include <QAbstractButton>
#include <QMouseEvent>
#include <QSplitter>
#include <QStyle>
#include <QWidget>
#include <dockwidget/qdfdocksplitter.h>
#include <qdf_global.h>


QDF_BEGIN_NAMESPACE

namespace internal {

    static const bool RestoreTesting = true;
    static const bool Restore = false;
    static const char *const ClosedProperty = "close";
    static const char *const DirtyProperty = "dirty";
    extern const int FloatingWidgetDragStartEvent;
    extern const int DockedWidgetDragStartEvent;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    xcb_atom_t xcb_get_atom(const char *name);
    void xcb_add_prop(bool state, WId window, const char *type, const char *prop);
    void xcb_update_prop(bool set, WId window, const char *type, const char *prop, const char *prop2 = nullptr);
    bool xcb_dump_props(WId window, const char *type);
    QString windowManager();
#endif

    void replaceSplitterWidget(QSplitter *Splitter, QWidget *From, QWidget *To);
    void hideEmptyParentSplitters(QdfDockSplitter *FirstParentSplitter);

    class QdfDockInsertParam : public QPair<Qt::Orientation, bool>
    {
    public:
        using QPair<Qt::Orientation, bool>::QPair;
        Qt::Orientation orientation() const { return this->first; }
        bool append() const { return this->second; }
        int insertOffset() const { return append() ? 1 : 0; }
    };

    QdfDockInsertParam dockAreaInsertParameters(DockWidgetArea area);

    template<class T>
    T findParent(const QWidget *w)
    {
        QWidget *parentWidget = w->parentWidget();
            while (parentWidget) {
                T parentImpl = qobject_cast<T>(parentWidget);
                    if (parentImpl) {
                        return parentImpl;
                    }
                parentWidget = parentWidget->parentWidget();
            }
        return 0;
    }

    QPixmap createTransparentPixmap(const QPixmap &source, qreal opacity);
    template<class T>
    void setFlag(T &flags, typename T::enum_type flag, bool on = true)
    {
#if QT_VERSION >= 0x050700
        flags.setFlag(flag, on);
#else
            if (on) {
                flags |= flag;
            }
            else {
                flags &= ~flag;
            }
#endif
    }

    template<class QObjectPtr>
    void setToolTip(QObjectPtr obj, const QString &tip)
    {
#ifndef QT_NO_TOOLTIP
        obj->setToolTip(tip);
#else
        Q_UNUSED(obj);
        Q_UNUSED(tip);
#endif
    }

    inline QPoint globalPositionOf(QMouseEvent *event)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        return event->globalPosition().toPoint();
#else
        return event->globalPos();
#endif
    }

    void setButtonIcon(QAbstractButton *button, QStyle::StandardPixmap standarPixmap,
                       DockWidgetTabIcon customIconId);

    enum RepolishChildOptions
    {
        RepolishIgnoreChildren,
        RepolishDirectChildren,
        RepolishChildrenRecursively
    };
    void repolishStyle(QWidget *w, RepolishChildOptions Options = RepolishIgnoreChildren);
    QRect globalGeometry(QWidget *w);
};// namespace internal

QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFDOCKWIDGETHELPER_H