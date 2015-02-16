#ifndef _SHAKE_H_
#define _SHAKE_H_

typedef struct shakeDev
{
	int fd;
	char *node;
	struct ff_effect effect;
	struct input_event play;
	unsigned long features[4];
	int n_effects; /* Number of effects the device can play at the same time */
} shakeDev;

/* libShake functions */
int shakeInit();
void shakeQuit();
void shakeListDevices();
int shakeNumOfDevices();
shakeDev *shakeOpen(unsigned int id);
void shakeClose(shakeDev *dev);
int shakeQuery(shakeDev *dev);
int shakeSetEffect(shakeDev *dev, int effect, int duration);
void shakeUploadEffect(shakeDev *dev);
void shakePlay(shakeDev *dev);

#endif /* _SHAKE_H_ */
