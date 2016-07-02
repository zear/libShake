#ifndef _HELPERS_H_
#define _HELPERS_H_

typedef struct listElement
{
	struct listElement *next;
	void *item;
} listElement;

listElement *listElementPrepend(listElement *head);
listElement *listElementDelete(listElement *head, listElement *toDelNode, void(*itemDel)());
listElement *listElementDeleteAll(listElement *head, void(*itemDel)(void *item));
listElement *listElementGet(listElement *head, unsigned int id);
unsigned int listLength(listElement *head);

#endif /* _HELPERS_H_ */
