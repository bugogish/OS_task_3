#include <stdatomic.h>
#include <../inc/spinlock.h>
#include "../inc/ints.h"
#include "../inc/thread.h"
#include "../inc/print.h"
//
//#define LOCKED 1
//#define UNLOCKED 0

// NOT SPINLOCK ANYMORE, USING INTERRUPTS INSTEAD

//void lock(struct spinlock *lock)
//{
//    do {
//        while (atomic_load_explicit(&lock->locked, memory_order_relaxed) == LOCKED);
//    } while (atomic_exchange_explicit(&lock->locked, LOCKED, memory_order_acquire) == LOCKED);
//}
//
//void unlock(struct spinlock *lock)
//{
//    atomic_store_explicit(&lock->locked, UNLOCKED, memory_order_release);
//}

static atomic_int acquired_locks;

void lock()
{
    acquired_locks ++;
    disable_ints();
}

void unlock()
{
    acquired_locks --;
    if ((acquired_locks == 0)&&(threads_activated)) // because otherwise will enable interrupts when it's too early!
        enable_ints();
}