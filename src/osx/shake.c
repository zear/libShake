/* libShake - a basic haptic library */

#include <ForceFeedback/ForceFeedback.h>
#include <ForceFeedback/ForceFeedbackConstants.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>

#include "shake.h"
#include "./shake_private.h"
#include "../common/helpers.h"

listElement *listHead;
unsigned int numOfDevices;

/* Prototypes */

int Shake_Probe(Shake_Device *dev);
int Shake_Query(Shake_Device *dev);
int convertMagnitude(int magnitude);

int convertMagnitude(int magnitude)
{
	return ((float)magnitude/0x8fff) * FF_FFNOMINALMAX;
}

/* Public functions */

int Shake_Init()
{
	IOReturn ret;
	io_iterator_t iter;
	CFDictionaryRef match;
	io_service_t device;

	match = IOServiceMatching(kIOHIDDeviceKey);

	if (!match)
	{
		return -1;
	}

	ret = IOServiceGetMatchingServices(kIOMasterPortDefault, match, &iter);

	if (ret != kIOReturnSuccess)
	{
		return -1;
	}

	if (!IOIteratorIsValid(iter))
	{
		return -1;
	}

	while ((device = IOIteratorNext(iter)) != IO_OBJECT_NULL)
	{
		Shake_Device dev;
		dev.service = device;
		dev.effectList = NULL;

		if (Shake_Probe(&dev) > 0)
		{
			dev.id = numOfDevices;
			listHead = listElementPrepend(listHead);
			listHead->item = malloc(sizeof(Shake_Device));
			memcpy(listHead->item, &dev, sizeof(Shake_Device));
			++numOfDevices;
		}
		else
		{
			IOObjectRelease(device);
		}
	}

	IOObjectRelease(iter);

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
			if (dev->service)
				IOObjectRelease(dev->service);
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
	if ((FFIsForceFeedback(dev->service)) != FF_OK)
	{
		return -1;
	}

	if (Shake_Query(dev))
	{
		return -1;
	}

	return 1;
}

Shake_Device *Shake_Open(unsigned int id)
{
	HRESULT result;
	Shake_Device *dev;

	if (id >= numOfDevices)
		return NULL;

	listElement *element = listElementGet(listHead, numOfDevices - 1 - id);
	dev = (Shake_Device *)element->item;

	if(!dev || !dev->service)
		return NULL;

	result = FFCreateDevice(dev->service, &dev->device);

	if (result != FF_OK)
	{
		return NULL;
	}

	return dev;
}

