CMAKE_MINIMUM_REQUIRED(VERSION 3.0..3.20)
PROJECT(qdf_framework)
ADD_LIBRARY(${PROJECT_NAME} SHARED)

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE QDF_LIBRARY)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE QDF_LIBRARY)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE -QDF_LIBRARY)
ENDIF()

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

set(INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/src/desktopframework/include")

TARGET_SOURCES(${PROJECT_NAME}
        PRIVATE
        ${INCLUDE_DIRECTORY}/qdfapplication.h
        ${INCLUDE_DIRECTORY}/qdfevent.h
        ${INCLUDE_DIRECTORY}/qdfmainwindow.h
        ${INCLUDE_DIRECTORY}/qdfplugin.h
        ${INCLUDE_DIRECTORY}/qdfribbon.h

        # ribbon
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonbackstageview.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonbar.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonbuttoncontrols.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribboncontrols.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribboncustomizedialog.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribboncustomizemanager.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribboncustomizepage.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbongallery.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbongallerycontrol.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbongroup.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribboninputcontrols.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonpage.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonquickaccessbar.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonsliderpane.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonstatusbar.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonsystempopupbar.h

        # dockwidget
        ${INCLUDE_DIRECTORY}/dockwidget/qdfautohidedockcontainer.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfautohidesidebar.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfautohidetab.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockareatabbar.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockareatitlebar.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockareawidget.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockcontainerwidget.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockfocuscontroller.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockmanager.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockoverlay.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdocksplitter.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockwidget.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfdockwidgettab.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdfelidinglabel.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdffloatingdockcontainer.h
        ${INCLUDE_DIRECTORY}/dockwidget/qdffloatingwidget.h
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
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbontooltip.h
        ribbon/qdfstylehelpers.cpp
        ${INCLUDE_DIRECTORY}/ribbon/qdfstylehelpers.h
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
        ${INCLUDE_DIRECTORY}/ribbon/qdfcommonstyle.h
        ribbon/style/qdfcommonstyle_p.h
        ribbon/style/qdfcommonstyle_win.cpp
        ribbon/style/qdfofficeframehelper.h
        ribbon/style/qdfofficeframehelper_win.cpp
        ribbon/style/qdfofficeframehelper_win.h
        ribbon/style/qdfofficepaintmanager.cpp
        ribbon/style/qdfofficepopupcolorbutton.cpp
        ${INCLUDE_DIRECTORY}/ribbon/qdfofficepopupcolorbutton.h
        ribbon/style/qdfofficepopupmenu.cpp
        ${INCLUDE_DIRECTORY}/ribbon/qdfofficepopupmenu.h
        ribbon/style/qdfofficepopupwindow_p.h
        ribbon/style/qdfofficepopupwindow.cpp
        ${INCLUDE_DIRECTORY}/ribbon/qdfofficepopupwindow.h
        ribbon/style/qdfofficestyle.cpp
        ${INCLUDE_DIRECTORY}/ribbon/qdfofficestyle.h
        ribbon/style/qdfofficestyle_p.h
        ribbon/style/qdfpopuphelpers.cpp
        ribbon/style/qdfpopuphelpers.h
        ribbon/style/qdfribbonpaintmanager.cpp
        ribbon/style/qdfribbonstyle.cpp
        ribbon/style/qdfribbonstyle_p.h
        ${INCLUDE_DIRECTORY}/ribbon/qdfribbonstyle.h

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

        qdfapplication.cpp
        qdfevent.cpp
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

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        PRIVATE
        Qt5::Core Qt5::Widgets Qt5::Gui
        user32
        gdi32
        shell32
        comctl32
        )

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}
        PRIVATE
        ${INCLUDE_DIRECTORY})