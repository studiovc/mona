/*!
    \file  kthread.h
    \brief kernel thread

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#ifndef _MONA_KTHREAD_
#define _MONA_KTHREAD_

#include<monaTypes.h>

/*! struct for kernel thread */
typedef struct Kthread {
    dword  eip;
    dword  cs;
    dword  eflags;
    dword  eax;
    dword  ecx;
    dword  edx;
    dword  ebx;
    dword  esp;
    dword  ebp;
    dword  esi;
    dword  edi;
    dword  tick;
    Kthread* prev;
    Kthread* next;
};

/*! struct for all thread information */
typedef struct KthreadInfo {
    dword threadNum;
    dword tick;
};

extern Kthread* current;


void kthread_init();
void kthread_tick();
void kthread_add_to_run(Kthread* thread);
void kthread_remove_from_run(Kthread* thread);
void kthread_yield();
Kthread* kthread_create_thread(dword stack, void (*f)());
dword kthread_allocate_stack();
void kthread_idle();
void kthread_idle2();
void kthread_schedule();
void kthread_switch();
void kthread_printInfo();
#endif
