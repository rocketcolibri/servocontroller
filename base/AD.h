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

#ifndef ADLIB_H
#define ADLIB_H

#include <stdio.h>


#define BLOCKSIZE   64                 /*number of atoms in each memory block*/
                                       /*used in MEMCHUNKS structure*/
#define HEAPINCREMENT 1024             /*bulk size of the heap*/
#define MC_CHUNK_DEFAULT_SIZE 128

/*Enumerated data types*/
typedef enum {AD_ERROR, AD_OK}   AD_STATUS;
typedef enum {AD_FALSE, AD_TRUE} AD_BOOLEAN;
typedef enum {AD_EQUAL, AD_LESS_THAN=-1, AD_GREATER_THAN=1} AD_RELATION;
typedef enum {AD_PRE_ORDER, AD_IN_ORDER, AD_POST_ORDER, AD_LEVEL_ORDER}  AD_TRAVERSE_ORDER;

/*Data Structures*/
typedef struct _CLINKLIST CLINKLIST;   /*circular linklist*/
typedef struct _LINKLIST LINKLIST;     /*linklist (circular singly-linklist)*/
typedef LINKLIST DEQUE;                /*deque*/
typedef DEQUE STACK;                   /*stack*/
typedef DEQUE QUEUE;                   /*queue*/
typedef struct _DLNODE DLNODE;         /*circular doubly linklist node*/
typedef struct _BTNODE BTNODE;         /*binary tree node*/
typedef struct _BTREE BTREE;           /*binary Search Tree*/
typedef struct _NTNODE  NTNODE;        /*node of an N-ary tree*/
typedef struct _NTREE NTREE;           /*N-ary Tree*/
typedef struct _HASHTABLE HASHTABLE;   /*Hash table*/
typedef struct _MEMCHUNK  MEMCHUNK;    /*Memory Chunks*/
typedef struct _HEAP HEAP;             /*Heap*/
typedef HEAP PQUEUE;                   /*Priority Queue*/


typedef DLNODE DLINKLIST;              /*circular doubly linklist*/

/*Pointers to function*/
typedef AD_BOOLEAN  (AD_EQUAL_FUNC)	    (void *, void *);
typedef AD_RELATION	(AD_COMPARE_FUNC)	    (void *, void *);
typedef unsigned long (AD_HASH_FUNC) (const void *);
/*typedef void (AD_FREE_FUNC)             (void *);*/
typedef AD_BOOLEAN (AD_TRAV_FUNC) (void *, void *);

/*Not so opaque data structures*/
struct _DLNODE
{
        struct _DLNODE *prev;    /*pointer to the previous node*/
        struct _DLNODE *next;    /*pointer to the next node*/
        void *data;              /*pointer to the data stored in the node*/
};


#define NTNODE  BTNODE


/*Circular linked list function prototypes*/
extern CLINKLIST *cl_init (void);
extern CLINKLIST *cl_prepend (CLINKLIST *list, void *newdata);
extern CLINKLIST *cl_append (CLINKLIST *list,void *newdata);
extern void *cl_delete (CLINKLIST *list, void *olddata, AD_EQUAL_FUNC *match);
extern void *cl_find (CLINKLIST *list, void *lookfor, AD_EQUAL_FUNC *match);
extern void *cl_find_adj (CLINKLIST *list, void *lookfor, AD_EQUAL_FUNC *match);
extern AD_BOOLEAN cl_empty (CLINKLIST *list);
AD_BOOLEAN cl_traverse (CLINKLIST *list, void *data, AD_TRAV_FUNC *func);
extern void cl_free (CLINKLIST *list);
extern void cl_merge(CLINKLIST *destination, CLINKLIST *source);
extern void cl_reverse(CLINKLIST *list);
#define cl_insert                             cl_prepend


/*Linklist Function Prototypes*/
extern LINKLIST *ll_init(void);
extern AD_BOOLEAN ll_empty (LINKLIST *list);
extern LINKLIST *ll_prepend (LINKLIST *list,void *newdata);
extern LINKLIST *ll_append (LINKLIST *list,void *newdata);
extern void *ll_delete (LINKLIST *list, void *key, AD_COMPARE_FUNC *match);
extern void *ll_find(LINKLIST *list, void *key, AD_COMPARE_FUNC *match);
extern void *ll_find_adj (LINKLIST *list, void *key, AD_COMPARE_FUNC *match);
extern AD_BOOLEAN ll_traverse (LINKLIST *list, AD_TRAV_FUNC  *travFunc, void *param);
extern void ll_free(LINKLIST *list);
extern void ll_merge(LINKLIST *destination, LINKLIST *source);
extern void ll_reverse(LINKLIST *list);
extern unsigned ll_count(LINKLIST *list);
#define ll_insert ll_prepend
#define ll_add    ll_append

