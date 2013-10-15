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
BINARY SEARCH TREE

Author: Rex Recio
Date last modified: June 2, 2004
Description: Implementation of binary search tree with root insertion.
--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "AD.h"

#define _bt_empty(bt) ((bt)->root == NULL ? AD_TRUE : AD_FALSE)

struct _BTNODE
{
        struct _BTNODE *left, *right;
        void *data;
};
struct _BTREE
{
	BTNODE *root;                  /*root node of the binary tree*/
	AD_COMPARE_FUNC *compFunc;     /*pointer to comparison function*/
        pthread_mutex_t mutex;
};

static MEMCHUNK *memChunk=NULL; /*memory chunk of nodes*/

static BTNODE *_new_node (void *newdata);
static BTNODE *_add (BTNODE *currentNode, void *newdata,
		AD_COMPARE_FUNC *compFunc);
static void _traverse (BTNODE *currentNode,
		AD_TRAVERSE_ORDER traverseType, AD_TRAV_FUNC *travFunc, void *param);
static BTNODE *_retrieve (BTNODE *currentNode,
		void *key, AD_COMPARE_FUNC *compFunc);
static BTNODE *_delete (BTNODE *currentNode,
                void *key, AD_COMPARE_FUNC *compFunc, void **data);
static void _free_nodes(BTNODE *node);
static void _free_single_node(BTNODE *node);
static BTNODE *_rotate_right (BTNODE *node);
static BTNODE *_rotate_left (BTNODE *node);
static unsigned _count(BTNODE *node);
static unsigned _height(BTNODE *node);
static BTNODE *_delete_min(BTNODE *node);


BTREE *bt_init (AD_COMPARE_FUNC *compare)
{
	BTREE *bt;

	bt = cx_malloc (sizeof(BTREE) );
	bt->root = NULL;
	bt->compFunc = compare;
        pthread_mutex_init(&(bt->mutex), NULL);

	return bt;
}
static BTNODE *_new_node (void *newdata)
{
	BTNODE *node;

	if (!memChunk)
		memChunk = mc_init (sizeof (BTNODE));
	node = mc_alloc (memChunk);
	node->data = newdata;
	node->left = node->right = NULL;

	return node;
}
static void _free_single_node(BTNODE *node)
{
  mc_free_atom(memChunk, node);
}

BTREE *bt_add (BTREE *bt, void *newdata)
{
  pthread_mutex_lock(&(bt->mutex));

  if (_bt_empty (bt) == AD_TRUE)
    bt->root = _new_node(newdata);
  else
    bt->root= _add (bt->root, newdata, bt->compFunc);

  pthread_mutex_unlock(&(bt->mutex));

  return bt;
}

/*uses root insertion with rotation*/
static BTNODE *_add (BTNODE *currentNode, void *newdata,
		AD_COMPARE_FUNC *compFunc)
{
	BTNODE *newnode;
	AD_RELATION rel;

	if (currentNode == NULL)
	{
		newnode = _new_node (newdata);
		currentNode = newnode;

		return currentNode;
	}
	rel = compFunc(newdata, currentNode->data);

	if (rel == AD_LESS_THAN)
	{
		currentNode->left = _add (currentNode->left,
			newdata, compFunc);
		currentNode = _rotate_right (currentNode);
	}
	else
	{
		currentNode->right = _add (currentNode->right,
			newdata, compFunc);
		currentNode = _rotate_left (currentNode);
	}

	return currentNode;  /*Don't forget this*/
}
AD_BOOLEAN bt_empty (BTREE *bt)
{
	return _bt_empty(bt);
}

