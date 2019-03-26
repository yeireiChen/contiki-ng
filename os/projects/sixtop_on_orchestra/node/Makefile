CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky

# Include the CoAP implementation
MODULES += os/net/app-layer/coap
MODULES += os/services/orchestra
MODULES += os/net/mac/tsch/sixtop
MODULES += os/project/sixtop_on_orchestra/sixtop_schdule

# Include CoAP resources
MODULES_REL += ./resources

MAKE_MAC = MAKE_MAC_TSCH
CONTIKI=../../../..
include $(CONTIKI)/Makefile.include