/*Deque*/
#define dq_init                  ll_init
#define dq_empty                 ll_empty
#define dq_push                  ll_prepend
#define dq_append                ll_append
#define dq_free                  ll_free
extern void *dq_pop (DEQUE *dq);
extern void *dq_eject (DEQUE *dq);
extern void *dq_top (DEQUE *dq);
extern void *dq_bottom (DEQUE *dq);

/*Stack*/
#define s_init			 dq_init
#define s_empty                  dq_empty
#define s_push			 dq_push
#define s_add                    dq_push
#define s_free                   dq_free
#define s_top			 dq_top
#define s_pop                    dq_pop

/*Queue*/
#define q_init		         dq_init
#define q_empty                  dq_empty
#define q_add                    dq_append
#define q_append                 dq_append
#define q_remove                 dq_pop
#define q_delete                 dq_pop
#define q_free                   dq_free

/*Binary Search Tree function prototypes*/
extern BTREE *bt_init (AD_COMPARE_FUNC *compFunc);
extern BTREE *bt_add (BTREE *bt, void *newitem);
extern void *bt_find (BTREE *bt, void *key);
extern void bt_traverse (BTREE *bt, AD_TRAVERSE_ORDER order,
		AD_TRAV_FUNC *travFunc, void *param);
extern AD_BOOLEAN bt_empty (BTREE *bt);
extern unsigned bt_count(BTREE *bt);
extern unsigned bt_height(BTREE *bt);
void *bt_delete (BTREE *bt, void *key);
extern void bt_free(BTREE *bt);
#define bt_left(node)    ((node)->left)
#define bt_right(node)   ((node)->right)
#define bt_data(node)   ((node)->data)

/*N-ary tree function prototypes*/
#define nt_fchild        bt_left
#define nt_sibling       bt_right
#define nt_data          bt_data
AD_BOOLEAN nt_is_leaf (NTNODE *node);
NTNODE *nt_new_node (void *newdata);
NTNODE *nt_append_child (NTNODE *parent, NTNODE *newnode);
void nt_traverse (NTNODE *parent, AD_TRAVERSE_ORDER order,
				  AD_TRAV_FUNC *travFunc, void *param);

/*Hash Table*/
extern HASHTABLE *ht_init (unsigned tablesize, AD_HASH_FUNC *hFunc,
						   AD_COMPARE_FUNC *mFunc);
extern HASHTABLE *ht_insert (HASHTABLE *ht, void *newdata);
extern void *ht_delete (HASHTABLE *ht, void *olddata);
extern void *ht_find (HASHTABLE *ht, void *lookfor);
extern void ht_free (HASHTABLE *ht);

/*Utility functions*/
extern void *cx_malloc (size_t size);
extern void *cx_calloc (size_t num, size_t size);
void *cx_realloc(void *pointer, size_t size);
void cx_free (void *mem);

/*Memory Chunks*/
extern MEMCHUNK* mc_init_old (size_t atom_size, unsigned long  area_size);
extern MEMCHUNK *mc_chunk_init(unsigned prealloc_size, size_t atom_size);
extern void mc_free_atom (MEMCHUNK *mem_chunk, void *mem);
extern void *mc_alloc (MEMCHUNK *mem_chunk);
extern void mc_free (MEMCHUNK *mem_chunk);
extern void mc_clean (MEMCHUNK *mem_chunk);
#define mc_init(size) mc_chunk_init(MC_CHUNK_DEFAULT_SIZE, size)

/*Doubly linklist*/
#define dl_next(node) ((node)->next)
#define dl_prev(node) ((node)->prev)
#define dl_data(node) ((node)->data)
#define dl_empty(dlist) (dlist ? AD_FALSE : AD_TRUE)
extern DLNODE *dl_new_node (void *newdata);
extern void dl_free_node (DLNODE *node);
extern DLINKLIST *dl_init (void);
extern DLINKLIST *dl_prepend (DLINKLIST *dlist, void *newdata);
extern DLINKLIST *dl_append (DLINKLIST *dlist,void *newdata);
extern DLINKLIST *dl_delete (DLINKLIST *dlist, void *olddata);
extern DLNODE *dl_find (DLINKLIST *dlist, void *lookfor, AD_EQUAL_FUNC *match);
extern void dl_traverse (DLINKLIST *dlist, void *param, AD_TRAV_FUNC *travFunc);

