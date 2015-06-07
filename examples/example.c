#include "shake.h"

int main()
{
	shakeDev *device;
	shakeEffect effect;
	int id;

	shakeInit();
	shakeListDevices();

	if (shakeNumOfDevices() > 0)
	{
		device = shakeOpen(0);

		shakeInitEffect(&effect, SHAKE_EFFECT_PERIODIC);
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

		id = shakeUploadEffect(device, effect);
		shakePlay(device, id);

		sleep(2);
		shakeEraseEffect(device, id);
		shakeClose(device);
	}

	shakeQuit();

	return 0;
}
