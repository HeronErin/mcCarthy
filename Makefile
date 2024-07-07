CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: MC

# op.o: src/disassembler/op.c
# 	$(CC) $(CFLAGS) -c src/disassembler/op.c -o op.o
MC: src/main.c
	$(CC) $(CFLAGS) -o MC src/main.c

run: clean MC
	./MC

clean:
	-$(RM) MC
