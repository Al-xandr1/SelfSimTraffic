#
# OMNeT++/OMNEST Makefile for unstable
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = unstable$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -ICommon_Modules \
    -IFromFile_Source \
    -IMGI_LOG_Source \
    -IMGI_Source \
    -IONOFF_LOG_Source \
    -IONOFF_Source \
    -IStatistics

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/Common_Modules/Simple_CBR_Source.o \
    $O/Common_Modules/Sink.o \
    $O/Common_Modules/TxQueue.o \
    $O/FromFile_Source/FromFile_Source.o \
    $O/MGI_LOG_Source/Ind_MGI_LOG_Source.o \
    $O/MGI_LOG_Source/MGI_LOG_Collector.o \
    $O/MGI_LOG_Source/MGI_LOG_Generator.o \
    $O/MGI_LOG_Source/MGI_LOG_Par.o \
    $O/MGI_Source/Individual_MGI_Source.o \
    $O/MGI_Source/MGI_Collector.o \
    $O/MGI_Source/MGI_Generator.o \
    $O/MGI_Source/MGI_Parameters_Calculator.o \
    $O/ONOFF_LOG_Source/Ind_ONOFF_LOG_Source.o \
    $O/ONOFF_LOG_Source/ONOFF_LOG_Collector.o \
    $O/ONOFF_LOG_Source/ONOFF_LOG_Par.o \
    $O/ONOFF_Source/Ind_ONOFF_Source.o \
    $O/ONOFF_Source/ONOFF_Collector.o \
    $O/ONOFF_Source/ONOFF_Parameters_Calculator.o \
    $O/Statistics/Jitter_Meter.o \
    $O/Statistics/Traffic_Stats.o \
    $O/Statistics/TrafficHistogram.o

# Message files
MSGFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f unstable unstable.exe libunstable.so libunstable.a libunstable.dll libunstable.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f Common_Modules/*_m.cc Common_Modules/*_m.h
	-rm -f FromFile_Source/*_m.cc FromFile_Source/*_m.h
	-rm -f MGI_LOG_Source/*_m.cc MGI_LOG_Source/*_m.h
	-rm -f MGI_Source/*_m.cc MGI_Source/*_m.h
	-rm -f ONOFF_LOG_Source/*_m.cc ONOFF_LOG_Source/*_m.h
	-rm -f ONOFF_Source/*_m.cc ONOFF_Source/*_m.h
	-rm -f Statistics/*_m.cc Statistics/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc Common_Modules/*.cc FromFile_Source/*.cc MGI_LOG_Source/*.cc MGI_Source/*.cc ONOFF_LOG_Source/*.cc ONOFF_Source/*.cc Statistics/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/Common_Modules/Simple_CBR_Source.o: Common_Modules/Simple_CBR_Source.cc
$O/Common_Modules/Sink.o: Common_Modules/Sink.cc
$O/Common_Modules/TxQueue.o: Common_Modules/TxQueue.cc
$O/FromFile_Source/FromFile_Source.o: FromFile_Source/FromFile_Source.cc
$O/MGI_LOG_Source/Ind_MGI_LOG_Source.o: MGI_LOG_Source/Ind_MGI_LOG_Source.cc
$O/MGI_LOG_Source/MGI_LOG_Collector.o: MGI_LOG_Source/MGI_LOG_Collector.cc
$O/MGI_LOG_Source/MGI_LOG_Generator.o: MGI_LOG_Source/MGI_LOG_Generator.cc
$O/MGI_LOG_Source/MGI_LOG_Par.o: MGI_LOG_Source/MGI_LOG_Par.cc
$O/MGI_Source/Individual_MGI_Source.o: MGI_Source/Individual_MGI_Source.cc
$O/MGI_Source/MGI_Collector.o: MGI_Source/MGI_Collector.cc
$O/MGI_Source/MGI_Generator.o: MGI_Source/MGI_Generator.cc
$O/MGI_Source/MGI_Parameters_Calculator.o: MGI_Source/MGI_Parameters_Calculator.cc
$O/ONOFF_LOG_Source/Ind_ONOFF_LOG_Source.o: ONOFF_LOG_Source/Ind_ONOFF_LOG_Source.cc
$O/ONOFF_LOG_Source/ONOFF_LOG_Collector.o: ONOFF_LOG_Source/ONOFF_LOG_Collector.cc
$O/ONOFF_LOG_Source/ONOFF_LOG_Par.o: ONOFF_LOG_Source/ONOFF_LOG_Par.cc
$O/ONOFF_Source/Ind_ONOFF_Source.o: ONOFF_Source/Ind_ONOFF_Source.cc
$O/ONOFF_Source/ONOFF_Collector.o: ONOFF_Source/ONOFF_Collector.cc
$O/ONOFF_Source/ONOFF_Parameters_Calculator.o: ONOFF_Source/ONOFF_Parameters_Calculator.cc
$O/Statistics/Jitter_Meter.o: Statistics/Jitter_Meter.cc
$O/Statistics/TrafficHistogram.o: Statistics/TrafficHistogram.cc
$O/Statistics/Traffic_Stats.o: Statistics/Traffic_Stats.cc \
  Statistics/TrafficHistogram.cc
