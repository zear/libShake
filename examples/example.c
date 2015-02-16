#include <linux/input.h>
#include "shake.h"

int main()
{
	shakeDev *device;

	shakeInit();
	shakeListDevices();

	if (shakeNumOfDevices() > 0)
	{
		device = shakeOpen(0);

		shakeSetEffect(device, FF_PERIODIC, 2000);
		shakeUploadEffect(device);
		shakePlay(device);

		sleep(2);
		shakeClose(device);
	}

	shakeQuit();

	return 0;
}
