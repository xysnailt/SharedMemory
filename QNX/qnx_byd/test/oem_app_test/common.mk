# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs_af.mk

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION=sv_oem
endef

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
EXTRA_SRCVPATH+=$(PROJECT_ROOT)/src

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(PROJECT_ROOT)/include \
        $(INSTALL_ROOT_nto)/usr/include/amss/ \
        $(INSTALL_ROOT_nto)/usr/include/amss/core/ \
        $(INSTALL_ROOT_nto)/usr/include/amss/core/asic_8064 \
        $(AMSS_INC)/core \
        $(AMSS_INC)

	
#===== CCFLAGS - add the flags to the C compiler command line. 
CCFLAGS+=-DFEATURE_LIBRARY_ONLY

#===== EXTRA_LIBVPATH - a space-separated list of directories to search for library files.
EXTRA_LIBVPATH+=$(PROJECT_ROOT)/pm_lib

CCFLAG_64 = -D_PADDR_BITS=64
CCFLAGS_$(BUILDENV) = -DBUILDENV_$(BUILDENV)
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE)_$(COMPILER_DRIVER)) $(CCFLAG_$(PADDR_SIZE)) $(CCFLAGS_$(BUILDENV))
CCFLAGS += -Werror

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS+=-Wl,-E

LIBS+=slog2 sv_oem_pm_client_lib

#===== INSTALLDIR - Subdirectory where the executable or library is to be installed. 
INSTALLDIR=bin
NAME=oem_app_test

#===== NAME - name of the project (default - name of project directory).

include $(MKFILES_ROOT)/qmacros.mk

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))
