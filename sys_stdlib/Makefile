OUTPUT_NAME = sys_stdlib

AS = $(TARGET)-as
CC = $(TARGET)-gcc
AR = $(TARGET)-ar
SOURCES = $(shell ls *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra# -O0

default: $(OUTPUT_NAME).a

$(OUTPUT_NAME).a: $(OBJECTS)
	$(AR) -rc $@ $^

# Compile source files.
%.o : %.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	@rm -f *.o
	@rm -f *.a
