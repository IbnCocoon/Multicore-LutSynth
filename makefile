# Project name
PRJ = SerialTest

# Source files
OBJS	= main.o kernel.o lutsynth.o voice.o oscillator.o mixer.o voicehandler.o midihandler.o serialmidi.o envelope.o sdload.o lfo.o noise.o moogfilter.o patch.o
# Path to Circle (adjust if different)
CIRCLEHOME = ../circle

LIBS	= $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/usb/gadget/libusbgadget.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/sound/libsound.a \
	  $(CIRCLEHOME)/lib/libcircle.a \
	  $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a

# Include Circle build rules
include $(CIRCLEHOME)/Rules.mk

-include $(DEPS)
