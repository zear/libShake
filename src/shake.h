#ifndef _SHAKE_H_
#define _SHAKE_H_

#include <dirent.h>

#define SHAKE_DIR_NODES		"/dev/input"

#define BITS_PER_LONG		(sizeof(long) * 8)
#define OFF(x)			((x)%BITS_PER_LONG)
#define BIT(x)			(1UL<<OFF(x))
#define LONG(x)			((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

typedef struct shakeDev
{
	int fd;
	char *node;
	struct ff_effect effect;
	struct input_event play;
} shakeDev;

typedef struct listElement
{
	struct listElement *next;
	shakeDev *dev;
} listElement;

extern listElement *listHead;
extern int numOfDevices;

/* Helper functions */
int nameFilter(const struct dirent *entry);
listElement *listElementPrepend(listElement *head);
listElement *listElementDelete(listElement *head, listElement *toDelNode);
listElement *listElementDeleteAll(listElement *head);
listElement *listElementGet(listElement *head, unsigned int id);

/* libShake functions */
int shakeInit();
void shakeQuit();
int shakeListDevices();
shakeDev *shakeOpen(unsigned int id);
void shakeClose(shakeDev *dev);
int shakeQuery(shakeDev *dev);
void shakeSetEffect(shakeDev *deve, int effect, int duration);
void shakeUploadEffect(shakeDev *dev);
void shakePlay(shakeDev *dev);

#endif /* _SHAKE_H_ */
