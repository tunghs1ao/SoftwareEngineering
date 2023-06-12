#-------------------------------------------------
#
# Project created by QtCreator 2023-05-20T16:57:05
#
#-------------------------------------------------

INCLUDEPATH += /home/tunghsiao/libxlsxwriter/include
LIBS += -L/home/tunghsiao/libxlsxwriter/lib -lxlsxwriter

QT       += core gui
QT      +=sql
QT      +=xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-project-SpoCompeManaSys
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    qdlglogin.cpp \
    playerwindow.cpp \
    playerdialog.cpp \
    eventswindow.cpp \
    eventsdialog.cpp \
    judgeswindow.cpp \
    judgesdialog.cpp \
    arrangewindow.cpp \
    arrangedialog.cpp \
    resultwindow.cpp \
    resultdialog.cpp \
    modifypwdialog.cpp

HEADERS += \
        mainwindow.h \
    qdlglogin.h \
    playerwindow.h \
    playerdialog.h \
    eventswindow.h \
    eventsdialog.h \
    judgeswindow.h \
    judgesdialog.h \
    arrangewindow.h \
    arrangedialog.h \
    resultwindow.h \
    resultdialog.h \
    modifypwdialog.h

FORMS += \
        mainwindow.ui \
    qdlglogin.ui \
    playerwindow.ui \
    playerdialog.ui \
    eventswindow.ui \
    eventsdialog.ui \
    judgeswindow.ui \
    judgesdialog.ui \
    arrangewindow.ui \
    arrangedialog.ui \
    resultwindow.ui \
    resultdialog.ui \
    modifypwdialog.ui

RESOURCES += \
    images.qrc
