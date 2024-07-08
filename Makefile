CC      = gcc
CFLAGS  = -g
RM      = rm -f

SRC     = src
TESTSRC = src/unittests
TESTS   = $(wildcard $(TESTSRC)/*.c)
TESTOBJS= $(TESTS:.c=.o)

default: MC

MC: $(SRC)/main.c
	$(CC) $(CFLAGS) -o MC $(SRC)/main.c

run: clean MC
	./MC

test: clean $(TESTOBJS)
	@for test in $(TESTOBJS); do \
		echo "Running $$test"; \
		./$$test; \
	done

$(TESTSRC)/%.o: $(TESTSRC)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-$(RM) MC $(TESTOBJS)
