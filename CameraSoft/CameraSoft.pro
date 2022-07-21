QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    counter.cpp \
    detector.cpp \
    main.cpp \
    mainwindow.cpp \
    processor.cpp

HEADERS += \
    counter.h \
    detector.h \
    mainwindow.h \
    processor.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ../opencv/include

# OpenCV
OCV_LIB = \
    core \
    highgui \
    imgproc \
    objdetect \
    videoio \
    dnn

LIBS += -L../opencv/bin -L../opencv/lib

for(ocv_name, OCV_LIB) {
    file_name = opencv_$${ocv_name}411
  CONFIG(debug, debug|release) {
    LIBS += -l$${file_name}d
  }
  CONFIG(release, debug|release) {
    LIBS += -l$${file_name}
  }
}

DEFINES += DEBUG_OUTPUT

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