/*Heap*/
extern HEAP *hp_init(AD_COMPARE_FUNC *compFunc);
extern AD_BOOLEAN hp_empty(HEAP *hp);
extern HEAP *hp_add(HEAP *hp, void *newdata);
extern void *hp_delete(HEAP *hp, unsigned element);
extern void hp_free(HEAP *hp);

/*Priority Queue*/
#define pq_init       hp_init
#define pq_empty      hp_empty
#define pq_add        hp_add
#define pq_free       hp_free
#define pq_delete(hp) hp_delete(hp, 0)



/*----------------------------------------------------------------------*
 * AVL Tree                 *
 *----------------------------------------------------------------------*/
typedef unsigned int  DSKEY;
typedef int   (*DSKEYCMPFUN)(const DSKEY,const DSKEY);


# define AVLTreeNoCopyKeys      0
# define AVLTreeCopyKeys        1
# define AVLTreeNoKeySize       0

# define AVLTreeNoCompFun       ((void*)0)
# define AVLTreeNullData        ((void*)0)

typedef DSKEYCMPFUN   AVLKEYCOMPFUN;

typedef struct avltree  AvlTree;
typedef AvlTree*  AVLTREE;
typedef struct avlnode  AvlNode;
typedef AvlNode*  AVLNODE;


/* Default tree making routine for nodes with string keys */
# define avlNewTreeStrKeys()  \
  avlNewTree((AVLKEYCOMPFUN)strcmp,AVLTreeCopyKeys,AVLTreeNoKeySize)
# define avlMake()    avlNewTree((void*)0,1,0)
# define avlAdd(t,k,d)    avlInsert((t),(const DSKEY)(k),(void*)(d))
# define avlRemove(t,k)   avlRemoveByKey((t),(k))
# define avlFirstNode(tr) avlMinimumNode((tr))
# define avlLastNode(tr)  avlMaximumNode((tr))
# define avlFirst(tr)   avlMinimum((tr))
# define avlLast(tr)    avlMaximum((tr))
# define avlSize(tr)    avlTotalNodes((tr))

# define AVLPreWalk   0
# define AVLInWalk    1
# define AVLPostWalk    2




extern DSKEY avlNodeKey(AVLNODE);
extern char * avlNodeKeyAsString(void *);
extern AVLTREE avlNewTree(int(*)(DSKEY,DSKEY),int,int);
extern void avlClose(AVLTREE);
extern void avlCloseWithFunction(AVLTREE,void(*)(void *));
extern void avlWalk(AVLTREE,void(*)(void *),int);
extern void avlWalkAscending(AVLTREE,void(*)(void *));
extern void avlWalkDescending(AVLTREE,void(*)(void *));
extern int avlHeight(AVLTREE);
extern int avlInsert(AVLTREE,const DSKEY,void *);
extern AVLNODE avlFindNode(AVLTREE,DSKEY);
extern void * avlFind(AVLTREE,DSKEY);
extern AVLNODE avlMinimumNode(AVLTREE);
extern void * avlMinimum(AVLTREE);
extern AVLNODE avlMaximumNode(AVLTREE);
extern void * avlMaximum(AVLTREE);
extern AVLNODE avlNextNode(AVLTREE,AVLNODE);
extern AVLNODE avlNextNodeByKey(AVLTREE,DSKEY);
extern AVLNODE avlPrevNode(AVLTREE,AVLNODE);
extern AVLNODE avlPrevNodeByKey(AVLTREE,DSKEY);
extern int avlGetData(AVLNODE,void **);
extern void * avlNodeData(AVLNODE);
extern void * avlNodeUpdateData(AVLNODE,void *);
extern void * avlUpdateData(AVLTREE,DSKEY,void *);
extern void * avlRemoveByKey(AVLTREE,DSKEY);
extern int avlRemoveNode(AVLTREE,AVLNODE);
extern int avlSetCurrent(AVLTREE,DSKEY);
extern int avlClearCurrent(AVLTREE);
extern void * avlCurrent(AVLTREE);
extern void * avlPrev(AVLTREE);
extern void * avlNext(AVLTREE);
extern void * avlCut(AVLTREE);
extern void avlFreeNode(void *);
extern int avlTotalNodes(AVLTREE);
extern AVLNODE avlRootNode(AVLTREE);
extern AVLNODE avlLeftNode(AVLNODE);
extern AVLNODE avlRightNode(AVLNODE);
extern int avlNodeHeight(AVLNODE);
extern int avlCheck(AVLTREE);
extern DSKEY avlGetNodeKey(AVLNODE);
extern void * avlGetNodeData(AVLNODE);

#endif /*endif for conditional include for this header file*/
