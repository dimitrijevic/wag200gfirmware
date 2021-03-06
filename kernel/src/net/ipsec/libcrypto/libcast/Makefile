#
# SSLeay/crypto/cast/Makefile
#

DIR=	cast
TOP=	../..
CC=	cc
CPP=	$(CC) -E
INCLUDES=
CFLAG=-g -I ../include -D__KERNEL__ -I/usr/src/linux/include
INSTALL_PREFIX=
OPENSSLDIR=     /usr/local/ssl
INSTALLTOP=/usr/local/ssl
MAKE=		make -f Makefile.ssl
MAKEDEPEND=	$(TOP)/util/domd $(TOP)
MAKEFILE=	Makefile.ssl
AR=		ar r
RANLIB=		ranlib
PERL=		perl

CAST_ENC := c_enc.o

ASM-$(ARCH_ASM):=1
ASM_X86:=$(ASM-i586)$(ASM-i686)
ifneq ($(strip $(ASM_X86)),)
	CAST_ENC:= asm/cx86-elf.o
endif

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile
TEST=casttest.c
APPS=

LIB=libcast.a
LIBSRC=c_skey.c c_enc.c
LIBOBJ=c_skey.o $(CAST_ENC)

SRC= $(LIBSRC)

EXHEADER= cast.h
HEADER=	cast_s.h cast_lcl.h $(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

#top:
#	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

all:	lib

lib:	$(LIB)

$(LIB):	$(LIBOBJ)
	$(AR) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB)

# elf
asm/cx86-elf.o: asm/cx86unix.cpp
	$(CPP) -DELF -x c asm/cx86unix.cpp | as -o asm/cx86-elf.o

# solaris
asm/cx86-sol.o: asm/cx86unix.cpp
	$(CC) -E -DSOL asm/cx86unix.cpp | sed 's/^#.*//' > asm/cx86-sol.s
	as -o asm/cx86-sol.o asm/cx86-sol.s
	rm -f asm/cx86-sol.s

# a.out
asm/cx86-out.o: asm/cx86unix.cpp
	$(CPP) -DOUT asm/cx86unix.cpp | as -o asm/cx86-out.o

# bsdi
asm/cx86bsdi.o: asm/cx86unix.cpp
	$(CPP) -DBSDI asm/cx86unix.cpp | sed 's/ :/:/' | as -o asm/cx86bsdi.o

asm/cx86unix.cpp: asm/cast-586.pl ../perlasm/x86asm.pl ../perlasm/cbc.pl
	(cd asm; $(PERL) cast-586.pl cpp $(PROCESSOR) >cx86unix.cpp)

files:
	$(PERL) $(TOP)/util/files.pl Makefile.ssl >> $(TOP)/MINFO

links:
	@$(TOP)/util/point.sh Makefile.ssl Makefile
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@for i in $(EXHEADER) ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	$(MAKEDEPEND) $(INCLUDES) $(DEPFLAG) $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f asm/cx86unix.cpp *.o asm/*.o *.obj $(LIB) tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.
