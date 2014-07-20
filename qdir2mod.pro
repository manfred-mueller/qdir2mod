CONFIG += release
DESTDIR +=   bin
FORMS +=   ui/mainwindow.ui
HEADERS +=   src/mainwindow.h
SOURCES +=   src/main.cpp src/mainwindow.cpp
MOC_DIR +=   build/moc
RCC_DIR +=   build/rcc
UI_DIR +=   build/ui
RC_FILE +=   qdir2mod.rc
RESOURCES +=   resources/qdir2mod.qrc
TRANSLATIONS +=   resources/translations/qdir2mod_de.ts
unix {
  #VARIABLES
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  isEmpty(SYSCONFDIR) {
    SYSCONFDIR = /etc
  }
  BINDIR = $$PREFIX/bin
  DATADIR =$$PREFIX/share

  DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\" SYSCONFDIR=\\\"$$SYSCONFDIR\\\"

  #MAKE INSTALL

  INSTALLS += target desktop icon sysconf

  target.path =$$BINDIR

  sysconf.path = $$SYSCONFDIR
  sysconf.files += $${TARGET}.cfg

  desktop.path = $$DATADIR/applications
  desktop.files += $${TARGET}.desktop

  icon.path = $$DATADIR/pixmaps
  icon.files += resources/icons/$${TARGET}.png
}