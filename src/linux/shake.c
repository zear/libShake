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
#include "../common/helpers.h"

listElement *listHead;
unsigned int numOfDevices;

/* Prototypes */

int Shake_Probe(Shake_Device *dev);
int Shake_Query(Shake_Device *dev);

int nameFilter(const struct dirent *entry)
{
	const char filter[] = "event";
	return !strncmp(filter, entry->d_name, strlen(filter));
}

/* Public functions */

int Shake_Init()
{
	struct dirent **nameList;
	int numOfEntries;

	numOfDevices = 0;

	numOfEntries = scandir(SHAKE_DIR_NODES, &nameList, nameFilter, alphasort);

	if (numOfEntries < 0)
	{
		perror("Shake_Init: Failed to retrieve device nodes.");
	}
	else
	{
		int i;

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
				dev.id = numOfDevices;
				listHead = listElementPrepend(listHead);
				listHead->item = malloc(sizeof(Shake_Device));
				memcpy(listHead->item, &dev, sizeof(Shake_Device));
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
		listElement *curElem = listHead;

		while(curElem)
		{
			Shake_Device *dev;
			listElement *toDelElem = curElem;
			curElem = curElem->next;
			dev = (Shake_Device *)toDelElem->item;

			Shake_Close(dev);
			if (dev->node != NULL)
				free(dev->node);
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
	Shake_Device *dev;

	if (id >= numOfDevices)
		return NULL;

	listElement *element = listElementGet(listHead, numOfDevices - 1 - id);
	dev = (Shake_Device *)element->item;

	if(!dev || !dev->node)
		return NULL;

	dev->fd = open(dev->node, O_RDWR);

	return dev->fd ? dev : NULL;
}

int Shake_Query(Shake_Device *dev)
{
	int size = sizeof(dev->features)/sizeof(unsigned long);
	int i;

	if(!dev)
		return -1;

	if (ioctl(dev->fd, EVIOCGBIT(EV_FF, sizeof(dev->features)), dev->features) == -1)
	{
		perror("Shake_Query: Failed to query for device features.");
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
		perror("Shake_Query: Failed to query for device effect capacity.");
		return -1;
	}

	if (dev->capacity <= 0) /* Device doesn't support uploading effects. Ignore it. */
		return -1;

	if (ioctl(dev->fd, EVIOCGNAME(sizeof(dev->name)), dev->name) == -1) /* Get device name */
	{
		strncpy(dev->name, "Unknown", sizeof(dev->name));
	}

	return 0;
}

int Shake_DeviceId(Shake_Device *dev)
{
	return dev ? dev->id : -1;
}

const char *Shake_DeviceName(Shake_Device *dev)
{
	return dev ? dev->name : NULL;
}

int Shake_DeviceEffectCapacity(Shake_Device *dev)
{
	return dev ? dev->capacity : -1;
}

int Shake_QueryEffectSupport(Shake_Device *dev, Shake_EffectType type)
{
	/* Starts at a magic, non-zero number, FF_RUMBLE.
	   Increments respectively to EffectType. */
	return test_bit(FF_RUMBLE + type, dev->features) ? 1 : 0;
}

int Shake_QueryWaveformSupport(Shake_Device *dev, Shake_PeriodicWaveform waveform)
{
	/* Starts at a magic, non-zero number, FF_SQUARE.
	   Increments respectively to PeriodicWaveform. */
	return test_bit(FF_SQUARE + waveform, dev->features) ? 1 : 0;
}

int Shake_QueryGainSupport(Shake_Device *dev)
{
	return test_bit(FF_GAIN, dev->features) ? 1 : 0;
}

int Shake_QueryAutocenterSupport(Shake_Device *dev)
{
	return test_bit(FF_AUTOCENTER, dev->features) ? 1 : 0;
}

void Shake_SetGain(Shake_Device *dev, int gain)
{
	struct input_event ie;

	if (!dev)
		return;

	if (gain < 0)
		gain = 0;
	if (gain > 100)
		gain = 100;

	ie.type = EV_FF;
	ie.code = FF_GAIN;
	ie.value = 0xFFFFUL * gain / 100;

	if (write(dev->fd, &ie, sizeof(ie)) == -1)
	{
		perror("Shake_SetGain: Failed to set gain.");
	}
}

void Shake_SetAutocenter(Shake_Device *dev, int autocenter)
{
	struct input_event ie;

	if (!dev)
		return;

	if (autocenter < 0)
		autocenter = 0;
	if (autocenter > 100)
		autocenter = 100;

	ie.type = EV_FF;
	ie.code = FF_AUTOCENTER;
	ie.value = 0xFFFFUL * autocenter / 100;

	if (write(dev->fd, &ie, sizeof(ie)) == -1)
	{
		perror("Shake_SetAutocenter: Failed to set auto-center.");
	}
}

void Shake_InitEffect(Shake_Effect *effect, Shake_EffectType type)
{
	if (!effect)
		return;
	memset(effect, 0, sizeof(*effect));
	if (type < 0 || type >= SHAKE_EFFECT_COUNT)
		perror("Shake_InitEffect: Unsupported effect.");
	effect->type = type;
	effect->id = -1;
}

int Shake_UploadEffect(Shake_Device *dev, Shake_Effect *effect)
{
	struct ff_effect e;

	if (!dev)
		return -1;
	if (!effect)
		return -1;
	if (effect->id < -1)
		return -1;

	if(effect->type == SHAKE_EFFECT_RUMBLE)
	{
		e.type = FF_RUMBLE;
		e.id = effect->id;
		e.u.rumble.strong_magnitude = effect->rumble.strongMagnitude;
		e.u.rumble.weak_magnitude = effect->rumble.weakMagnitude;
		e.replay.delay = effect->delay;
		e.replay.length = effect->length;
	}
	else if(effect->type == SHAKE_EFFECT_PERIODIC)
	{
		e.type = FF_PERIODIC;
		e.id = effect->id;
		e.u.periodic.waveform = FF_SQUARE + effect->periodic.waveform;
		e.u.periodic.period = effect->periodic.period;
		e.u.periodic.magnitude = effect->periodic.magnitude;
		e.u.periodic.offset = effect->periodic.offset;
		e.u.periodic.phase = effect->periodic.phase;
		e.u.periodic.envelope.attack_length = effect->periodic.envelope.attackLength;
		e.u.periodic.envelope.attack_level = effect->periodic.envelope.attackLevel;
		e.u.periodic.envelope.fade_length = effect->periodic.envelope.fadeLength;
		e.u.periodic.envelope.fade_level = effect->periodic.envelope.fadeLevel;
		e.trigger.button = 0;
		e.trigger.interval = 0;
		e.direction = effect->direction;
		e.replay.delay = effect->delay;
		e.replay.length = effect->length;
	}
	else if(effect->type == SHAKE_EFFECT_CONSTANT)
	{
		e.type = FF_CONSTANT;
		e.id = effect->id;
		e.u.constant.level = effect->constant.level;
		e.u.constant.envelope.attack_length = effect->constant.envelope.attackLength;
		e.u.constant.envelope.attack_level = effect->constant.envelope.attackLevel;
		e.u.constant.envelope.fade_length = effect->constant.envelope.fadeLength;
		e.u.constant.envelope.fade_level = effect->constant.envelope.fadeLevel;
		e.trigger.button = 0;
		e.trigger.interval = 0;
		e.replay.delay = effect->delay;
		e.replay.length = effect->length;
	}
	else if(effect->type == SHAKE_EFFECT_RAMP)
	{
		e.type = FF_RAMP;
		e.id = effect->id;
		e.u.ramp.start_level = effect->ramp.startLevel;
		e.u.ramp.end_level = effect->ramp.endLevel;
		e.u.ramp.envelope.attack_length = effect->ramp.envelope.attackLength;
		e.u.ramp.envelope.attack_level = effect->ramp.envelope.attackLevel;
		e.u.ramp.envelope.fade_length = effect->ramp.envelope.fadeLength;
		e.u.ramp.envelope.fade_level = effect->ramp.envelope.fadeLevel;
		e.trigger.button = 0;
		e.trigger.interval = 0;
		e.replay.delay = effect->delay;
		e.replay.length = effect->length;
	}
	else
	{
		perror("Shake_UploadEffect: Unsupported effect.");
		return -2;
	}

	if (ioctl(dev->fd, EVIOCSFF, &e) == -1)
	{
		perror("Shake_UploadEffect: Failed to upload effect.");
		return -3;
	}

	return e.id;
}

void Shake_EraseEffect(Shake_Device *dev, int id)
{
	if (!dev)
		return;

	if (id < 0)
		return;

	if (ioctl(dev->fd, EVIOCRMFF, id) == -1)
	{
		perror("Shake_EraseEffect: Failed to erase effect.");
		return;
	}
}

void Shake_Play(Shake_Device *dev, int id)
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
		perror("Shake_Play: Failed to send play event.");
		return;
	}
}

void Shake_Stop(Shake_Device *dev, int id)
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
		perror("Shake_Stop: Failed to send stop event.");
		return;
	}
}

void Shake_Close(Shake_Device *dev)
{
	if (!dev)
		return;

	close(dev->fd);
}
