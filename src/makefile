all : write read
.PHONY : clean

str = MyBus.* AllocPort.h socketBus.*  socketTCP.* 
ctr = NetComm.* Epoll* socketBus.*

write : testWrite.o
	g++ testWrite.cpp $(str) -o write -lpthread

read : testRead.o
	g++ testRead.cpp $(str) -o read -lpthread

transit : MyBusServer.o
	g++ MyBusServer.cpp $(ctr) - o transit

clean : 
	rm *.o *.gch