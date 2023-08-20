# ArcCore.pro
TEMPLATE = lib
CONFIG += dll



HEADERS += Point2D.h \
           GemBezierCurve.h



SOURCES += Point2D.cpp \
           GemBezierCurve.cpp



# ��̬������Ŀ¼
DESTDIR = $$PWD/../../out/libs
TARGET = ArcCore

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,d)
}

INCLUDEPATH += $$PWD
