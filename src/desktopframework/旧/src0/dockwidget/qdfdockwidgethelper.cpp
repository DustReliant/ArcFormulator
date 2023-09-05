#include "qdfdockwidgethelper.h"
#include <QPainter>

QDF_BEGIN_NAMESPACE

namespace internal {

const int FloatingWidgetDragStartEvent = QEvent::registerEventType();
const int DockedWidgetDragStartEvent = QEvent::registerEventType();
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
static QString _window_manager;
static QHash<QString, xcb_atom_t> _xcb_atom_cache;

bool is_platform_x11()
{
    return QGuiApplication::platformName() == QLatin1String("xcb");
}

xcb_connection_t *x11_connection()
{
    if (!qApp)
    {
        return nullptr;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native)
    {
        return nullptr;
    }

    void *connection = native->nativeResourceForIntegration(QByteArray("connection"));
    return reinterpret_cast<xcb_connection_t *>(connection);
}

xcb_atom_t xcb_get_atom(const char *name)
{
    if (!is_platform_x11())
    {
        return XCB_ATOM_NONE;
    }
    auto key = QString(name);
    if (_xcb_atom_cache.contains(key))
    {
        return _xcb_atom_cache[key];
    }
    xcb_connection_t *connection = x11_connection();
    xcb_intern_atom_cookie_t request = xcb_intern_atom(connection, 1, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, request, NULL);
    if (!reply)
    {
        return XCB_ATOM_NONE;
    }
    xcb_atom_t atom = reply->atom;
    if (atom != XCB_ATOM_NONE)
    {
        _xcb_atom_cache.insert(key, atom);
    }
    free(reply);
    return atom;
}

void xcb_update_prop(bool set, WId window, const char *type, const char *prop, const char *prop2)
{
    auto connection = x11_connection();
    xcb_atom_t type_atom = xcb_get_atom(type);
    xcb_atom_t prop_atom = xcb_get_atom(prop);
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = window;
    event.type = type_atom;
    event.data.data32[0] = set ? 1 : 0;
    event.data.data32[1] = prop_atom;
    event.data.data32[2] = prop2 ? xcb_get_atom(prop2) : 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, window,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_PROPERTY_CHANGE,
                   (const char *) &event);
    xcb_flush(connection);
}

xcb_get_property_reply_t *_xcb_get_props(WId window, const char *type, unsigned int atom_type)
{
    if (!is_platform_x11())
    {
        return nullptr;
    }
    xcb_connection_t *connection = x11_connection();
    xcb_atom_t type_atom = xcb_get_atom(type);
    if (type_atom == XCB_ATOM_NONE)
    {
        return nullptr;
    }
    xcb_get_property_cookie_t request = xcb_get_property_unchecked(connection, 0, window, type_atom, atom_type, 0, 1024);
    xcb_get_property_reply_t *reply = xcb_get_property_reply(connection, request, nullptr);
    if (reply && reply->type != atom_type)
    {
        free(reply);
        return nullptr;
    }
    return reply;
}

template<typename T>
void xcb_get_prop_list(WId window, const char *type, QVector<T> &ret, unsigned int atom_type)
{
    xcb_get_property_reply_t *reply = _xcb_get_props(window, type, atom_type);
    if (reply && reply->format == 32 && reply->type == atom_type && reply->value_len > 0)
    {
        const xcb_atom_t *data = static_cast<const T *>(xcb_get_property_value(reply));
        ret.resize(reply->value_len);
        memcpy((void *) &ret.first(), (void *) data, reply->value_len * sizeof(T));
    }
    free(reply);
}

QString xcb_get_prop_string(WId window, const char *type)
{
    QString ret;
    xcb_atom_t utf_atom = xcb_get_atom("UTF8_STRING");
    if (utf_atom != XCB_ATOM_NONE)
    {
        xcb_get_property_reply_t *reply = _xcb_get_props(window, type, utf_atom);
        if (reply && reply->format == 8 && reply->type == utf_atom)
        {
            const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
            ret = QString::fromUtf8(value, xcb_get_property_value_length(reply));
            free(reply);
            return ret;
        }
        free(reply);
    }
    xcb_get_property_reply_t *reply = _xcb_get_props(window, type, XCB_ATOM_STRING);
    if (reply && reply->format == 8 && reply->type == XCB_ATOM_STRING)
    {
        const char *value = reinterpret_cast<const char *>(xcb_get_property_value(reply));
        ret = QString::fromLatin1(value, xcb_get_property_value_length(reply));
    }
    free(reply);
    return ret;
}

bool xcb_dump_props(WId window, const char *type)
{
    QVector<xcb_atom_t> atoms;
    xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
    qDebug() << "\n\n!!!" << type << "  -  " << atoms.length();
    xcb_connection_t *connection = x11_connection();
    for (auto atom: atoms)
    {
        auto foo = xcb_get_atom_name(connection, atom);
        auto bar = xcb_get_atom_name_reply(connection, foo, nullptr);
        qDebug() << "\t" << xcb_get_atom_name_name(bar);
        free(bar);
    }
    return true;
}

