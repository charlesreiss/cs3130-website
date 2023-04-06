CC = clang
CFLAGS = -g -O3 -pthread -Wall -Werror
# ASAN = Address SANitizer, see https://github.com/google/sanitizers/wiki/AddressSanitizer
CFLAGS_ASAN = $(CFLAGS) -Og -fsanitize=address -fno-omit-frame-pointer
# ASAN = Address SANitizer, see https://github.com/google/sanitizers/wiki/ThreadSanitizer
CFLAGS_TSAN = $(CFLAGS) -Og -fsanitize=thread -fno-omit-frame-pointer
LDFLAGS = -lpthread
LDFLAGS_ASAN = $(LDFLAGS) -fsanitize=address
LDFLAGS_TSAN = $(LDFLAGS) -fsanitize=thread

all: life life-asan life-tsan

%-asan.o: %.c life.h timing.h
	$(CC) -c $(CFLAGS_ASAN) -o $@ $<

%-tsan.o: %.c life.h timing.h
	$(CC) -c $(CFLAGS_TSAN) -o $@ $<

%.o: %.c life.h
	$(CC) -c $(CFLAGS) -o $@ $<

life: main.o life.o timing.o life-parallel.o life-serial.o
	$(CC) $(LDFLAGS) -o $@ $^

life-asan: main-asan.o life-asan.o timing-asan.o life-parallel-asan.o life-serial-asan.o
	$(CC) $(LDFLAGS_ASAN) -o $@ $^

life-tsan: main-tsan.o life-tsan.o timing-tsan.o life-parallel-tsan.o life-serial-tsan.o
	$(CC) $(LDFLAGS_TSAN) -o $@ $^

clean:
	rm -f *.o

distclean: clean
	rm -f life life-asan life-tsan

.PHONY: all clean distclean
