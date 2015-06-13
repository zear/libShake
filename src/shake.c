/* libShake - a basic haptic library */

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "shake.h"
#include "shake_private.h"

listElement *listHead;
unsigned int numOfDevices;

/* Prototypes */

int Shake_Probe(Shake_Device *dev);
int Shake_Query(Shake_Device *dev);

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

int Shake_Init()
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
			Shake_Device dev;

			dev.node = malloc(strlen(SHAKE_DIR_NODES) + strlen("/") + strlen(nameList[i]->d_name) + 1);
			if (dev.node == NULL)
				return -1;

			strcpy(dev.node, SHAKE_DIR_NODES);
			strcat(dev.node, "/");
			strcat(dev.node, nameList[i]->d_name);

			if (Shake_Probe(&dev))
			{
				listHead = listElementPrepend(listHead);
				listHead->dev = malloc(sizeof(Shake_Device));
				memcpy(listHead->dev, &dev, sizeof(Shake_Device));
				++numOfDevices;
			}
			free(nameList[i]);
		}

		free(nameList);
	}

	return 0;
}

void Shake_Quit()
{
	if (listHead != NULL)
	{
		listElement *toDelElem;
		listElement *curElem = listHead;

		while(curElem)
		{
			toDelElem = curElem;
			curElem = curElem->next;

			Shake_Close(toDelElem->dev);
			if (toDelElem->dev->node != NULL)
				free(toDelElem->dev->node);
		}

		listElementDeleteAll(listHead);
	}
}

int Shake_NumOfDevices()
{
	return numOfDevices;
}

int Shake_Probe(Shake_Device *dev)
{
	int isHaptic;

	if(!dev || !dev->node)
		return -1;

	dev->fd = open(dev->node, O_RDWR);

	if (!dev->fd)
		return -1;

	isHaptic = !Shake_Query(dev);
	dev->fd = close(dev->fd);
	
	return isHaptic;
}

Shake_Device *Shake_Open(unsigned int id)
{
	if (id >= numOfDevices)
		return NULL;

	listElement *element = listElementGet(listHead, id);

	if(!element->dev || !element->dev->node)
		return NULL;

	element->dev->fd = open(element->dev->node, O_RDWR);

	return element->dev->fd ? element->dev : NULL;
}

int Shake_Query(Shake_Device *dev)
{
	int size = sizeof(dev->features)/sizeof(unsigned long);
	int i;

	if(!dev)
		return -1;

	if (ioctl(dev->fd, EVIOCGBIT(EV_FF, sizeof(dev->features)), dev->features) == -1)
	{
/*		perror("Ioctl query");*/
		return -1;
	}

	for (i = 0; i < size; ++i)
	{
		if (dev->features[i])
			break;
	}

	if (i >= size) /* Device doesn't support any force feedback effects. Ignore it. */
		return -1;

	if (ioctl(dev->fd, EVIOCGEFFECTS, &dev->capacity) == -1)
	{
/*		perror("Ioctl query");*/
		return -1;
	}

	if (dev->capacity <= 0) /* Device doesn't support uploading effects. Ignore it. */
		return -1;

	return 0;
}

int Shake_EffectCapacity(Shake_Device *dev)
{
	return dev->capacity;
}

int Shake_QueryEffectType(Shake_Device *dev, Shake_EffectType type)
{
	return test_bit(FF_RUMBLE + type, dev->features) ? 1 : 0;
}

int Shake_QueryPeriodicWaveform(Shake_Device *dev, Shake_PeriodicWaveform waveform)
{
	return test_bit(FF_SQUARE + waveform, dev->features) ? 1 : 0;
}

int Shake_QueryGainAdjustable(Shake_Device *dev)
{
	return test_bit(FF_GAIN, dev->features) ? 1 : 0;
}

void Shake_SetGain(const Shake_Device *dev, int gain)
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

void Shake_InitEffect(Shake_Effect *effect, Shake_EffectType type)
{
	if (!effect)
		return;
	memset(effect, 0, sizeof(*effect));
	if (type < 0 || type >= SHAKE_EFFECT_COUNT)
		perror("Unsupported effect");
	effect->type = type;
	effect->id = -1;
}

int Shake_UploadEffect(const Shake_Device *dev, Shake_Effect effect)
{
	struct ff_effect e;

	if (!dev)
		return -1;

	if(effect.type == SHAKE_EFFECT_RUMBLE)
	{
		e.type = FF_RUMBLE;
		e.id = -1;
		e.u.rumble.strong_magnitude = effect.rumble.strongMagnitude;
		e.u.rumble.weak_magnitude = effect.rumble.weakMagnitude;
		e.replay.delay = effect.delay;
		e.replay.length = effect.length;
	}
	else if(effect.type == SHAKE_EFFECT_PERIODIC)
	{
		e.type = FF_PERIODIC;
		e.id = -1;
		e.u.periodic.waveform = FF_SQUARE + effect.periodic.waveform;
		e.u.periodic.period = effect.periodic.period;
		e.u.periodic.magnitude = effect.periodic.magnitude;
		e.u.periodic.offset = effect.periodic.offset;
		e.u.periodic.phase = effect.periodic.phase;
		e.u.periodic.envelope.attack_length = effect.periodic.envelope.attackLength;
		e.u.periodic.envelope.attack_level = effect.periodic.envelope.attackLevel;
		e.u.periodic.envelope.fade_length = effect.periodic.envelope.fadeLength;
		e.u.periodic.envelope.fade_level = effect.periodic.envelope.fadeLevel;
		e.trigger.button = 0;
		e.trigger.interval = 0;
		e.direction = effect.direction;
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

void Shake_EraseEffect(const Shake_Device *dev, int id)
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

void Shake_Play(const Shake_Device *dev, int id)
{
	if(!dev)
		return;

	if(id < 0)
		return;

	struct input_event play;
	play.type = EV_FF;
	play.code = id; /* the id we got when uploading the effect */
	play.value = FF_STATUS_PLAYING; /* play: FF_STATUS_PLAYING, stop: FF_STATUS_STOPPED */

	if (write(dev->fd, (const void*) &play, sizeof(play)) == -1)
	{
		perror("sending event");
		return;
	}
}

void Shake_Stop(const Shake_Device *dev, int id)
{
	if(!dev)
		return;

	if(id < 0)
		return;

	struct input_event stop;
	stop.type = EV_FF;
	stop.code = id; /* the id we got when uploading the effect */
	stop.value = FF_STATUS_STOPPED;

	if (write(dev->fd, (const void*) &stop, sizeof(stop)) == -1)
	{
		perror("sending event");
		return;
	}
}

void Shake_Close(const Shake_Device *dev)
{
	close(dev->fd);
}
