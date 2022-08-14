QT -= gui
QT += widgets network

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    httpserver.cpp \
    jsonanswer.cpp \
    main.cpp \
    socketrunnable.cpp \
    cpp-lib/zerostoragecaptcha.cpp

HEADERS += \
    httpserver.h \
    jsonanswer.h \
    socketrunnable.h \
    cpp-lib/zerostoragecaptcha.h
