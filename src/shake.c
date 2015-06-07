/* libShake - a basic haptic library */

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "shake_private.h"
#include "shake.h"

listElement *listHead;
unsigned int numOfDevices;

/* Prototypes */

int shakeProbe(shakeDev *dev);

/* Helper functions */

int nameFilter(const struct dirent *entry)
{
	const char filter[] = "event";
	return !strncmp(filter, entry->d_name, strlen(filter));
}

listElement *listElementPrepend(listElement *head)
{
	listElement *newNode = malloc(sizeof(listElement));
	if(!newNode)
	{
		return head;
	}

	newNode->next = head;
	return newNode;

}

listElement *listElementDelete(listElement *head, listElement *toDelNode)
{
	listElement *prevNode = NULL;
	listElement *curNode = head;

	while(curNode)
	{
		if(curNode == toDelNode)
		{
			if(!prevNode)
			{
				head = curNode->next;
			}
			else
			{
				prevNode->next = curNode->next;
			}

			free(curNode);
			return head;
		}
		prevNode = curNode;
		curNode = curNode->next;
	}

	return head;
}

listElement *listElementDeleteAll(listElement *head)
{
	listElement *toDelNode;
	listElement *curNode = head;

	while(curNode)
	{
		toDelNode = curNode;
		curNode = curNode->next;
		free(toDelNode);
	}

	return NULL;
}

listElement *listElementGet(listElement *head, unsigned int id)
{
	listElement *curNode = head;
	int i = 0;

	while(curNode)
	{
		if (i == id)
			return curNode;

		curNode = curNode->next;
		++i;
	}

	return NULL;
}

/* libShake functions */

int shakeInit()
{
	struct dirent **nameList;
	int numOfEntries;
	int i;

	numOfDevices = 0;

	numOfEntries = scandir(SHAKE_DIR_NODES, &nameList, nameFilter, alphasort);

	if (numOfEntries < 0)
	{
		perror("scandir");
	}
	else
	{
		for (i = 0; i < numOfEntries; ++i)
		{
			shakeDev dev;

			dev.node = malloc(strlen(SHAKE_DIR_NODES) + strlen("/") + strlen(nameList[i]->d_name) + 1);
			if (dev.node == NULL)
				return -1;

			strcpy(dev.node, SHAKE_DIR_NODES);
			strcat(dev.node, "/");
			strcat(dev.node, nameList[i]->d_name);

			if (shakeProbe(&dev))
			{
				listHead = listElementPrepend(listHead);
				listHead->dev = malloc(sizeof(shakeDev));
				memcpy(listHead->dev, &dev, sizeof(shakeDev));
				++numOfDevices;
			}
			free(nameList[i]);
		}

		free(nameList);
	}

	printf("Detected devices: %d\n", numOfDevices);
}

void shakeQuit()
{
	if (listHead != NULL)
	{
		listElement *toDelElem;
		listElement *curElem = listHead;

		while(curElem)
		{
			toDelElem = curElem;
			curElem = curElem->next;

			shakeClose(toDelElem->dev);
			if (toDelElem->dev->node != NULL)
				free(toDelElem->dev->node);
		}

		listElementDeleteAll(listHead);
	}
}

void shakeListDevices()
{
	listElement *curElem;
	int i;

	for (curElem = listHead, i = 0; curElem != NULL; curElem = curElem->next, ++i)
	{
		printf("%d) %s\n", i, curElem->dev->node);
	}
}

int shakeNumOfDevices()
{
	return numOfDevices;
}

int shakeProbe(shakeDev *dev)
{
	int isHaptic;

	if(!dev || !dev->node)
		return -1;

	dev->fd = open(dev->node, O_RDWR);

	if (!dev->fd)
		return -1;

	isHaptic = !shakeQuery(dev);
	dev->fd = close(dev->fd);
	
	return isHaptic;
}

shakeDev *shakeOpen(unsigned int id)
{
	if (id >= numOfDevices)
		return NULL;

	listElement *element = listElementGet(listHead, id);

	if(!element->dev || !element->dev->node)
		return NULL;

	element->dev->fd = open(element->dev->node, O_RDWR);

	return element->dev->fd ? element->dev : NULL;
}

