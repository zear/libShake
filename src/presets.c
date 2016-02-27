#include "shake.h"
#include "shake_private.h"
#include <limits.h>

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

void Shake_SimpleRumble(Shake_Effect *effect, float strongPercent, float weakPercent, float secs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_RUMBLE);
	effect->u.rumble.strongMagnitude = INT_MAX * strongPercent;
	effect->u.rumble.weakMagnitude = INT_MAX * weakPercent;
	effect->direction = 0x4000;
	effect->length = 1000 * secs;
	effect->delay = 0;
}
