#ifndef _LIST_H
#define _LIST_H

/* simple list implement
 *
 * by gexin1023@gmail.com
 *
 * */



/* container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member
 * @type:   the type of the container sturcture this is embedded in
 * @member: the name of the member with in this structure
 */
#define container_of(ptr, type, member)                                    \
                ({                                                         \
                    const typeof( ((type *)0)->member ) *__mptr = (ptr);   \
                    (type *)( (char *)__mptr - offsetof(type,member) );    \
                    })   


/* list_entry  - get the structure for this entry
 * @ptr:    the pointer of a list_head_t type node
 * @type:   the type of the container sturcture this is embedded in
 * @member: the name of the member with in this structure
 */
#define list_entry(ptr, type, mem) \
    container_of(ptr, type, mem)


/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)
	
/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)
	
	
/* list_for_each_entry
 * 
 * @pos:    the type * to use as a iteritor
 * @head:   the head for your list
 * @member: the name of the list_node within the struct
 */
#define list_for_each_entry(pos, head, member) \
    for(pos = list_entry((head)->next, typeof(*pos), member); &pos->member != (head); pos=list_entry(pos->member.next, typeof(*pos), member))

	

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(n, member))


typedef struct list_node
{
    struct list_node *next;
    struct list_node *prev;
} list_node_t;
/*
#define LIST_HEAD(name) \
        list_node_t name = {&(name), &(name)}
*/
static inline void  list_init(list_node_t *head)
{
    head->next = head;
    head->prev = head;
}

static inline void __list_add(list_node_t * prev, list_node_t *next, list_node_t *node)
{
    prev->next = node;
    node->prev = prev;

    node->next = next;
    next->prev = node;
}
/* add node as the first node */
static inline void list_add(list_node_t *head, list_node_t *node)
{
    __list_add(head, head->next, node);
}


/* add node as the last node */
static inline void list_add_tail(list_node_t *head, list_node_t *node)
{
    __list_add(head->prev, head, node);
}


static inline void __list_del(list_node_t *prev, list_node_t *next, list_node_t *node)
{
    prev->next = next;
    next->prev = prev;

    node->next = node;
    node->prev = node;
}
/* delete node */
static inline void list_del(list_node_t *node)
{
    __list_del(node->prev, node->next, node);
}

/* if the list is empty */
static inline int list_empty(list_node_t *head)
{
    return head == head->next;
}


#endif

