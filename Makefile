CC = clang
OUT_DIR = out
CFLAGS = -Wall -fPIC -ansi -pedantic -g
LDFLAGS = -L$(OUT_DIR)
LDLIBS = -lpthread -ltermboy
_OBJECTS = keyboard screen sprite

OBJECTS = $(patsubst %,$(OUT_DIR)/%.o,$(_OBJECTS))
LIBTERMBOY = $(OUT_DIR)/libtermboy.so
DEMO = $(OUT_DIR)/demo

$(DEMO): demo.c $(LIBTERMBOY)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $< -o $@

$(LIBTERMBOY): $(OBJECTS)
	$(CC) -shared $^ -o $@

$(OUT_DIR)/%.o: %.c termboy.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(DEMO)
	LD_LIBRARY_PATH=$(OUT_DIR) $(DEMO)

.PHONY: clean
clean:
	rm -f out/*
