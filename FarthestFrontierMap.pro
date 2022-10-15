QT       += core gui widgets concurrent

PRECOMPILED_HEADER = stdafx.h

CONFIG += c++17

SOURCES += \
    DataDefines.cpp \
    GameMap.cpp \
    GameMapChanger.cpp \
    MapWidget.cpp \
    ParseData.cpp \
    SaveDialog.cpp \
    main.cpp \
    FarthestFrontierMapFrame.cpp \
    stdafx.cpp

HEADERS += \
    DataDefines.h \
    FarthestFrontierMapFrame.h \
    GameMap.h \
    GameMapChanger.h \
    MapWidget.h \
    ParseData.h \
    SaveDialog.h \
    stdafx.h

FORMS += \
    FarthestFrontierMapFrame.ui \
    SaveDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FarthestFrontierMap.qrc
