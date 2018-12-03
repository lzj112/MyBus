#ifndef _PROTABMGT_H_
#define _PROTABMGT_H_

#include <mutex>

#include "BusInfo.h"

class ProTabMgt 
{
public:
    int initProShmList(int id);
    int creShmQueue(int proj_id);
    int updateList(const struct ProComm& str);
    int getProShmQueue(const struct ProComm& str);
    void copy(PacketBody* ptr, const PacketBody& str);
    void saveMessage(int offset, const PacketBody& str);
    void realseAll();
    int getShmID()
    {
        return proTabList[0];
    }
private:
    int proTabList[3];
    std::mutex myLock;
};

#endif