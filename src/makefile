all : write read
.PHONY : clean

write : testWrite.o
	g++ testWrite.cpp MyBus.h MyBus.cc ShmManage.h ShmManage.cc -o write

read : testRead.o
	g++ testRead.cpp MyBus.h MyBus.cc ShmManage.h ShmManage.cc -o read
clean : 
	rm *.o