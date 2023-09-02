# ui.pro
TEMPLATE = app

QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    view/QArcGraphicsView.cpp \
    view/QArcGraphicsItem.cpp \
    #ribbon/*.cpp

HEADERS += \
    MainWindow.h \
    view/QArcGraphicsView.h \
    view/QArcGraphicsItem.h \
    #ribbon/*.h

#INCLUDEPATH += $$PWD/../../3dparty/SARibbon/include
#INCLUDEPATH += $$PWD/../ArcCore

#LIBS += -L$$PWD/../../libs -lArcCore

#CONFIG(debug, debug|release) {
#    LIBS += -lSARibbonBard
#} else {
#    LIBS += -lSARibbonBar
#}

# 链接SARibbonBar动态库文件
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../3dparty/SARibbon/lib/ -lSARibbonBar
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../3dparty/SARibbon/lib/ -lSARibbonBard

INCLUDEPATH += $$PWD/../../3dparty/SARibbon/include
DEPENDPATH += $$PWD/../../3dparty/SARibbon/include
