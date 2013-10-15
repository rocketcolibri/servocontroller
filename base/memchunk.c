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

/*This file is based on the file named gmem.c, part of GLIB*/


/*-------------------------------------------------------------------*/
/*Modifications by Rex Recio (Mar 23, 2000)
1. Removed the 'tree' of mem_areas, relying only on the list
2. Removed the link list of mem_chunks
3. Remove the option of allocating only and not freeing it
4. Naming conventions, also some functions and macros were dropped
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "AD.h"
#include "GEN.h"


static void *locateMemArea (MEMCHUNK *mem_chunk, void *mem);

typedef struct _GFreeAtom      GFreeAtom;
typedef struct _GMemArea       GMemArea;


struct _GFreeAtom
{
  GFreeAtom *next;
};

struct _GMemArea
{
  GMemArea *next;            /* the next mem area */
  GMemArea *prev;            /* the previous mem area */
  unsigned long index;       /* the current index into the "mem" array */
  unsigned long free;        /* the number of free bytes in this mem area */
  unsigned long allocated;   /* the number of atoms allocated from this area */
  int mark;                  /* is this mem area marked for deletion */
  char *array   ;            /* the mem array from which atoms get allocated*/
};

struct _MEMCHUNK
{
  int num_mem_areas;         /* the number of memory areas */
  int num_marked_areas;      /* the number of areas marked for deletion */
  size_t atom_size;          /* the size of an atom */
  unsigned long  area_size;  /* the size of a memory area */
  GMemArea *mem_area;        /* the current memory area */
  GMemArea *mem_areas;       /* a list of all the mem areas owned by this chunk */
  GMemArea *free_mem_area;   /* the free area...which is about to be destroyed */
  GFreeAtom *free_atoms;     /* the free atoms list */
  pthread_mutex_t mutex;
  /*GTree *mem_tree;*/       /* tree of mem areas sorted by memory address */
};


/*OK for Rex*/
MEMCHUNK* mc_chunk_init (unsigned prealloc_size, size_t atom_size)
{
  MEMCHUNK *mem_chunk;

  mem_chunk = cx_malloc (sizeof (MEMCHUNK) );
  mem_chunk->num_mem_areas = 0;
  mem_chunk->num_marked_areas = 0;
  mem_chunk->mem_area = NULL;
  mem_chunk->free_mem_area = NULL;
  mem_chunk->free_atoms = NULL;
  mem_chunk->mem_areas = NULL;
  mem_chunk->atom_size = atom_size;
  mem_chunk->area_size = prealloc_size * atom_size;

  pthread_mutex_init(&(mem_chunk->mutex), NULL);

  return mem_chunk;
}
MEMCHUNK* mc_init_old (size_t atom_size, unsigned long  area_size)
{
  MEMCHUNK *mem_chunk;

  area_size = (area_size + atom_size - 1) / atom_size;
  area_size *= atom_size;

  mem_chunk = cx_malloc (sizeof (MEMCHUNK) );
  mem_chunk->num_mem_areas = 0;
  mem_chunk->num_marked_areas = 0;
  mem_chunk->mem_area = NULL;
  mem_chunk->free_mem_area = NULL;
  mem_chunk->free_atoms = NULL;
  mem_chunk->mem_areas = NULL;
  mem_chunk->atom_size = atom_size;
  mem_chunk->area_size = area_size;

  pthread_mutex_init(&(mem_chunk->mutex), NULL);

  return mem_chunk;
}

