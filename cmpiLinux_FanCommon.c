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

#include "cmpiLinux_FanCommon.h"

#define UNUSED(x) ((void) (x))

static CMPIArray * _get_OperationalStatus(
       	CMPIBroker const *_broker,
       	CMPIStatus *rc,
       	struct cim_fan const *sptr)
{
    CMPIArray *res = CMNewArray(_broker, 2, CMPI_uint16, rc);
    unsigned short val;
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 2, CMPI_uint16, rc)");
	_OSBASE_TRACE(2, ("--- _get_OperationalStatus failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	val = sptr->fault ? 3:1; // Error : OK
	*rc = CMSetArrayElementAt(res, 0, (CMPIValue*)(&val), CMPI_uint16);
	if (sptr->alarm || sptr->alarm_min || sptr->alarm_max) {
	    val = 4; // Stressed
	    *rc = CMSetArrayElementAt(res, 1, (CMPIValue*)(&val), CMPI_uint16);
	}
    }
    return res;
}

static CMPIArray * _get_StatusDescriptions(
       	CMPIBroker const *_broker,
       	CMPIStatus *rc,
       	struct cim_fan const *sptr)
{
    CMPIArray *res = CMNewArray(_broker, 2, CMPI_string, rc);
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 2, CMPI_string, rc)");
	_OSBASE_TRACE(2, ("--- _get_StatusDescriptions failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	*rc = CMSetArrayElementAt(res, 0, sptr->fault ?
	       "Chip indicates, that fan is in fault state."
	       " Possible causes are open diodes, unconnected fan etc."
	       " Thus the measurement for this channel should not be trusted."
	    :  "Fan seems to be functioning correctly.", CMPI_chars);
	if (!rc->rc && (sptr->alarm || sptr->alarm_min || sptr->alarm_max)) {
	    char buf[100];
	    snprintf(buf, 100, "These alarm flags are set by the fan's chip:"
		   "  alarm=%s, min_alarm=%s, max_alarm=%s",
		    sptr->alarm ? "1":"0",
		    sptr->alarm_min ? "1":"0",
		    sptr->alarm_max ? "1":"0");
	    CMSetArrayElementAt(res, 1, buf, CMPI_chars);
	}
    }
    return res;
}

static CMPIArray * _get_OtherIdentifyingInfo(
	CMPIBroker const *_broker,
	CMPIStatus *rc,
	struct cim_fan const *sptr)
{
    CMPIArray *res = CMNewArray(_broker, 2, CMPI_string, rc);
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 2, CMPI_string, rc)");
	_OSBASE_TRACE(2, ("--- _get_OtherIdentifyingInfo failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	CMSetArrayElementAt(res, 0, sptr->chip_name, CMPI_chars);
	CMSetArrayElementAt(res, 1, sptr->sys_path, CMPI_chars);
    }
    return res;
}

static CMPIArray * _get_IdentifyingDescriptions(
	CMPIBroker const *_broker,
	CMPIStatus *rc)
{
    CMPIArray *res = CMNewArray(_broker, 2, CMPI_string, rc);
    if (!res) {
	CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
		"CMNewArray(_broker, 2, CMPI_string, rc)");
	_OSBASE_TRACE(2, ("--- _get_IdentifyingDescriptions failed: %s",
		    CMGetCharPtr(rc->msg)));
    }else {
	CMSetArrayElementAt(res, 0, 
		"ChipName - name of fan's chip.", CMPI_chars);
	CMSetArrayElementAt(res, 1,
		"SysPath - system path of fan's chip.", CMPI_chars);
    }
    return res;
}

CMPIObjectPath * _makePath_FanCommon(
	char const *class_name,
	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        struct cim_fan *sptr,
        CMPIStatus *rc)
{
    UNUSED(ctx);

    CMPIObjectPath * op = NULL;

    _OSBASE_TRACE(2,("--- _makePath_FanCommon() called"));

    op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, rc)),
            class_name, rc);
    if (CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                "Create CMPIObjectPath failed");
        _OSBASE_TRACE(2, ("--- _makePath_FanCommon() failed : %s",
                    CMGetCharPtr(rc->msg)));
    }else {
        CMAddKey(op, "CreationClassName", class_name, CMPI_chars);
        CMAddKey(op, "SystemCreationClassName", CSCreationClassName,
                CMPI_chars);
        CMAddKey(op, "SystemName", get_system_name(), CMPI_chars);
        CMAddKey(op, "DeviceID", sptr->device_id, CMPI_chars);
	CMAddKey(op, "Name", sptr->name, CMPI_chars);
    }
    _OSBASE_TRACE(2, ("--- _makePath_FanCommon() exited"));
    return op;
}

