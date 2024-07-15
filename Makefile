CC      = gcc
CFLAGS  = -g -Isrc -IcJSON -Izlib -fPIC
RM      = rm -f


TESTSRC = src/unittests
TESTS   = $(wildcard $(TESTSRC)/*.c)
TESTOBJS= $(TESTS:.c=.o)

default: MC


MC: callback.o packet.o loginCallbacks.o cJSON/cJSON.o zlib/libz.a src/main.c
	$(CC) $(CFLAGS) -o MC src/main.c zlib/libz.a cJSON/cJSON.o packet.o loginCallbacks.o callback.o

cJSON/cJSON.o:
	$(CC) $(CFLAGS) -c -o cJSON/cJSON.o cJSON/cJSON.c
zlib/libz.a:
	cd zlib && sh ./configure
	make static -C zlib -j 4

packet.o: src/decoding/packet.c
	$(CC) $(CFLAGS) -c -o packet.o src/decoding/packet.c
loginCallbacks.o: src/connection/server/world/loginCallbacks.c
	$(CC) $(CFLAGS) -c -o loginCallbacks.o src/connection/server/world/loginCallbacks.c
callback.o: src/connection/server/world/callback.c
	$(CC) $(CFLAGS) -c -o callback.o src/connection/server/world/callback.c

run: clean MC
	./MC

test: clean zlib/libz.a cJSON/cJSON.o $(TESTOBJS)
	@for test in $(TESTOBJS); do \
		echo "Running $$test"; \
		./$$test; \
	done
	make run -C nbt
$(TESTSRC)/%.o: $(TESTSRC)/%.c
	$(CC) $(CFLAGS) -o $@ $< zlib/libz.a cJSON/cJSON.o

clean:
	-$(RM) MC $(TESTOBJS) *.o
