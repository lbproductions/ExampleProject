TARGET = lbdatabase
TEMPLATE = lib

QT += sql network
DEFINES *= QT_USE_QSTRINGBUILDER

OBJECTS_DIR = $$OUT_PWD/../build/
DESTDIR = $$OUT_PWD/../

INCLUDEPATH += $$PWD/include

macx {
    QMAKE_LFLAGS += -F$$PWD/../frameworks/
    LIBS += -framework QxtCore \
            -framework QxtNetwork \
            -framework QxtWeb

    INCLUDEPATH += $$PWD/../include/QxtCore \
                    $$PWD/../include/QxtWeb \
                    $$PWD/../include/QxtNetwork \
                    $$PWD/../frameworks/QxtCore.framework/Headers \
                    $$PWD/../frameworks/QxtWeb.framework/Headers \
                    $$PWD/../frameworks/QxtNetwork.framework/Headers
}

HEADERS += \
    LBDatabase.h \
    property.h \
    propertyvalue.h \
    local/column.h \
    local/database.h \
    local/row.h \
    local/table.h \
    local/entitylocal.h \
    local/storagelocal.h \
    local/contextlocal.h \
    local/entitytypelocal.h \
    local/attributevaluelocal.h \
    local/relationlocal.h \
    local/relationvaluelocal.h \
    local/relationvaluelocal_p.h \
    local/functionlocal.h \
    local/functionvaluelocal.h \
    local/calculator.h \
    local/concreterelation.h \
    local/enumattributelocal.h \
    export/graphvizexporter.h \
    export/cppexporter.h \
    export/storagewriter.h \
    export/writer.h \
    export/contextwriter.h \
    export/entitytypewriter.h \
    export/calculatorwriter.h \
    server/restserver.h \
    attribute.h \
    attributevalue.h \
    context.h \
    entity.h \
    entitytype.h \ #\
    local/attributelocal.h \
    local/attributelocal_p.h \
    function.h \
    functionvalue.h \
    storage.h \
    relation.h \
    relationvalue.h
    #enumattribute.h \
    #functionvalue.h \
    #function.h \
    #relation.h \
    #storage.h \
    #relationvalue.h

SOURCES += \
    property.cpp \
    propertyvalue.cpp \
    local/column.cpp \
    local/database.cpp \
    local/row.cpp \
    local/table.cpp \
    local/entitylocal.cpp \
    local/storagelocal.cpp \
    local/contextlocal.cpp \
    local/entitytypelocal.cpp \
    local/attributelocal.cpp \
    local/attributevaluelocal.cpp \
    local/relationlocal.cpp \
    local/relationvaluelocal.cpp \
    local/functionlocal.cpp \
    local/functionvaluelocal.cpp \
    local/calculator.cpp \
    local/enumattributelocal.cpp \
    export/graphvizexporter.cpp \
    export/cppexporter.cpp \
    export/storagewriter.cpp \
    export/writer.cpp \
    export/contextwriter.cpp \
    export/entitytypewriter.cpp \
    export/calculatorwriter.cpp \
    server/restserver.cpp

OTHER_FILES += \
    DoxyFile \
    LBDatabase.pro.user

