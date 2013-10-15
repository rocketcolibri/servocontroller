/**
 *  ADLIB - a C library of fundamental algorithms and their data structures
 *  Copyright (C) 2004 Rex Recio
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA
 */

/*---------------------------------------------------------------------------*/
/* Linklist.c - a thread-safe implementattion of a circular singly-linklist */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "AD.h"

#define _empty(list) ((list)->tail ? AD_FALSE : AD_TRUE)
#define APPEND 1
#define PREPEND !(APPEND)

/*Linklist*/
struct _LLNODE
{
  struct _LLNODE *next;
  void *data;
};
struct _LINKLIST
{
  struct _LLNODE *tail;     /*tail of the circular linklist*/
  pthread_mutex_t mutex;
};

typedef struct _LLNODE LLNODE;


void *ll_delete_head (LINKLIST *list);
void *ll_delete_tail (LINKLIST *list);
static LLNODE *_allocate(LINKLIST *list);
static void _deallocate (LLNODE *node);

static MEMCHUNK *nodeChunk=NULL; /*memory chunk of nodes*/

LINKLIST *ll_init(void)
{
  LINKLIST *list;
  list = cx_malloc (sizeof (LINKLIST) );
  list->tail = NULL;
  pthread_mutex_init(&(list->mutex), NULL);

  return list;
}
static LLNODE *_allocate(LINKLIST *list)
{
  if (nodeChunk == NULL)
    nodeChunk = mc_init (sizeof (LLNODE));

  return mc_alloc (nodeChunk);
}
static void _deallocate (LLNODE *node)
{
  mc_free_atom (nodeChunk, node);
}
AD_BOOLEAN ll_empty (LINKLIST *list)
{
  return _empty (list);
}
static LINKLIST *_add(LINKLIST *list, void *newdata, int insertType)
{
  LLNODE *newnode;

  pthread_mutex_lock(&(list->mutex));

  newnode = _allocate(list);
  newnode->data = newdata;

  if (_empty (list) == AD_TRUE)
  {
    newnode->next = newnode;
    list->tail = newnode;
  }
  else
  {
    newnode->next = list->tail->next;
    list->tail->next = newnode;
    if (insertType == APPEND)
      list->tail = list->tail->next;
  }
  pthread_mutex_unlock(&(list->mutex));

  return list;
}


LINKLIST *ll_prepend (LINKLIST *list,void *newdata)
{
    return _add(list, newdata, PREPEND);
}

