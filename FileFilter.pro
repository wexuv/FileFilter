#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T16:17:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileFilter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    sqlite3/codec.c \
    sqlite3/codecext.c \
    sqlite3/rijndael.c \
    sqlite3/sha2.c \
    sqlite3/sqlite3.c \
    sqlite3/sqlite3secure.c \
    sqlite3/userauth.c \
    sqliteUtility/sqlite_handler.cpp \
    sqliteUtility/sqlite_result.cpp \
    log.cpp \
    FilterThread.cpp \
    sqliteUtility/sqliteDB.cpp

HEADERS += \
        mainwindow.h \
    sqlite3/codec.h \
    sqlite3/rijndael.h \
    sqlite3/sha2.h \
    sqlite3/sqlite3.h \
    sqliteUtility/sqlite_handler.h \
    sqliteUtility/sqlite_result.h \
    log.h \
    FilterThread.h \
    sqliteUtility/sqliteDB.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += "./sqlite3"
