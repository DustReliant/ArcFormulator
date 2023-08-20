# ArcCore.pro
TEMPLATE = lib
CONFIG += dll



HEADERS += Point2D.h \
           GemBezierCurve.h



SOURCES += Point2D.cpp \
           GemBezierCurve.cpp



# 动态库生成目录
DESTDIR = $$PWD/../../out/libs
TARGET = ArcCore

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,d)
}

INCLUDEPATH += $$PWD