/*this replaces the tree search of the original gmem, Rex Recio*/
void *locateMemArea (MEMCHUNK *mem_chunk, void *mem)
{
	GMemArea *current;

	for (current = mem_chunk->mem_areas; current != NULL; current = current->next)
	{
		if (current->array <= (char *) mem &&
			(char *) mem  < &current->array[current->index] )
				return current;
	}
	return NULL;
}
void mc_free_atom (MEMCHUNK *mem_chunk, void *mem)
{
  GMemArea *temp_area;
  GFreeAtom *free_atom;

      /* Place the memory on the "free_atoms" list
       */
  pthread_mutex_lock(&(mem_chunk->mutex));
  free_atom = (GFreeAtom*) mem;
  free_atom->next = mem_chunk->free_atoms;
  mem_chunk->free_atoms = free_atom;

  temp_area = locateMemArea (mem_chunk, mem);
  temp_area->allocated -= 1;

  if (temp_area->allocated == 0)
  {
	temp_area->mark = 1;
	mem_chunk->num_marked_areas += 1;
  }
  pthread_mutex_unlock(&(mem_chunk->mutex));
}
void *mc_alloc (MEMCHUNK *mem_chunk)
{
  GMemArea *temp_area;
  void *mem;

  pthread_mutex_lock(&(mem_chunk->mutex));
  while (mem_chunk->free_atoms)
  {
      /* Get the first piece of memory on the "free_atoms" list.
       * We can go ahead and destroy the list node we used to keep
       *  track of it with and to update the "free_atoms" list to
       *  point to its next element.
       */
      mem = mem_chunk->free_atoms;
      mem_chunk->free_atoms = mem_chunk->free_atoms->next;

      /* Determine which area this piece of memory is allocated from */
	  temp_area = locateMemArea (mem_chunk, mem);

      /* If the area has been marked, then it is being destroyed.
       *  (ie marked to be destroyed).
       * We check to see if all of the segments on the free list that
       *  reference this area have been removed. This occurs when
       *  the ammount of free memory is less than the allocatable size.
       * If the chunk should be freed, then we place it in the "free_mem_area".
       * This is so we make sure not to free the mem area here and then
       *  allocate it again a few lines down.
       * If we don't allocate a chunk a few lines down then the "free_mem_area"
       *  will be freed.
       * If there is already a "free_mem_area" then we'll just free this mem area.
       */
      if (temp_area->mark)
      {
          /* Update the "free" memory available in that area */
          temp_area->free += mem_chunk->atom_size;
          if (temp_area->free == mem_chunk->area_size)
		  {
			  if (temp_area == mem_chunk->mem_area)
				  mem_chunk->mem_area = NULL;

              if (mem_chunk->free_mem_area)
              {
                  mem_chunk->num_mem_areas -= 1;

                  if (temp_area->next)
                    temp_area->next->prev = temp_area->prev;
                  if (temp_area->prev)
                    temp_area->prev->next = temp_area->next;
                  if (temp_area == mem_chunk->mem_areas)
                    mem_chunk->mem_areas = mem_chunk->mem_areas->next;

                  free (temp_area->array);
                  free (temp_area);

              }
              else
				  mem_chunk->free_mem_area = temp_area;

			  mem_chunk->num_marked_areas -= 1;
		  }
	  }
      else
	  {
          /* Update the number of allocated atoms count.
	   */
          temp_area->allocated += 1;

          /* The area wasn't marked...return the memory
	   */
		  goto outa_here;
	  }
  }

  /* If there isn't a current mem area or the current mem area is out of space
   *  then allocate a new mem area. We'll first check and see if we can use
   *  the "free_mem_area". Otherwise we'll just malloc the mem area.
   */
  if ((!mem_chunk->mem_area) ||
      ((mem_chunk->mem_area->index + mem_chunk->atom_size) > mem_chunk->area_size))
  {
	  if (mem_chunk->free_mem_area)
      {
		  mem_chunk->mem_area = mem_chunk->free_mem_area;
		  mem_chunk->free_mem_area = NULL;
      }
      else
      {
		  mem_chunk->mem_area =    (GMemArea*) cx_malloc (sizeof (GMemArea) );
		  mem_chunk->mem_area->array = cx_malloc (mem_chunk->area_size);

		  mem_chunk->num_mem_areas += 1;
		  mem_chunk->mem_area->next = mem_chunk->mem_areas;
		  mem_chunk->mem_area->prev = NULL;

		  if (mem_chunk->mem_areas)
			  mem_chunk->mem_areas->prev = mem_chunk->mem_area;
		  mem_chunk->mem_areas = mem_chunk->mem_area;

      }
      mem_chunk->mem_area->index = 0;
      mem_chunk->mem_area->free = mem_chunk->area_size;
      mem_chunk->mem_area->allocated = 0;
      mem_chunk->mem_area->mark = 0;
  }

  /* Get the memory and modify the state variables appropriately.
   */
  mem =  &mem_chunk->mem_area->array[mem_chunk->mem_area->index];
  mem_chunk->mem_area->index += mem_chunk->atom_size;
  mem_chunk->mem_area->free -= mem_chunk->atom_size;
  mem_chunk->mem_area->allocated += 1;

outa_here:

  pthread_mutex_unlock(&(mem_chunk->mutex));

  return mem;
}
void mc_free (MEMCHUNK *mem_chunk)
{
  GMemArea *mem_areas;
  GMemArea *temp_area;

  pthread_mutex_lock(&(mem_chunk->mutex));
  mem_areas = mem_chunk->mem_areas;
  while (mem_areas)
  {
      temp_area = mem_areas;
      mem_areas = mem_areas->next;
      free (temp_area->array);
      free (temp_area);
  }
  free (mem_chunk);
  mem_chunk = NULL;
  pthread_mutex_unlock(&(mem_chunk->mutex));
}
/* This doesn't free the free_area if there is one */
void mc_clean (MEMCHUNK *mem_chunk)
{
  GMemArea *mem_area;
  GFreeAtom *prev_free_atom;
  GFreeAtom *temp_free_atom;
  void *mem;

  pthread_mutex_lock(&(mem_chunk->mutex));
  prev_free_atom = NULL;
  temp_free_atom = mem_chunk->free_atoms;

  while (temp_free_atom)
  {
	  mem = temp_free_atom;
	  mem_area = locateMemArea (mem_chunk, mem);

          /* If this mem area is marked for destruction then delete the
	   *  area and list node and decrement the free mem.
           */
	  if (mem_area->mark)
	  {
		  if (prev_free_atom)
			  prev_free_atom->next = temp_free_atom->next;
	      else
			  mem_chunk->free_atoms = temp_free_atom->next;
	      temp_free_atom = temp_free_atom->next;

	      mem_area->free += mem_chunk->atom_size;
	      if (mem_area->free == mem_chunk->area_size)
		  {
			  mem_chunk->num_mem_areas -= 1;
			  mem_chunk->num_marked_areas -= 1;

			  if (mem_area->next)
				  mem_area->next->prev = mem_area->prev;
			  if (mem_area->prev)
				  mem_area->prev->next = mem_area->next;
			  if (mem_area == mem_chunk->mem_areas)
				  mem_chunk->mem_areas = mem_chunk->mem_areas->next;
			  if (mem_area == mem_chunk->mem_area)
				  mem_chunk->mem_area = NULL;
			  free (mem_area->array);
			  free (mem_area);
		  }
	  }
	  else
	  {
	      prev_free_atom = temp_free_atom;
	      temp_free_atom = temp_free_atom->next;
	  }
  }
  pthread_mutex_unlock(&(mem_chunk->mutex));
}

/*Extension of malloc*/
void *cx_malloc (size_t size)
{
	void *result;

	result = malloc (size);
	if (!result)
	{
		perror ("No more memory\n.");
		exit(-1);
	}
	return result;
}
/*extension of calloc*/
void *cx_calloc (size_t num, size_t size)
{
	void *result;

	result = calloc (num, size);
	if (!result)
	{
		perror ("No more memory\n.");
		exit(-1);
	}
	return result;
}
void *cx_realloc(void *pointer, size_t size)
{
  pointer = realloc(pointer, size);
  if (!pointer)
  {
    perror("No more memory\n.");
    exit(-1);
  }
  return pointer;
}
/*extension of free*/
void cx_free (void *mem)
{
	free (mem);
	mem = NULL;
}
