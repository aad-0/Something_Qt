QT  += core gui
QT  += serialport
QT += widgets datavisualization



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets datavisualization

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
		uiaddport.cpp \
	acceldevice.cpp \
	comm.c \
	vcomhandler.cpp \
	main.cpp \
	mainwindow.cpp \
    threadmanager.cpp \
    csvwriter.cpp

HEADERS += \
		uiaddport.h \
	acceldevice.h \
	comm.h \
	vcomhandler.h \
	mainwindow.h \
    threadmanager.h \
    csvwriter.h

FORMS += \
		uiaddport.ui \
		mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
