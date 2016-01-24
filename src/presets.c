#include "shake.h"
#include "shake_private_linux.h"
#include "helpers.h"
#include <limits.h>

void Shake_SimplePeriodic(Shake_Effect *effect, Shake_PeriodicWaveform waveform, float forcePercent, float attackSecs, float sustainSecs, float fadeSecs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_PERIODIC);
	effect->periodic.waveform = waveform;
	effect->periodic.period = 0.1*0x100;
	effect->periodic.magnitude = 0x8fff * forcePercent;
	effect->periodic.envelope.attackLength = 1000 * attackSecs;
	effect->periodic.envelope.attackLevel = 0;
	effect->periodic.envelope.fadeLength = 1000 * fadeSecs;
	effect->periodic.envelope.fadeLevel = 0;
	effect->direction = 0x4000;
	effect->length = 1000 * (sustainSecs + attackSecs + fadeSecs);
	effect->delay = 0;
}

void Shake_SimpleRumble(Shake_Effect *effect, float strongPercent, float weakPercent, float secs)
{
	Shake_InitEffect(effect, SHAKE_EFFECT_RUMBLE);
	effect->rumble.strongMagnitude = INT_MAX * strongPercent;
	effect->rumble.weakMagnitude = INT_MAX * weakPercent;
	effect->direction = 0x4000;
	effect->length = 1000 * secs;
	effect->delay = 0;
}
