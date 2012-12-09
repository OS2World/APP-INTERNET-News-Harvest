/* charqueue.h,v 1.2 2001/06/20 00:42:19 root Exp */

#ifndef __CHARQUEUE_H__
#define __CHARQUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <new.h>

#include "log.h"

// fixme to be fully thread-safe

class CharQueue
{
public:
    CharQueue(unsigned long initial_size = 1L << 18)
    {
	elements = 0;
	current_size = 0;
	head = 0;
	tail = 0;
	reported = 0;
	Resize(initial_size);
    }

    ~CharQueue()
    {
	delete[] elements;
    }

    int Empty()
    {
	return (!elements || head == tail);
    }

    int Full()
    {
	if (!elements)
	    return 1;
	else
	{
	    unsigned long new_tail = tail + 1;
	    if (new_tail == current_size)
		new_tail = 0;
	    return new_tail == head;
	}
    }

    unsigned long ContentSize()
    {
	unsigned long total;

	if (Empty())
	    total = 0;
	else
	{
	    if (tail < head)
		total = current_size - (head - tail);
	    else
		total = tail - head;

	    total *= sizeof(char);
	}

	return total;
    }

    char *getContentAsString()
    {
	// user must be sure string is NULL terminated
	return elements;
    }

    void Enqueue(char item)			// To tail
    {
	if (!Full() || Resize())
	{
	    elements[tail] = item;
	    unsigned long new_tail = tail + 1;
	    if (new_tail == current_size)
		new_tail = 0;
	    tail = new_tail;
	}
    }

    int Enqueue(char *items,int count)		// Return true if OK
    {
	while (count &&
	       (!Full() || Resize()))
	{
	    elements[tail] = *items;
	    items++;

	    unsigned long new_tail = tail + 1;
	    if (new_tail == current_size)
		new_tail = 0;
	    tail = new_tail;
	    count--;
	}

	return !count;

    } // Enqueue


    void Stuff(char item)		// To head
    {
	if (!Full() || Resize())
	{
	  unsigned long new_head = head;
	  if (new_head == 0)
	      head = current_size - 1;
	  elements[new_head] = item;
	  head = new_head;
	}
    }

    char Dequeue()
    {
	char item;

	if (!elements)
	    item = 0;			// Oh well
	else
	{
	  item = elements[head];
	  unsigned long new_head = head + 1;
	  if (new_head == current_size)
	      new_head = 0;
	  head = new_head;
	}

	return item;
    }

private:
    int Resize(unsigned long new_size = 0)	// Return true if not full after resize
    {
	char *new_block = 0;

	if (!new_size)
	{
	    if (current_size)
		new_size = current_size * 2;
	    else
		new_size = 1L << 18;		// 2^18 = 256K
	}

	// Try to double, degrade gracefully

	while (!new_block && new_size > current_size)
	{
	    new_block = new char[new_size];
	    if (!new_block)
		new_size = current_size + (new_size - current_size) / 2;
	}

	if (new_size == current_size)
	{
	    if (!reported)
	    {
	        reported = 1;
	        log("    Error: can not resize queue from %d.  Data lost.");
	    }
	}

	// Copy if got block

	if (new_block)
	{
	    int new_tail = 0;

	    while(!Empty())
	    {
		new_block[new_tail] = Dequeue();
		new_tail++;
	    }
	    delete[] elements;
	    head = 0;
	    tail = new_tail;
	    current_size = new_size;
	    elements = new_block;
	    reported = 0;
	}

	return !Full();

    } // Resize

    char *elements;
    unsigned long current_size;		// Allocated size
    unsigned long head;			// Next dequeue (in use)
    unsigned long tail;			// Next enqueue (free)
    unsigned reported;			// Overflow logged
};


#endif //#ifndef __CHARQUEUE_H__
