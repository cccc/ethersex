TOPDIR ?= ../..
include $(TOPDIR)/.config

$(AUTOC4_SUPPORT)_SRC += services/autoc4/autoc4.c
$(AUTOC4_SUPPORT)_NP_SIMPLE_META_SRC += services/autoc4/autoc4_defs.m4

services/autoc4/autoc4.c: services/autoc4/autoconfig.c services/autoc4/conf.c
# remove quotation marks from config file path
# also make this file depend on the global config file
services/autoc4/autoconfig.c services/autoc4/pinning.m4: $(patsubst "%",%,$(AUTOC4_CONFIG_FILE)) services/autoc4/convert_config.py .config
	python services/autoc4/convert_config.py $< services/autoc4/autoconfig.c services/autoc4/pinning.m4

##############################################################################
# generic fluff
include $(TOPDIR)/scripts/rules.mk
