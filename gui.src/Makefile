#
# uCsim gui.src/Makefile
#
# (c) Drotos Daniel, Talker Bt. 1999
#

SHELL		= /bin/sh
CXX		= g++
CPP		= gcc -E
CXXCPP		= g++ -E
RANLIB		= ranlib
INSTALL		= /usr/bin/install -c

PRJDIR		= ..

DEFS            = $(subs -DHAVE_CONFIG_H,,-DHAVE_CONFIG_H)
CPPFLAGS        =  -I. -I$(PRJDIR) \
		  -I$(PRJDIR)/sim.src -I$(PRJDIR)/cmd.src
CFLAGS          = -g -O2 -Wall
CXXFLAGS        = -g -O2 -gstabs+ -g3 -pipe -Wall
M_OR_MM         = -MM

LIBS		= -lnsl  -lpanel -lcurses -L$(PRJDIR)

curses_ok	= yes

prefix          = /usr/local
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libdir          = ${exec_prefix}/lib
datadir         = ${prefix}/share
includedir      = ${prefix}/include
mandir          = ${prefix}/man
man1dir         = $(mandir)/man1
man2dir         = $(mandir)/man2
infodir         = ${prefix}/info
srcdir          = .

enable_serio	= yes
PKGS		=
ifeq ($(enable_serio),yes)
SERIO_PKG	= serio.src
else
SERIO_PKG	=
endif
CURSES_PKGS	= $(SERIO_PKG)

# Common code
OBJECTS		= rec.o
SOURCES		= $(patsubst %.o,%.cc,$(OBJECTS))

# uCsim only
UCSIM_OBJECTS	= gui.o if.o
UCSIM_SOURCES	= $(patsubst %.o,%.cc,$(UCSIM_OBJECTS))

# Tool only
TOOL_OBJECTS	=
TOOL_SOURCES	= $(patsubst %.o,%.cc,$(TOOL_OBJECTS))

ALL_SOURCES	= $(SOURCES) $(UCSIM_SOURCES) $(TOOL_SOURCES)


# Compiling entire program or any subproject
# ------------------------------------------
all: gui.src

gui.src: checkconf gui_api
ifeq ($(curses_ok),yes)
	@for pkg in dummy $(CURSES_PKGS); do\
	  if test -d $$pkg; then $(MAKE) -C $$pkg $$pkg; else : ; fi ;\
	done
endif
#	@for pkg in $(PKGS); do\
#	  $(MAKE) -C $$pkg $$pkg ;\
#	done

gui_api: ucsim_lib tool_lib


# Compiling and installing everything and runing test
# ---------------------------------------------------
install: install_gui_api
ifeq ($(curses_ok),yes)
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) install ; cd ..;\
	done
endif

install_gui_api:


# Deleting all the installed files
# --------------------------------
uninstall: uninstall_gui_api
ifeq ($(curses_ok),yes)
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) uninstall ; cd ..;\
	done
endif

uninstall_gui_api:


# Performing self-test
# --------------------
check: check_local
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) check ; cd ..;\
	done

check_local:

test: test_local
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) test ; cd ..;\
	done

test_local:


# Performing installation test
# ----------------------------
installcheck: installcheck_gui_api
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) installcheck ; cd ..;\
	done

installcheck_gui_api:


# Creating installation directories
# ---------------------------------
installdirs:
	@for pkg in $(CURSES_PKGS); do\
	  cd $$pkg && $(MAKE) installdirs ; cd ..;\
	done


# Creating dependencies
# ---------------------
dep: Makefile.dep

Makefile.dep: $(ALL_SOURCES) *.h
	$(CXXCPP) -MM $(CPPFLAGS) $(M_OR_MM) $(ALL_SOURCES) >Makefile.dep

include Makefile.dep
include clean.mk


# My rules
# --------
ucsim_lib: $(PRJDIR)/libguiucsim.a

$(PRJDIR)/libguiucsim.a: $(OBJECTS) $(UCSIM_OBJECTS)
	ar -rcu $*.a $(OBJECTS) $(UCSIM_OBJECTS)
	$(RANLIB) $*.a

tool_lib: $(PRJDIR)/libguitool.a

$(PRJDIR)/libguitool.a: $(OBJECTS) $(TOOL_OBJECTS)
	ar -rcu $*.a $(OBJECTS) $(TOOL_OBJECTS)
	$(RANLIB) $*.a

.cc.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@


# Remaking configuration
# ----------------------
checkconf:
	@if [ -f $(PRJDIR)/devel ]; then\
	  $(MAKE) -f conf.mk srcdir="$(srcdir)" PRJDIR="$(PRJDIR)" freshconf;\
	fi

# End of gui.src/Makefile
