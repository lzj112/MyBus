all : write.out read.out transit1.out transit2.out
.PHONY : clean

str = MyBus.* AllocPort.h socketBus.*  socketTCP.* -g -Wall
ctr = NetComm.* Epoll* socketBus.* socketTCP* -g -Wall

write.out : MyBus.* AllocPort.h socketBus.*  socketTCP.*  testWrite.cpp
	g++ testWrite.cpp $(str) -o write.out -lpthread

read.out : MyBus.* AllocPort.h socketBus.*  socketTCP.* testRead.cpp 
	g++ testRead.cpp $(str) -o read.out -lpthread

transit1.out : NetComm.* Epoll* socketBus.* socketTCP* BusServer1.cpp
	g++ BusServer1.cpp $(ctr) -o transit1.out -lpthread

transit2.out : NetComm.* Epoll* socketBus.* socketTCP* BusServer2.cpp
	g++ BusServer2.cpp $(ctr) -o transit2.out -lpthread

clean : 
	rm *.o *.gch *.out