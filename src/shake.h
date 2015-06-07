#ifndef _SHAKE_H_
#define _SHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct shakeDev;
typedef struct shakeDev shakeDev;

typedef enum shakePeriodicWaveform
{
	SHAKE_PERIODIC_SQUARE = 0,
	SHAKE_PERIODIC_TRIANGLE,
	SHAKE_PERIODIC_SINE,
	SHAKE_PERIODIC_SAW_UP,
	SHAKE_PERIODIC_SAW_DOWN,
	SHAKE_PERIODIC_CUSTOM,
	SHAKE_PERIODIC_COUNT
} ShakePeriodicWaveform;

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

typedef struct ShakeEffectRumble
{
	int strongMagnitude;
	int weakMagnitude;
} ShakeEffectRumble;

typedef struct ShakeEffectPeriodic
{
	ShakePeriodicWaveform waveform;
	int period;
	int magnitude;
	int offset;
	int phase;

	// envelope

	int customLen;
	// custom data
} ShakeEffectPeriodic;

typedef struct ShakeEffect
{
	shakeEffectType type;
	int id;
	int length;
	int delay;
	union {
		ShakeEffectRumble rumble;
		ShakeEffectPeriodic periodic;
	};
} shakeEffect;

/* libShake functions */
int shakeInit();
void shakeQuit();
void shakeListDevices();
int shakeNumOfDevices();
shakeDev *shakeOpen(unsigned int id);
void shakeClose(shakeDev *dev);
int shakeQuery(shakeDev *dev);
void shakeSetGain(shakeDev *dev, int gain);
void shakeInitEffect(shakeEffect *effect, shakeEffectType type);
int shakeUploadEffect(shakeDev *dev, shakeEffect effect);
void shakeEraseEffect(shakeDev *dev, int id);
void shakePlay(shakeDev *dev, int id);
void shakeStop(shakeDev *dev, int id);

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_H_ */
