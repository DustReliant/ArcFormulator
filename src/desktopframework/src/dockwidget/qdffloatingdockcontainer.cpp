#include "private/qdffloatingdockcontainer_p.h"
#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QPointer>
#include <QTime>
#include <dockwidget/qdfdockareawidget.h>
#include <dockwidget/qdfdockoverlay.h>
#include <dockwidget/qdffloatingdockcontainer.h>
#ifdef Q_OS_WIN
    #include <windows.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "User32.lib")
    #endif
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    #include "linux/qdffloatingwidgettitlebar.h"
    #include <xcb/xcb.h>
#endif

QDF_USE_NAMESPACE

#ifdef Q_OS_WIN
    #if 0
static const char* windowsMessageString(int MessageId)
{
	switch (MessageId)
	{
    case 0: return "WM_NULL";
    case 1: return "WM_CREATE";
    case 2: return "WM_DESTROY";
    case 3: return "WM_MOVE";
    case 5: return "WM_SIZE";
    case 6: return "WM_ACTIVATE";
    case 7: return "WM_SETFOCUS";
    case 8: return "WM_KILLFOCUS";
    case 10: return "WM_ENABLE";
    case 11: return "WM_SETREDRAW";
    case 12: return "WM_SETTEXT";
    case 13: return "WM_GETTEXT";
    case 14: return "WM_GETTEXTLENGTH";
    case 15: return "WM_PAINT";
    case 16: return "WM_CLOSE";
    case 17: return "WM_QUERYENDSESSION";
    case 18: return "WM_QUIT";
    case 19: return "WM_QUERYOPEN";
    case 20: return "WM_ERASEBKGND";
    case 21: return "WM_SYSCOLORCHANGE";
    case 22: return "WM_ENDSESSION";
    case 24: return "WM_SHOWWINDOW";
    case 25: return "WM_CTLCOLOR";
    case 26: return "WM_WININICHANGE";
    case 27: return "WM_DEVMODECHANGE";
    case 28: return "WM_ACTIVATEAPP";
    case 29: return "WM_FONTCHANGE";
    case 30: return "WM_TIMECHANGE";
    case 31: return "WM_CANCELMODE";
    case 32: return "WM_SETCURSOR";
    case 33: return "WM_MOUSEACTIVATE";
    case 34: return "WM_CHILDACTIVATE";
    case 35: return "WM_QUEUESYNC";
    case 36: return "WM_GETMINMAXINFO";
    case 38: return "WM_PAINTICON";
    case 39: return "WM_ICONERASEBKGND";
    case 40: return "WM_NEXTDLGCTL";
    case 42: return "WM_SPOOLERSTATUS";
    case 43: return "WM_DRAWITEM";
    case 44: return "WM_MEASUREITEM";
    case 45: return "WM_DELETEITEM";
    case 46: return "WM_VKEYTOITEM";
    case 47: return "WM_CHARTOITEM";
    case 48: return "WM_SETFONT";
    case 49: return "WM_GETFONT";
    case 50: return "WM_SETHOTKEY";
    case 51: return "WM_GETHOTKEY";
    case 55: return "WM_QUERYDRAGICON";
    case 57: return "WM_COMPAREITEM";
    case 61: return "WM_GETOBJECT";
    case 65: return "WM_COMPACTING";
    case 68: return "WM_COMMNOTIFY";
    case 70: return "WM_WINDOWPOSCHANGING";
    case 71: return "WM_WINDOWPOSCHANGED";
    case 72: return "WM_POWER";
    case 73: return "WM_COPYGLOBALDATA";
    case 74: return "WM_COPYDATA";
    case 75: return "WM_CANCELJOURNAL";
    case 78: return "WM_NOTIFY";
    case 80: return "WM_INPUTLANGCHANGEREQUEST";
    case 81: return "WM_INPUTLANGCHANGE";
    case 82: return "WM_TCARD";
    case 83: return "WM_HELP";
    case 84: return "WM_USERCHANGED";
    case 85: return "WM_NOTIFYFORMAT";
    case 123: return "WM_CONTEXTMENU";
    case 124: return "WM_STYLECHANGING";
    case 125: return "WM_STYLECHANGED";
    case 126: return "WM_DISPLAYCHANGE";
    case 127: return "WM_GETICON";
    case 128: return "WM_SETICON";
    case 129: return "WM_NCCREATE";
    case 130: return "WM_NCDESTROY";
    case 131: return "WM_NCCALCSIZE";
    case 132: return "WM_NCHITTEST";
    case 133: return "WM_NCPAINT";
    case 134: return "WM_NCACTIVATE";
    case 135: return "WM_GETDLGCODE";
    case 136: return "WM_SYNCPAINT";
    case 160: return "WM_NCMOUSEMOVE";
    case 161: return "WM_NCLBUTTONDOWN";
    case 162: return "WM_NCLBUTTONUP";
    case 163: return "WM_NCLBUTTONDBLCLK";
    case 164: return "WM_NCRBUTTONDOWN";
    case 165: return "WM_NCRBUTTONUP";
    case 166: return "WM_NCRBUTTONDBLCLK";
    case 167: return "WM_NCMBUTTONDOWN";
    case 168: return "WM_NCMBUTTONUP";
    case 169: return "WM_NCMBUTTONDBLCLK";
    case 171: return "WM_NCXBUTTONDOWN";
    case 172: return "WM_NCXBUTTONUP";
    case 173: return "WM_NCXBUTTONDBLCLK";
    case 176: return "EM_GETSEL";
    case 177: return "EM_SETSEL";
    case 178: return "EM_GETRECT";
    case 179: return "EM_SETRECT";
    case 180: return "EM_SETRECTNP";
    case 181: return "EM_SCROLL";
    case 182: return "EM_LINESCROLL";
    case 183: return "EM_SCROLLCARET";
    case 185: return "EM_GETMODIFY";
    case 187: return "EM_SETMODIFY";
    case 188: return "EM_GETLINECOUNT";
    case 189: return "EM_LINEINDEX";
    case 190: return "EM_SETHANDLE";
    case 191: return "EM_GETHANDLE";
    case 192: return "EM_GETTHUMB";
    case 193: return "EM_LINELENGTH";
    case 194: return "EM_REPLACESEL";
    case 195: return "EM_SETFONT";
    case 196: return "EM_GETLINE";
    case 197: return "EM_LIMITTEXT / EM_SETLIMITTEXT";
    case 198: return "EM_CANUNDO";
    case 199: return "EM_UNDO";
    case 200: return "EM_FMTLINES";
    case 201: return "EM_LINEFROMCHAR";
    case 202: return "EM_SETWORDBREAK";
    case 203: return "EM_SETTABSTOPS";
    case 204: return "EM_SETPASSWORDCHAR";
    case 205: return "EM_EMPTYUNDOBUFFER";
    case 206: return "EM_GETFIRSTVISIBLELINE";
    case 207: return "EM_SETREADONLY";
    case 209: return "EM_SETWORDBREAKPROC / EM_GETWORDBREAKPROC";
    case 210: return "EM_GETPASSWORDCHAR";
    case 211: return "EM_SETMARGINS";
    case 212: return "EM_GETMARGINS";
    case 213: return "EM_GETLIMITTEXT";
    case 214: return "EM_POSFROMCHAR";
    case 215: return "EM_CHARFROMPOS";
    case 216: return "EM_SETIMESTATUS";
    case 217: return "EM_GETIMESTATUS";
    case 224: return "SBM_SETPOS";
    case 225: return "SBM_GETPOS";
    case 226: return "SBM_SETRANGE";
    case 227: return "SBM_GETRANGE";
    case 228: return "SBM_ENABLE_ARROWS";
    case 230: return "SBM_SETRANGEREDRAW";
    case 233: return "SBM_SETSCROLLINFO";
    case 234: return "SBM_GETSCROLLINFO";
    case 235: return "SBM_GETSCROLLBARINFO";
    case 240: return "BM_GETCHECK";
    case 241: return "BM_SETCHECK";
    case 242: return "BM_GETSTATE";
    case 243: return "BM_SETSTATE";
    case 244: return "BM_SETSTYLE";
    case 245: return "BM_CLICK";
    case 246: return "BM_GETIMAGE";
    case 247: return "BM_SETIMAGE";
    case 248: return "BM_SETDONTCLICK";
    case 255: return "WM_INPUT";
    case 256: return "WM_KEYDOWN";
    case 257: return "WM_KEYUP";
    case 258: return "WM_CHAR";
    case 259: return "WM_DEADCHAR";
    case 260: return "WM_SYSKEYDOWN";
    case 261: return "WM_SYSKEYUP";
    case 262: return "WM_SYSCHAR";
    case 263: return "WM_SYSDEADCHAR";
    case 265: return "WM_UNICHAR / WM_WNT_CONVERTREQUESTEX";
    case 266: return "WM_CONVERTREQUEST";
    case 267: return "WM_CONVERTRESULT";
    case 268: return "WM_INTERIM";
    case 269: return "WM_IME_STARTCOMPOSITION";
    case 270: return "WM_IME_ENDCOMPOSITION";
    case 272: return "WM_INITDIALOG";
    case 273: return "WM_COMMAND";
    case 274: return "WM_SYSCOMMAND";
    case 275: return "WM_TIMER";
    case 276: return "WM_HSCROLL";
    case 277: return "WM_VSCROLL";
    case 278: return "WM_INITMENU";
    case 279: return "WM_INITMENUPOPUP";
    case 280: return "WM_SYSTIMER";
    case 287: return "WM_MENUSELECT";
    case 288: return "WM_MENUCHAR";
    case 289: return "WM_ENTERIDLE";
    case 290: return "WM_MENURBUTTONUP";
    case 291: return "WM_MENUDRAG";
    case 292: return "WM_MENUGETOBJECT";
    case 293: return "WM_UNINITMENUPOPUP";
    case 294: return "WM_MENUCOMMAND";
    case 295: return "WM_CHANGEUISTATE";
    case 296: return "WM_UPDATEUISTATE";
    case 297: return "WM_QUERYUISTATE";
    case 306: return "WM_CTLCOLORMSGBOX";
    case 307: return "WM_CTLCOLOREDIT";
    case 308: return "WM_CTLCOLORLISTBOX";
    case 309: return "WM_CTLCOLORBTN";
    case 310: return "WM_CTLCOLORDLG";
    case 311: return "WM_CTLCOLORSCROLLBAR";
    case 312: return "WM_CTLCOLORSTATIC";
    case 512: return "WM_MOUSEMOVE";
    case 513: return "WM_LBUTTONDOWN";
    case 514: return "WM_LBUTTONUP";
    case 515: return "WM_LBUTTONDBLCLK";
    case 516: return "WM_RBUTTONDOWN";
    case 517: return "WM_RBUTTONUP";
    case 518: return "WM_RBUTTONDBLCLK";
    case 519: return "WM_MBUTTONDOWN";
    case 520: return "WM_MBUTTONUP";
    case 521: return "WM_MBUTTONDBLCLK";
    case 522: return "WM_MOUSEWHEEL";
    case 523: return "WM_XBUTTONDOWN";
    case 524: return "WM_XBUTTONUP";
    case 525: return "WM_XBUTTONDBLCLK";
    case 528: return "WM_PARENTNOTIFY";
    case 529: return "WM_ENTERMENULOOP";
    case 530: return "WM_EXITMENULOOP";
    case 531: return "WM_NEXTMENU";
    case 532: return "WM_SIZING";
    case 533: return "WM_CAPTURECHANGED";
    case 534: return "WM_MOVING";
    case 536: return "WM_POWERBROADCAST";
    case 537: return "WM_DEVICECHANGE";
    case 544: return "WM_MDICREATE";
    case 545: return "WM_MDIDESTROY";
    case 546: return "WM_MDIACTIVATE";
    case 547: return "WM_MDIRESTORE";
    case 548: return "WM_MDINEXT";
    case 549: return "WM_MDIMAXIMIZE";
    case 550: return "WM_MDITILE";
    case 551: return "WM_MDICASCADE";
    case 552: return "WM_MDIICONARRANGE";
    case 553: return "WM_MDIGETACTIVE";
    case 560: return "WM_MDISETMENU";
    case 561: return "WM_ENTERSIZEMOVE";
    case 562: return "WM_EXITSIZEMOVE";
    case 563: return "WM_DROPFILES";
    case 564: return "WM_MDIREFRESHMENU";
    case 640: return "WM_IME_REPORT";
    case 641: return "WM_IME_SETCONTEXT";
    case 642: return "WM_IME_NOTIFY";
    case 643: return "WM_IME_CONTROL";
    case 644: return "WM_IME_COMPOSITIONFULL";
    case 645: return "WM_IME_SELECT";
    case 646: return "WM_IME_CHAR";
    case 648: return "WM_IME_REQUEST";
    case 656: return "WM_IME_KEYDOWN";
    case 657: return "WM_IME_KEYUP";
    case 672: return "WM_NCMOUSEHOVER";
    case 673: return "WM_MOUSEHOVER";
    case 674: return "WM_NCMOUSELEAVE";
    case 675: return "WM_MOUSELEAVE";
    case 768: return "WM_CUT";
    case 769: return "WM_COPY";
    case 770: return "WM_PASTE";
    case 771: return "WM_CLEAR";
    case 772: return "WM_UNDO";
    case 773: return "WM_RENDERFORMAT";
    case 774: return "WM_RENDERALLFORMATS";
    case 775: return "WM_DESTROYCLIPBOARD";
    case 776: return "WM_DRAWCLIPBOARD";
    case 777: return "WM_PAINTCLIPBOARD";
    case 778: return "WM_VSCROLLCLIPBOARD";
    case 779: return "WM_SIZECLIPBOARD";
    case 780: return "WM_ASKCBFORMATNAME";
    case 781: return "WM_CHANGECBCHAIN";
    case 782: return "WM_HSCROLLCLIPBOARD";
    case 783: return "WM_QUERYNEWPALETTE";
    case 784: return "WM_PALETTEISCHANGING";
    case 785: return "WM_PALETTECHANGED";
    case 786: return "WM_HOTKEY";
    case 791: return "WM_PRINT";
    case 792: return "WM_PRINTCLIENT";
    case 793: return "WM_APPCOMMAND";
    case 856: return "WM_HANDHELDFIRST";
    case 863: return "WM_HANDHELDLAST";
    case 864: return "WM_AFXFIRST";
    case 895: return "WM_AFXLAST";
    case 896: return "WM_PENWINFIRST";
    case 897: return "WM_RCRESULT";
    case 898: return "WM_HOOKRCRESULT";
    case 899: return "WM_GLOBALRCCHANGE / WM_PENMISCINFO";
    case 900: return "WM_SKB";
    case 901: return "WM_HEDITCTL / WM_PENCTL";
    case 902: return "WM_PENMISC";
    case 903: return "WM_CTLINIT";
    case 904: return "WM_PENEVENT";
    case 911: return "WM_PENWINLAST";
    default:
    	return "unknown WM_ message";
	}

	return "unknown WM_ message";
}
    #endif
