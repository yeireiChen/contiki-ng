CONTIKI_PROJECT = coap-example-server
all: $(CONTIKI_PROJECT)
CONTIKI=../../../..

# The BR is either native or embedded, and in the latter case must support SLIP
PLATFORMS_EXCLUDE = sky nrf52dk native
BOARDS_EXCLUDE = srf06/cc13xx launchpad/cc1310 launchpad/cc1350 sensortag/cc2650 sensortag/cc1350

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# Include the CoAP implementation
MODULES += os/net/app-layer/coap

# Include CoAP resources
MODULES_REL += ./resources
# force Orchestra from command line
MAKE_WITH_ORCHESTRA ?= 0
# force Security from command line
MAKE_WITH_SECURITY ?= 0
 # print #routes periodically, used for regression tests
MAKE_WITH_PERIODIC_ROUTES_PRINT ?= 0
MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/shell

ifeq ($(MAKE_WITH_ORCHESTRA),1)
MODULES += os/services/orchestra
endif

ifeq ($(MAKE_WITH_SECURITY),1)
CFLAGS += -DWITH_SECURITY=1
endif

ifeq ($(MAKE_WITH_PERIODIC_ROUTES_PRINT),1)
CFLAGS += -DWITH_PERIODIC_ROUTES_PRINT=1
endif


include $(CONTIKI)/Makefile.include
