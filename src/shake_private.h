#ifndef _SHAKE_PRIVATE_H_
#define _SHAKE_PRIVATE_H_

#include <dirent.h>
#include <linux/input.h>

#define SHAKE_DIR_NODES		"/dev/input"

#define BITS_PER_LONG		(sizeof(long) * 8)
#define OFF(x)			((x)%BITS_PER_LONG)
#define BIT(x)			(1UL<<OFF(x))
#define LONG(x)			((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)


typedef struct Shake_Device
{
	int fd;
	char *node;
	unsigned long features[4];
	int n_effects; /* Number of effects the device can play at the same time */
} Shake_Device;

typedef struct listElement
{
	struct listElement *next;
	Shake_Device *dev;
} listElement;

extern listElement *listHead;
extern unsigned int numOfDevices;

/* Helper functions */
int nameFilter(const struct dirent *entry);
listElement *listElementPrepend(listElement *head);
listElement *listElementDelete(listElement *head, listElement *toDelNode);
listElement *listElementDeleteAll(listElement *head);
listElement *listElementGet(listElement *head, unsigned int id);

#endif /* _SHAKE_PRIVATE_H_ */
