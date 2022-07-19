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

LIBS += -L../opencv/bin -L../opencv/lib -lopencv_core411 \
                                        -lopencv_videoio411   \
                                        -lopencv_highgui411   \
                                        -lopencv_objdetect411 \
                                        -lopencv_imgproc411   \
                                        -lopencv_tracking411

INCLUDEPATH += ../opencv/include




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
