QT       += core gui sql

TARGET = ExampleProject
TEMPLATE = app

DEPLOY_DIR = $$OUT_PWD/../deploy
DESTDIR = $$OUT_PWD/../
OBJECTS_DIR = $$OUT_PWD/../build/
MYICON = $$PWD/resources/icon/ApplicationIcon.icns
APPCASTURL = http://dl.dropbox.com/u/140012/$$TARGET

DEFINES +=  'APPCASTURL=\'\"$$APPCASTURL\"\''

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
    copyFrameworks.commands += $$snc( cp -R $$PWD/../frameworks/* $$DESTDIR/$${TARGET}.app/Contents/Frameworks/ )

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

     CONFIG(release, debug|release) {
        QMAKE_POST_LINK += && rm -R $$DESTDIR/deploy && $$PWD/../util/deployment/mac/deploy.sh $$PWD $$OUT_PWD/../ $$DEPLOY_DIR $$MYICON $$TARGET $$APPCASTURL && \
                             rm -R $$DESTDIR/deploy/Contents
    }
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
    mainwindow/views/sampleview.h \
    misc/updater.h

OTHER_FILES += \
    ../util/deployment/mac/deploy.sh

macx {
    HEADERS  += \
        misc/sparkleupdater.h \
        misc/cocoainitializer.h
    LIBS += \
        -framework AppKit \
        -framework Sparkle
    OBJECTIVE_SOURCES += \
        misc/sparkleupdater.mm \
        misc/cocoainitializer.mm
}














