#include "shake.h"
#include <stdio.h>

void deviceInfo(Shake_Device *device)
{
	printf(" Adjustable gain: %s\n", Shake_QueryGainAdjustable(device) ? "yes" : "no"); 
	printf(" Effect capacity: %d\n", Shake_EffectCapacity(device));
	printf(" Supported effects:\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_RUMBLE)) printf(" SHAKE_EFFECT_RUMBLE\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_PERIODIC))
	{
		printf(" SHAKE_EFFECT_PERIODIC\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_SQUARE)) printf(" * SHAKE_PERIODIC_SQUARE\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_TRIANGLE)) printf(" * SHAKE_PERIODIC_TRIANGLE\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_SINE)) printf(" * SHAKE_PERIODIC_SINE\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_SAW_UP)) printf(" * SHAKE_PERIODIC_SAW_UP\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_SAW_DOWN)) printf(" * SHAKE_PERIODIC_SAW_DOWN\n");
		if (Shake_QueryPeriodicWaveform(device, SHAKE_PERIODIC_CUSTOM)) printf(" * SHAKE_PERIODIC_CUSTOM\n");
	}
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_CONSTANT)) printf(" SHAKE_EFFECT_CONSTANT\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_SPRING)) printf(" SHAKE_EFFECT_SPRING\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_FRICTION)) printf(" SHAKE_EFFECT_FRICTION\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_DAMPER)) printf(" SHAKE_EFFECT_DAMPER\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_INERTIA)) printf(" SHAKE_EFFECT_INERTIA\n");
	if (Shake_QueryEffectType(device, SHAKE_EFFECT_RAMP)) printf(" SHAKE_EFFECT_RAMP\n");
}

int main()
{
	Shake_Device *device;
	Shake_Effect effect;
	int id;

	Shake_Init();
	printf("Detected devices: %d\n", Shake_NumOfDevices());

	if (Shake_NumOfDevices() > 0)
	{
		device = Shake_Open(0);
		printf("\nDevice #0\n");
		deviceInfo(device);

		Shake_InitEffect(&effect, SHAKE_EFFECT_PERIODIC);
		effect.periodic.waveform		= SHAKE_PERIODIC_SINE;
		effect.periodic.period			= 0.1*0x100;
		effect.periodic.magnitude		= 0x6000;
		effect.periodic.envelope.attackLength	= 0x100;
		effect.periodic.envelope.attackLevel	= 0;
		effect.periodic.envelope.fadeLength	= 0x100;
		effect.periodic.envelope.fadeLevel	= 0;
		effect.direction			= 0x4000;
		effect.length				= 2000;
		effect.delay				= 0;

		id = Shake_UploadEffect(device, effect);
		Shake_Play(device, id);

		sleep(2);
		Shake_EraseEffect(device, id);
		Shake_Close(device);
	}

	Shake_Quit();

	return 0;
}
