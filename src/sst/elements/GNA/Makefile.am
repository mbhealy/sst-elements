# -*- Makefile -*-
#
#

AM_CPPFLAGS = \
	$(MPI_CPPFLAGS) \
	-I$(top_srcdir)/src

compdir = $(pkglibdir)
comp_LTLIBRARIES = libGNA.la
libGNA_la_SOURCES = \
	gna_lib.h \
	neuron.h \
	sts.h \
	sts.cc \
	GNA.cc \
	GNA.h

EXTRA_DIST = \
	tests/test.py \
	tests/test.ref.out \
	README


libGNA_la_LDFLAGS = -module -avoid-version

install-exec-hook:
	$(SST_REGISTER_TOOL) SST_ELEMENT_SOURCE     GNA=$(abs_srcdir)
	$(SST_REGISTER_TOOL) SST_ELEMENT_TESTS      GNA=$(abs_srcdir)/tests

##########################################################################
##########################################################################
##########################################################################


