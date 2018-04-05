/*
 *  COPYRIGHT NOTICE  
 *  Copyright (C) 2015, Jhuster, All Rights Reserved
 *  Author: Jhuster(lujun.hust@gmail.com)
 *  
 *  https://github.com/Jhuster/TLV
 *   
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, 
 *  or (at your option) any later version.
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/socket.h>
#include "tlv_box.h"

/*
static void tlv_box_release_tlv(value_t value)
{
    tlv_t *tlv = (tlv_t *)value.value;
    free(tlv->value);
    free(tlv);
}
*/

tlv_box_t *tlv_box_create()
{
    tlv_box_t* box = (tlv_box_t*)malloc(sizeof(tlv_box_t));
    list_init(&(box->m_list));
    
    box->m_serialized_buffer = NULL;
    box->m_serialized_bytes = 0;
    return box;
}

int tlv_box_putobject(tlv_box_t *box, int type, void *value, int length)
{
    if (box->m_serialized_buffer != NULL) {
        return -1;
    }

    tlv_t *tlv = (tlv_t *)malloc(sizeof(tlv_t));
    tlv->type = type;
    tlv->length = htonl(length);
    tlv->value = (unsigned char *) malloc(length);
    memcpy(tlv->value, value, length);

    list_init(&tlv->node);

    list_add_tail(&box->m_list, &tlv->node);
    
    box->m_serialized_bytes += sizeof(int) * 2 + length;
    
    return 0;
}
tlv_box_t *tlv_box_parse(unsigned char *buffer, int buffersize)
{
    tlv_box_t *box = tlv_box_create();

    unsigned char *cached = (unsigned char*) malloc(buffersize);
    memcpy(cached, buffer, buffersize);

    int offset = 0, length = 0;
    while (offset < buffersize) {
        int type = (*(int *)(cached + offset));
        offset += sizeof(int);
        int length = ntohl(*(int *)(cached + offset));
        offset += sizeof(int);
        tlv_box_putobject(box, type, cached+offset, length);
        offset += length;
    }

    box->m_serialized_buffer = cached;
    box->m_serialized_bytes = buffersize;

    return box;
}

int tlv_box_destroy(tlv_box_t *box)
{
   // key_list_destroy(box->m_list);
    
    tlv_t *pos, *n;
    
    list_for_each_entry_safe(pos, n, &box->m_list, node)
    {
        list_del(&pos->node);
        free(pos->value);
        free(pos);
        printf("*");
    }

    if (box->m_serialized_buffer != NULL) {
        free(box->m_serialized_buffer);        
    }

    free(box);

    return 0;
}

unsigned char *tlv_box_get_buffer(tlv_box_t *box)
{
    return box->m_serialized_buffer;
}

int tlv_box_get_size(tlv_box_t *box)
{
    return box->m_serialized_bytes;
}


int tlv_box_put_char(tlv_box_t *box, int type, char value)
{
    return tlv_box_putobject(box, type, &value, sizeof(char));
}

int tlv_box_put_short(tlv_box_t *box, int type, short value)
{
    return tlv_box_putobject(box, type, &value, sizeof(short));
}

int tlv_box_put_int(tlv_box_t *box, int type, int value)
{
    return tlv_box_putobject(box, type, &value, sizeof(int));
}

int tlv_box_put_long(tlv_box_t *box, int type, long value)
{
    return tlv_box_putobject(box, type, &value, sizeof(long));
}

int tlv_box_put_longlong(tlv_box_t *box, int type, long long value)
{
    return tlv_box_putobject(box, type, &value, sizeof(long long));
}

int tlv_box_put_float(tlv_box_t *box, int type, float value)
{
    return tlv_box_putobject(box, type, &value, sizeof(float));
}

int tlv_box_put_double(tlv_box_t *box, int type, double value)
{
    return tlv_box_putobject(box, type, &value, sizeof(double));
}

int tlv_box_put_string(tlv_box_t *box, int type, char *value)
{
    return tlv_box_putobject(box, type, value, strlen(value)+1);
}

int tlv_box_put_bytes(tlv_box_t *box, int type, unsigned char *value, int length)
{
    return tlv_box_putobject(box, type, value, length);
}

int tlv_box_put_object(tlv_box_t *box, int type, tlv_box_t *object)
{
    return tlv_box_putobject(box, type, tlv_box_get_buffer(object), tlv_box_get_size(object));
}

int tlv_box_serialize(tlv_box_t *box)
{
    if (box->m_serialized_buffer != NULL) {
        return -1;
    }

    int offset = 0;
    unsigned char* buffer = (unsigned char*) malloc(box->m_serialized_bytes);    
    tlv_t *pos; 
    list_for_each_entry(pos,&box->m_list, node) 
    {
        memcpy(buffer+offset, &pos->type, sizeof(int));
        offset += sizeof(int);        
        memcpy(buffer+offset, &pos->length, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer+offset, pos->value, ntohl(pos->length));        
        offset +=ntohl(pos->length);
    }

    box->m_serialized_buffer = buffer;

    return 0;
}

int tlv_box_get_char(tlv_box_t *box, int tag, char *value)
{
    int ret = -1;
    
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *(pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    
    return ret;
}

int tlv_box_get_short(tlv_box_t *box, int tag, short *value)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((short*)pos->value);
            free(pos->value);
            free(pos);
            ret= 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_int(tlv_box_t *box, int tag, int *value)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((int*)pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_long(tlv_box_t *box, int tag, long *value)
{
    int ret = -1;
    tlv_t *pos,*n;
list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((long*)pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_longlong(tlv_box_t *box, int tag , long long *value)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((long long*)pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_float(tlv_box_t *box, int tag, float *value)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((float*)pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_double(tlv_box_t *box, int tag, double *value)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);
            *value = *((float*)pos->value);
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}

int tlv_box_get_string(tlv_box_t *box, int type, char *value, int* length)
{
    return tlv_box_get_bytes(box,type,(unsigned char *)value,length);
}

int tlv_box_get_bytes(tlv_box_t *box, int tag, unsigned char *value, int* length)
{
    int ret = -1;
    tlv_t *pos,*n;
    list_for_each_entry(pos, &box->m_list, node)
    {
        if(tag == pos->type)
        {
            list_del(&pos->node);

            if(*length < ntohl(pos->length))
            {
                return -1;
            }
            memset(value, 0, *length);
            *length = ntohl(pos->length);
            memcpy(value, pos->value, ntohl(pos->length));
            free(pos->value);
            free(pos);
            ret = 0;
            break;
        }
    }
    return ret;
}
#if 0
int tlv_box_get_bytes_ptr(tlv_box_t *box, int type, unsigned char **value, int* length)
{
    /*
    value_t object;
    if (key_list_get(box->m_list, type, &object) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *) object.value;
    *value = tlv->value;
    *length = tlv->length;
    */
    return 0;
}

int tlv_box_get_object(tlv_box_t *box, int type, tlv_box_t **object)
{
    /*
    value_t value;
    if (key_list_get(box->m_list, type, &value) != 0) {
        return -1;
    }
    tlv_t *tlv = (tlv_t *) value.value;
    *object = (tlv_box_t *)tlv_box_parse(tlv->value, tlv->length);
    */
    return 0;
}

#endif
