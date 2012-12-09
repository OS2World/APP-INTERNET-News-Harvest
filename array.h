
/***********************************************************************

array.h,v 1.2 2003/12/16 03:45:19 root Exp

array template classes

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2003 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	18 Nov 03 SHL - Avoid immediate death on new() failures
		18 Nov 03 SHL - add is_dead()

***********************************************************************/

#ifndef __ARRAY_H__
#define __ARRAY_H__

#ifndef UNIX
#include <new.h>
#endif

#include <string.h>
#include <stdlib.h>

static int (*theCompare) (const void *one, const void *two);

#ifdef OS2
extern int _Optlink arrayQSortCompare(const void *one, const void *two);
#else
extern int arrayQSortCompare(const void *one, const void *two);
#endif

#define ARRAY_BASE_SIZE 4

template < class T > class array
{
  public:
    array(void)
    {
	dead = 0;
	deadT = new T;
	the_array = (T **) new T *[ARRAY_BASE_SIZE];

	if (the_array)
	{
	    array_size = ARRAY_BASE_SIZE;
	    for (unsigned long i = 0; i < array_size; i++)
		the_array[i] = 0;
	}
    }

    array(unsigned long start_size)
    {
	dead = 0;
	deadT = new T;
	the_array = (T **) new T *[start_size];

	if (the_array)
	{
	    array_size = start_size;
	    // Delay element create until first reference
	    for (unsigned long i = 0; i < array_size; i++)
		the_array[i] = 0;
	}
    }

    // void shuffle(unsigned long index)
    // {
    //	// shuffle the rest back one
    //	for (unsigned long i = index; i < array_size; i++)
    //	    the_array[i] = the_array[i + 1];
    // }

       ~array(void)
    {
	if (the_array)
	{
	    for (unsigned long i = 0; i < array_size; i++)
	    {
		if (the_array[i])
		    delete the_array[i];
	    }
	}
	delete[](the_array);
    }

    T & operator[](unsigned long index)
    {
	if (index < array_size && the_array[index])
	    return *(the_array[index]);	// Fast return

	else
	{
	    // Need a new element
	    if (index >= array_size)
	    {
		// Try to expand
		// should resize exponentially - it's more efficient
		// unless the request is more than 2x current length
		if (index < 2 * array_size)
		    resize(2 * array_size);
		else
		    resize(index + 1);
	    }
	    if (index < array_size)
	    {
		// Try to create
		the_array[index] = new T;
		if (the_array[index])
		    return *(the_array[index]);
	    }
	    dead = 1;			// We are going to die without help
	    return *deadT;

	}
    }

    // EXAMPLES for comparison functions
    // remember to de-reference the argument

    //   int charSorter(const void* one,const void *two)
    //   {
    //       return stricmp(*(const char **)one,*(const char **)two);
    //   }
    //
    //   int numsort(const void *One, const void *Two)
    //   {
    //
    //       int one = (int)*(int *)One;
    //       int two = (int)*(int *)Two;
    //
    //       if (one < two) return -1;
    //       else if (two < one) return 1;
    //       else
    //           return 0;
    //   }

    void sort(int number, int (*compare) (const void *one, const void *two))
    {
	// compare() *should* handle NULL entries, could get away without it
	theCompare = compare;

	qsort(the_array, number, sizeof(T *), arrayQSortCompare);
    }

  unsigned is_dead(void) {
      return dead;
  }

  private:

    unsigned long array_size;
    T **the_array;
    T *deadT;
    unsigned dead;

    void resize(unsigned long new_size)
    {
	T **new_array = (T **) new T *[new_size];

	if (new_array)
	{
	    unsigned long i;
	    // copy original entries
	    for (i = 0; i < array_size; i++)
		    new_array[i] = the_array[i];

	    // clear new entries
	    for (i = array_size; i < new_size; i++)
		    new_array[i] = 0;

		array_size = new_size;
		delete[] the_array;
		the_array = new_array;
	}
    }

};

#endif // #ifndef __ARRAY_H__
