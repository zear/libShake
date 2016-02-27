#ifndef _SHAKE_H_
#define _SHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SHAKE_MAJOR_VERSION 0
#define SHAKE_MINOR_VERSION 2
#define SHAKE_PATCH_VERSION 0

struct Shake_Device;
typedef struct Shake_Device Shake_Device;

typedef enum Shake_PeriodicWaveform
{
	SHAKE_PERIODIC_SQUARE = 0,
	SHAKE_PERIODIC_TRIANGLE,
	SHAKE_PERIODIC_SINE,
	SHAKE_PERIODIC_SAW_UP,
	SHAKE_PERIODIC_SAW_DOWN,
	SHAKE_PERIODIC_CUSTOM,
	SHAKE_PERIODIC_COUNT
} Shake_PeriodicWaveform;

typedef enum Shake_EffectType
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
} Shake_EffectType;

typedef struct Shake_Envelope
{
	int attackLength;
	int attackLevel;
	int fadeLength;
	int fadeLevel;
} Shake_Envelope;

typedef struct Shake_EffectRumble
{
	int strongMagnitude;
	int weakMagnitude;
} Shake_EffectRumble;

typedef struct Shake_EffectPeriodic
{
	Shake_PeriodicWaveform waveform;
	int period;
	int magnitude;
	int offset;
	int phase;
	Shake_Envelope envelope;
} Shake_EffectPeriodic;

typedef struct Shake_EffectConstant
{
	int level;
	Shake_Envelope envelope;
} Shake_EffectConstant;

typedef struct Shake_EffectRamp
{
	int startLevel;
	int endLevel;
	Shake_Envelope envelope;
} Shake_EffectRamp;

typedef struct Shake_Effect
{
	Shake_EffectType type;
	int id;
	int direction;
	int length;
	int delay;
	union
	{
		Shake_EffectRumble rumble;
		Shake_EffectPeriodic periodic;
		Shake_EffectConstant constant;
		Shake_EffectRamp ramp;
	} u;
} Shake_Effect;

/* libShake functions */
int Shake_Init();
void Shake_Quit();
int Shake_NumOfDevices();
Shake_Device *Shake_Open(unsigned int id);
void Shake_Close(const Shake_Device *dev);
int Shake_DeviceId(const Shake_Device *dev);
const char *Shake_DeviceName(const Shake_Device *dev);
int Shake_DeviceEffectCapacity(const Shake_Device *dev);
int Shake_QueryEffectSupport(const Shake_Device *dev, Shake_EffectType type);
int Shake_QueryWaveformSupport(const Shake_Device *dev, Shake_PeriodicWaveform waveform);
int Shake_QueryGainSupport(const Shake_Device *dev);
int Shake_QueryAutocenterSupport(const Shake_Device *dev);
void Shake_SetGain(const Shake_Device *dev, int gain);
void Shake_SetAutocenter(const Shake_Device *dev, int autocenter);
void Shake_InitEffect(Shake_Effect *effect, Shake_EffectType type);
int Shake_UploadEffect(const Shake_Device *dev, Shake_Effect *effect);
void Shake_EraseEffect(const Shake_Device *dev, int id);
void Shake_Play(const Shake_Device *dev, int id);
void Shake_Stop(const Shake_Device *dev, int id);

void Shake_SimplePeriodic(Shake_Effect *effect, Shake_PeriodicWaveform waveform, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs);
void Shake_SimpleRumble(Shake_Effect *effect, float strongPercent, float weakPercent, float secs);

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_H_ */
