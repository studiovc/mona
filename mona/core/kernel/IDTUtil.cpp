/*!
    \file  IDTUtil.cpp
    \brief class functions for set up IDT

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2003/06/08 update:$Date$
*/

#include "global.h"
#include "operator.h"
#include "IDTUtil.h"

/*!
    \brief call lidt

    \param idtr idtr
    \author Higepon
    \date   create:2003/06/08 update:
*/
void IDTUtil::lidt(IDTR* idtr) {

    asm volatile("lidt (%0) ": :"q" (idtr));
    return;
}

/*!
    \brief set gate descriptor

    \param descZero pointer to idt
    \param selector selector
    \param handler handler

    \author Higepon
    \date   create:2003/06/08 update:
*/
void IDTUtil::setGateDesc(GateDesc* descZero, uint16_t selector, InterruptHandlers* handler) {

    GateDesc* desc = descZero + handler->number;

    desc->offsetL  = (uint32_t)(handler->handler) & 0x0000FFFF;
    desc->offsetH  = ((uint32_t)(handler->handler) & 0xFFFF0000) >> 16;
    desc->selector = selector;
    if (handler->number == 0x80 || /* system call     */
        handler->number == 0x3) {  /* debug exception */
        // DPL = 3
        desc->type = 0xEE;
    } else {
        // DPL = 0
        desc->type = 0x8E;
    }
    desc->unused   = 0x00;
    return;
}

/*!
    \brief set up IDT

    \author Higepon
    \date   create:2003/06/08 update:
*/
void IDTUtil::setup() {

    g_idt = (GateDesc*)malloc(sizeof(GateDesc) * IHANDLER_NUM);

    extern InterruptHandlers handlers[IHANDLER_NUM];

    for (int i = 0; i < IHANDLER_NUM; i++) {

        setGateDesc(g_idt, KERNEL_CS, &handlers[i]);
    }

    /* lidt */
    IDTR idtr;
    idtr.base  = (uint32_t)g_idt;
    idtr.limit = sizeof(GateDesc) * IHANDLER_NUM - 1;
    lidt(&idtr);
    return;
}
