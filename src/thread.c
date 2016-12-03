#include "../inc/alloc.h"
#include "../inc/thread_list.h"
#include "../inc/thread.h"
#include "../inc/print.h"
#include "../inc/ints.h"
#include "../inc/spinlock.h"

const size_t STACK_SIZE = 4096;

extern void switch_threads(void **prev, void *next);
extern void awrapper();

atomic_int count; //just for tracking thread_id for debugging purposes

static struct thread_list_head pending_threads_pool;
static struct thread_list_head to_be_killed;
static struct thread_list_head* current_thread;

int threads_setup()
{
    printf("Setting up threads...\n");

    thread_list_init(&pending_threads_pool);
    thread_list_init(&to_be_killed);

    struct thread_list_head *main_thread = (struct thread_list_head*)mem_alloc(sizeof(struct thread_list_head));

    if (main_thread == NULL)
    {
        printf("Couldn't allocate memory for a thread!");
        return -1;
    }


    main_thread -> stack_ptr = (void*)((uintptr_t) mem_alloc(STACK_SIZE) + STACK_SIZE);
    main_thread -> status = ACTIVE;
    main_thread -> thread_id = count++;
    current_thread = main_thread;

    return 0;
}


int create_thread(void (*fptr)(void))
{
    lock();

    struct thread_list_head *new_thread = mem_alloc(sizeof(*new_thread));

    if (new_thread == NULL)
    {
        printf("Couldn't allocate memory for a thread!");
        unlock();
        return -1;
    }

    new_thread -> stack_ptr = (void*)((uintptr_t) mem_alloc(STACK_SIZE) + STACK_SIZE);

    if (new_thread -> stack_ptr == NULL)
    {
        printf("Couldn't allocate memory for stack!");
        mem_free(new_thread);
        unlock();
        return -1;
    }

    //initialize state
    void* rsp = new_thread -> stack_ptr;
    struct registry_state* state =  (struct registry_state*)((uintptr_t)rsp - sizeof(*state));
    state -> rflags = 0x00;
    state -> ret_value = (uintptr_t) awrapper;
    state -> rbx = (uintptr_t) new_thread;
    state -> rbp = (uintptr_t) fptr;
    new_thread -> stack_ptr = state;

    thread_list_add_tail(new_thread, &pending_threads_pool);
    new_thread -> status = PENDING;
    new_thread -> thread_id = count++;

    unlock();
    return 0;
}

void wrapper (void (*fptr) (void), struct thread_list_head* cur_thread)
{
    enable_ints();
    fptr();
    kill_thread(cur_thread);
}



void kill_thread(struct thread_list_head *thread)
{
    lock();
    thread_list_add(thread, &to_be_killed);
    thread ->  status = FINISHED;
    unlock();
    round_robin_planner();
}

void wait(struct thread_list_head *thread)
{
    while (thread -> status != FINISHED)
    {
        __asm__ volatile ("":::"memory");
    }

}

void thread_resources_free()
{
    while (!thread_list_empty(&to_be_killed))
    {
        struct thread_list_head* thread = thread_list_first(&to_be_killed);
        mem_free(thread ->  stack_ptr);
        thread_list_del(thread); //deletes from to_be_deleted
        mem_free(thread);

    }
}

void round_robin_planner()
{
    lock();

    if ((! thread_list_empty(&to_be_killed)) && (current_thread -> status != FINISHED))
        thread_resources_free();

    if (thread_list_empty(&pending_threads_pool))
    {
        unlock();
        return;
    }

    struct thread_list_head* first_in_line = thread_list_first(&pending_threads_pool);
    thread_list_del(first_in_line);
    first_in_line -> status = ACTIVE;
    if (current_thread -> status != FINISHED)
    {
        current_thread ->  status = PENDING;
        thread_list_add_tail(current_thread, &pending_threads_pool);
    }

    struct thread_list_head *temp = current_thread;
    current_thread = first_in_line;
    switch_threads(&temp ->  stack_ptr, first_in_line -> stack_ptr);

    unlock();
}
//time.c contains implementation - call of round-robin in PIT interrupt