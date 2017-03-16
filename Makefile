# Spectrometer Web

# Target configuration
CHIP ?= esp8266
BOARD ?= nodemcuv2

# Serial flashing parameters
UPLOAD_PORT ?= COM3
UPLOAD_SPEED ?= 921600
UPLOAD_VERB ?= -v

# HTTP upload
HTTP_ADDR ?= 10.50.0.101
HTTP_URI ?= /update
HTTP_USR ?= cosmic
HTTP_PWD ?= cosmicSpace

# SPIFS Directory
FS_DIR ?= ./data

# Libs
LIBS = $(shell cygpath -m $(LOCALAPPDATA)/Arduino15/packages/esp8266/hardware/esp8266/2.3.0/libraries/)

-include makeEspArduino/makeEspArduino.mk

serial: flash
	stty -F /dev/ttyS2 cs8 115200 -cread -clocal
	tio /dev/ttyS2

# % :
# 	$(warning This is the empty rule. Something went wrong.)
# 	@true

ifndef START_TIME
$(info Modules were not checked out... use git clone --recursive in the future. Pulling now.)
$(shell git submodule update --init --recursive)
endif

# Example for a custom rule.
# Most of the build is handled in makeEspArduino.mk
.PHONY : showvars
showvars:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))
	true
