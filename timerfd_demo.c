/*
 *  Description : linux 应用层编程之定时器timerfd的使用
 *  Date        ：20180611
 *  Author      ：mason
 *  Mail        : mrsonko@126.com
 *
 */

#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include<errno.h>
#include <time.h>

#define TIME_MAX 2 
#define log(fmt, arg...) printf(""fmt, ##arg)

void main() {
    int tfd;    //定时器描述符
    int efd;    //epoll描述符
    int fds, ret;
    uint64_t value;
    struct epoll_event ev, *evptr;
    struct itimerspec time_intv; //用来存储时间

    tfd = timerfd_create(CLOCK_MONOTONIC, 0);   //创建定时器
    if(tfd == -1) {
        log("create timer fd fail \r\n");
        return ;
    }

    time_intv.it_value.tv_sec = TIME_MAX; //设定2s超时
    time_intv.it_value.tv_nsec = 0;
    time_intv.it_interval.tv_sec = time_intv.it_value.tv_sec;   //每隔2s超时
    time_intv.it_interval.tv_nsec = time_intv.it_value.tv_nsec;

    log("timer start ...\n");
    timerfd_settime(tfd, 0, &time_intv, NULL);  //启动定时器
    
    efd = epoll_create1(0); //创建epoll实例
    if (efd == -1) {
        log("create epoll fail \r\n");
        close(tfd);
        return ;
    }
    
    evptr = (struct epoll_event *)calloc(1, sizeof(struct epoll_event));
    if (evptr == NULL) {
        log("epoll event calloc fail \r\n");
        close(tfd);
        close(efd);
        return ;
    }

    ev.data.fd = tfd; 
    ev.events = EPOLLIN;    //监听定时器读事件，当定时器超时时，定时器描述符可读。
    epoll_ctl(efd, EPOLL_CTL_ADD, tfd, &ev); //添加到epoll监听队列中

    while(1) {
        fds = epoll_wait(efd, evptr, 1, -1);    //阻塞监听，直到有事件发生
        if(evptr[0].events & EPOLLIN){   
                ret = read(evptr->data.fd, &value, sizeof(uint64_t));
                if (ret == -1) 
                    log("read return -1, errno :%d \r\n", errno);
                else
                    log("*** timer up  *** \n");               
       }            
    }

    return ;
}

