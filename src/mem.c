#include "mem.h"

void* heap_init(){
	void * Heap;
	struct mem_t * Newel;
	Heap = (void *)mmap(HEAP_START, PAGE_LEN ,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);
	Newel = (struct mem_t *)Heap;
	Newel -> is_free=1;
	Newel -> next = NULL;
	Newel -> capacity= PAGE_LEN - SIZE_HEAD;
	return Heap;
}

void add_heap(size_t query, struct mem_t * New_Block){
	size_t free_memory, PAGES;
	char *free,*End_old_heap;
	struct mem_t * Free_memory;
	free = (char *)(New_Block + 1);
	End_old_heap = free + New_Block->capacity;
	PAGES = (((query+SIZE_HEAD - New_Block -> capacity)/PAGE_LEN)+1)*PAGE_LEN;
	(void *)mmap((void *)End_old_heap, PAGES,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);
	if(errno==ENOMEM)
	{
		New_Block->next = NULL;
		return;
	}
	free_memory = PAGES - query - SIZE_HEAD + New_Block -> capacity;
	New_Block -> capacity = query;
	New_Block -> is_free=0;
	New_Block->next = (struct mem_t *)(free + query);
	Free_memory = New_Block->next;
	Free_memory->next=NULL;
	Free_memory->capacity = free_memory;
	Free_memory->is_free=1;

}

void add_any(size_t query, struct mem_t * New_Block){
	void * Heap;
	struct mem_t * Newel;
	size_t PAGES;
	PAGES = (((query+2*SIZE_HEAD)/PAGE_LEN)+1)*PAGE_LEN;
	Heap=(void *)mmap(NULL, PAGES,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if(errno==ENOMEM)
    {
	  New_Block->next = NULL;
	  return;
	}
	Newel = (struct mem_t *)Heap;
	New_Block->next = Newel;
	Newel -> is_free=1;
	Newel -> next = NULL;
	Newel -> capacity= PAGES  - (query+2*SIZE_HEAD);
}


void* _malloc( size_t query ){
	char * free;
	size_t free_memory;
	struct mem_t * Free_memory,*buf;
	struct mem_t * Newel = (struct mem_t *)HEAP_START;
	while( Newel -> next != NULL && (Newel -> capacity - SIZE_HEAD) < query)
		Newel = Newel -> next;
	
	if(Newel -> next == NULL  && (Newel -> capacity - SIZE_HEAD) < query)
	{
		
		add_heap(query, Newel);
		if(Newel->next != NULL)
		{
		  free = (char *)(Newel + 1);
		  return (void *)free;
		}
		else
		{
			add_any(query, Newel);
			if(Newel->next != NULL)
		    {
		      free = (char *)(Newel + 1);
		      return (void *)free;
		    }
		}
		return NULL;

	}
	if((Newel -> capacity - SIZE_HEAD) >= query)
	{
		free_memory = Newel->capacity - query - SIZE_HEAD;
		Newel->capacity = query;
	    Newel->is_free=0;
	    free = (char *)(Newel + 1);
		buf=Newel->next;
	    Newel->next = (struct mem_t *)(free + query);
	    Free_memory = Newel->next;
		Free_memory->next=buf;
	    Free_memory->capacity = free_memory;
	    Free_memory->is_free=1;
	}
	return (void *)free;
}
struct mem_t * concat_free(struct mem_t * Free_Block)
{
	struct mem_t * Next_Block = Free_Block->next;
	if(Next_Block==NULL)
		return Free_Block;
	if(Next_Block->is_free==1)
	{
		Free_Block->next=Next_Block->next;
		Free_Block->capacity=Free_Block->capacity + Next_Block->capacity + SIZE_HEAD;
		Free_Block = concat_free(Free_Block);
	}
		return Free_Block;
}
void _free( void* mem )
{
	struct mem_t * Free_Block=(struct mem_t *)mem - 1;
	mem=NULL;
	Free_Block->is_free=1;
	Free_Block = concat_free(Free_Block);
}


void memalloc_debug_struct_info( FILE* f, struct mem_t * address ) {
    fprintf( f, 
            "start: %p\nsize: %lu\nis_free: %d\n",
            (void*)address, 
            address-> capacity, 
            address-> is_free );
    putc( '\n', f );
}

void memalloc_debug_heap( FILE* f,  struct mem_t * ptr ) {
    for( ; ptr; ptr = ptr->next ) 
        memalloc_debug_struct_info( f, ptr ); 
}