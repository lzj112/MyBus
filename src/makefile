all : write.out read.out transit1.out transit2.out
.PHONY : clean

str = MyBus.* AllocPort.h socketBus.*  socketTCP.* 
ctr = NetComm.* Epoll* socketBus.* socketTCP*

write.out : testWrite.o
	g++ testWrite.cpp $(str) -o write.out -lpthread

read.out : testRead.o
	g++ testRead.cpp $(str) -o read.out -lpthread

transit1.out : BusServer1.o
	g++ BusServer1.cpp $(ctr) -o transit1.out -lpthread

transit2.out : BusServer2.o
	g++ BusServer2.cpp $(ctr) -o transit2.out -lpthread

clean : 
	rm *.o *.gch *.out