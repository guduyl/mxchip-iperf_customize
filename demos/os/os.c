/**
 ******************************************************************************
 * @file    os.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    20-May-2019
 * @brief   MOS demo!
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2019 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "OS", M, ##__VA_ARGS__)

/* demo sem*/
static mos_semphr_id_t sem;
#define SEM_COUNT 10

static void semrx_thread(void *arg)
{
    int fd;
    struct timeval t;
    fd_set rd;
    int ret;
    
    fd = mos_event_fd_new(sem);
    app_log("event fd: %d", fd);
    while(1) {
        t.tv_sec = 10;
        t.tv_usec = 0;
        FD_ZERO(&rd);
        FD_SET(fd, &rd);
        select(fd+1, &rd, NULL, NULL, &t);
        if (FD_ISSET(fd, &rd)) {
            ret = mos_semphr_acquire(sem, 1);
            app_log("RX sem ret %d", ret);
        } else {
            app_log("RX sem timeout");
            break;
        }
    }
    
    mos_event_fd_delete(fd);

    app_log("semrx_thread exit");
    mos_thread_delete(NULL);
}

static void semtx_thread(void *arg)
{
    int i;
    int ret;
    
    for(i=0;i<20;i++) {
        ret = mos_semphr_release(sem);
        if (ret != kNoErr) {
            app_log("put sem %d fail %d", i, ret);
        }
    }
    mos_sleep(5);
    for(i=0;i<5;i++) {
        ret = mos_semphr_release(sem);
        if (ret != kNoErr) {
            app_log("put sem %d fail", i);
        }
        mos_sleep(1);
    }

    app_log("semtx_thread exit");
    mos_thread_delete(NULL);
}


static void sem_demo(void)
{
    mos_thread_id_t rt, tt;

    app_log("sem demo start");
    sem = mos_semphr_new(SEM_COUNT);
    rt = mos_thread_new(5, "rx", semrx_thread, 0x500, NULL);
    tt = mos_thread_new(5, "tx", semtx_thread, 0x500, NULL);
    mos_thread_join(rt);
    mos_thread_join(tt);
    app_log("sem demo end");
    mos_semphr_delete(sem);
    
    mos_sleep(1);
}


/* demo queue */
static mos_queue_id_t _que;
#define QUEUE_LEN 10

static void rx_queue_thread(void *arg)
{
    int fd;
    struct timeval t;
    fd_set rd;
    int val, ret;
    
    fd = mos_event_fd_new(_que);
    app_log("event fd: %d", fd);
    while(1) {
        t.tv_sec = 10;
        t.tv_usec = 0;
        FD_ZERO(&rd);
        FD_SET(fd, &rd);
        select(fd+1, &rd, NULL, NULL, &t);
        if (FD_ISSET(fd, &rd)) {
            ret = mos_queue_pop(_que, (void*)&val, 1);
            app_log("RX queue %d ret %d", val, ret);
        } else {
            app_log("RX queue timeout");
            break;
        }
    }
    
    mos_event_fd_delete(fd);
    app_log("rx_queue_thread exit");
    mos_thread_delete(NULL);
}

static void tx_queue_thread(void *arg)
{
    int i;
    int ret;
    
    for(i=0;i<20;i++) {
        ret = mos_queue_push(_que, &i,0);
        if (ret != kNoErr) {
            app_log("push %d fail", i);
        }
    }
    mos_sleep(5);
    for(i=0;i<5;i++) {
        ret = mos_queue_push(_que, &i,0);
        if (ret != kNoErr) {
            app_log("push %d fail", i);
        }
        mos_sleep(1);
    }
    app_log("tx_queue_thread exit");
    mos_thread_delete(NULL);
}


static void queue_demo(int queue_size)
{
    mos_thread_id_t rt, tt;

    app_log("queue demo start, queue size %d", queue_size);
    _que = mos_queue_new(queue_size, 10);
    rt = mos_thread_new(5, "rx", rx_queue_thread, 0x500, NULL);
    tt = mos_thread_new(5, "tx", tx_queue_thread, 0x500, NULL);
    mos_thread_join(rt);
    mos_thread_join(tt);
    app_log("queue demo end");
    mos_queue_delete(_que);
    
    mos_sleep(1);
}

/* demo timer */
static void _timer_cb( void* arg )
{
   char *name =(char*)arg;

   app_log("timer %s CB", name);
}

static void timer_demo(void)
{
    mos_timer_id_t id1, id2;
        
    app_log("timer demo start");
    id1 = mos_timer_new(100, _timer_cb, true, "auto reload timer");
    id2 = mos_timer_new(100, _timer_cb, false, "once timer");
    mos_timer_start(id1);
    mos_timer_start(id2);
    mos_sleep(10);
    app_log("delete all timer");
    mos_timer_delete(id1);
    mos_timer_delete(id2);
    app_log("timer demo end");
    mos_sleep(1);
}

/* demo thread */
struct thread_info {
    uint8_t prio;
    char    name[16];
};

static void _thread1(void *arg)
{
    struct thread_info *p=(struct thread_info*)arg;
    int i=0;

    app_log("thread %s enter", p->name);
    while(1) {
        app_log("thread %s, prio %d, i=%d", p->name, p->prio, i++);
        mos_sleep(1);
        if (i == 10)
            break;
    }

    app_log("thread %s EXIT", p->name);
    free(arg);
    mos_thread_delete(NULL);
}

static void thread_demo(void)
{
    int i;
    struct thread_info *p;
    mos_thread_id_t t[10];
    
    app_log("thread demo start");
    for(i=0;i<10;i++) {
        p = malloc(sizeof(struct thread_info));
        p->prio = i;
        sprintf(p->name, "thread%d", i);
        t[i] = mos_thread_new(p->prio, p->name, _thread1, 0x500, p);
    }
    for(i=0;i<10;i++) {
        mos_thread_join(t[i]);
    }
    app_log("thread demo end");
    mos_sleep(1);
}

int main(void)
{   
    thread_demo();

    timer_demo();

    sem_demo();
    
    queue_demo(1);
    queue_demo(2);
    queue_demo(3);
    queue_demo(4);

	return 0;
}
