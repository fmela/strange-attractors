SRC = $(wildcard *.c)
OUTPUT_DIR = bin
IMAGES_DIR = images
TARGETS = $(SRC:%.c=$(OUTPUT_DIR)/%)

CAIRO_PREFIX=/opt/X11

CC = $(shell which clang || which gcc)
CFLAGS = -pipe -O2 -Wall -W -Wshadow -I$(CAIRO_PREFIX)/include
LDFLAGS = -L$(CAIRO_PREFIX)/lib -lm -lcairo

all: $(OUTPUT_DIR) $(IMAGES_DIR) $(TARGETS)

$(OUTPUT_DIR):
	[ -d $(OUTPUT_DIR) ] || mkdir -m 755 $(OUTPUT_DIR)

$(IMAGES_DIR):
	[ -d $(IMAGES_DIR) ] || mkdir -m 755 $(IMAGES_DIR)

$(OUTPUT_DIR)/% : %.c
	$(CC) $(CFLAGS) -o $(@) $(<) $(LDFLAGS)

.PHONY : clean
clean :
	rm -rf $(OUTPUT_DIR)
