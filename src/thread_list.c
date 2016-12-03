#include <stdint.h>
#include "../inc/thread_list.h"


void thread_list_init(struct thread_list_head *head)
{ head->next = head->prev = head; }

static void thread_list_insert(struct thread_list_head *new, struct thread_list_head *prev,
                        struct thread_list_head *next)
{
    new->prev = prev;
    new->next = next;
    prev->next = new;
    next->prev = new;
}

void thread_list_add(struct thread_list_head *new, struct thread_list_head *head)
{ thread_list_insert(new, head, head->next); }

void thread_list_add_tail(struct thread_list_head *new, struct thread_list_head *head)
{ thread_list_insert(new, head->prev, head); }

static void __thread_list_del(struct thread_list_head *prev, struct thread_list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

void thread_list_del(struct thread_list_head *entry)
{
    __thread_list_del(entry->prev, entry->next);
    entry->next = 0;
    entry->prev = 0;
}

static void __thread_list_splice(struct thread_list_head *list, struct thread_list_head *prev,
                          struct thread_list_head *next)
{
    struct thread_list_head *first = list->next;
    struct thread_list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;
    last->next = next;
    next->prev = last;
}

void thread_list_splice(struct thread_list_head *list, struct thread_list_head *head)
{
    if (!thread_list_empty(list)) {
        __thread_list_splice(list, head, head->next);
        thread_list_init(list);
    }
}

int thread_list_empty(const struct thread_list_head *head)
{ return head->next == head; }

struct thread_list_head *thread_list_first(struct thread_list_head *head)
{ return head->next; }

size_t thread_list_size(const struct thread_list_head *head)
{
    size_t size = 0;

    for (const struct thread_list_head *pos = head->next; pos != head;
         pos = pos->next)
        ++size;
    return size;
}
