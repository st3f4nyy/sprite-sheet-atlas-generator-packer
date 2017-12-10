QT       += core gui widgets
CONFIG   += c++11


TARGET = sprite-sheet-atlas-generator-packer
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS # The following define makes your compiler emit warnings if you use any feature of Qt which has been marked as deprecated

SOURCES += \
    main.cpp \
    filesystemexplorer.cpp \
    configuration.cpp \
    imagevisualization.cpp \
    imagecreator.cpp \
    exportsettings.cpp \
    resolution.cpp

HEADERS += \
    filesystemexplorer.h \
    configuration.h \
    imagevisualization.h \
    imagecreator.h \
    exportsettings.h \
    resolution.h
