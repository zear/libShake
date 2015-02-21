#ifndef _SHAKE_H_
#define _SHAKE_H_

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
	shakeEffect effect;
	//struct ff_effect effect;
	struct input_event play;
	unsigned long features[4];
	int n_effects; /* Number of effects the device can play at the same time */
} shakeDev;

/* libShake functions */
int shakeInit();
void shakeQuit();
void shakeListDevices();
int shakeNumOfDevices();
shakeDev *shakeOpen(unsigned int id);
void shakeClose(shakeDev *dev);
int shakeQuery(shakeDev *dev);
void shakeInitEffect(shakeEffect *effect, shakeEffectType type);
int shakeSetEffect(shakeDev *dev, shakeEffect effect, int duration);
void shakeUploadEffect(shakeDev *dev);
void shakePlay(shakeDev *dev);

#endif /* _SHAKE_H_ */
