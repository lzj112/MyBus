#ifndef _TIMER_H
#define _TIMER_H 

#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/timerfd.h>

#include <iostream>

class Timer 
{
public:
    Timer() : clock(-1){}
    int startTimerfd() 
    {

        struct itimerspec new_value;
        struct timespec now;
        
        int res = clock_gettime(CLOCK_REALTIME, &now);    //获取时钟时间
        if (res == -1) 
        {
            std::cout << "clock_gettime is wrong" << std::endl;
            return -1;
        }

        new_value.it_value.tv_sec = now.tv_sec + 30;    //第一次到期时间
        new_value.it_value.tv_nsec = now.tv_nsec;

        new_value.it_interval.tv_sec = 60;  //z之后每次到期的时间间隔
        new_value.it_interval.tv_nsec = 0;

        clock = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);    //创建定时器
        if (clock == -1) 
        {
            std::cout << "timerfd_create is wrong " << std::endl;
        }

        res = timerfd_settime(clock, TFD_TIMER_ABSTIME, &new_value, nullptr); //启动定时器
        if (res == -1) 
        {
            std::cout << "timer_settime is wrong" << std::endl;
        }
        return clock;
    }
    
    int getTimerfd() 
    {
        return clock;
    }
    void func() 
    {
        close(clock);
    }

private:
    int clock;   //定时时间  time_t就是long int
};


#endif