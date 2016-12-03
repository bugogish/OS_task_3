#ifndef OS_SPINLOCK_H
#define OS_SPINLOCK_H

#include <stdatomic.h>
//
//struct spinlock {
//    atomic_int locked;
//};

void lock();
void unlock();

#endif //OS_SPINLOCK_H
