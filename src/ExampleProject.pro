QT       += core gui sql

TARGET = ExampleProject
TEMPLATE = app

DESTDIR = $$OUT_PWD/../
OBJECTS_DIR = $$OUT_PWD/../build/

INCLUDEPATH += $$PWD/../lib/LBDatabase/include
INCLUDEPATH += $$PWD/../lib/LBGui/include

# add a build command
defineReplace( nc  ) {
    return( $$escape_expand(\\n\\t)$$1    )
}
# add a silent build command
defineReplace( snc ) {
    return( $$escape_expand(\\n\\t)"@"$$1 )
}
# add end of line
defineReplace( nl  ) {
    return( $$escape_expand(\\n)         )
}

macx {
    LIBS += -L$$OUT_PWD/../lib/LBDatabase/
    LIBS += -llbdatabase

    LIBS += -L$$OUT_PWD/../lib/LBGui/
    LIBS += -llbgui

    copyFrameworks.target = frameworks
    copyFrameworks.commands += rm -Rf $$DESTDIR/$${TARGET}.app/Contents/Frameworks/
    copyFrameworks.commands += $$snc( mkdir -p $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ )
    copyFrameworks.commands += $$snc( cp -R $$PWD/../lib/LBDatabase/frameworks/* $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ )

    copyDylibs.target = dylibs
    copyDylibs.commands += mkdir -p $$DESTDIR/$${TARGET}.app/Contents/MacOS/
    copyDylibs.commands += $$snc(   cp $$DESTDIR/lib/LBDatabase/liblbdatabase.1.0.0.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/ )
    copyDylibs.commands += $$snc(   cp $$DESTDIR/lib/LBDatabase/liblbdatabase.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/ )
    copyDylibs.commands += $$snc(   cp $$DESTDIR/lib/LBGui/liblbgui.1.0.0.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/ )
    copyDylibs.commands += $$snc(   cp $$DESTDIR/lib/LBGui/liblbgui.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/ )

    QMAKE_POST_LINK = install_name_tool -id @executable_path/liblbdatabase.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/liblbdatabase.1.dylib &&
    QMAKE_POST_LINK += install_name_tool -id @executable_path/liblbgui.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/liblbgui.1.dylib &&
    QMAKE_POST_LINK += install_name_tool -change liblbdatabase.1.dylib @executable_path/liblbdatabase.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/$$TARGET &&
    QMAKE_POST_LINK += install_name_tool -change liblbgui.1.dylib @executable_path/liblbgui.1.dylib $$DESTDIR/$${TARGET}.app/Contents/MacOS/$$TARGET

    QMAKE_EXTRA_TARGETS += copyFrameworks copyDylibs
    PRE_TARGETDEPS += frameworks dylibs
}

SOURCES += main.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/actions.cpp \
    mainwindow/controller.cpp \
    mainwindow/sidebar.cpp \
    misc/logger.cpp \
    mainwindow/views/sampleview.cpp

HEADERS  += \
    mainwindow/mainwindow.h \
    mainwindow/actions.h \
    mainwindow/controller.h \
    mainwindow/sidebar.h \
    misc/logger.h \
    mainwindow/views/sampleview.h


















