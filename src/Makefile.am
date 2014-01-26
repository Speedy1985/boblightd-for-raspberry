## Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS= 

AM_CFLAGS = 

bin_PROGRAMS = \
	boblightd

boblightd_SOURCES = \
	util/condition.cpp \
	util/log.cpp \
	util/misc.cpp \
	util/mutex.cpp \
	util/thread.cpp \
	util/timer.cpp \
	util/serialport.cpp \
	util/timeutils.cpp \
	main.cpp \
	util/tcpsocket.cpp \
	util/messagequeue.cpp \
	client.cpp \
	light.cpp \
	configuration.cpp \
	device/device.cpp \
	device/devicepopen.cpp \
	device/devicers232.cpp \
	device/deviceltbl.cpp \
	device/devicedioder.cpp \
	device/deviceambioder.cpp \
	device/deviceibelight.cpp \
	device/devicelightpack.cpp \
	device/devicespi.cpp \
	util/signaltimer.cpp \
	util/daemonize.cpp

boblightd_LDFLAGS = -lusb-1.0
boblightd_LDADD = 

lib_LTLIBRARIES = \
	libboblight.la

libboblight_la_SOURCES = \
	lib/boblight_client.cpp \
	lib/boblight.cpp \
	util/messagequeue.cpp \
	util/misc.cpp \
	util/tcpsocket.cpp \
	util/modp_numtoa.c

libboblight_la_LDFLAGS =
   
libboblight_la_CPPFLAGS = $(AM_CPPFLAGS)
