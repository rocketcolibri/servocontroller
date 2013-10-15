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

/*--------------------------------------------------------------------
HEAP

Author: Rex Recio
Date last modified: Not yet implemented
Description: Heap data structure implementation.  A heap is an ordered,
     balanced binary tree in which the value of the node at the root of
     any subtree is greater than or equal to the value of either of its
     children
--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "AD.h"

struct _HEAP
{
    void **array;
    unsigned nextelement;
    unsigned heapsize;
    AD_COMPARE_FUNC *compFunc;
    pthread_mutex_t mutex;
};

static void _siftup(HEAP *hp, unsigned element);
static void _siftdown(HEAP *hp, unsigned parent);

HEAP *hp_init(AD_COMPARE_FUNC *compFunc)
{
  HEAP *hp;

  hp = cx_malloc(sizeof(HEAP));
  hp->array = cx_malloc (HEAPINCREMENT * sizeof (void *));
  hp->nextelement = 0;
  hp->heapsize = HEAPINCREMENT;
  hp->compFunc = compFunc;
  pthread_mutex_init(&(hp->mutex), NULL);

  return hp;
}
AD_BOOLEAN hp_empty(HEAP *hp)
{
  return hp->nextelement ? AD_TRUE : AD_FALSE;
}
HEAP *hp_add(HEAP *hp, void *newdata)
{
  pthread_mutex_lock(&(hp->mutex));

  if (hp->nextelement == hp->heapsize)
  {
    hp->array = cx_realloc (hp->array, (hp->heapsize + HEAPINCREMENT) * sizeof (void *));
    hp->heapsize += HEAPINCREMENT;
  }
  hp->array[hp->nextelement] = newdata;
  _siftup(hp, hp->nextelement);
  hp->nextelement++;

  pthread_mutex_unlock(&(hp->mutex));

  return hp;
}
static void _siftup(HEAP *hp, unsigned element)
{
  unsigned parent;
  void *temp;
  AD_RELATION relation;

  if (element == 0)
    return;
  parent = (element - 1) / 2;
  relation = hp->compFunc(hp->array[element], hp->array[parent]);
  if (relation == AD_LESS_THAN || relation == AD_EQUAL)
    return;
  temp = hp->array[element];
  hp->array[element] = hp->array[parent];
  hp->array[parent] = temp;

  _siftup(hp, parent);
}
void *hp_delete(HEAP *hp, unsigned element)
{
  void *deletedData;

  if (element >= hp->nextelement)
    return NULL;

  pthread_mutex_lock(&(hp->mutex));

  deletedData = hp->array[element];
  hp->nextelement--;
  if (element != hp->nextelement)
  {
    hp->array[element] = hp->array[hp->nextelement];
    _siftdown(hp, element);
  }
  pthread_mutex_unlock(&(hp->mutex));

  return deletedData;
}
static void _siftdown(HEAP *hp, unsigned parent)
{
  unsigned leftChild, rightChild, swapElement;
  AD_RELATION leftComp, rightComp, childrenComp;
  void *temp;

  leftChild  = 2 * parent + 1;
  rightChild = leftChild + 1;

  /*if no children*/
  if (leftChild >= hp->nextelement)
    return;

  leftComp = hp->compFunc(hp->array[parent], hp->array[leftChild]);
 /*if no right child*/
  if (rightChild >= hp->nextelement)
  {
    if (leftComp == AD_LESS_THAN)
    {
      temp = hp->array[parent];
      hp->array[parent] = hp->array[leftChild];
      hp->array[leftChild] = temp;
    }
    return;
  }

  rightComp = hp->compFunc(hp->array[parent], hp->array[rightChild]);
  if (leftComp == AD_LESS_THAN || rightComp == AD_LESS_THAN)
  {
    /*Two children, swap the parent with the bigger child*/
    childrenComp = hp->compFunc(hp->array[leftChild], hp->array[rightChild]);
    swapElement = childrenComp == AD_GREATER_THAN ? leftChild : rightChild;
    temp = hp->array[parent];
    hp->array[parent] = hp->array[swapElement];
    hp->array[swapElement] = temp;

    _siftdown(hp, swapElement);
  }
}
void hp_free(HEAP *hp)
{
  pthread_mutex_lock(&(hp->mutex));

  free(hp->array);
  pthread_mutex_unlock(&(hp->mutex));
  pthread_mutex_destroy(&(hp->mutex));

  free(hp);
}
