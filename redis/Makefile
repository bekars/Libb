################################################################################
# redis c interface:
#
################################################################################

ifndef top_srcdir
    top_srcdir := $(shell pwd)/..
endif

CC       := gcc
CFLAGS   := -g -O2 -Wall -Wunused -Wstrict-prototypes
LIBS     := -lhiredis
COMPILE   = $(CC) $(CFLAGS) $(INCLUDE) -std=c99

TARGET := redis_go
SOURCE := $(wildcard *.c)
HEADER := $(wildcard *.h)
OBJS   := $(SOURCE:.c=.o)
RM_F   := rm -f

$(TARGET): $(OBJS) $(HEADER)
	$(COMPILE) -o $@ $^ $(LIBS)

%.o : %.c
	$(COMPILE) -c $<

PHONY += install
install:

PHONY += clean
clean:
	-$(RM_F) $(TARGET) $(OBJS)

.PHONY: $(PHONY)

