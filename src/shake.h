#ifndef _SHAKE_H_
#define _SHAKE_H_

#include "shake_private.h"

struct shakeDev;

/* libShake functions */
int shakeInit();
void shakeQuit();
void shakeListDevices();
int shakeNumOfDevices();
shakeDev *shakeOpen(unsigned int id);
void shakeClose(shakeDev *dev);
int shakeQuery(shakeDev *dev);
void shakeInitEffect(shakeEffect *effect, shakeEffectType type);
int shakeUploadEffect(shakeDev *dev, shakeEffect effect);
void shakeEraseEffect(shakeDev *dev, int id);
void shakePlay(shakeDev *dev, int id);
void shakeStop(shakeDev *dev, int id);

#endif /* _SHAKE_H_ */
