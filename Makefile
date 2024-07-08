CC      = gcc
CFLAGS  = -g -IcJSON
RM      = rm -f


TESTSRC = src/unittests
TESTS   = $(wildcard $(TESTSRC)/*.c)
TESTOBJS= $(TESTS:.c=.o)

default: MC

MC: cJSON/cJSON.o src/main.c
	$(CC) $(CFLAGS) -o MC src/main.c cJSON/cJSON.o

cJSON/cJSON.o:
	$(CC) $(CFLAGS) -shared -o cJSON/cJSON.o cJSON/cJSON.c

run: clean MC
	./MC

test: clean cJSON/cJSON.o $(TESTOBJS)
	@for test in $(TESTOBJS); do \
		echo "Running $$test"; \
		./$$test; \
	done

$(TESTSRC)/%.o: $(TESTSRC)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-$(RM) MC $(TESTOBJS)  
