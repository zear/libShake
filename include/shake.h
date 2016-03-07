#ifndef _SHAKE_H_
#define _SHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SHAKE_MAJOR_VERSION 0
#define SHAKE_MINOR_VERSION 2
#define SHAKE_PATCH_VERSION 0

#define SHAKE_RANGE_MAX	0x7FFF

typedef enum Shake_Status
{
	SHAKE_ERROR	= -1,
	SHAKE_OK	= 0,
} Shake_Status;

typedef enum Shake_Bool
{
	SHAKE_FALSE	= 0,
	SHAKE_TRUE	= 1
} Shake_Bool;

typedef enum Shake_ErrorCode {
	SHAKE_EC_UNSET,
	SHAKE_EC_SUPPORT,
	SHAKE_EC_DEVICE,
	SHAKE_EC_EFFECT,
	SHAKE_EC_QUERY,
	SHAKE_EC_ARG,
	SHAKE_EC_TRANSFER
} Shake_ErrorCode;

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
Shake_Status Shake_Init();
void Shake_Quit();
int Shake_NumOfDevices();
Shake_Device *Shake_Open(unsigned int id);
Shake_Status Shake_Close(Shake_Device *dev);
int Shake_DeviceId(Shake_Device *dev);
const char *Shake_DeviceName(Shake_Device *dev);
int Shake_DeviceEffectCapacity(Shake_Device *dev);
Shake_Bool Shake_QueryEffectSupport(Shake_Device *dev, Shake_EffectType type);
Shake_Bool Shake_QueryWaveformSupport(Shake_Device *dev, Shake_PeriodicWaveform waveform);
Shake_Bool Shake_QueryGainSupport(Shake_Device *dev);
Shake_Bool Shake_QueryAutocenterSupport(Shake_Device *dev);
Shake_Status Shake_SetGain(Shake_Device *dev, int gain);
Shake_Status Shake_SetAutocenter(Shake_Device *dev, int autocenter);
Shake_Status Shake_InitEffect(Shake_Effect *effect, Shake_EffectType type);
int Shake_UploadEffect(Shake_Device *dev, Shake_Effect *effect);
Shake_Status Shake_EraseEffect(Shake_Device *dev, int id);
Shake_Status Shake_Play(Shake_Device *dev, int id);
Shake_Status Shake_Stop(Shake_Device *dev, int id);

void Shake_SimpleRumble(Shake_Effect *effect, float strongPercent, float weakPercent, float secs);
void Shake_SimplePeriodic(Shake_Effect *effect, Shake_PeriodicWaveform waveform, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs);
void Shake_SimpleConstant(Shake_Effect *effect, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs);
void Shake_SimpleRamp(Shake_Effect *effect, float startForcePercent, float endForcePercent, float attackSecs, float sustainSecs, float fadeSecs);

Shake_ErrorCode Shake_GetErrorCode();

#ifdef __cplusplus
}
#endif

#endif /* _SHAKE_H_ */
