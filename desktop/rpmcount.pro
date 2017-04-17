TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lserial

SOURCES += \
    main.cc \
    term.cc

HEADERS += \
    types.h \
    serial_utils.h \
    term.h