LINKLIST *ll_append (LINKLIST *list,void *newdata)
{
  return _add(list, newdata, APPEND);
}
void *ll_find_adj (LINKLIST *list, void *key, AD_COMPARE_FUNC *match)
{
  LLNODE *current, *head, *prev;

  if (_empty (list) == AD_TRUE)
    return NULL;

  head = list->tail->next; current = head; prev = list->tail;
  do
  {
    if ( match(current->data,key) == AD_EQUAL)
    {
      if (current == head)  /*head is retrieved*/
        return current->data;

      pthread_mutex_lock(&(list->mutex));
      if (current == list->tail) /*tail is retrieved*/
        list->tail = prev;
      else /*general case*/
      {
        /*put the retrieved node at the head*/
        prev->next = current->next;
        current->next = head;
        list->tail->next = current;
      }
      pthread_mutex_unlock(&(list->mutex));

      return current->data;
    }
    prev = current;
    current = current->next;
  }
  while (current != head); /*end when you return to head*/

  return NULL;
}
void *ll_find (LINKLIST *list, void *key, AD_COMPARE_FUNC *match)
{
  LLNODE *current, *head;

  if (_empty (list) == AD_TRUE)
    return NULL;

  /*begin current at the start of the list*/
  current = head = list->tail->next;
  do
  {
    if ( match(current->data,key) == AD_EQUAL)
      return current->data;
    current = current->next;
  }
  while (current != head); /*end when you return to head*/

  return NULL;
}
void *ll_delete (LINKLIST *list, void *key, AD_COMPARE_FUNC *match)
{
  LLNODE *current, *prev, *head;
  void *data;

  if (_empty (list) == AD_TRUE)
    return NULL;

  current = head = list->tail->next;
  prev = list->tail;


  do
  {
    if (match(current->data, key) == AD_EQUAL)
    {
      pthread_mutex_lock(&(list->mutex));

      if (current == prev)  /*only node*/
        list->tail = NULL;
      else
      {
        prev->next = current->next;  /*unlink the node to be deleted*/
        if (current == list->tail)   /*last node is to be deleted*/
          list->tail = prev;
      }
      data = current->data;
      _deallocate (current);

      pthread_mutex_unlock(&(list->mutex));

      return data;
    }
    prev = current;
    current = current->next;
  }
  while (current != head);


  return NULL;
}
AD_BOOLEAN ll_traverse (LINKLIST *list, AD_TRAV_FUNC  *travFunc, void *param)
{
  LLNODE *current, *head;

  if (_empty (list) == AD_TRUE)
    return AD_FALSE;

  head = list->tail->next;
  current = head;
  do
  {
    if ( travFunc(current->data, param) == AD_FALSE )
      return AD_FALSE;
    current = current->next;
  }
  while (current != head);

  return AD_TRUE;
}
void ll_free(LINKLIST *list)
{
  LLNODE *current, *temp;

  pthread_mutex_lock(&(list->mutex));
  if (_empty (list) == AD_FALSE)
  {
    /*initialise*/
    current = list->tail->next;   /*start at the head*/
    list->tail->next = NULL;      /*break the circular nature*/

    while(current != NULL)
    {
      temp = current;
      current = current->next;
      _deallocate (temp);
    }

  }
  pthread_mutex_unlock(&(list->mutex));
  pthread_mutex_destroy(&(list->mutex));
  cx_free(list);
}
/*to be used by the deque data type*/
void *ll_delete_head (LINKLIST *list)
{
  void *outdata;
  LLNODE *head;

  if (_empty (list) == AD_TRUE)
    return NULL;

  pthread_mutex_lock(&(list->mutex));
  head = list->tail->next;
  outdata = head->data;

  if (head == list->tail)           /*only one node*/
    list->tail = NULL;
  else                              /*more than one one*/
    list->tail->next = head->next;

  _deallocate (head);
  pthread_mutex_unlock(&(list->mutex));

  return outdata;
}
void *ll_delete_tail (LINKLIST *list)
{
  LLNODE *previous;
  void *data;

        /*empty list*/
  if (_empty (list) == AD_TRUE)
    return NULL;

  pthread_mutex_lock(&(list->mutex));
  data = list->tail->data;

  /*locate the node prior to the tail*/
  previous = list->tail;
  do
  {
    previous = previous->next;
  }
  while (previous->next != list->tail);

  /*only one element in the list*/
  if(previous == list->tail)
  {
    _deallocate(list->tail);
    list->tail = NULL;
  }
        /*general case of more than one element*/
  else
  {
    previous->next = list->tail->next;
    _deallocate(list->tail);
    list->tail = previous;
  }
  pthread_mutex_unlock(&(list->mutex));
  return data;
}
void *ll_head (LINKLIST *list)
{
  if (_empty (list) == AD_TRUE)
    return NULL;
  else
    return list->tail->next->data;
}
void *ll_tail (LINKLIST *list)
{
  if (_empty (list) == AD_TRUE)
    return NULL;
  else
    return list->tail->data;
}
void ll_merge(LINKLIST *destination, LINKLIST *source)
{
  LLNODE *head;

  if (_empty(source) == AD_TRUE)
    return;

  pthread_mutex_lock(&(destination->mutex));
  pthread_mutex_lock(&(source->mutex));
  if (_empty(destination) == AD_TRUE)
  {
    destination->tail = source->tail;
    source->tail = NULL;
    return;
  }
  head = destination->tail->next;
  destination->tail->next = source->tail->next;
  source->tail->next = head;
  destination->tail = source->tail;
  source->tail = NULL;     /*the source list will now be empty*/
  pthread_mutex_unlock(&(destination->mutex));
  pthread_mutex_unlock(&(source->mutex));
}
void ll_reverse(LINKLIST *list)
{
  LLNODE *p, *q, *r, *head;

  if (_empty(list) == AD_TRUE)
    return;
  pthread_mutex_lock(&(list->mutex));
  q = list->tail;
  p = head = list->tail->next;
  do
  {
    r = q;
    q = p;
    p = q->next;
    q->next = r;
  }
  while (p!=head);
  list->tail = head;
  pthread_mutex_unlock(&(list->mutex));
}
unsigned ll_count(LINKLIST *list)
{
  LLNODE *cursor;
  unsigned count;

  if (_empty(list) == AD_TRUE)
    return 0;

  count = 0;
  cursor = list->tail;

  do
  {
    count++;
    cursor = cursor->next;
  }
  while (cursor != list->tail);

  return count;
}
