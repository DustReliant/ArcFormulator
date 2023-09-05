#pragma once

#include <QSharedPointer>
#include <QtCore/QProcess>
#include <QtCore/qglobal.h>


#define QDF_NAMESPACE Qdf

#if !defined(QDF_NAMESPACE)
    #define QDF_BEGIN_NAMESPACE
    #define QDF_END_NAMESPACE
    #define QDF_USE_NAMESPACE
    #define QT_PREPEND_NAMESPACE_QDF(name) ::name
#else
    #define QDF_BEGIN_NAMESPACE namespace QDF_NAMESPACE {
    #define QDF_END_NAMESPACE }
    #define QDF_USE_NAMESPACE using namespace QDF_NAMESPACE;
    #define QT_PREPEND_NAMESPACE_QDF(name) ::QDF_NAMESPACE::name
#endif

#define QDF_ENUM_STRING_VALUE(X)


QDF_BEGIN_NAMESPACE

#ifndef QDF_LIBRARY_STATIC
    #ifdef QDF_LIBRARY
        #define QDF_EXPORT Q_DECL_EXPORT
    #else
        #define QDF_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define QDF_EXPORT
#endif


#define QDF_DECLARE_PRIVATE(Class)                \
    friend class Class##Private;                  \
    Class##Private *qdf_d_ptr;                    \
    Class##Private *qdf_d() { return qdf_d_ptr; } \
    const Class##Private *qdf_d() const { return qdf_d_ptr; }


#define QDF_DECLARE_PUBLIC(Class)                          \
    friend class Class;                                    \
    Class *qdf_q_ptr;                                      \
    inline void setPublic(Class *ptr) { qdf_q_ptr = ptr; } \
    Class *qdf_q() { return qdf_q_ptr; }                   \
    const Class *qdf_q() const { return qdf_q_ptr; }

#define QDF_INIT_PRIVATE(Class)       \
    qdf_d_ptr = new Class##Private(); \
    qdf_d_ptr->setPublic(this);

#define QDF_FINI_PRIVATE() \
    delete qdf_d_ptr;      \
    qdf_d_ptr = nullptr;

#define QDF_D(Class) Class##Private *d = qdf_d();
#define QDF_Q(Class) Class *q = qdf_q();

#define QDF_FINI_PRIVATE() \
    delete qdf_d_ptr;      \
    qdf_d_ptr = nullptr;

#define QDF_INIT_EX_PRIVATE(Class, __d_ptr)                  \
    qdf_d_ptr = reinterpret_cast<Class##Private *>(__d_ptr); \
    qdf_d_ptr->setPublic(this);

#define QDF_DECLARE_EX_PRIVATE(Class)                               \
    inline Class##Private *qdf_d()                                  \
    {                                                               \
        return reinterpret_cast<Class##Private *>(qdf_d_ptr);       \
    }                                                               \
    inline const Class##Private *qdf_d() const                      \
    {                                                               \
        return reinterpret_cast<const Class##Private *>(qdf_d_ptr); \
    }                                                               \
    friend class Class##Private;

#define QDF_DECLARE_EX_PUBLIC(Class)            \
    Class *qdf_q()                              \
    {                                           \
        return static_cast<Class *>(qdf_q_ptr); \
    }                                           \
    const Class *qdf_q() const                  \
    {                                           \
        return static_cast<Class *>(qdf_q_ptr); \
    }                                           \
    friend class Class;


enum PopupLocation
{
#ifdef Q_OS_WIN
    PopupLocationNearTaskBar,
#endif// Q_OS_WIN
    PopupLocationBottomRight,
    PopupLocationCenter
};

enum class PopupAnimation
{
    PA_None,
    PA_Fade,
    PA_Slide,
    PA_Unfold
};

enum DockWidgetArea
{
    NoDockWidgetArea = 0x00,
    LeftDockWidgetArea = 0x01,
    RightDockWidgetArea = 0x02,
    TopDockWidgetArea = 0x04,
    BottomDockWidgetArea = 0x08,
    CenterDockWidgetArea = 0x10,

    InvalidDockWidgetArea = NoDockWidgetArea,
    OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
    AllDockAreas = OuterDockAreas | CenterDockWidgetArea
};
Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)


enum class TitleBarButton
{
    TabsMenu,
    Undock,
    Close,
    AutoHide
};

enum class DragState
{
    DS_Inactive,
    DS_MousePressed,
    DS_Tab,
    DS_FloatingWidget
};

enum class DockWidgetTabIcon
{
    TabClose,
    AutoHide,
    DockAreaMenu,
    DockAreaUndock,
    DockAreaClose,
    DockWidgetTabIconMax
};

enum class BitwiseOperator
{
    BitwiseAnd,
    BitwiseOr
};

enum SideBarLocation
{
    SL_Top,
    SL_Left,
    SL_Right,
    SL_Bottom,
    SL_None
};

QDF_END_NAMESPACE


#ifdef _MSC_VER
    #pragma warning(disable : 4018)

    #pragma warning(disable : 4100)
    #pragma warning(disable : 4101)
    #pragma warning(disable : 4189)
    #pragma warning(disable : 4192)

    #pragma warning(disable : 4238)
    #pragma warning(disable : 4239)
    #pragma warning(disable : 4244)
    #pragma warning(disable : 4245)
    #pragma warning(disable : 4251)
    #pragma warning(disable : 4267)

    #pragma warning(disable : 4305)
    #pragma warning(disable : 4311)
    #pragma warning(disable : 4389)

    #pragma warning(disable : 4482)
    #pragma warning(disable : 4635)
    #pragma warning(disable : 4701)

    #pragma warning(disable : 4800)
    #pragma warning(disable : 4996)

    #pragma warning(disable : 4456)
    #pragma warning(disable : 4457)
    #pragma warning(disable : 4458)

    #pragma warning(disable : 4063)

#endif