int shakeQuery(shakeDev *dev)
{
	if(!dev)
		return -1;

	if (ioctl(dev->fd, EVIOCGBIT(EV_FF, sizeof(dev->features)), dev->features) == -1)
	{
/*		perror("Ioctl query");*/
		return -1;
	}

/*	printf("Effect id:\tEffect name:\n");*/
/*	if (test_bit(FF_CONSTANT, dev->features)) printf("#%d\t\tConstant\n", FF_CONSTANT);*/
/*	if (test_bit(FF_PERIODIC, dev->features)) printf("#%d\t\tPeriodic\n", FF_PERIODIC);*/
/*	if (test_bit(FF_SPRING, dev->features))   printf("#%d\t\tSpring\n", FF_SPRING);*/
/*	if (test_bit(FF_FRICTION, dev->features)) printf("#%d\t\tFriction\n", FF_FRICTION);*/
/*	if (test_bit(FF_RUMBLE, dev->features))   printf("#%d\t\tRumble\n", FF_RUMBLE);*/
/*	printf("-end-\n");*/

	if (!dev->features[0] && !dev->features[1] && !dev->features[2] && !dev->features[3])
		return 1;

	if (ioctl(dev->fd, EVIOCGEFFECTS, &dev->n_effects) == -1)
	{
/*		perror("Ioctl query");*/
		return -1;
	}

	return 0;
}

void shakeSetGain(shakeDev *dev, int gain)
{
	struct input_event ie;

	if (gain < 0)
		gain = 0;
	if (gain > 100)
		gain = 100;

	ie.type = EV_FF;
	ie.code = FF_GAIN;
	ie.value = 0xFFFFUL * gain / 100;

	if (write(dev->fd, &ie, sizeof(ie)) == -1)
	{
		perror("set gain");
	}
}

void shakeInitEffect(shakeEffect *effect, shakeEffectType type)
{
	if (!effect)
		return;
	if (type < 0 || type > SHAKE_EFFECT_COUNT)
			perror("Unsupported effect");
	effect->type = type;
	effect->id = -1;
}

int shakeUploadEffect(shakeDev *dev, shakeEffect effect)
{
	struct ff_effect e;

	if (!dev)
		return -1;

	if(effect.type == SHAKE_EFFECT_RUMBLE)
	{
		e.type = FF_RUMBLE;
		e.id = -1;
		e.u.rumble.strong_magnitude = effect.rumble.strongMagnitude;
		e.u.rumble.weak_magnitude   = effect.rumble.weakMagnitude;
		e.replay.delay  = effect.delay;
		e.replay.length = effect.length;
	}
	else if(effect.type == SHAKE_EFFECT_PERIODIC)
	{
		e.type = FF_PERIODIC;
		e.id = -1;
		e.u.periodic.waveform = effect.periodic.waveform;
		e.u.periodic.period = effect.periodic.period;
		e.u.periodic.magnitude = effect.periodic.magnitude;
		e.u.periodic.offset = effect.periodic.offset;
		e.u.periodic.phase = effect.periodic.phase;
		e.direction = 0x4000;
		e.u.periodic.envelope.attack_length = 0x100;
		e.u.periodic.envelope.attack_level  = 0;
		e.u.periodic.envelope.fade_length = 0x100;
		e.u.periodic.envelope.fade_level  = 0;
		e.trigger.button = 0;
		e.trigger.interval = 0;
		e.replay.delay = effect.delay;
		e.replay.length = effect.length;
	}
	else
	{
		perror("Unsupported effect");
		return -2;
	}

	if (ioctl(dev->fd, EVIOCSFF, &e) == -1)
	{
		perror("upload effect");
		return -3;
	}

	return e.id;
}

void shakeEraseEffect(shakeDev *dev, int id)
{
	if (!dev)
		return;

	if (id < 0)
		return;

	if (ioctl(dev->fd, EVIOCRMFF, id) == -1)
	{
		perror("erase effect");
		return;
	}
}

void shakePlay(shakeDev *dev, int id)
{
	if(!dev)
		return;

	if(id < 0)
		return;

/*	printf("Playing effect #%d for a duration of %d ms\n", dev->effect.type, dev->effect.replay.length);*/
	dev->play.type = EV_FF;
	dev->play.code = id; /* the id we got when uploading the effect */
	dev->play.value = FF_STATUS_PLAYING; /* play: FF_STATUS_PLAYING, stop: FF_STATUS_STOPPED */

	if (write(dev->fd, (const void*) &dev->play, sizeof(dev->play)) == -1)
	{
		perror("sending event");
		return;
	}
}

void shakeStop(shakeDev *dev, int id)
{
	if(!dev)
		return;

	if(id < 0)
		return;

	dev->stop.type = EV_FF;
	dev->stop.code = id; /* the id we got when uploading the effect */
	dev->stop.value = FF_STATUS_STOPPED;

	if (write(dev->fd, (const void*) &dev->stop, sizeof(dev->stop)) == -1)
	{
		perror("sending event");
		return;
	}
}

void shakeClose(shakeDev *dev)
{
	close(dev->fd);
}
