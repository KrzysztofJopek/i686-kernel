#ifndef _LIST_H_
#define _LIST_H_

//list head has to be last element of the list struct
//eg:
//struct node{
//	int val;
//	list_head head;
//}

#define LIST_GET_NODE(addr, type) (type*)((void*)addr - (sizeof(type)-sizeof(struct list_head)))
struct list_head{
	struct list_head* next;
	struct list_head* prev;
};

void list_set_head(struct list_head* head);
void list_add(struct list_head* head, struct list_head* new);
void list_del(struct list_head* head);

#endif
