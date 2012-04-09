#ifndef CMPILINUX_FAN_H_
#define CMPILINUX_FAN_H_

#include "cmpiLinux_FanCommon.h"

static char const *_ClassName = "Linux_Fan";

CMPIObjectPath * _makePath_Fan(
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc);

CMPIInstance * _makeInst_Fan(
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        char const **properties,
        struct cim_fan *sptr,
        CMPIStatus *rc);

#endif /* ----- CMPILINUX_FAN_H_ ----- */

