#include <stdio.h>
#include "mem.h"  
typedef unsigned long size_t;

int MySum(const int* const ar, const size_t len)
{
	size_t i;
	int sum=0;
	if(ar==NULL)
		return 0;
	for(i=0;i < len; i++)
      sum += ar[i];
	return sum;
}

int main(int argc, char* argv[])
{
	size_t d, i;
	int *a;
	struct mem_t * ptr;
	FILE* output_file;
	const char* path2 = "data2";
	
	if ((output_file = fopen(path2, "wb")) == NULL)
    {
	   puts("Wrong_path");
	   return 0;
    }
	ptr = (struct mem_t *)heap_init();

    printf("Enter amount of elements in integer array: ");
	scanf("%lu", &d);
	if(d>0)
	{
	  printf("Elements:\n");
      a = (int*) _malloc(d*4);
      for (i = 0; i < d; i++)
      scanf("%i", &a[i]);
	}
    printf("Sum: %i\n", MySum(a, d));
	
	/*b = (int *)_malloc(5*sizeof(int));
	b[0]=1;
	b[1]=2;
	b[3]=3;
	c = (int *)_malloc(100);
	c[0]=4;
	c[1]=3;
	c[3]=1;
	_free(b);
	_free(c);*/

	memalloc_debug_heap(output_file, ptr);
	puts("See file data2, and check result");
	_free(a);
	fclose(output_file);
	return 0;
}
