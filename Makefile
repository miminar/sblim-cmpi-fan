CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
LIBS=Linux_Fan Linux_FanSensor Linux_FanAssociatedSensor
LIBRARIES=$(foreach l,$(LIBS),cmpi$(l)Provider)
LIBRNS=$(foreach l,$(LIBRARIES),lib$(l).so)
LINKS=$(foreach l,$(LIBRNS),/usr/lib64/cmpi/$(l))

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

/usr/lib64/cmpi/%.so: %.so
	ln -sf `pwd`/$? /usr/lib64/cmpi/

install: $(LIBRNS)
	cp $^ /usr/lib64/cmpi
	sfcbstage -r mof/Linux_Fan.reg mof/Linux_Fan.mof
	sfcbrepos -f
