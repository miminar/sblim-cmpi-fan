CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
LIBS=Linux_Fan Linux_FanSensor Linux_FanAssociatedSensor
LIBRARIES=$(foreach l,$(LIBS),cmpi$(l)Provider)
LIBRNS=$(foreach l,$(LIBRARIES),lib$(l).so)
DESTLIBDIR=$(shell [ `uname -m` = "x86_64" ] && echo /usr/lib64/cmpi || echo /usr/lib/cmpi)
LINKS=$(foreach l,$(LIBRNS),$(DESTLIBDIR)/$(l))

providers: $(LIBRNS)

libcmpiLinux_FanAssociatedSensorProvider.so: \
	    cmpiLinux_FanAssociatedSensorProvider.o
	$(CC) -shared -Wl -o $@ $^ -lcmpiOSBase_Common

libcmpi%Provider.so: cmpi%Provider.o cmpi%.o \
    cmpiLinux_FanCommon.o Linux_Fan.o
	$(CC) -shared -Wl -o $@ $^ -lcmpiOSBase_Common -lsensors

%.o: %.c
	$(CC) $(CFLAGS) -I/usr/include/cmpi -c -fPIC -o $@ $^

clean:
	-rm *.o *.so

sfcb: $(LINKS)
	sfcbstage -r mof/Linux_Fan.reg mof/Linux_Fan.mof
	sfcbrepos -f

$(DESTLIBDIR)/%.so: %.so
	ln -sf `pwd`/$? $(DESTLIBDIR)

install: $(LIBRNS)
	cp $^ $(DESTLIBDIR)
	sfcbstage -r mof/Linux_Fan.reg mof/Linux_Fan.mof
	sfcbrepos -f
