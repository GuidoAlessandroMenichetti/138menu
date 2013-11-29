#ifndef GBALLOC_HEADER
#define GBALLOC_HEADER

#include <stdlib.h>
#include "types.h"

typedef struct s_node
{
    tData data;
    struct s_node * next;
} node;

class gbAlloc
{
    public:

	enum GBALLOC_ERRORS {GBALLOC_SUCCESS = 0, GBALLOC_NO_MEMORY=-1, GBALLOC_NO_DATA=-2, GBALLOC_NOT_FOUND=-3};

	void Create();
	int Add(const tData *);
	tData * Get(unsigned);
	int GetCount();
	void FreeMemory();
	void Sort(int (* comparation)(const void * a, const void * b));
	gbAlloc();
	~gbAlloc();

	private:

	node * start;
	int count;
	//void (* clean_function)(const void * n);
};


#endif
