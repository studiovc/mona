TARGET  = MSHELL
SOURCES = Form1.cpp Terminal.cpp
INSTDIR = $(BINDIR)/APPS/MONAFRMS

ifndef $(MONADIR)
export MONADIR=$(shell cd $(PWD)/../../../mona; pwd)
endif

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lgui-imp
ADDLINKDEP = $(MONADIR)/lib/libgui-imp.a
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lgui
ADDLINKDEP = $(MONADIR)/lib/libgui.a
include $(SHAREDIR)/configs/monapi-el5.inc
endif

CXXFLAGS += -DDEBUG
-include dependencies
