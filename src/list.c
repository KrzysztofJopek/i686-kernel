#include "list.h"
#include "kerndefs.h"

void list_set_head(struct list_head* head)
{
	head->next = head;
	head->prev = head;
}

void list_add(struct list_head* head, struct list_head* new)
{
	head->next->prev = new;	
	new->next = head->next;
	head->next = new;
	new->prev = head;
}

void list_del(struct list_head* head)
{
	head->prev->next = head->next;
	head->next->prev = head->prev;
	head->next = NULL;
	head->prev = NULL;
}
