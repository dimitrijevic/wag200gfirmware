#
# Makefile for the Linux network (CPMAC) device drivers.
#

O_TARGET := avalanche_cpmac.o

obj-m := avalanche_cpmac.o

avalanche_cpmac-objs += cpmac.o cpmacHalLx.o hcpmac.o psp_config_build.o psp_config_mgr.o psp_config_parse.o psp_config_util.o

include $(TOPDIR)/Rules.make

avalanche_cpmac.o: $(avalanche_cpmac-objs)
	$(LD) -r $(LDOPTS) -o $@ $(avalanche_cpmac-objs)

