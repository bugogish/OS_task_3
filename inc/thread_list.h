#ifndef SRC_THREAD_LIST_H
#define SRC_THREAD_LIST_H

#include "kernel.h"

enum status
{
    ACTIVE,
    PENDING,
    FINISHED
};


struct thread_list_head
{
    struct thread_list_head *next;
    struct thread_list_head *prev;
    int thread_id;
    enum status status;
    void *stack_ptr;
};


void thread_list_init(struct thread_list_head *head);
void thread_list_add(struct thread_list_head *new, struct thread_list_head *head);

static inline void thread_list_insert_after(struct thread_list_head *new,
                                     struct thread_list_head *node)
{ thread_list_add(new, node); }

void thread_list_add_tail(struct thread_list_head *new, struct thread_list_head *head);

static inline void thread_list_insert_before(struct thread_list_head *new,
                                      struct thread_list_head *node)
{ thread_list_add_tail(new, node); }


void thread_list_del(struct thread_list_head *entry);
void thread_list_splice(struct thread_list_head *list, struct thread_list_head *head);
int thread_list_empty(const struct thread_list_head *head);
struct thread_list_head *thread_list_first(struct thread_list_head *head);
size_t thread_list_size(const struct thread_list_head *head);

#endif //SRC_THREAD_LIST_H
