TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    element.cpp \
    match3model.cpp

RESOURCES += qml.qrc \
    sprites.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES +=

HEADERS += \
    element.h \
    match3model.h
