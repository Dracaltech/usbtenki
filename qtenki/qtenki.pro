######################################################################
# Automatically generated by qmake (2.01a) Sun Nov 27 14:30:53 2011
######################################################################
CONFIG += release
TEMPLATE = app
TARGET = qtenki
DEPENDPATH += .
INCLUDEPATH += . ../common ../library
RESOURCES	= qtenki.qrc

win32: INCLUDEPATH += ../../libusb/include
win32: DEFINES += WINDOWS_VERSION
win32: LIBS += -L../../libusb/lib/gcc
win32:RC_FILE = qtenki.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

# Input

HEADERS += ../library/usbtenki.h ../common/usbtenki_cmds.h ../common/usbtenki_version.h About.h \
			 TextViewer.h TenkiSources.h DashSensor.h TenkiDashboard.h TenkiDevice.h Logger.h \
			DataSourceCheckBox.h SimpleLogger.h MainWindow.h BigView.h GraphView.h BigLabel.h SourceAliasEdit.h \
			PowerPreference.h CurrentPreference.h TemperaturePreference.h PressurePreference.h FrequencyPreference.h ConfigCheckbox.h \
			VoltagePreference.h ConfigPanel.h SelectableColor.h globals.h qcustomplot.h GraphLegendPreference.h single_application.h LengthPreference.h \
			MinMaxResettable.h


SOURCES += ../library/usbtenki.c ../library/convertRaw.c main.cpp \
			TextViewer.cpp TenkiSources.cpp SimpleLogger.cpp DashSensor.cpp TenkiDashboard.cpp \
			TenkiDevice.cpp Logger.cpp DataSourceCheckBox.cpp About.cpp MainWindow.cpp BigView.cpp GraphView.cpp \
			PowerPreference.cpp CurrentPreference.cpp VoltagePreference.cpp TemperaturePreference.cpp PressurePreference.cpp FrequencyPreference.cpp ConfigCheckbox.cpp \
			SourceAliasEdit.cpp BigLabel.cpp ConfigPanel.cpp SelectableColor.cpp globals.cpp qcustomplot.cpp GraphLegendPreference.cpp single_application.cpp \
			LengthPreference.cpp MinMaxResettable.cpp

LIBS += -lusb
