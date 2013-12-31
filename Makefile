MODNAME=keystone_test
SO=$(MODNAME).so
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
TARGETDIR=/lib64/security
DIRS=test

CC=cc
CFLAGS=-fPIC -fno-stack-protector
LIBS=-lpam -lpam_misc
LDFLAGS=-x --shared

$(SO): $(OBJS)
	ld $(LDFLAGS) -o $(SO) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

force_check:
	@true

test: force_check
	cd test; make

clean:
	rm -f $(OBJS) $(SO)
	@for dd in $(DIRS); do (cd $$dd; make clean); done

install:
	cp -u $(SO) $(TARGETDIR)/$(SO)

