all : write1.out read.out  write2.out transit1.out transit2.out
.PHONY : clean

str = MyBus.cc socketBus.cc  socketTCP.cc Parsing.cc cJSON.c
ctr = NetComm.cc Epoll.cc socketBus.cc socketTCP.cc RouTabMgt.cc ProTabMgt.cc 

write1.out : $(str)
	g++ testWrite.cpp $(str) -o  write1.out -lpthread -g -Wall

read.out : $(str)
	g++ testRead.cpp $(str) -o read.out -lpthread -g -Wall

write2.out : $(str)
	g++ testWrite2.cpp $(str) -o write2.out -lpthread -g -Wall

transit1.out : $(ctr)
	g++ BusServer1.cpp $(ctr) -o transit1.out -lpthread -g -Wall

transit2.out : $(ctr)
	g++ BusServer2.cpp $(ctr) -o transit2.out -lpthread -g -Wall
	

clean : 
	rm *.o *.gch *.out
