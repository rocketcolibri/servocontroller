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
DOUBLY LINKLIST

Author: Rex Recio
Date last modified: Mar. 23, 2000
Description: Implementation of a doubly linklist data structure. It is
    also  circular.
--------------------------------------------------------------------*/

#include "AD.h"



static MEMCHUNK *memChunk=NULL;  /*memory chunks of nodes*/

DLNODE *dl_new_node (void *newdata)
{
	DLNODE *node;

	if (!memChunk)
		memChunk = mc_init (sizeof (DLNODE));
	node = mc_alloc (memChunk);
	node->data = newdata;

	return node;
}
void dl_free_node (DLNODE *node)
{
	mc_free_atom (memChunk, node);
}
DLINKLIST *dl_init (void)
{
	return NULL;
}
DLINKLIST *dl_prepend (DLINKLIST *dlist, void *newdata)
{
	DLNODE *newnode;

	newnode = dl_new_node (newdata);

	/*non-empty list*/
	if (dl_empty (dlist) == AD_FALSE)
	{
		newnode->next = dlist;
		newnode->prev = dlist->prev;
		dlist->prev->next = newnode;
		dlist->prev = newnode;

		return dlist;
	}
	/*empty list*/
	else
	{
		newnode->next = newnode->prev = newnode;
		return newnode;
	}

}
DLINKLIST *dl_append (DLINKLIST *dlist,void *newdata)
{
	DLINKLIST *newlist;

	newlist = dl_prepend (dlist, newdata);
	newlist = newlist->prev;

	return newlist;
}
DLINKLIST *dl_delete (DLINKLIST *dlist, void *olddata)
{
	DLNODE *temp;

	temp = dlist;
	do
	{
		if (temp->data == olddata)
		{
			if (temp->prev)
				temp->prev->next = temp->next;
			if (temp->next)
				temp->next->prev = temp->prev;
			dl_free_node (temp);

			return dlist;
		}
		temp = temp->next;
	}
	while (temp != dlist);

	return NULL; /*not found*/

}
DLNODE *dl_find (DLINKLIST *dlist, void *lookfor, AD_EQUAL_FUNC *match)
{
	DLNODE *temp;

	if (dl_empty (dlist) == AD_TRUE )
		return NULL; /*empty list*/

	temp = dlist;
	do
	{
		if (match ? (*match) (temp->data, lookfor) : temp->data == lookfor )
			return temp;

		temp = temp->next;
	}
	while (temp != dlist);

	return NULL; /*not found*/
}

void dl_traverse (DLINKLIST *dlist, void *param, AD_TRAV_FUNC *travFunc)
{
	DLNODE *temp;

	if (dl_empty (dlist) == AD_FALSE)
	{
		temp = dlist;
		do
		{
			if ((*travFunc)(temp->data, param) == AD_FALSE)
				return;
			temp = temp->next;
		}
		while (temp != dlist);
	}
	else
		return;
}
void dl_free (DLINKLIST *dlist)
{
	DLNODE *temp;

	/*remove the circular nature*/
	if (dl_empty (dlist) == AD_FALSE)
	{
		dlist->prev->next = NULL;

		while (dlist)
		{
			temp = dlist;
			dlist = dlist->next;
			mc_free_atom (memChunk, temp);
		}
	}
}
