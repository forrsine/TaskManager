QT       += core gui sql printsupport widgets

CONFIG += c++17

# 解决中文乱码（MSVC编译器需要，MinGW通常不需要但也无害）
msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8

TARGET = TaskManager
TEMPLATE = app

HEADERS += \
    taskdata.h \
    dbmanager.h \
    taskmodel.h \
    mainwindow.h \
    reminderthread.h

SOURCES += \
    main.cpp \
    dbmanager.cpp \
    taskmodel.cpp \
    mainwindow.cpp \
    reminderthread.cpp

FORMS += \
    mainwindow.ui
