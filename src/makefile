all : write read transit
.PHONY : clean

str = MyBus.* AllocPort.h socketBus.*  socketTCP.* 
ctr = NetComm.* Epoll* socketBus.* socketTCP*

write : testWrite.o
	g++ testWrite.cpp $(str) -o write -lpthread

read : testRead.o
	g++ testRead.cpp $(str) -o read -lpthread

transit : MyBusServer.o
	g++ MyBusServer.cpp $(ctr) -o transit -lpthread

clean : 
	rm *.o *.gch