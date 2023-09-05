CMAKE_MINIMUM_REQUIRED(VERSION 3.0..3.20)
PROJECT(gsframework)
ADD_LIBRARY(${PROJECT_NAME} SHARED)

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE QDF_LIBRARY)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE QDF_LIBRARY)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE -QDF_LIBRARY)
ENDIF()

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(FRAMEWORK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/desktopframework/include")
TARGET_SOURCES(${PROJECT_NAME}
        PRIVATE
        ${FRAMEWORK_INCLUDE_DIR}/qdfmainwindow.h
        ${FRAMEWORK_INCLUDE_DIR}/qdfribbon.h

        # ribbon
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonbackstageview.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonbar.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonbuttoncontrols.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribboncontrols.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribboncustomizedialog.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribboncustomizemanager.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribboncustomizepage.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbongallery.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbongallerycontrol.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbongroup.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribboninputcontrols.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonpage.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonquickaccessbar.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonsliderpane.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonstatusbar.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonsystempopupbar.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbontooltip.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfstylehelpers.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfcommonstyle.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfofficepopupcolorbutton.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfofficepopupmenu.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfofficepopupwindow.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfofficestyle.h
        ${FRAMEWORK_INCLUDE_DIR}/ribbon/qdfribbonstyle.h

        # dockwidget
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfautohidedockcontainer.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfautohidesidebar.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfautohidetab.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockareatabbar.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockareatitlebar.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockareawidget.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockcontainerwidget.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockfocuscontroller.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockmanager.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockoverlay.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdocksplitter.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockwidget.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfdockwidgettab.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdfelidinglabel.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdffloatingdockcontainer.h
        ${FRAMEWORK_INCLUDE_DIR}/dockwidget/qdffloatingwidget.h
        )

TARGET_SOURCES(${PROJECT_NAME}
        PRIVATE
        ribbon/private/qdfribbonbarcustomizepage.ui
        ribbon/private/qdfribbonquickaccessbarcustomizepage.ui
        ribbon/private/ui_qdfribbonbarcustomizepage.h
        ribbon/private/ui_qdfribbonquickaccessbarcustomizepage.h        
        ribbon/qdfribbonrenamedialog.ui
        ribbon/ui_qdfribbonrenamedialog.h

        # ribbon
        ribbon/qdfribbon_def.cpp
        ribbon/qdfribbon_def.h
        ribbon/qdfribbonbackstageview.cpp
        ribbon/qdfribbonbar.cpp
        ribbon/qdfribbonbuttoncontrols.cpp
        ribbon/qdfribboncontrols.cpp
        ribbon/qdfribboncustomizedialog.cpp
        ribbon/qdfribboncustomizemanager.cpp
        ribbon/qdfribboncustomizepage.cpp
        ribbon/qdfribbongallery.cpp
        ribbon/qdfribbongallerycontrol.cpp
        ribbon/qdfribbongroup.cpp
        ribbon/qdfribboninputcontrols.cpp
        ribbon/qdfribbonpage.cpp
        ribbon/qdfribbonquickaccessbar.cpp
        ribbon/qdfribbonsliderpane.cpp
        ribbon/qdfribbonstatusbar.cpp
        ribbon/qdfribbonsystempopupbar.cpp
        ribbon/qdfribbontabbar.cpp
        ribbon/qdfribbontabbar.h
        ribbon/qdfribbontooltip.cpp
        ribbon/qdfstylehelpers.cpp
        ribbon/ribbonworkspace.cpp
        ribbon/ribbonworkspace.h
    
        ribbon/private/qdfribbon_p.cpp
        ribbon/private/qdfribbon_p.h
        ribbon/private/qdfribbonbackstageview_p.h
        ribbon/private/qdfribbonbar_p.h
        ribbon/private/qdfribbonbutton_p.cpp
        ribbon/private/qdfribbonbutton_p.h
        ribbon/private/qdfribboncontrols_p.h
        ribbon/private/qdfribboncustomizedialog_p.h
        ribbon/private/qdfribboncustomizemanager_p.h
        ribbon/private/qdfribboncustomizepage_p.h
        ribbon/private/qdfribbongallery_p.h
        ribbon/private/qdfribbongroup_p.h
        ribbon/private/qdfribbonpage_p.h
        ribbon/private/qdfribbonquickaccessbar_p.h
        ribbon/private/qdfribbonsliderpane_p.h
        ribbon/private/qdfribbonstatusbar_p.h
        ribbon/private/qdfribbonsystempopupbar_p.h

        ribbon/style/qdfcommonstyle.cpp
        ribbon/style/qdfcommonstyle_p.h
        ribbon/style/qdfcommonstyle_win.cpp
        ribbon/style/qdfofficeframehelper.h
        ribbon/style/qdfofficeframehelper_win.cpp
        ribbon/style/qdfofficeframehelper_win.h
        ribbon/style/qdfofficepaintmanager.cpp
        ribbon/style/qdfofficepopupcolorbutton.cpp
        ribbon/style/qdfofficepopupmenu.cpp
        ribbon/style/qdfofficepopupwindow_p.h
        ribbon/style/qdfofficepopupwindow.cpp
        ribbon/style/qdfofficestyle.cpp
        ribbon/style/qdfofficestyle_p.h
        ribbon/style/qdfpopuphelpers.cpp
        ribbon/style/qdfpopuphelpers.h
        ribbon/style/qdfribbonpaintmanager.cpp
        ribbon/style/qdfribbonstyle.cpp
        ribbon/style/qdfribbonstyle_p.h
        
        # dockwidget
        dockwidget/qdfautohidedockcontainer.cpp
        dockwidget/qdfautohidesidebar.cpp
        dockwidget/qdfautohidetab.cpp
        dockwidget/qdfdockareatabbar.cpp
        dockwidget/qdfdockareatitlebar.cpp
        dockwidget/qdfdockareawidget.cpp
        dockwidget/qdfdockcomponentsfactory.cpp
        dockwidget/qdfdockcomponentsfactory.h
        dockwidget/qdfdockcontainerwidget.cpp
        dockwidget/qdfdockfocuscontroller.cpp
        dockwidget/qdfdockmanager.cpp
        dockwidget/qdfdockoverlay.cpp
        dockwidget/qdfdockoverlaycross.cpp
        dockwidget/qdfdockoverlaycross.h
        dockwidget/qdfdocksplitter.cpp
        dockwidget/qdfdockwidget.cpp
        dockwidget/qdfdockwidgethelper.cpp
        dockwidget/qdfdockwidgethelper.h
        dockwidget/qdfdockwidgettab.cpp
        dockwidget/qdfelidinglabel.cpp
        dockwidget/qdffloatingdockcontainer.cpp
        dockwidget/qdffloatingdragpreview.cpp
        dockwidget/qdffloatingdragpreview.h
        dockwidget/qdfresizehandle.cpp
        dockwidget/qdfresizehandle.h
        dockwidget/private/qdfautohidedockcontainer_p.h
        dockwidget/private/qdfautohidesidebar_p.h
        dockwidget/private/qdfautohidetab_p.h
        dockwidget/private/qdfdockareatabbar_p.h
        dockwidget/private/qdfdockareatitlebar_p.h
        dockwidget/private/qdfdockareawidget_p.h
        dockwidget/private/qdfdockcontainerwidget_p.h
        dockwidget/private/qdfdockfocuscontroller_p.h
        dockwidget/private/qdfdockmanager_p.h
        dockwidget/private/qdfdockwidgettab_p.h
        dockwidget/private/qdfdockwidget_p.h
        dockwidget/private/qdfelidinglabel_p.h
        dockwidget/private/qdffloatingdockcontainer_p.h
        dockwidget/private/qdffloatingdragpreview_p.h
        dockwidget/private/qdfresizehandle_p.h

        #qdfapplication.cpp
        #qdfevent.cpp
        qdfmainwindow.cpp
        ribbon/qeffects.cpp
        ribbon/qeffects.h
        )

IF(UNIX AND NOT APPLE)
TARGET_SOURCES(${PROJECT_NAME}
        PRIVATE
        dockwidget/linux/qdffloatingwidgettitlebar.cpp
        dockwidget/linux/qdffloatingwidgettitlebar.h
)
ENDIF()

IF(WIN32)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        PRIVATE
        user32
        gdi32
        shell32
        comctl32
        )
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        PRIVATE
        Qt5::Core Qt5::Widgets Qt5::Gui
        )

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}
        PRIVATE
        ${FRAMEWORK_INCLUDE_DIR})