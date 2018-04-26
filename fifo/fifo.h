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

#include "list.h"

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
fifo_t * fifo_init(unsigned char *buf, unsigned int size);

/* fifo_alloc:   create a fifo 
 *
 * size: the length of the allocated memory
 */
fifo_t *fifo_alloc(unsigned int size);

/* fifo_free: 
 *
 */

void fifo_free(fifo_t *fifo);


/* fifo_put, 向fifo加入数据
 * @fifo,   目标fifo
 * @buf,    数据
 * @len,    数据长度
 * 如果空间不够，就删除最旧的数据，新数据覆盖旧数据 */
unsigned int fifo_put(fifo_t *fifo, unsigned char *buf, unsigned int len);

unsigned int fifo_put_tail(fifo_t *fifo, unsigned char *buf, unsigned int len);

int fifo_get(fifo_t *fifo, unsigned char *buf, unsigned int *p_len);

int fifo_get_len(fifo_t *fifo);

#endif
