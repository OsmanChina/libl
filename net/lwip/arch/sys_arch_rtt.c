/*
 * File      : sys_arch.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-12-8      Bernard      add file header
 *                             export bsd socket symbol for RT-Thread Application Module 
 */

#include <os/rtt/rtthread.h>

#include <net/lwip/sys.h>
#include <net/lwip/opt.h>
#include <net/lwip/stats.h>
#include <net/lwip/err.h>
#include <net/lwip/arch/sys_arch.h>
#include <net/lwip/debug.h>
#include <net/lwip/netif.h>
#include <net/lwip/tcpip.h>
#if TCPPS_ETH_ENABLE
#include <net/lwip/netif/ethernetif.h>
#endif
#include <net/lwip/sio.h>

#include <string.h>

#if TCPPS_ETH_ENABLE
static err_t netif_device_init(struct netif *netif)
{
    struct eth_device *ethif;

    ethif = (struct eth_device *)netif->state;
    if (ethif != RT_NULL)
    {
        rt_device_t device;

        /* get device object */
        device = (rt_device_t) ethif;
        if (rt_device_init(device) != RT_EOK)
        {
            return ERR_IF;
        }

        /* copy device flags to netif flags */
        netif->flags = ethif->flags;

        return ERR_OK;
    }

    return ERR_IF;
}
#endif


void sys_init(void)
{
    /* nothing on RT-Thread porting */
}


/** Ticks/jiffies since power up. */
u32_t sys_jiffies(void)
{

	return rt_tick_get();
}


/** Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it. */
u32_t sys_now(void)
{

	return OS_TICK_MS * sys_jiffies();
}


err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    sys_sem_t tmpsem;

    RT_DEBUG_NOT_IN_INTERRUPT;

    tmpsem = rt_sem_create(NULL, count, RT_IPC_FLAG_FIFO);
    if (tmpsem == RT_NULL)
        return ERR_MEM;
    else
    {
        *sem = tmpsem;

        return ERR_OK;
    }
}

void sys_sem_free(sys_sem_t *sem)
{
    RT_DEBUG_NOT_IN_INTERRUPT;
    rt_sem_delete(*sem);
}

void sys_sem_signal(sys_sem_t *sem)
{
    rt_sem_release(*sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    rt_err_t ret;
    s32_t t;
    u32_t tick;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = rt_tick_get();
    if (timeout == 0)
        t = RT_WAITING_FOREVER;
    else
    {
        /* convert msecond to os tick */
        if (timeout < (1000/RT_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000/RT_TICK_PER_SECOND);
    }

    ret = rt_sem_take(*sem, t);

    if (ret == -RT_ETIMEOUT)
        return SYS_ARCH_TIMEOUT;

    /* get elapse msecond */
    tick = rt_tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / RT_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

#ifndef sys_sem_valid
/** Check if a semaphore is valid/allocated:
 *  return 1 for valid, 0 for invalid
 */
int sys_sem_valid(sys_sem_t *sem)
{
    return (int)(*sem);
}
#endif

#ifndef sys_sem_set_invalid
/** Set a semaphore invalid so that sys_sem_valid returns 0
 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = RT_NULL;
}
#endif

/* ====================== Mailbox ====================== */

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    sys_mbox_t tmpmbox;

    RT_DEBUG_NOT_IN_INTERRUPT;

    tmpmbox = rt_mb_create(NULL, size, RT_IPC_FLAG_FIFO);
    if (tmpmbox != RT_NULL)
    {
        *mbox = tmpmbox;

        return ERR_OK;
    }

    return ERR_MEM;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    RT_DEBUG_NOT_IN_INTERRUPT;

    rt_mb_delete(*mbox);

    return;
}

/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    RT_DEBUG_NOT_IN_INTERRUPT;

    rt_mb_send_wait(*mbox, (rt_uint32_t)msg, RT_WAITING_FOREVER);

    return;
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (rt_mb_send(*mbox, (rt_uint32_t)msg) == RT_EOK)
        return ERR_OK;

    return ERR_MEM;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    rt_err_t ret;
    s32_t t;
    u32_t tick;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = rt_tick_get();

    if(timeout == 0)
        t = RT_WAITING_FOREVER;
    else
    {
        /* convirt msecond to os tick */
        if (timeout < (1000/RT_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000/RT_TICK_PER_SECOND);
    }

    ret = rt_mb_recv(*mbox, (rt_uint32_t *)msg, t);

    if(ret == -RT_ETIMEOUT)
        return SYS_ARCH_TIMEOUT;
    else
    {
        LWIP_ASSERT("rt_mb_recv returned with error!", ret == RT_EOK);
    }

    /* get elapse msecond */
    tick = rt_tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / RT_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int ret;

    ret = rt_mb_recv(*mbox, (rt_uint32_t *)msg, 0);

    if(ret == -RT_ETIMEOUT)
        return SYS_ARCH_TIMEOUT;
    else
    {
        if (ret == RT_EOK) 
            ret = 1;
    }

    return ret;
}

#ifndef sys_mbox_valid
/** Check if an mbox is valid/allocated:
 *  return 1 for valid, 0 for invalid
 */
int sys_mbox_valid(sys_mbox_t *mbox)
{
    return (int)(*mbox);
}
#endif

#ifndef sys_mbox_set_invalid
/** Set an mbox invalid so that sys_mbox_valid returns 0
 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = RT_NULL;
}
#endif

/* ====================== System ====================== */

sys_thread_t sys_thread_new(const char    *name,
                            lwip_thread_fn thread,
                            void          *arg,
                            int            stacksize,
                            int            prio)
{
    rt_thread_t t;

    RT_DEBUG_NOT_IN_INTERRUPT;

    /* create thread */
    t = rt_thread_create(name, thread, arg, stacksize, prio, 20);
    RT_ASSERT(t != RT_NULL);

    /* startup thread */
    rt_thread_startup(t);

    return t;
}

sys_prot_t sys_arch_protect(void)
{
    rt_base_t level;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    return level;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    /* enable interrupt */
    rt_hw_interrupt_enable(pval);

    return;
}

void sys_arch_assert(const char *file, int line)
{
    rt_kprintf("\nAssertion: %d in %s, thread %s\n",
               line, file, rt_thread_self()->name);
    RT_ASSERT(0);
}



