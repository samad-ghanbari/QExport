QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./modules/QXlsx/       # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./modules/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./modules/QXlsx/source/  # current QXlsx source path is ./source/
include(./modules/QXlsx/QXlsx.pri)


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lib/excel/exceljsontable.cpp \
    lib/exporter.cpp \
    lib/json/jsontable.cpp \
    lib/pdf/pdfjsontable.cpp \
    main.cpp \

HEADERS += \
    lib/excel/exceljsontable.h \
    lib/exporter.h \
    lib/json/jsontable.h \
    lib/pdf/pdfjsontable.h \
    lib/templates/tableTemplate.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets.qrc