#endif


void QdfFloatingDockContainerPrivate::titleMouseReleaseEvent()
{
    QDF_Q(QdfFloatingDockContainer)
    setState(DragState::DS_Inactive);
    if (!dropContainer)
    {
        return;
    }

    if (dockManager->dockAreaOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea ||
        dockManager->containerOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea)
    {
        QdfDockOverlay *overlay = dockManager->containerOverlay();
        if (!overlay->dropOverlayRect().isValid())
        {
            overlay = dockManager->dockAreaOverlay();
        }

        QRect rect = overlay->dropOverlayRect();
        int frameWidth = (q->frameSize().width() - q->rect().width()) / 2;
        int titleBarHeight = q->frameSize().height() - q->rect().height() - frameWidth;
        if (rect.isValid())
        {
            QPoint topLeft = overlay->mapToGlobal(rect.topLeft());
            topLeft.ry() += titleBarHeight;
            q->setGeometry(
                    QRect(topLeft,
                          QSize(rect.width(), rect.height() - titleBarHeight)));
            QApplication::processEvents();
        }
        dropContainer->dropFloatingWidget(q, QCursor::pos());
    }

    dockManager->containerOverlay()->hideOverlay();
    dockManager->dockAreaOverlay()->hideOverlay();
}

void QdfFloatingDockContainerPrivate::updateDropOverlays(const QPoint &globalPos)
{
    QDF_Q(QdfFloatingDockContainer)
    if (!q->isVisible() || !dockManager)
    {
        return;
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (qApp->activeModalWidget())
    {
        return;
    }
#endif

    auto containers = dockManager->dockContainers();
    QdfDockContainerWidget *topContainer = nullptr;
    for (auto &containerWidget: containers)
    {
        if (!containerWidget->isVisible())
        {
            continue;
        }

        if (dockContainer == containerWidget)
        {
            continue;
        }

        QPoint point = containerWidget->mapFromGlobal(globalPos);
        if (containerWidget->rect().contains(point))
        {
            if (!topContainer || containerWidget->isInFrontOf(topContainer))
            {
                topContainer = containerWidget;
            }
        }
    }

    dropContainer = topContainer;
    auto containerOverlay = dockManager->containerOverlay();
    auto dockAreaOverlay = dockManager->dockAreaOverlay();

    if (!topContainer)
    {
        containerOverlay->hideOverlay();
        dockAreaOverlay->hideOverlay();
        return;
    }

    int visibleDockAreas = topContainer->visibleDockAreaCount();
    containerOverlay->setAllowedAreas(
            visibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
    DockWidgetArea containerArea = containerOverlay->showOverlay(topContainer);
    containerOverlay->enableDropPreview(containerArea != InvalidDockWidgetArea);
    auto dockArea = topContainer->dockAreaAt(globalPos);
    if (dockArea && dockArea->isVisible() && visibleDockAreas > 0)
    {
        dockAreaOverlay->enableDropPreview(true);
        dockAreaOverlay->setAllowedAreas(
                (visibleDockAreas == 1) ? NoDockWidgetArea : dockArea->allowedAreas());
        DockWidgetArea area = dockAreaOverlay->showOverlay(dockArea);
        if ((area == CenterDockWidgetArea) && (containerArea != InvalidDockWidgetArea))
        {
            dockAreaOverlay->enableDropPreview(false);
            containerOverlay->enableDropPreview(true);
        }
        else
        {
            containerOverlay->enableDropPreview(InvalidDockWidgetArea == area);
        }
    }
    else
    {
        dockAreaOverlay->hideOverlay();
    }
}

bool QdfFloatingDockContainerPrivate::testConfigFlag(QdfDockManager::ConfigFlag flag)
{
    return QdfDockManager::testConfigFlag(flag);
}
bool QdfFloatingDockContainerPrivate::isState(DragState id) const
{
    return id == draggingState;
}
void QdfFloatingDockContainerPrivate::setState(DragState id)
{
    QDF_Q(QdfFloatingDockContainer)
    if (draggingState == id)
    {
        return;
    }

    draggingState = id;
    if (DragState::DS_FloatingWidget == draggingState)
    {
        qApp->postEvent(q, new QEvent((QEvent::Type) internal::FloatingWidgetDragStartEvent));
    }
}

void QdfFloatingDockContainerPrivate::setWindowTitle(const QString &text)
{
    QDF_Q(QdfFloatingDockContainer)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (TitleBar)
    {
        TitleBar->setTitle(text);
    }
#endif
    q->setWindowTitle(text);
}

void QdfFloatingDockContainerPrivate::reflectCurrentWidget(QdfDockWidget *currentWidget)
{
    QDF_Q(QdfFloatingDockContainer)
    if (testConfigFlag(QdfDockManager::FloatingContainerHasWidgetTitle))
    {
        setWindowTitle(currentWidget->windowTitle());
    }
    else
    {
        setWindowTitle(floatingContainersTitle());
    }

    QIcon icon = currentWidget->icon();
    if (testConfigFlag(QdfDockManager::FloatingContainerHasWidgetIcon) && !icon.isNull())
    {
        q->setWindowIcon(currentWidget->icon());
    }
    else
    {
        q->setWindowIcon(QApplication::windowIcon());
    }
}

void QdfFloatingDockContainerPrivate::handleEscapeKey()
{
    setState(DragState::DS_Inactive);
    dockManager->containerOverlay()->hideOverlay();
    dockManager->dockAreaOverlay()->hideOverlay();
}

QString QdfFloatingDockContainerPrivate::floatingContainersTitle()
{
    return QdfDockManager::floatingContainersTitle();
}


QdfFloatingDockContainer::QdfFloatingDockContainer(QdfDockManager *dockManager)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    : QDockWidget(dockManager)
#else
    : QWidget(dockManager)
#endif
{
    QDF_INIT_PRIVATE(QdfFloatingDockContainer)
    QDF_D(QdfFloatingDockContainer)
    d->dockManager = dockManager;
    d->dockContainer = new QdfDockContainerWidget(dockManager, this);
    connect(d->dockContainer, SIGNAL(dockAreasAdded()), this,
            SLOT(onDockAreasAddedOrRemoved()));
    connect(d->dockContainer, SIGNAL(dockAreasRemoved()), this,
            SLOT(onDockAreasAddedOrRemoved()));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QDockWidget::setWidget(d->dockContainer);
    QDockWidget::setFloating(true);
    QDockWidget::setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    bool native_window = true;

    auto env = qgetenv("ADS_UseNativeTitle").toUpper();
    if (env == "1")
    {
        native_window = true;
    }
    else if (env == "0")
    {
        native_window = false;
    }
    else if (DockManager->testConfigFlag(QdfDockManager::FloatingContainerForceNativeTitleBar))
    {
        native_window = true;
    }
    else if (DockManager->testConfigFlag(QdfDockManager::FloatingContainerForceQWidgetTitleBar))
    {
        native_window = false;
    }
    else
    {
        QString window_manager = internal::windowManager().toUpper().split(" ")[0];
        native_window = window_manager != "KWIN";
    }

    if (native_window)
    {
        QString XdgSessionType = qgetenv("XDG_SESSION_TYPE").toLower();
        if ("wayland" == XdgSessionType)
        {
            native_window = false;
        }
    }

    if (native_window)
    {
        setTitleBarWidget(new QWidget());
        setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    }
    else
    {
        d->TitleBar = new QdfFloatingWidgetTitleBar(this);
        setTitleBarWidget(d->TitleBar);
        setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
        d->TitleBar->enableCloseButton(isClosable());
        connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
        connect(d->TitleBar, &QdfFloatingWidgetTitleBar::maximizeRequested,
                this, &QdfFloatingDockContainer::onMaximizeRequest);
    }
#else
    setWindowFlags(
            Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->dockContainer);
#endif

    dockManager->registerFloatingWidget(this);
}


QdfFloatingDockContainer::QdfFloatingDockContainer(QdfDockAreaWidget *dockArea)
    : QdfFloatingDockContainer(dockArea->dockManager())
{
    QDF_D(QdfFloatingDockContainer)
    d->dockContainer->addDockArea(dockArea);

    auto widget = topLevelDockWidget();
    if (widget)
    {
        widget->emitTopLevelChanged(true);
    }

    d->dockManager->notifyWidgetOrAreaRelocation(dockArea);
}


QdfFloatingDockContainer::QdfFloatingDockContainer(QdfDockWidget *DockWidget)
    : QdfFloatingDockContainer(DockWidget->dockManager())
{
    QDF_D(QdfFloatingDockContainer)
    d->dockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
    auto TopLevelDockWidget = topLevelDockWidget();
    if (TopLevelDockWidget)
    {
        TopLevelDockWidget->emitTopLevelChanged(true);
    }

    d->dockManager->notifyWidgetOrAreaRelocation(DockWidget);
}


QdfFloatingDockContainer::~QdfFloatingDockContainer()
{
    QDF_D(const QdfFloatingDockContainer)
    if (d->dockManager)
    {
        d->dockManager->removeFloatingWidget(this);
    }
    QDF_FINI_PRIVATE()
}


QdfDockContainerWidget *QdfFloatingDockContainer::dockContainer() const
{
    QDF_D(const QdfFloatingDockContainer)
    return d->dockContainer;
}


void QdfFloatingDockContainer::changeEvent(QEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QDockWidget::changeEvent(event);
#else
    QWidget::changeEvent(event);
#endif

    switch (event->type())
    {
        case QEvent::ActivationChange:
            if (isActiveWindow())
            {
                d->zOrderIndex = ++zOrderCounter;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
                if (d->draggingState == DragState::DS_FloatingWidget)
                {
                    d->titleMouseReleaseEvent();
                    d->draggingState = DragState::DS_Inactive;
                }
#endif
            }
            break;

        case QEvent::WindowStateChange:
            if (d->dockManager->isLeavingMinimizedState())
            {
                QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(event);
                if (ev->oldState().testFlag(Qt::WindowMaximized))
                {
                    this->showMaximized();
                }
            }
            break;

        default:
            break;
    }
}


#ifdef Q_OS_WIN
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool QdfFloatingDockContainer::nativeEvent(const QByteArray &eventType, void *message, long *result)
    #else
bool QdfFloatingDockContainer::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
    #endif
{
    QDF_D(QdfFloatingDockContainer)
    QWidget::nativeEvent(eventType, message, result);
    MSG *msg = static_cast<MSG *>(message);
    switch (msg->message)
    {
        case WM_MOVING:
            {
                if (d->isState(DragState::DS_FloatingWidget))
                {
                    d->updateDropOverlays(QCursor::pos());
                }
            }
            break;

        case WM_NCLBUTTONDOWN:
            if (msg->wParam == HTCAPTION && d->isState(DragState::DS_Inactive))
            {
                d->dragStartPos = pos();
                d->setState(DragState::DS_MousePressed);
            }
            break;

        case WM_NCLBUTTONDBLCLK:
            d->setState(DragState::DS_Inactive);
            break;

        case WM_ENTERSIZEMOVE:
            if (d->isState(DragState::DS_MousePressed))
            {
                d->setState(DragState::DS_FloatingWidget);
                d->updateDropOverlays(QCursor::pos());
            }
            break;

        case WM_EXITSIZEMOVE:
            if (d->isState(DragState::DS_FloatingWidget))
            {
                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                {
                    d->handleEscapeKey();
                }
                else
                {
                    d->titleMouseReleaseEvent();
                }
            }
            break;
    }
    return false;
}
#endif


void QdfFloatingDockContainer::closeEvent(QCloseEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
    d->setState(DragState::DS_Inactive);
    event->ignore();
    if (!isClosable())
    {
        return;
    }

    bool hasOpenDockWidgets = false;
    for (auto &dockWidget: d->dockContainer->openedDockWidgets())
    {
        if (dockWidget->features().testFlag(QdfDockWidget::DockWidgetDeleteOnClose) || dockWidget->features().testFlag(QdfDockWidget::CustomCloseHandling))
        {
            bool closed = dockWidget->closeDockWidgetInternal();
            if (!closed)
            {
                hasOpenDockWidgets = true;
            }
        }
        else
        {
            dockWidget->toggleView(false);
        }
    }

    if (hasOpenDockWidgets)
    {
        return;
    }

    this->hide();
}


void QdfFloatingDockContainer::hideEvent(QHideEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QDockWidget::hideEvent(event);
#else
    QWidget::hideEvent(event);
#endif
    if (event->spontaneous())
    {
        return;
    }

    if (d->dockManager->isRestoringState())
    {
        return;
    }

    if (d->autoHideChildren)
    {
        d->hiding = true;
        for (auto &dockArea: d->dockContainer->openedDockAreas())
        {
            for (auto &dockWidget: dockArea->openedDockWidgets())
            {
                dockWidget->toggleView(false);
            }
        }
        d->hiding = false;
    }
}


void QdfFloatingDockContainer::showEvent(QShowEvent *event)
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QDockWidget::showEvent(event);
    if (QdfDockManager::testConfigFlag(QdfDockManager::FocusHighlighting))
    {
        this->window()->activateWindow();
    }
#else
    QWidget::showEvent(event);
#endif
}


void QdfFloatingDockContainer::startFloating(const QPoint &dragStartMousePos,
                                             const QSize &size, DragState dragState, QWidget *mouseEventHandler)
{
    QDF_D(QdfFloatingDockContainer)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (!isMaximized())
    {
        resize(Size);
        d->dragStartMousePosition = dragStartMousePos;
    }
    d->setState(dragState);
    if (DragState::DS_FloatingWidget == dragState)
    {
        d->mouseEventHandler = mouseEventHandler;
        if (d->mouseEventHandler)
        {
            d->mouseEventHandler->grabMouse();
        }
    }

    if (!isMaximized())
    {
        moveFloating();
    }
    show();
#else
    Q_UNUSED(mouseEventHandler)
    resize(size);
    d->dragStartMousePosition = dragStartMousePos;
    d->setState(dragState);
    moveFloating();
    show();
#endif
}


void QdfFloatingDockContainer::moveFloating()
{
    QDF_D(QdfFloatingDockContainer)
    int BorderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->dragStartMousePosition - QPoint(BorderSize, 0);
    move(moveToPos);
    switch (d->draggingState)
    {
        case DragState::DS_MousePressed:
            d->setState(DragState::DS_FloatingWidget);
            d->updateDropOverlays(QCursor::pos());
            break;

        case DragState::DS_FloatingWidget:
            d->updateDropOverlays(QCursor::pos());
#ifdef Q_OS_MACOS
            // In OSX when hiding the DockAreaOverlay the application would set
            // the main window as the active window for some reason. This fixes
            // that by resetting the active window to the floating widget after
            // updating the overlays.
            QApplication::setActiveWindow(this);
#endif
            break;
        default:
            break;
    }
}


bool QdfFloatingDockContainer::isClosable() const
{
    QDF_D(const QdfFloatingDockContainer)
    return d->dockContainer->features().testFlag(
            QdfDockWidget::DockWidgetClosable);
}


void QdfFloatingDockContainer::onDockAreasAddedOrRemoved()
{
    QDF_D(QdfFloatingDockContainer)
    auto TopLevelDockArea = d->dockContainer->topLevelDockArea();
    if (TopLevelDockArea)
    {
        d->singleDockArea = TopLevelDockArea;
        QdfDockWidget *CurrentWidget = d->singleDockArea->currentDockWidget();
        d->reflectCurrentWidget(CurrentWidget);
        connect(d->singleDockArea, SIGNAL(currentChanged(int)), this,
                SLOT(onDockAreaCurrentChanged(int)));
    }
    else
    {
        if (d->singleDockArea)
        {
            disconnect(d->singleDockArea, SIGNAL(currentChanged(int)), this,
                       SLOT(onDockAreaCurrentChanged(int)));
            d->singleDockArea = nullptr;
        }
        d->setWindowTitle(d->floatingContainersTitle());
        setWindowIcon(QApplication::windowIcon());
    }
}


void QdfFloatingDockContainer::updateWindowTitle()
{
    QDF_D(QdfFloatingDockContainer)
    if (d->hiding)
    {
        return;
    }

    auto topLevelDockArea = d->dockContainer->topLevelDockArea();
    if (topLevelDockArea)
    {
        QdfDockWidget *currentWidget = topLevelDockArea->currentDockWidget();
        if (currentWidget)
        {
            d->reflectCurrentWidget(currentWidget);
        }
    }
    else
    {
        d->setWindowTitle(d->floatingContainersTitle());
        setWindowIcon(QApplication::windowIcon());
    }
}


void QdfFloatingDockContainer::onDockAreaCurrentChanged(int index)
{
    QDF_D(QdfFloatingDockContainer)
    Q_UNUSED(index);
    QdfDockWidget *CurrentWidget = d->singleDockArea->currentDockWidget();
    d->reflectCurrentWidget(CurrentWidget);
}

bool QdfFloatingDockContainer::hasTopLevelDockWidget() const
{
    QDF_D(const QdfFloatingDockContainer)
    return d->dockContainer->hasTopLevelDockWidget();
}

QdfDockWidget *QdfFloatingDockContainer::topLevelDockWidget() const
{
    QDF_D(const QdfFloatingDockContainer)
    return d->dockContainer->topLevelDockWidget();
}

QList<QdfDockWidget *> QdfFloatingDockContainer::dockWidgets() const
{
    QDF_D(const QdfFloatingDockContainer)
    return d->dockContainer->dockWidgets();
}

void QdfFloatingDockContainer::hideAndDeleteLater()
{
    QDF_D(QdfFloatingDockContainer)
    d->autoHideChildren = false;
    hide();
    deleteLater();
}


void QdfFloatingDockContainer::finishDragging()
{
    QDF_D(QdfFloatingDockContainer)
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowOpacity(1);
    activateWindow();
    if (d->mouseEventHandler)
    {
        d->mouseEventHandler->releaseMouse();
        d->mouseEventHandler = nullptr;
    }
#endif
    d->titleMouseReleaseEvent();
}

#ifdef Q_OS_MACOS

bool QdfFloatingDockContainer::event(QEvent *e)
{
    QDF_D(QdfFloatingDockContainer)
    switch (d->draggingState)
    {
        case DragState::DS_Inactive:
            {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
                if (e->type() == QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
    #else
                if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
    #endif
                {
                    d->DragStartPos = pos();
                    d->setState(DragState::DS_MousePressed);
                }
            }
            break;

        case DragState::DS_MousePressed:
            switch (e->type())
            {
                case QEvent::NonClientAreaMouseButtonDblClick:
                    d->setState(DragState::DS_Inactive);
                    break;

                case QEvent::Resize:
                    if (!isMaximized())
                    {
                        d->setState(DragState::DS_Inactive);
                    }
                    break;

                default:
                    break;
            }
            break;

        case DragState::DS_FloatingWidget:
            if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
            {
                d->titleMouseReleaseEvent();
            }
            break;

        default:
            break;
    }

    #if (ADS_DEBUG_LEVEL > 0)
    qDebug() << QTime::currentTime() << "QdfFloatingDockContainer::event " << e->type();
    #endif
    return QWidget::event(e);
}


void QdfFloatingDockContainer::moveEvent(QMoveEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
    QWidget::moveEvent(event);
    switch (d->draggingState)
    {
        case DragState::DS_MousePressed:
            d->setState(DragState::DS_FloatingWidget);
            d->updateDropOverlays(QCursor::pos());
            break;

        case DragState::DS_FloatingWidget:
            d->updateDropOverlays(QCursor::pos());
            QApplication::setActiveWindow(this);
            break;
        default:
            break;
    }
}
#endif


#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

void QdfFloatingDockContainer::onMaximizeRequest()
{
    if (windowState() == Qt::WindowMaximized)
    {
        showNormal();
    }
    else
    {
        showMaximized();
    }
}


void QdfFloatingDockContainer::showNormal(bool fixGeometry)
{
    QDF_D(QdfFloatingDockContainer)
    if (windowState() == Qt::WindowMaximized)
    {
        QRect oldNormal = normalGeometry();
        QDockWidget::showNormal();
        if (fixGeometry)
        {
            setGeometry(oldNormal);
        }
    }
    if (d->TitleBar)
    {
        d->TitleBar->setMaximizedIcon(false);
    }
}


void QdfFloatingDockContainer::showMaximized()
{
    QDF_D(QdfFloatingDockContainer)
    QDockWidget::showMaximized();
    if (d->TitleBar)
    {
        d->TitleBar->setMaximizedIcon(true);
    }
}


bool QdfFloatingDockContainer::isMaximized() const
{
    return windowState() == Qt::WindowMaximized;
}


void QdfFloatingDockContainer::show()
{
    internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
    internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER");
    QDockWidget::show();
}


void QdfFloatingDockContainer::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
    d->IsResizing = true;
    QDockWidget::resizeEvent(event);
}

static bool s_mousePressed = false;

void QdfFloatingDockContainer::moveEvent(QMoveEvent *event)
{
    QDF_D(QdfFloatingDockContainer)
    QDockWidget::moveEvent(event);
    if (!d->IsResizing && event->spontaneous() && s_mousePressed)
    {
        d->setState(DragState::DS_FloatingWidget);
        d->updateDropOverlays(QCursor::pos());
    }
    d->IsResizing = false;
}


bool QdfFloatingDockContainer::event(QEvent *e)
{
    bool result = QDockWidget::event(e);
    switch (e->type())
    {
        case QEvent::WindowActivate:
            s_mousePressed = false;
            break;
        case QEvent::WindowDeactivate:
            s_mousePressed = true;
            break;
        default:
            break;
    }
    return result;
}


bool QdfFloatingDockContainer::hasNativeTitleBar()
{
    QDF_D(QdfFloatingDockContainer)
    return d->TitleBar == nullptr;
}
#endif