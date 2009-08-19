#ifndef CMALLOCMEMORY_H_
#define CMALLOCMEMORY_H_

#include <cstdlib>

class CMallocMemory
{
public:
	CMallocMemory();
	virtual ~CMallocMemory();
	
public:
	virtual void free(void* _ptr) = 0;
	virtual void* allocatetor(size_t _s) = 0;
};

#endif /*CMALLOCMEMORY_H_*/
