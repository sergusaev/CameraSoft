QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/counter.cpp \
    src/cvimagewidget.cpp \
    src/detector.cpp \
    main.cpp \
    src/mainwindow.cpp \
    src/processor.cpp

HEADERS += \
    include/counter.h \
    include/cvimagewidget.h \
    include/detector.h \
    include/mainwindow.h \
    include/processor.h

FORMS += \
    ui/mainwindow.ui

INCLUDEPATH += ../opencv/include
INCLUDEPATH += include

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
    file_name = opencv_$${ocv_name}460
  CONFIG(debug, debug|release) {
    LIBS += -l$${file_name}d
  }
  CONFIG(release, debug|release) {
    LIBS += -l$${file_name}
  }
}


win32: RC_FILE = icon_reg.rc

#DEFINES += DEBUG_OUTPUT

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
