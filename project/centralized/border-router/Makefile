CONTIKI_PROJECT = border-router
all: $(CONTIKI_PROJECT)
CONTIKI=../../..

# The BR is either native or embedded, and in the latter case must support SLIP
PLATFORMS_EXCLUDE = nrf52dk

# Include RPL BR module
MODULES += os/services/rpl-border-router
#MODULES += os/services/orchestra

# Include TASA Centralized module
MODULES += os/services/s-tasa
# Include Blacklist module
MODULES += os/services/blacklist
# Include CoAP resources
MODULES_REL += ../node/resources
# Include the CoAP implementation
MODULES += os/net/app-layer/coap

MAKE_MAC = MAKE_MAC_TSCH

# Include webserver module
MODULES_REL += webserver
# Include optional target-specific module
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)




include $(CONTIKI)/Makefile.include