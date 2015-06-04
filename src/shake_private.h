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

typedef enum shakeEffectType
{
	/* Force feedback effects */
	SHAKE_EFFECT_RUMBLE = 0,
	SHAKE_EFFECT_PERIODIC,
	SHAKE_EFFECT_CONSTANT,
	SHAKE_EFFECT_SPRING,
	SHAKE_EFFECT_FRICTION,
	SHAKE_EFFECT_DAMPER,
	SHAKE_EFFECT_INERTIA,
	SHAKE_EFFECT_RAMP,
	SHAKE_EFFECT_COUNT
} shakeEffectType;

typedef struct ShakeEffectCommon
{
	shakeEffectType type;
	int id;
	int length;
	int delay;
} shakeEffectCommon;

typedef struct ShakeEffectRumble
{
	shakeEffectCommon common;

	int strongMagnitude;
	int weakMagnitude;
} ShakeEffectRumble;

typedef struct ShakeEffectPeriodic
{
	shakeEffectCommon common;

	int waveform;
	int period;
	int magnitude;
	int offset;
	int phase;

	// envelope

	int customLen;
	// custom data
} ShakeEffectPeriodic;

typedef struct ShakeEffectConstant
{
	shakeEffectCommon common;
} ShakeEffectConstant;

typedef struct ShakeEffectSpring
{
	shakeEffectCommon common;
} ShakeEffectSpring;

typedef struct ShakeEffectFriction
{
	shakeEffectCommon common;
} ShakeEffectFriction;

typedef struct ShakeEffectDamper
{
	shakeEffectCommon common;
} ShakeEffectDamper;

typedef struct ShakeEffectInertia
{
	shakeEffectCommon common;
} ShakeEffectInertia;

typedef struct ShakeEffectRamp
{
	shakeEffectCommon common;
} ShakeEffectRamp;

typedef union shakeEffect
{
	shakeEffectCommon common;

	ShakeEffectRumble rumble;
	ShakeEffectPeriodic periodic;
	ShakeEffectConstant constant;
	ShakeEffectSpring spring;
	ShakeEffectFriction friction;
	ShakeEffectDamper damper;
	ShakeEffectInertia inertia;
	ShakeEffectRamp ramp;
} shakeEffect;

typedef struct shakeDev
{
	int fd;
	char *node;
	struct input_event play;
	struct input_event stop;
	unsigned long features[4];
	int n_effects; /* Number of effects the device can play at the same time */
} shakeDev;

typedef struct listElement
{
	struct listElement *next;
	shakeDev *dev;
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
