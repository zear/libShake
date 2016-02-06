#ifndef _SHAKE_PRIVATE_H_
#define _SHAKE_PRIVATE_H_

#if defined(PLATFORM_OSX)

#include <ForceFeedback/ForceFeedback.h>
#include <IOKit/IOKitLib.h>
#include "helpers.h"

#define BITS_PER_LONG		(sizeof(long) * 8)
#define OFF(x)			((x)%BITS_PER_LONG)
#define BIT(x)			(1UL<<OFF(x))
#define LONG(x)			((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array >> OFF(bit)) & 1)
#define BITS_TO_LONGS(x) \
	(((x) + 8 * sizeof (unsigned long) - 1) / (8 * sizeof (unsigned long)))

typedef struct effectContainer
{
	int id;
	FFEffectObjectReference effect;
} effectContainer;

typedef struct Shake_Device
{
	char name[128];
	int id;
	int capacity; /* Number of effects the device can play at the same time */
	/* Platform dependent section */
	io_service_t service;
	FFDeviceObjectReference device;
	listElement *effectList;
	FFCAPABILITIES features;
} Shake_Device;

extern listElement *listHead;
extern unsigned int numOfDevices;

#endif /* PLATFORM_OSX */

#endif /* _SHAKE_PRIVATE_H_ */
