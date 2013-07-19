# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = klaid.h rndclass.h ver.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/klaid.pot

KDE_ICON = ktimer

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = klaid

# the application source, library search path, and link libraries
klaid_SOURCES = main.cpp klaid.cpp klaidwidget.cpp \
	klaidwidgetbase.ui rndclass.cpp
klaid_LDFLAGS = $(KDE_RPATH) $(all_libraries)
klaid_LDADD = $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = klaid.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/klaid
shellrc_DATA = klaidui.rc