int Shake_Query(Shake_Device *dev)
{
	HRESULT result;
	io_name_t deviceName;

	if(!dev)
		return -1;

	if(!dev->service)
	{
		return -1;
	}

	result = FFCreateDevice(dev->service, &dev->device);

	if (result != FF_OK)
	{
		perror("Shake_Query: Failed to create device");
		return -1;
	}

	result = FFDeviceGetForceFeedbackCapabilities(dev->device, &dev->features);
	if (result != FF_OK)
	{
		perror("Shake_Query: Failed to query for device features");
		return -1;
	}

	if (!dev->features.supportedEffects) /* Device doesn't support any force feedback effects. Ignore it. */
	{
		return -1;
	}

	dev->capacity = dev->features.storageCapacity;

	if (dev->capacity <= 0) /* Device doesn't support uploading effects. Ignore it. */
		return -1;

	IORegistryEntryGetName(dev->service, deviceName); /* Get device name */
	if (strlen((char *)deviceName))
	{
		strncpy(dev->name, (char *)deviceName, sizeof(dev->name));
	}
	else
	{
		strncpy(dev->name, "Unknown", sizeof(dev->name));
	}

	if (FFReleaseDevice(dev->device) == FF_OK)
	{
		dev->device = 0;
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
	FFCapabilitiesEffectType query;

	switch (type)
	{
		case SHAKE_EFFECT_RUMBLE:
			return 0; // TODO
		break;
		case SHAKE_EFFECT_PERIODIC:
		{
			Shake_PeriodicWaveform waveform;

			for (waveform = SHAKE_PERIODIC_SQUARE; waveform < SHAKE_PERIODIC_COUNT; ++waveform)
			{
				if (Shake_QueryWaveformSupport(dev, waveform))
					return 1;
			}

			return 0;
		}
		break;
		case SHAKE_EFFECT_CONSTANT:
			query = FFCAP_ET_CONSTANTFORCE;
		break;
		case SHAKE_EFFECT_SPRING:
			query = FFCAP_ET_SPRING;
		break;
		case SHAKE_EFFECT_FRICTION:
			query = FFCAP_ET_FRICTION;
		break;
		case SHAKE_EFFECT_DAMPER:
			query = FFCAP_ET_DAMPER;
		break;
		case SHAKE_EFFECT_INERTIA:
			query = FFCAP_ET_INERTIA;
		break;
		case SHAKE_EFFECT_RAMP:
			query = FFCAP_ET_RAMPFORCE;
		break;

		default:
		return 0;
	}

	return test_bit(query, dev->features.supportedEffects) ? 1 : 0;
}

int Shake_QueryWaveformSupport(Shake_Device *dev, Shake_PeriodicWaveform waveform)
{
	FFCapabilitiesEffectType query;

	switch (waveform)
	{
		case SHAKE_PERIODIC_SQUARE:
			query = FFCAP_ET_SQUARE;
		break;
		case SHAKE_PERIODIC_TRIANGLE:
			query = FFCAP_ET_TRIANGLE;
		break;
		case SHAKE_PERIODIC_SINE:
			query = FFCAP_ET_SINE;
		break;
		case SHAKE_PERIODIC_SAW_UP:
			query = FFCAP_ET_SAWTOOTHUP;
		break;
		case SHAKE_PERIODIC_SAW_DOWN:
			query = FFCAP_ET_SAWTOOTHDOWN;
		break;
		case SHAKE_PERIODIC_CUSTOM:
			query = FFCAP_ET_CUSTOMFORCE;
		break;

		default:
		return 0;
	}

	return test_bit(query, dev->features.supportedEffects) ? 1 : 0;
}

int Shake_QueryGainSupport(Shake_Device *dev)
{
	HRESULT result;
	int value = 0; /* Unused for now. */

	result = FFDeviceGetForceFeedbackProperty(dev->device, FFPROP_FFGAIN, &value, sizeof(value));

	if (result == FF_OK)
	{
		return 1;
	}
	else if (result != FFERR_UNSUPPORTED)
	{
		perror("Shake_QueryGainSupport: Failed to query for gain support");
	}

	return 0;
}

int Shake_QueryAutocenterSupport(Shake_Device *dev)
{
	HRESULT result;
	int value = 0; /* Unused for now. */

	result = FFDeviceGetForceFeedbackProperty(dev->device, FFPROP_AUTOCENTER, &value, sizeof(value));

	if (result == FF_OK)
	{
		return 1;
	}
	else if (result != FFERR_UNSUPPORTED)
	{
		perror("Shake_QueryAutocenterSupport: Failed to query for autocenter support");
	}

	return 0;
}

void Shake_SetGain(Shake_Device *dev, int gain)
{
	if (!dev)
		return;

	if (gain < 0)
		gain = 0;
	if (gain > 100)
		gain = 100;

	gain = ((float)gain/100) * FF_FFNOMINALMAX;

	if (FFDeviceSetForceFeedbackProperty(dev->device, FFPROP_FFGAIN, &gain) != FF_OK)
	{
		perror("Shake_SetGain: Failed to set gain");
	}
}

void Shake_SetAutocenter(Shake_Device *dev, int autocenter)
{
	if (!dev)
		return;

	if (autocenter) /* OSX supports only OFF and ON values */
	{
		autocenter = 1;
	}

	if (FFDeviceSetForceFeedbackProperty(dev->device, FFPROP_AUTOCENTER, &autocenter) != FF_OK)
	{
		perror("Shake_SetAutocenter: Failed to set auto-center");
	}
}

void Shake_InitEffect(Shake_Effect *effect, Shake_EffectType type)
{
	if (!effect)
		return;
	memset(effect, 0, sizeof(*effect));
	if (type >= SHAKE_EFFECT_COUNT)
		perror("Shake_InitEffect: Unsupported effect");
	effect->type = type;
	effect->id = -1;
}

int Shake_UploadEffect(Shake_Device *dev, Shake_Effect *effect)
{
	HRESULT result;
	FFEFFECT e;
	CFUUIDRef effectType;
	effectContainer *container;
	DWORD rgdwAxes[2];
	LONG rglDirection[2];

	if (!dev)
		return -1;
	if (!effect)
		return -1;
	if (effect->id < -1)
		return -1;

	rgdwAxes[0] = FFJOFS_X;
	rgdwAxes[1] = FFJOFS_Y;
	rglDirection[0] = effect->direction;
	rglDirection[1] = 0;

	/* Global parameters. */
	memset(&e, 0, sizeof(e));
	e.dwSize = sizeof(FFEFFECT);
	e.dwFlags = FFEFF_POLAR | FFEFF_OBJECTOFFSETS;
	e.dwDuration = effect->length * 1000;
	e.dwSamplePeriod = 0;
	e.cAxes = 2;
	e.rgdwAxes = rgdwAxes;
	e.rglDirection = rglDirection;
	e.dwStartDelay = effect->delay;

	e.dwGain = FF_FFNOMINALMAX;

/*
	if(effect->type == SHAKE_EFFECT_RUMBLE)
	{
		// TODO
	}
*/
	if(effect->type == SHAKE_EFFECT_PERIODIC)
	{
		FFPERIODIC pf;

		switch (effect->u.periodic.waveform)
		{
			case SHAKE_PERIODIC_SQUARE:
				effectType = kFFEffectType_Square_ID;
			break;
			case SHAKE_PERIODIC_TRIANGLE:
				effectType = kFFEffectType_Triangle_ID;
			break;
			case SHAKE_PERIODIC_SINE:
				effectType = kFFEffectType_Sine_ID;
			break;
			case SHAKE_PERIODIC_SAW_UP:
				effectType = kFFEffectType_SawtoothUp_ID;
			break;
			case SHAKE_PERIODIC_SAW_DOWN:
				effectType = kFFEffectType_SawtoothDown_ID;
			break;
			case SHAKE_PERIODIC_CUSTOM:
				effectType = kFFEffectType_CustomForce_ID;
			break;

			default:
				perror("Shake_UploadEffect: Unsupported waveform");
			return -2;
		}

		pf.dwMagnitude = convertMagnitude(effect->u.periodic.magnitude);
		pf.lOffset = effect->u.periodic.offset;
		pf.dwPhase = effect->u.periodic.phase;
		pf.dwPeriod = effect->u.periodic.period * 1000;

		e.lpEnvelope = malloc(sizeof(FFENVELOPE));
		e.lpEnvelope->dwSize = sizeof(FFENVELOPE);
		e.lpEnvelope->dwAttackTime = effect->u.periodic.envelope.attackLength * 1000;
		e.lpEnvelope->dwAttackLevel = effect->u.periodic.envelope.attackLevel;
		e.lpEnvelope->dwFadeTime = effect->u.periodic.envelope.fadeLength * 1000;
		e.lpEnvelope->dwFadeLevel = effect->u.periodic.envelope.fadeLevel;
		e.dwTriggerButton = FFEB_NOTRIGGER;
		e.dwTriggerRepeatInterval = 0;

		e.cbTypeSpecificParams = sizeof(FFPERIODIC);
		e.lpvTypeSpecificParams = &pf;
	}

	else if(effect->type == SHAKE_EFFECT_CONSTANT)
	{
		FFCONSTANTFORCE cf;

		cf.lMagnitude = convertMagnitude(effect->u.constant.level);

		effectType = kFFEffectType_ConstantForce_ID;
		e.lpEnvelope = malloc(sizeof(FFENVELOPE));
		e.lpEnvelope->dwSize = sizeof(FFENVELOPE);
		e.lpEnvelope->dwAttackTime = effect->u.constant.envelope.attackLength * 1000;
		e.lpEnvelope->dwAttackLevel = effect->u.constant.envelope.attackLevel;
		e.lpEnvelope->dwFadeTime = effect->u.constant.envelope.fadeLength * 1000;
		e.lpEnvelope->dwFadeLevel = effect->u.constant.envelope.fadeLevel;
		e.dwTriggerButton = FFEB_NOTRIGGER;
		e.dwTriggerRepeatInterval = 0;

		e.cbTypeSpecificParams = sizeof(FFCONSTANTFORCE);
		e.lpvTypeSpecificParams = &cf;
	}

	else if(effect->type == SHAKE_EFFECT_RAMP)
	{
		FFRAMPFORCE rf;

		rf.lStart = ((float)effect->u.ramp.startLevel/0xffff) * FF_FFNOMINALMAX;
		rf.lEnd = ((float)effect->u.ramp.endLevel/0xffff) * FF_FFNOMINALMAX;

		effectType = kFFEffectType_RampForce_ID;
		e.lpEnvelope = malloc(sizeof(FFENVELOPE));
		e.lpEnvelope->dwSize = sizeof(FFENVELOPE);
		e.lpEnvelope->dwAttackTime = effect->u.ramp.envelope.attackLength * 1000;
		e.lpEnvelope->dwAttackLevel = effect->u.ramp.envelope.attackLevel;
		e.lpEnvelope->dwFadeTime = effect->u.ramp.envelope.fadeLength * 1000;
		e.lpEnvelope->dwFadeLevel = effect->u.ramp.envelope.fadeLevel;
		e.dwTriggerButton = FFEB_NOTRIGGER;
		e.dwTriggerRepeatInterval = 0;

		e.cbTypeSpecificParams = sizeof(FFRAMPFORCE);
		e.lpvTypeSpecificParams = &rf;
	}

	else
	{
		perror("Shake_UploadEffect: Unsupported effect");
		return -2;
	}

	dev->effectList = listElementPrepend(dev->effectList);
	dev->effectList->item = malloc(sizeof(effectContainer));
	container = dev->effectList->item;
	container->id = listLength(dev->effectList) - 1;
	container->effect = 0;

	result = FFDeviceCreateEffect(dev->device, effectType, &e, &container->effect);

	free(e.lpEnvelope);

	if ((unsigned int)result != FF_OK)
	{
		dev->effectList = listElementDelete(dev->effectList, dev->effectList);
	}

	if ((unsigned int)result == FFERR_EFFECTTYPEMISMATCH)
	{
		perror("Shake_UploadEffect: Effect type mismatch");
		return -3;
	}
	else if ((unsigned int)result == FFERR_EFFECTTYPENOTSUPPORTED)
	{
		perror("Shake_UploadEffect: Effect not supported");
		return -3;
	}
	else if ((unsigned int)result == FFERR_DEVICEFULL)
	{
		perror("Shake_UploadEffect: Device is full");
		return -3;
	}
	else if ((unsigned int)result == FFERR_INVALIDPARAM)
	{
		perror("Shake_UploadEffect: Invalid parameter");
		return -3;
	}
	else if ((unsigned int)result != FF_OK)
	{
		perror("Shake_UploadEffect: Failed to create device");
		return -3;
	}

	return ((effectContainer *)dev->effectList->item)->id;
}

void Shake_EraseEffect(Shake_Device *dev, int id)
{
	listElement *node;
	effectContainer *effect = NULL;

	if(!dev)
		return;

	if(id < 0)
		return;

	node = dev->effectList;

	while (node)
	{
		effect = (effectContainer *)node->item;
		if (effect->id == id)
		{
			break;
		}

		node = node->next;
	}

	if (!node || !effect)
	{
		return;
	}

	if (FFDeviceReleaseEffect(dev->device, effect->effect))
	{
		perror("Shake_EraseEffect: Failed to erase effect");
		return;
	}

	dev->effectList = listElementDelete(dev->effectList, node);
}

void Shake_Play(Shake_Device *dev, int id)
{
	listElement *node;
	effectContainer *effect = NULL;

	if(!dev)
		return;

	if(id < 0)
		return;

	node = dev->effectList;

	while (node)
	{
		effect = (effectContainer *)node->item;
		if (effect->id == id)
		{
			break;
		}

		node = node->next;
	}

	if (!node || !effect)
	{
		return;
	}

	if (FFEffectStart(effect->effect, 1, 0) != FF_OK)
	{
		perror("Shake_Play: Failed to send play event");
		return;
	}
}

void Shake_Stop(Shake_Device *dev, int id)
{
	listElement *node;
	effectContainer *effect = NULL;

	if(!dev)
		return;

	if(id < 0)
		return;

	node = dev->effectList;

	while (node)
	{
		effect = (effectContainer *)node->item;
		if (effect->id == id)
		{
			break;
		}

		node = node->next;
	}

	if (!node || !effect)
	{
		return;
	}

	if (FFEffectStop(effect->effect))
	{
		perror("Shake_Stop: Failed to send stop event");
		return;
	}
}

void Shake_Close(Shake_Device *dev)
{
	int effectLen;
	int i;

	if (!dev)
		return;
	if (!dev->device)
		return;

	effectLen = listLength(dev->effectList);

	for (i = 0; i < effectLen; ++i)
	{
		effectContainer *effect = (effectContainer *)listElementGet(dev->effectList, i);
		if (FFDeviceReleaseEffect(dev->device, effect->effect))
		{
			perror("Shake_Close: Failed to release effect");
			return;
		}
	}

	dev->effectList = listElementDeleteAll(dev->effectList);
	if (FFReleaseDevice(dev->device) == FF_OK)
	{
		dev->device = 0;
	}
}
