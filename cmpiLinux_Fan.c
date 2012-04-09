#ifndef _XOPEN_SOURCE
    //this is for strdup
    #define _XOPEN_SOURCE 500
#endif
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <cmpi/cmpimacs.h>
#include <sblim/OSBase_Common.h>
#include <sblim/cmpiOSBase_Common.h>

#include "cmpiLinux_Fan.h"

static CMPIArray * _get_AccessibleFeatures(
	CMPIBroker const *_broker,
	CMPIStatus *rc,
	struct cim_fan const *sptr)
{
    int index = 0;
    uint16_t value = 1;
    unsigned int i = 1;

    CMPIArray *res = CMNewArray(_broker, 8, CMPI_uint16, rc);
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 8, CMPI_uint16, rc)");
	_OSBASE_TRACE(2, ("--- _get_AccessibleFeatures failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	while (i <= CIM_FAN_AF_FEATURE_MAX) {
	    if (i & sptr->accessible_features) {
		CMSetArrayElementAt(res, index++, (CMPIValue*)(&value),
		       	CMPI_uint16);
	    }
	    ++value;
	    i = i << 1;
	}
    }
    return res;
}

CMPIObjectPath * _makePath_Fan(
	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc)
{
    return _makePath_FanCommon(_ClassName, _broker, ctx, cop, sptr, rc);
}

CMPIInstance * _makeInst_Fan(
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        char const **properties,
        struct cim_fan *sptr,
        CMPIStatus *rc)
{
    CMPIObjectPath *op;
    CMPIInstance *ci;
    CMPIValue val;

    _OSBASE_TRACE(2, ("--- _makeInst_Fan() called"));
    if (!(ci = _makeInst_FanCommon(_ClassName, _broker, ctx, cop, properties,
	    sptr, rc, &op))) {
	return NULL;
    }

    // CoolingDevice
    val.boolean = true;
    CMSetProperty(ci, "ActiveCooling", &val, CMPI_boolean);

    // CIM_Fan
    // Linux_Fan
    val.array = _get_AccessibleFeatures(_broker, rc, sptr);
    if (!rc->rc) {
	CMSetProperty(ci, "AccessibleFeatures", &val, CMPI_uint16A);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_MIN_SPEED) {
	val.uint64 = (uint64_t) sptr->min_speed;
	CMSetProperty(ci, "MinSpeed", &val, CMPI_uint64);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_MAX_SPEED) {
	val.uint64 = (uint64_t) sptr->max_speed;
	CMSetProperty(ci, "MaxSpeed", &val, CMPI_uint64);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_DIV) {
	CMSetProperty(ci, "Divisor", &sptr->divisor, CMPI_uint32);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_PULSES) {
	CMSetProperty(ci, "Pulses", &sptr->pulses, CMPI_uint32);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_BEEP) {
	val.boolean = sptr->beep;
	CMSetProperty(ci, "Beep", &val, CMPI_boolean);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_ALARM) {
	CMSetProperty(ci, "Alarm", &sptr->alarm, CMPI_boolean);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_ALARM_MIN) {
	CMSetProperty(ci, "MinAlarm", &sptr->alarm_min, CMPI_boolean);
    }
    if (!rc->rc && sptr->accessible_features & CIM_FAN_AF_ALARM_MAX) {
	CMSetProperty(ci, "MaxAlarm", &sptr->alarm_max, CMPI_boolean);
    }

    _OSBASE_TRACE(2, ("--- _makeInst_Fan() exited"));
    return ci;
}

