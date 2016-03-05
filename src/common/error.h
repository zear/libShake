#ifndef _SHAKE_ERROR_H_
#define _SHAKE_ERROR_H_

#include "shake.h"

Shake_Status Shake_EcDevice();
Shake_Status Shake_EcEffectId();
Shake_Status Shake_EcComm();
Shake_Status Shake_EcQueryFeatures();
Shake_Status Shake_EcQueryCapacity();
Shake_Status Shake_EcAccess();
Shake_Status Shake_EcSupport();
Shake_Status Shake_EcUpload();
Shake_Status Shake_EcEffect();
Shake_Status Shake_EcErase();
Shake_ErrorCode Shake_GetErrorCode();

#endif
