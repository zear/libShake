#include "error.h"

Shake_ErrorCode errorCode = SHAKE_EC_UNSET;

/* Private functions */

Shake_Status Shake_EcDevice()
{
        errorCode = SHAKE_EC_DEVICE;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcEffectId()
{
        errorCode = SHAKE_EC_EFFECT_ID;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcComm()
{
        errorCode = SHAKE_EC_COMM;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcQueryFeatures()
{
        errorCode = SHAKE_EC_QUERY_FEATURES;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcQueryCapacity()
{
        errorCode = SHAKE_EC_QUERY_CAPACITY;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcAccess()
{
        errorCode = SHAKE_EC_ACCESS;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcSupport()
{
        errorCode = SHAKE_EC_SUPPORT;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcUpload()
{
        errorCode = SHAKE_EC_UPLOAD;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcEffect()
{
        errorCode = SHAKE_EC_EFFECT;
        return SHAKE_ERROR;
}

Shake_Status Shake_EcErase()
{
        errorCode = SHAKE_EC_ERASE;
        return SHAKE_ERROR;
}

/* Public functions */

Shake_ErrorCode Shake_GetErrorCode()
{
        return errorCode;
}
