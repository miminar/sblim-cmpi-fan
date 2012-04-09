#ifndef CMPILINUX_FANCOMMON_H_
#define CMPILINUX_FANCOMMON_H_

#include <cmpi/cmpidt.h>
#include "Linux_Fan.h"

CMPIObjectPath * _makePath_FanCommon(
	char const *class_name,
	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc);

CMPIInstance * _makeInst_FanCommon(
	char const *class_name,
	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        char const **properties,
        struct cim_fan *sptr,
        CMPIStatus *rc,
	CMPIObjectPath **op);

#endif /* ----- CMPILINUX_FANCOMMON_H_ ----- */
