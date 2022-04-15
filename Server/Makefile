all: testServer



CROSS_COMPILE   = arm-linux-gnueabihf-
#CROSS_COMPILE   = arm-none-eabi-
GCC		= $(CROSS_COMPILE)g++
GCC             = g++

testServer: Connect.cpp NetServer.cpp ProtoBuffer.cpp Server.cpp
	$(GCC) Connect.cpp NetServer.cpp ProtoBuffer.cpp Server.cpp -L. -lpthread -o testServer

liblocation.so: location.c
	$(GCC) location.c  gsm_usb.c xml.c -fPIC -shared -L./ -o liblocation.so

test_location: location.c test_location.c
	$(GCC) test_location.c -L. -llocation -o test_location

clean:
	rm  testServer
