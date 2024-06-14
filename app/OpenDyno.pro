QT       += core gui charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dynochartseriespen.cpp \
    dynochartview.cpp \
    dynochartviewinfo.cpp \
    dynochartviewlegend.cpp \
    dynodevice.cpp \
    dynorun.cpp \
    dynorunresult.cpp \
    dynosettings.cpp \
    kalmanfilter.cpp \
    main.cpp \
    wndabout.cpp \
    wndcalibrate.cpp \
    wndmain.cpp \
    wndprofile.cpp \
    wndsettings.cpp \
    wndspeedacc.cpp

HEADERS += \
    dynochartseriespen.h \
    dynochartview.h \
    dynochartviewinfo.h \
    dynochartviewlegend.h \
    dynodevice.h \
    dynorun.h \
    dynorunresult.h \
    dynosettings.h \
    kalmanfilter.h \
    vehicleprofile.h \
    wndabout.h \
    wndcalibrate.h \
    wndmain.h \
    wndprofile.h \
    wndsettings.h \
    wndspeedacc.h

FORMS += \
    wndabout.ui \
    wndcalibrate.ui \
    wndmain.ui \
    wndprofile.ui \
    wndsettings.ui \
    wndspeedacc.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
