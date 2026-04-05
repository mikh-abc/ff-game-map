QT       += core gui widgets network concurrent svg

PRECOMPILED_HEADER = stdafx.h

CONFIG += c++17

SOURCES += \
    DataDefines.cpp \
    DataReader.cpp \
    FileDataReader.cpp \
    GameMapChanger.cpp \
    MapWidget.cpp \
    MemoryDataReader.cpp \
    SaveDialog.cpp \
    StaticData.cpp \
    VillagerModel.cpp \
    main.cpp \
    FarthestFrontierMapFrame.cpp \
    stdafx.cpp

HEADERS += \
    DataDefines.h \
    DataReader.h \
    FarthestFrontierMapFrame.h \
    FileDataReader.h \
    GameMapChanger.h \
    MapWidget.h \
    MemoryDataReader.h \
    SaveDialog.h \
    StaticData.h \
    VillagerModel.h \
    stdafx.h

FORMS += \
    FarthestFrontierMapFrame.ui \
    SaveDialog.ui

RC_FILE = FarthestFrontierMapFrame.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FarthestFrontierMap.qrc