void xcb_add_prop(bool state, WId window, const char *type, const char *prop)
{
    if (!is_platform_x11())
    {
        return;
    }
    xcb_atom_t prop_atom = xcb_get_atom(prop);
    xcb_atom_t type_atom = xcb_get_atom(type);
    if (prop_atom == XCB_ATOM_NONE || type_atom == XCB_ATOM_NONE)
    {
        return;
    }
    QVector<xcb_atom_t> atoms;
    xcb_get_prop_list(window, type, atoms, XCB_ATOM_ATOM);
    int index = atoms.indexOf(prop_atom);
    if (state && index == -1)
    {
        atoms.push_back(prop_atom);
    }
    else if (!state && index >= 0)
    {
        atoms.remove(index);
    }
    xcb_connection_t *connection = x11_connection();
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, type_atom, XCB_ATOM_ATOM, 32, atoms.count(), atoms.constData());
    xcb_flush(connection);
}

QString detectWindowManagerX11()
{
    if (!is_platform_x11())
    {
        return "UNKNOWN";
    }
    xcb_connection_t *connection = x11_connection();
    xcb_screen_t *first_screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    if (!first_screen)
    {
        return "UNKNOWN";
    }
    // Get supporting window ()
    xcb_window_t root = first_screen->root;
    xcb_window_t support_win = 0;
    QVector<xcb_window_t> sup_windows;
    xcb_get_prop_list(root, "_NET_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_WINDOW);
    if (sup_windows.length() == 0)
    {
        xcb_get_prop_list(root, "_WIN_SUPPORTING_WM_CHECK", sup_windows, XCB_ATOM_CARDINAL);
    }
    if (sup_windows.length() == 0)
    {
        return "UNKNOWN";
    }
    support_win = sup_windows[0];
    QString ret = xcb_get_prop_string(support_win, "_NET_WM_NAME");
    if (ret.length() == 0)
    {
        return "UNKNOWN";
    }
    return ret;
}

QString windowManager()
{
    if (_window_manager.length() == 0)
    {
        _window_manager = detectWindowManagerX11();
    }
    return _window_manager;
}
#endif

void replaceSplitterWidget(QSplitter *Splitter, QWidget *From, QWidget *To)
{
    int index = Splitter->indexOf(From);
    From->setParent(nullptr);
    Splitter->insertWidget(index, To);
}

QdfDockInsertParam dockAreaInsertParameters(DockWidgetArea Area)
{
    switch (Area)
    {
        case TopDockWidgetArea:
            return QdfDockInsertParam(Qt::Vertical, false);
        case RightDockWidgetArea:
            return QdfDockInsertParam(Qt::Horizontal, true);
        case CenterDockWidgetArea:
        case BottomDockWidgetArea:
            return QdfDockInsertParam(Qt::Vertical, true);
        case LeftDockWidgetArea:
            return QdfDockInsertParam(Qt::Horizontal, false);
        default:
            QdfDockInsertParam(Qt::Vertical, false);
    }

    return QdfDockInsertParam(Qt::Vertical, false);
}

QPixmap createTransparentPixmap(const QPixmap &source, qreal opacity)
{
    QPixmap TransparentPixmap(source.size());
    TransparentPixmap.fill(Qt::transparent);
    QPainter p(&TransparentPixmap);
    p.setOpacity(opacity);
    p.drawPixmap(0, 0, source);
    return TransparentPixmap;
}

void hideEmptyParentSplitters(QdfDockSplitter *Splitter)
{
    while (Splitter && Splitter->isVisible())
    {
        if (!Splitter->hasVisibleContent())
        {
            Splitter->hide();
        }
        Splitter = internal::findParent<QdfDockSplitter *>(Splitter);
    }
}

void setButtonIcon(QAbstractButton *button, QStyle::StandardPixmap StandarPixmap,
                   DockWidgetTabIcon icon)
{
    QIcon ico /*= QdfDockManager::iconProvider().customIcon(icon)*/;
    if (!ico.isNull())
    {
        button->setIcon(ico);
        return;
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    button->setIcon(button->style()->standardIcon(StandarPixmap));
#else
    QPixmap normalPixmap = button->style()->standardPixmap(StandarPixmap, 0, button);
    ico.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
    ico.addPixmap(normalPixmap, QIcon::Normal);
    button->setIcon(ico);
#endif
}

void repolishStyle(QWidget *w, RepolishChildOptions options)
{
    if (!w)
    {
        return;
    }
    w->style()->unpolish(w);
    w->style()->polish(w);

    if (RepolishIgnoreChildren == options)
    {
        return;
    }

    QList<QWidget *> Children = w->findChildren<QWidget *>(QString(),
                                                           (RepolishDirectChildren == options) ? Qt::FindDirectChildrenOnly : Qt::FindChildrenRecursively);
    for (auto Widget: Children)
    {
        Widget->style()->unpolish(Widget);
        Widget->style()->polish(Widget);
    }
}

QRect globalGeometry(QWidget *w)
{
    QRect g = w->geometry();
    g.moveTopLeft(w->mapToGlobal(QPoint(0, 0)));
    return g;
}

};// namespace internal

QDF_END_NAMESPACE