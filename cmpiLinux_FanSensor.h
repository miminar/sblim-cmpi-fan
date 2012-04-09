#ifndef CMPILINUX_FANSENSOR_H_
#define CMPILINUX_FANSENSOR_H_

#include "cmpiLinux_FanCommon.h"

static char const *_ClassName = "Linux_FanSensor";

CMPIObjectPath * _makePath_FanSensor(
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc);

CMPIInstance * _makeInst_FanSensor(
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        char const **properties,
        struct cim_fan *sptr,
        CMPIStatus *rc);

#endif /* ----- CMPILINUX_FANSENSOR_H_ ----- */

