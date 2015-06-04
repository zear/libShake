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
		effect.periodic.period = 0.1*0x100;
		effect.periodic.magnitude = 0x6000;
		effect.common.length = 2000;

		id = shakeUploadEffect(device, effect);
		shakePlay(device, id);

		sleep(2);
		shakeEraseEffect(device, id);
		shakeClose(device);
	}

	shakeQuit();

	return 0;
}
