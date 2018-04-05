#ifndef _FIFO_H
#define _FIFO_H

/* simple fifo implement
 *
 * 分配固定长度的buffer作为环形缓冲区，通过一个list记录每条加入数据的位置及长度；
 * 当要加入数据长度超过剩余空间时，删除旧数据，相当于新数据把旧数据覆盖。
 * 考虑到每条数据的长度不等，因此需要一个链表保存每条数据的长度
 *
 * gexin1023@gmail.com
 * 2018-03-30
 */
#include <string.h>
#include <malloc.h>
#include "../list/list.h"
#pragma pack(1)


#define min(A, B)   \
    ((A)<(B)?(A):(B))
typedef struct _pos_t_
{
    unsigned int pos;   // position index in buffer
    unsigned int len;   // the length of data
    list_node_t  node;
}pos_t;

typedef struct _fifo_t
{
    unsigned char   *buffer;
    unsigned int    size;
    unsigned int    in;
    unsigned int    out;
    list_node_t     pos_head;
} fifo_t;

/* fifo_init:   create a fifo using a preallocated memory
 *
 * buf:     preallocated memory
 * size:    the length of the preallocated memory, 取以2为底的整数
 */
static inline fifo_t * fifo_init(unsigned char *buf, unsigned int size)
{
    fifo_t *fifo = (fifo_t *)malloc(sizeof(fifo_t));
    
    fifo->buffer = buf;
    fifo->size   = size;
    fifo->in = fifo->out = 0;
    fifo->pos_head.next = &(fifo->pos_head);
    fifo->pos_head.prev = &(fifo->pos_head);
    return fifo;
}

/* fifo_alloc:   create a fifo 
 *
 * size: the length of the allocated memory
 */
static inline fifo_t *fifo_alloc(unsigned int size)
{
    unsigned char * buf = (unsigned char *)malloc(size);
    return fifo_init(buf, size);
}

/* fifo_free: 
 *
 */

static inline void fifo_free(fifo_t *fifo)
{
    free(fifo->buffer);
    free(fifo);
}

/* fifo_put, 向fifo加入数据
 * @fifo,   目标fifo
 * @buf,    数据
 * @len,    数据长度
 * 如果空间不够，就删除最旧的数据，新数据覆盖旧数据 */
static unsigned int __fifo_put(fifo_t *fifo, unsigned char *buf, unsigned int len)
{
    unsigned int l;
    
    /* fifo 空间不足时，删除旧内容，直到可以容纳新的数据 */
    while(len>(fifo->size - fifo->in + fifo->out))
    {
        pos_t *pos = list_entry(fifo->pos_head.prev, pos_t, node);
        fifo->out += pos->len;
        list_del(fifo->pos_head.prev);
        free(pos);
    }
    

    /* 首先复制数据从（ in % buf_size）位置到buffer结尾 */
    l = min(len , fifo->size - (fifo->in & (fifo->size-1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size-1)), buf ,l);

    /* 然后复制剩下的数据从buffer开头开始 */
    memcpy(fifo->buffer, buf+l, len-l);

    /* 加入新的位置节点 */
    pos_t *pos = (pos_t *)malloc(sizeof(pos_t));
    pos->len=len;
    pos->pos=fifo->in;
    list_add(&(fifo->pos_head), &(pos->node));

    /* 更改写入点索引 */
    fifo->in += len;
    
    return len;
}

static unsigned int fifo_put(fifo_t *fifo, unsigned char *buf, unsigned int len)
{
    return __fifo_put(fifo, buf, len);
}

static unsigned int fifo_put_tail(fifo_t *fifo, unsigned char *buf, unsigned int len)
{
    unsigned int l;
    
    /* fifo 空间不足时，删除旧内容，直到可以容纳新的数据 */
    while(len>(fifo->size - fifo->in + fifo->out))
    {
        pos_t *pos = list_entry(fifo->pos_head.prev, pos_t, node);
        fifo->out += pos->len;
        list_del(fifo->pos_head.prev);
        free(pos);
    }

    
    fifo->out -= len;

    /* 首先复制数据从（ out % buf_size）位置到buffer结尾 */
    l = min(len , fifo->size - (fifo->out & (fifo->size-1)));
    memcpy(fifo->buffer + (fifo->out & (fifo->size-1)), buf ,l);

    /* 然后复制剩下的数据从buffer开头开始 */
    memcpy(fifo->buffer, buf+l, len-l);

    /* 加入新的位置节点 */
    pos_t *pos = (pos_t *)malloc(sizeof(pos_t));
    pos->len=len;
    pos->pos=fifo->out;
    list_add_tail(&(fifo->pos_head), &(pos->node));
    
    return len;
}

static int fifo_get(fifo_t *fifo, unsigned char *buf, unsigned int *p_len)
{
    if(fifo->pos_head.next == &(fifo->pos_head))
    {
        // fifo is emperty
        return -1;
    }

    pos_t *pos = list_entry(fifo->pos_head.prev, pos_t, node);
    *p_len = pos->len;

    list_del(&(pos->node));

    free(pos);


    int l = min(*p_len, fifo->size - (fifo->out &(fifo->size-1)));
    memcpy(buf, fifo->buffer+(fifo->out & (fifo->size-1)), l);
    memcpy(buf+l, fifo->buffer, *p_len-l);
   

    fifo->out += *p_len;
    return *p_len;
}





#endif
