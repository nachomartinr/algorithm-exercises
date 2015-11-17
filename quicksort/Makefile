CC=gcc
CFLAGS=-I. -O2 -std=c99
OBJDIR=obj

_OBJS = quicksort.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(OBJDIR) 
	$(CC) -c -o $@ $< $(CFLAGS)

quicksort: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

all: ${OBJS}

.PHONY: clean

 $(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o quicksort.exe quicksort
