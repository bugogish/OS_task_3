#ifndef SRC_THREAD_H_H
#define SRC_THREAD_H_H
#include <stdint.h>
#include "../inc/thread_list.h"

struct registry_state
{
    uint64_t rflags;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t ret_value;
}__attribute__((packed));



extern int threads_activated;
int threads_setup();
int create_thread(void (*fptr)(void));
void wait(struct thread_list_head *thread);
void kill_thread(struct thread_list_head* thread);
void round_robin_planner();
#endif //SRC_THREAD_H_H
