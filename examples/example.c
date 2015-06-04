#include "shake.h"

int main()
{
	shakeDev *device;
	shakeEffect effect;


	shakeInit();
	shakeListDevices();

	if (shakeNumOfDevices() > 0)
	{
		device = shakeOpen(0);

		shakeInitEffect(&effect, SHAKE_EFFECT_PERIODIC);
		effect.periodic.period = 0.1*0x100;
		effect.periodic.magnitude = 0x6000;

		if (!shakeSetEffect(device, effect, 2000))
		{
			shakeUploadEffect(device);
			shakePlay(device);
		}

		sleep(2);
		shakeClose(device);
	}

	shakeQuit();

	return 0;
}
