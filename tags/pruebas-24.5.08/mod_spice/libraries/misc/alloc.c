/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
**********/

/*
 * Memory alloction functions
 */

#include "spice.h"
#include "stdio.h"
#include "misc.h"
#include "suffix.h"

char* tmalloc(int num)
{
	char* ret;

	ret = my_tmalloc(num);
	
	record_alloc(ret);

	return ret;
}

char* trealloc(void* old_p, size_t new_size)
{
	char* ret;
	unrecord_alloc(old_p);
	ret = my_trealloc(old_p, new_size);
	record_alloc(ret);
	return ret;
}

void txfree(char* ptr)
{
	unrecord_alloc(ptr);
	my_txfree(ptr);
}
