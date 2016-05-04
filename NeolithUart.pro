greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets  serialport  network  winextras
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = NeolithUart
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    SoftInServer/softinserver.cpp \
    uartsend.cpp \
    uartreceive.cpp \
    saverevtofile.cpp \
    TrayIcon/trayiconserver.cpp \
    SendTxtFileSetting/sendtxtfilesetting.cpp \
    uartsendtxtfile.cpp \
    QDelayTime/qdelaytime.cpp \
    RStartScreen/rstartscreen.cpp \
    SaveFileSetting/savefilesettingdialog.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    SoftInServer/softinserver.h \
    TrayIcon/trayiconserver.h \
    sendtxtfilesetting.h \
    SendTxtFileSetting/sendtxtfilesetting.h \
    QDelayTime/qdelaytime.h \
    RStartScreen/rstartscreen.h \
    SaveFileSetting/savefilesettingdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    SendTxtFileSetting/sendtxtfilesetting.ui \
    RStartScreen/rstartscreen.ui \
    SaveFileSetting/savefilesettingdialog.ui

RESOURCES += \
    terminal.qrc \
    neolithuart.qrc

RC_FILE = app.rc


