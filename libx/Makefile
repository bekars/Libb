################################################################################
# LibX
# 	package C library
#
#
# History:
#   2010.08.01	bekars	create the Makefile
#
################################################################################

ifndef top_srcdir
    top_srcdir := $(shell pwd)/..
endif
 
include $(top_srcdir)/Rules.mak

COMPILE   = $(CC) $(CFLAGS) $(INCLUDE) $(DEFS) $(LDFLAGS_EX) $(LDFLAGS)
COMPILE_C = $(CC) $(CFLAGS) $(INCLUDE) $(DEFS)

TARGET  := libx.a
SOURCE  := $(wildcard *.c)
HEADER  := $(wildcard *.h)
OBJS    := $(SOURCE:.c=.o)

$(TARGET): $(OBJS) $(HEADER)
	$(call now_make_it) 
	$(AR) r $@ $^

%.o : %.c
	$(COMPILE_C) -c $<

PHONY += install
install:

PHONY += clean
clean:
	-$(RM_F) $(TARGET) $(OBJS)

.PHONY: $(PHONY)