void bt_traverse (BTREE *bt, AD_TRAVERSE_ORDER order,
				  AD_TRAV_FUNC *travFunc, void *param)
{
  if (order == AD_LEVEL_ORDER)
  {
    QUEUE *q;
    BTNODE *node;

    q = q_init();
    q_add (q, bt->root);
    while ( (node = q_remove (q)) )
    {
      if ( travFunc(node->data, param) == AD_FALSE)
	return;
      if (node->left)
	q_add (q, node->left);
      if (node->right)
	q_add (q, node->right);
    }
    q_free (q);
  }
  else
    _traverse (bt->root, order, travFunc, param);
}
static void _traverse (BTNODE *currentNode,
	AD_TRAVERSE_ORDER order, AD_TRAV_FUNC *travFunc, void *param)
{

	if (!currentNode)  return;

	switch (order)
	{
		case AD_PRE_ORDER:
			travFunc(currentNode->data, param);
			_traverse (currentNode->left,
				order, travFunc, param);
			_traverse (currentNode->right,
				order, travFunc, param);
			break;
		case AD_IN_ORDER:
			_traverse (currentNode->left,
				order, travFunc, param);
			travFunc(currentNode->data, param);
			_traverse (currentNode->right,
				order, travFunc, param);
			break;
		case AD_POST_ORDER:
			_traverse (currentNode->left,
				order, travFunc, param);
			_traverse (currentNode->right,
				order, travFunc, param);
			travFunc (currentNode->data, param);
			break;
		default:
		break;
	}
}
void *bt_find (BTREE *bt, void *key)
{
	BTNODE *node;

	node = _retrieve (bt->root, key, bt->compFunc);
	if (!node)
		return NULL;
	else
		return node->data;
}
static BTNODE *_retrieve (BTNODE *currentNode,
		void *key, AD_COMPARE_FUNC *compFunc)
{
  AD_RELATION relation;

  if (currentNode == NULL)
	return NULL;       /*not found*/

  relation = compFunc(key, currentNode->data);

  if (relation == AD_LESS_THAN)
    return _retrieve (currentNode->left, key, compFunc); /*go left*/
  else if (relation == AD_GREATER_THAN)
    return _retrieve (currentNode->right, key, compFunc); /*go right*/
  else
    return currentNode;  /*you found it*/
}
/*static BTNODE *_retrieveBak (BTNODE *currentNode,
                void *key, AD_COMPARE_FUNC *compFunc)
{
        if (currentNode == NULL)
                return NULL;


        else if (compFunc(key, currentNode->data) == AD_LESS_THAN)
                return _retrieve (currentNode->left, key, compFunc);
        else if (compFunc(key, currentNode->data) == AD_GREATER_THAN)
                return _retrieve (currentNode->right, key, compFunc);
        else
                return currentNode;
}*/
static BTNODE *_rotate_right (BTNODE *node)
{
	/*Make the old parent the right child of the new parent*/
	/*The new parent is the old left child of the old parent*/
	BTNODE *temp;

	temp = node->left;
	node->left = temp->right;
	temp->right = node;

	return temp;
}
static BTNODE *_rotate_left (BTNODE *node)
{
	/*The new parent is the old right child of the old parent*/
	/*Make the old parent the left child of the old parent*/
	BTNODE *temp;

	temp = node->right;
	node->right = temp->left;
	temp->left = node;

	return temp;
}
unsigned bt_count(BTREE *bt)
{
  return _count(bt->root);
}
static unsigned _count(BTNODE *node)
{
  if (node == NULL)
    return 0;
  return _count(node->left) + _count(node->right) + 1;
}
unsigned bt_height(BTREE *bt)
{
  return  _height(bt->root);
}
static unsigned _height(BTNODE *node)
{
  int leftHeight, rightHeight;

  if (node == NULL)
    return 0;
  leftHeight = _height(node->left);
  rightHeight = _height(node->right);

  return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}
void bt_free(BTREE *bt)
{
  pthread_mutex_lock(&(bt->mutex));

  _free_nodes(bt->root);
  pthread_mutex_unlock(&(bt->mutex));
  pthread_mutex_destroy(&(bt->mutex));
  cx_free(bt);
}
static void _free_nodes(BTNODE *node)
{
  if (node)
  {
    _free_nodes(node->left);
    _free_nodes(node->right);

    mc_free_atom(memChunk, node);
  }
}
#if 0
static BTNODE *_find_min(BTNODE *node)
{
  if (node == NULL)
    return NULL;
  else if (node->left == NULL)
    return node;
  else
    return (_find_min(node->left));
}
#endif
/*Delete the smallest node in this subtree. an exemption is when the minimum is
at the top, i,e., the input parameter 'node' in which case we return the node
without unlinking it*/

static BTNODE *_delete_min(BTNODE *node)
{
  /*not yet tested*/
  BTNODE *parent;

  if (node == NULL)
    return NULL;   /*node not found*/

  parent = NULL;
  while (node->left)
  {
    parent = node;
    node = node->left;
  }
  if (parent == NULL)
    return node; /*only one node, therefore it is the 'min'*/
  else
  {
    parent->left = node->right;  /*disconnect the minimum node*/
    return node; /* the calling function should be the one to free this node*/

  }
}


void *bt_delete (BTREE *bt, void *key)
{
  BTNODE *node;
  void *data;

  pthread_mutex_lock(&(bt->mutex));

  data = NULL;
  node = _delete(bt->root, key, bt->compFunc, &data);

  /*if there's only 1 node and it is the one to be deleted*/
  if (bt->root->left == NULL && bt->root->right == NULL && data !=NULL)
    bt->root = NULL;
  /*data = node->data;*/
  _free_single_node(node);

  pthread_mutex_unlock(&(bt->mutex));

  return data;

}

static BTNODE *_delete (BTNODE *currentNode,
                void *key, AD_COMPARE_FUNC *compFunc, void **data)
{
  BTNODE *tempNode, *child;
  AD_RELATION rel;

  if (currentNode == NULL)
  {
    *data = NULL;
    return NULL;   /*node not found*/
  }
  rel = compFunc(key, currentNode->data);
  if (rel == AD_LESS_THAN)
    currentNode->left = _delete(currentNode->left, key, compFunc, data);
  else if (rel == AD_GREATER_THAN)
    currentNode->right = _delete(currentNode->right, key, compFunc, data);

  else if (currentNode->right && currentNode->left)
  {
    /*two children*/
    *data = currentNode->data;

    if (currentNode->right->left == NULL) /*only one node in the right subtree*/
    {
      /*swap the contents of the current node and right child*/
      tempNode = currentNode->right;
      currentNode->data = tempNode->data;
      currentNode->right = currentNode->right->right; /*Unlink*/
    }
    else
    {
      tempNode = _delete_min(currentNode->right);
      currentNode->data = tempNode->data;
    }
    _free_single_node(tempNode);
  }
  else
  {
    /*zero or one child*/
    *data = currentNode->data;
    tempNode = currentNode;
    if (currentNode->left == NULL)
      child = currentNode->right;
    if (currentNode->right == NULL)
      child = currentNode->left;

    /*free the temp node*/
    _free_single_node(tempNode);

    return child;
  }
  return currentNode;
}
