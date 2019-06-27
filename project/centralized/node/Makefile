CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky

# Include the CoAP implementation
MODULES += os/net/app-layer/coap
#MODULES += os/services/orchestra

# Include TASA Centralized module
MODULES += os/services/s-tasa
# Include CoAP resources
MODULES_REL += ./resources

MAKE_MAC = MAKE_MAC_TSCH
CONTIKI=../../..

include $(CONTIKI)/Makefile.include