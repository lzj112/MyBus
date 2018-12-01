all : write1.out read.out  write2.out transit1.out transit2.out
.PHONY : clean

str = MyBus.cc socketBus.cc  socketTCP.cc -g -Wall
ctr = NetComm.cc Epoll.cc socketBus.cc socketTCP.cc -g -Wall

write1.out : MyBus.cc  socketBus.cc  socketTCP.cc  testWrite.cpp
	g++ testWrite.cpp $(str) -o write1.out -lpthread

read.out : MyBus.cc socketBus.cc  socketTCP.cc testRead.cpp 
	g++ testRead.cpp $(str) -o read.out -lpthread

write2.out : MyBus.cc  socketBus.cc  socketTCP.cc  testWrite2.cpp
	g++ testWrite2.cpp $(str) -o write2.out -lpthread

transit1.out : NetComm.* Epoll* socketBus.* socketTCP* BusServer1.cpp
	g++ BusServer1.cpp $(ctr) -o transit1.out -lpthread

transit2.out : NetComm.* Epoll* socketBus.* socketTCP* BusServer2.cpp
	g++ BusServer2.cpp $(ctr) -o transit2.out -lpthread
	

clean : 
	rm *.o *.gch *.out