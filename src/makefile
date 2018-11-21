all : write read
.PHONY : clean

str = MyBus.* ShmManage.* socketBus.*  socketTCP.* Epoll.* NetComm.*

write : testWrite.o
	g++ testWrite.cpp $(str) -o write -lpthread

read : testRead.o
	g++ testRead.cpp $(str) -o read -lpthread
clean : 
	rm *.o *.gch