#include "shake.h"
#include "helpers.h"
#include <limits.h>

void Shake_SimpleRumble(Shake_Effect *effect, float strongPercent, float weakPercent, float secs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_RUMBLE);
	effect->u.rumble.strongMagnitude = INT_MAX * strongPercent;
	effect->u.rumble.weakMagnitude = INT_MAX * weakPercent;
	effect->direction = 0x4000;
	effect->length = 1000 * secs;
	effect->delay = 0;
}

void Shake_SimplePeriodic(Shake_Effect *effect, Shake_PeriodicWaveform waveform, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_PERIODIC);
	effect->u.periodic.waveform = waveform;
	effect->u.periodic.period = 0.1*0x100;
	effect->u.periodic.magnitude = 0x8fff * forcePercent;
	effect->u.periodic.envelope.attackLength = 1000 * attackSecs;
	effect->u.periodic.envelope.attackLevel = 0;
	effect->u.periodic.envelope.fadeLength = 1000 * fadeSecs;
	effect->u.periodic.envelope.fadeLevel = 0;
	effect->direction = 0x4000;
	effect->length = 1000 * (sustainSecs + attackSecs + fadeSecs);
	effect->delay = 0;
}

void Shake_SimpleConstant(Shake_Effect *effect, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_CONSTANT);
	effect->u.constant.level = 0x8fff * forcePercent;
	effect->u.constant.envelope.attackLength = 1000 * attackSecs;
	effect->u.constant.envelope.attackLevel = 0;
	effect->u.constant.envelope.fadeLength = 1000 * fadeSecs;
	effect->u.constant.envelope.fadeLevel = 0;
	effect->length = 1000 * (sustainSecs + attackSecs + fadeSecs);
	effect->delay = 0;
}

void Shake_SimpleRamp(Shake_Effect *effect, float startForcePercent, float endForcePercent, float attackSecs, float sustainSecs, float fadeSecs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_RAMP);
	effect->u.ramp.startLevel = 0x8fff * startForcePercent;
	effect->u.ramp.endLevel = 0x8fff * endForcePercent;
	effect->u.ramp.envelope.attackLength = 1000 * attackSecs;
	effect->u.ramp.envelope.attackLevel = 0;
	effect->u.ramp.envelope.fadeLength = 1000 * fadeSecs;
	effect->u.ramp.envelope.fadeLevel = 0;
	effect->length = 1000 * (sustainSecs + attackSecs + fadeSecs);
	effect->delay = 0;
}