CMPIInstance * _makeInst_FanCommon(
	char const *class_name,
       	CMPIBroker const *_broker,
        CMPIContext const *ctx,
        CMPIObjectPath const *cop,
        char const **properties,
        struct cim_fan *sptr,
        CMPIStatus *rc,
	CMPIObjectPath **op)
{
    UNUSED(ctx);

    CMPIInstance *ci = NULL;
    CMPIValue val;
    uint16_t uint16;
    char const *keys[] = {
	"CreationClassName",
	"SystemCreationClassName",
	"SystemName",
	"DeviceId",
	"Name",
	NULL
    };
    char buf[200];

    _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() called"));

    /* the sblim-cmpi-base package offers some tool methods to get common
    * system data
    */
    if( !get_system_name() ) {   
        CMSetStatusWithChars( _broker, rc, 
                CMPI_RC_ERR_FAILED, "no host name found" );
        _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() failed : %s", 
		    CMGetCharPtr(rc->msg)));
        return NULL;
    }

    if( !get_os_name() ) {
        CMSetStatusWithChars( _broker, rc,
        CMPI_RC_ERR_FAILED, "no OS name found" );
        _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() failed : %s",
		    CMGetCharPtr(rc->msg)));
        return NULL;
    }

    *op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, rc)),
            class_name, rc);
    if (CMIsNullObject(*op)) {
        CMSetStatusWithChars(_broker, rc,
                CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() failed : %s",
		    CMGetCharPtr(rc->msg)));
        return NULL;
    }
    ci = CMNewInstance(_broker, *op, rc);
    if (CMIsNullObject(ci)) {
        CMSetStatusWithChars(_broker, rc,
                CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() failed : %s",
                    CMGetCharPtr(rc->msg)));
        return NULL;
    }
    CMSetPropertyFilter(ci, properties, keys);

    //* keys ******************************************************************
    CMSetProperty(ci, "CreationClassName", class_name, CMPI_chars);
    CMSetProperty(ci, "SystemCreationClassName", CSCreationClassName,
	    CMPI_chars);
    CMSetProperty(ci, "SystemName", get_system_name(), CMPI_chars);
    CMSetProperty(ci, "DeviceID", sptr->device_id, CMPI_chars);
    CMSetProperty(ci, "Name", sptr->name, CMPI_chars);

    // other properties *******************************************************
    // ManagedElement
    CMSetProperty(ci, "Caption", "Computer's fan", CMPI_chars);
    CMSetProperty(ci, "Description", "Computer's fan.", CMPI_chars);
    snprintf(buf, 200, "Fan \"%s\" on chip \"%s\"", sptr->name,
	    sptr->chip_name);
    CMSetProperty(ci, "ElementName", buf, CMPI_chars);

    // ManagedSystemElement
    val.array = _get_OperationalStatus(_broker, rc, sptr);
    CMSetProperty(ci, "OperationalStatus", &val, CMPI_uint16A);
    val.array = _get_StatusDescriptions(_broker, rc, sptr);
    CMSetProperty(ci, "StatusDescriptions", &val, CMPI_stringA);
    uint16 = sptr->fault ? 20:5; // Major failure : OK
    CMSetProperty(ci, "HealthState", &uint16, CMPI_uint16);
    uint16 = sptr->fault ? 5:16; // Stopped : In Service
    CMSetProperty(ci, "OperatingStatus", &uint16, CMPI_uint16);
    uint16 = sptr->fault ? 3:1;  // Error : OK
    CMSetProperty(ci, "PrimaryStatus", &uint16, CMPI_uint16);

    // EnabledLogicalElement
    val.array = _get_OtherIdentifyingInfo(_broker, rc, sptr);
    CMSetProperty(ci, "OtherIdentifyingInfo", &val, CMPI_stringA);
    val.array = _get_IdentifyingDescriptions(_broker, rc);
    CMSetProperty(ci, "IdentifyingDescriptions", &val, CMPI_stringA);

    _OSBASE_TRACE(2, ("--- _makeInst_FanCommon() exited"));
    return ci;
}

