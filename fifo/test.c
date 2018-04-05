#include <stdio.h>
#include "../fifo/fifo.h"

int main()
{
    
    fifo_t *fifo=fifo_alloc(128);
    int i =0;
    char str[100];
    while(i++ < 10)
    {
        sprintf(str, "dasfkajshdflasldfjhlaksdlhfasdfstr%d", i);
        fifo_put(fifo, str, strlen(str)); 
    }
  

    fifo_put_tail(fifo, str, strlen(str));


    printf("*****************************************\n");
   
    pos_t *pos;
    list_for_each_entry(pos, &(fifo->pos_head), node)
    {
        printf("%d,%d\n", pos->pos, pos->len);

    }

    memset(str, 0, 20);

    while(1)
    {
        int t = fifo_get(fifo, str, &i);
        if(t < 0)
            return 0;

        printf("string: \"%s\"\tlength:%d\n", str, i);
    }
    return 0;
}
