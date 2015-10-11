#
# On Linux just run
#   > make
#
# this will build udpclient and udpserver, which can be used for testing
#
# On node1 run:
#   > ./udpserver
# On node0 run:
#   > ./udpclient
#
#
# On FreeBSD run:
#   > sudo make dummy
#
# this will build, reload and display status of the dummynet kernel module
#
# On a new FreeBSD machine this must be done before building dummynet:
#   > sudo cp -R /share/freebsd/8.3/src/ /usr/src
#   Edit /usr/src/sys/modules/dummynet/Makefile to add this line: SRCS+= dn_sched_reorder.c
#
#
# On any machine run this to build scanner:
#   > make scanner
#
# and to run the scanner:
#   > ./scanner
#
# the scanner reads input.txt, which has to be a list of numbers starting with 1
# it prints out a C structure initializer which can be pasted in ipfw/dn_reorder.h
#

.PHONY: all clean dummy tail freebsd ipfw

all: udpclient udpserver

udpclient: udpclient.c
	cc -o udpclient -g udpclient.c

udpserver: udpserver.c
	cc -o udpserver -g udpserver.c

scanner: scanner.c ipfw/dn_reorder.h
	cc -o scanner -g scanner.c

dummy: scanner
	./scanner input.txt > ipfw/reordering.h
	cp ipfw/*.c /usr/src/sys/netinet/ipfw
	cp ipfw/*.h /usr/src/sys/netinet/ipfw
	cd /usr/src/sys/modules/dummynet && make
	-kldstat -q -m dummynet && kldunload dummynet
	kldload /usr/src/sys/modules/dummynet/dummynet.ko
	kldstat -v -n dummynet
	$(MAKE) ipfw

tail:
	tail -f /var/log/tiplogs/pc250.run

clean:
	rm udpclient udpserver scanner

freebsd:
	[ -e "/usr/src/sys/modules/dummynet/Makefile" ] || cp -v -R /share/freebsd/8.3/src/ /usr/src && patch -N /usr/src/sys/modules/dummynet/Makefile Makefile.patch

ipfw:
	-ipfw delete 60110 > /dev/null 2>&1                    # remove preexisting rule that breaks ping
	-ipfw delete 60120 > /dev/null 2>&1                    # remove preexisting rule that breaks ping
	-ipfw delete 100 > /dev/null 2>&1                      # remove rule we previously may have added
	ipfw sched 500 config type  REORDER                   # setup schedular
	ipfw queue 105 config sched 500                       # setup queue with schedular
	ipfw add 100 queue 105 ip from 10.1.1.2 to 10.1.1.3   # add our pipe, connect to queue
