/**
 * libx list implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement list function.
 *
 */

#ifndef __LIBX_LIST_H__
#define __LIBX_LIST_H__

/**
 * list implements, from kernel ;-)
 **/

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void xlist_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void xlist_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void xlist_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline int xlist_is_last(const struct list_head *list,
		const struct list_head *head)
{
	return list->next == head;
}

static inline int xlist_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline void
xlist_destroy(struct list_head *head, void (*free_callback)(struct list_head *))
{
	struct list_head *list;

	while (!xlist_empty(head)) {
		list = head->next;
		xlist_del(list);
		free_callback(list);
	}
}

#define container_of(ptr, type, member) ({                      \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})

#define xlist_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define xlist_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define xlist_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define xlist_for_each_entry(pos, head, member)                          \
	for (pos = xlist_entry((head)->next, typeof(*pos), member);      \
			&pos->member != (head);                         \
			pos = xlist_entry(pos->member.next, typeof(*pos), member))

#define xlist_for_each_entry_reverse(pos, head, member)                  \
	for (pos = xlist_entry((head)->prev, typeof(*pos), member);      \
			&pos->member != (head);                         \
			pos = xlist_entry(pos->member.prev, typeof(*pos), member))

/**
 * xlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define xlist_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * xlist_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define xlist_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     prefetch(pos->prev), pos != (head); \
	     pos = n, n = pos->prev)

/**
 * xlist_for_each_entry_from - iterate over list of given type from the current point
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define xlist_for_each_entry_from(pos, head, member)                    \
	for (; &pos->member != (head);      				\
	     pos = xlist_entry(pos->member.next, typeof(*pos), member))

/**
 * xlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define xlist_for_each_entry_safe(pos, n, head, member)			\
	for (pos = xlist_entry((head)->next, typeof(*pos), member),	\
		n = xlist_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_for_each_entry_safe_from
 * @pos:        the type * to use as a loop cursor.
 * @n:          another type * to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define xlist_for_each_entry_safe_from(pos, n, head, member)                     \
	for (n = xlist_entry(pos->member.next, typeof(*pos), member);            \
	     &pos->member != (head);                                            \
	     pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_for_each_entry_safe_reverse
 * @pos:        the type * to use as a loop cursor.
 * @n:          another type * to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define xlist_for_each_entry_safe_reverse(pos, n, head, member)          \
	for (pos = xlist_entry((head)->prev, typeof(*pos), member),      \
	     	n = xlist_entry(pos->member.prev, typeof(*pos), member); \
	     &pos->member != (head);                                    \
	     pos = n, n = xlist_entry(n->member.prev, typeof(*n), member))

/**
 * xlist_dump - dump list data
 * @head: list head node
 * @dump_callback: dump call back function
 *
 * display each list data by call cb function
 */ 
static inline void
xlist_dump(struct list_head *head, void (*dump_callback)(struct list_head *))
{
	struct list_head *list;

	xlist_for_each(list, head) {
		dump_callback(list);
	}
}

#endif /* __LIBX_LIST_H__ */

