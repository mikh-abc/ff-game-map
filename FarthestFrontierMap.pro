QT       += core gui widgets

PRECOMPILED_HEADER = stdafx.h

CONFIG += c++17

SOURCES += \
    FarthestFrontierMap.cpp \
    main.cpp \
    FarthestFrontierMapFrame.cpp \
    stdafx.cpp

HEADERS += \
    DataDefines.h \
    FarthestFrontierMap.h \
    FarthestFrontierMapFrame.h \
    stdafx.h

FORMS += \
    FarthestFrontierMapFrame.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
