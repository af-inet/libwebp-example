TARGET  = example
SRC     = main.c
CFLAGS  = -Wall -L/usr/local/lib -l webp -l webpmux

.PHONY: default all clean run

default: $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@
