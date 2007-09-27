TEMPLATE	= app
LANGUAGE	= C++
HEADERS		= aboutdialog.h chartdialog.h console.h exportdialog.h \
		  hostdialog.h infodialog.h kmchart.h openviewdialog.h \
		  recorddialog.h saveviewdialog.h seealsodialog.h \
		  settingsdialog.h tabdialog.h \
		  chart.h fileiconprovider.h main.h namespace.h \
		  qcolorpicker.h source.h tab.h \
		  timeaxis.h timebutton.h timecontrol.h
SOURCES		= aboutdialog.cpp chartdialog.cpp console.cpp exportdialog.cpp \
		  hostdialog.cpp infodialog.cpp kmchart.cpp openviewdialog.cpp \
		  recorddialog.cpp saveviewdialog.cpp seealsodialog.cpp \
		  settingsdialog.cpp tabdialog.cpp \
		  chart.cpp fileiconprovider.cpp main.cpp namespace.cpp \
		  qcolorpicker.cpp source.cpp tab.cpp \
		  timeaxis.cpp timebutton.cpp timecontrol.cpp \
		  view.cpp
FORMS		= aboutdialog.ui chartdialog.ui console.ui exportdialog.ui \
		  hostdialog.ui infodialog.ui kmchart.ui openviewdialog.ui \
		  recorddialog.ui saveviewdialog.ui seealsodialog.ui \
		  settingsdialog.ui tabdialog.ui
ICON		= kmchart.icns
RESOURCES	= kmchart.qrc
INCLUDEPATH	+= ../include ../libqmc ../libqwt
LIBS		= -lpcp -lqmc -lqwt
LIBS		+= -L../libqmc -L../libqmc/build/Default
LIBS		+= -L../libqwt -L../libqwt/build/Default
QT		+= assistant network
CONFIG		+= qt warn_on debug #release
