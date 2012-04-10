/* This file is part of cmpiLinux_FanProvider.
 *
 * cmpiLinux_FanProvider is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cmpiLinux_FanProvider is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cmpiLinux_FanProvider. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <cmpi/cmpimacs.h>
#include <sblim/OSBase_Common.h>
#include <sblim/cmpiOSBase_Common.h>

#include "cmpiLinux_FanSensor.h"
#include "Linux_FanUtil.h"

static CMPIArray * _get_PossibleStates(
	CMPIBroker const *_broker,
	CMPIStatus *rc,
	struct cim_fan const *sptr)
{
    int index = 0;

    CMPIArray *res = CMNewArray(_broker, 5, CMPI_string, rc);
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 5, CMPI_string, rc)");
	_OSBASE_TRACE(2, ("--- _get_AccessibleFeatures failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	if (sptr->accessible_features & CIM_FAN_AF_MIN_SPEED) {
	    CMSetArrayElementAt(res, index++, "Below Minimum", CMPI_chars);
	    CMSetArrayElementAt(res, index++, "At Minumum", CMPI_chars);
	}
	CMSetArrayElementAt(res, index++, "Normal", CMPI_chars);
	if (sptr->accessible_features & CIM_FAN_AF_MAX_SPEED) {
	    CMSetArrayElementAt(res, index++, "At Maximum", CMPI_chars);
	    CMSetArrayElementAt(res, index++, "Above Maximum", CMPI_chars);
	}
    }
    return res;
}

static char const * _get_CurrentState(
	struct cim_fan const *sptr)
{
    if (sptr->accessible_features & CIM_FAN_AF_MIN_SPEED) {
	if (sptr->speed < sptr->min_speed) return "Below Minimum";
	if (sptr->speed == sptr->min_speed) return "At Minimum";
    }
    if (sptr->accessible_features & CIM_FAN_AF_MAX_SPEED) {
	if (sptr->speed > sptr->max_speed) return "Above Maximum";
	if (sptr->speed == sptr->max_speed) return "At Maximum";
    }
    return "Normal";
}


CMPIObjectPath * _makePath_FanSensor(
	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc)
{
    return _makePath_FanCommon(_ClassName, _broker, ctx, cop, sptr, rc);
}

CMPIInstance * _makeInst_FanSensor(
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

    char buf[200];

    _OSBASE_TRACE(2, ("--- _makeInst_FanSensor() called"));
    if (!(ci = _makeInst_FanCommon(_ClassName, _broker, ctx, cop, properties,
	    sptr, rc, &op))) {
	return NULL;
    }

    // ManagedElement
    CMSetProperty(ci, "Caption", "Fan's tachometer", CMPI_chars);
    CMSetProperty(ci, "Description", "Associated sensor of fan."
           " Giving information about its speed.", CMPI_chars);
    snprintf(buf, 200, "Tachometer of fan \"%s\" on chip \"%s\"", sptr->name,
	    sptr->chip_name);
    CMSetProperty(ci, "ElementName", buf, CMPI_chars);

    // Sensor
    val.uint16 = 5; // Tachometer
    CMSetProperty(ci, "SensorType", &val, CMPI_uint16);
    val.array = _get_PossibleStates(_broker, rc, sptr);
    CMSetProperty(ci, "PossibleStates", &val, CMPI_stringA);
    CMSetProperty(ci, "CurrentState", _get_CurrentState(sptr), CMPI_chars);

    // NumericSensor
    val.uint16 = 38; // Revolutions
    CMSetProperty(ci, "BaseUnits", &val, CMPI_uint16);
    val.sint32 = 0;
    CMSetProperty(ci, "UnitModifier", &val, CMPI_sint32);
    val.uint16 = 4;  // Per Minute
    CMSetProperty(ci, "RateUnits", &val, CMPI_uint16);
    val.sint32 = (int32_t) sptr->speed;
    CMSetProperty(ci, "CurrentReading", &val, CMPI_sint32);
    if (sptr->accessible_features & CIM_FAN_AF_MAX_SPEED) {
	val.sint32 = (int32_t) sptr->min_speed;
	CMSetProperty(ci, "NormalMax", &val, CMPI_sint32);
    }
    if (sptr->accessible_features & CIM_FAN_AF_MIN_SPEED) {
	val.sint32 = (int32_t) sptr->min_speed;
	CMSetProperty(ci, "NormalMin", &val, CMPI_sint32);
    }
    val.sint32 = 0;
    CMSetProperty(ci, "MinReadable", &val, CMPI_sint32);
    val.boolean = true;
    CMSetProperty(ci, "IsLinear", &val, CMPI_boolean);

    // Linux_FanSensor

    _OSBASE_TRACE(2, ("--- _makeInst_FanSensor() exited"));
    return ci;
}

