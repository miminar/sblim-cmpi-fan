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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <strings.h>

#include <cmpi/cmpidt.h>
#include <cmpi/cmpift.h>
#include <cmpi/cmpimacs.h>
#include <sblim/OSBase_Common.h>
#include <sblim/cmpiOSBase_Common.h>

#include "cmpiLinux_Fan.h"
#include "Linux_FanUtil.h"

#define UNUSED(x) ((void) (x))
static const CMPIBroker * _broker;

#ifdef CMPI_VER_100
    #define Linux_FanProviderSetInstance Linux_FanProviderModifyInstance
#endif

/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/

CMPIStatus Linux_FanProviderCleanup( CMPIInstanceMI * mi,
           const CMPIContext * ctx, CMPIBoolean terminate) {
    UNUSED(mi); UNUSED(ctx); UNUSED(terminate);

    _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
    cleanup_linux_fan_module();
    util_cleanup();
    _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
    CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_FanProviderEnumInstanceNames( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref) {
    UNUSED(mi);

    CMPIObjectPath     * op    = NULL;
    CMPIStatus           rc    = {CMPI_RC_OK, NULL};
    struct fanlist * lptr  = NULL;
    struct fanlist * rm    = NULL;

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

    if (enum_all_fans(&lptr) != 0 ) {
        CMSetStatusWithChars( _broker, &rc,
                CMPI_RC_ERR_FAILED, "Could not list get fan list." );
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed : %s",
                    _ClassName,CMGetCharPtr(rc.msg)));
        return rc;
    }

    // iterate fan list
    for (rm = lptr; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
        // method call to create the CMPIInstance object
        op = _makePath_Fan(_broker, ctx, ref, lptr->f, &rc);
        if (op == NULL || rc.rc != CMPI_RC_OK) {
            if (rc.msg != NULL) {
                _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed"
                            ": %s", _ClassName, CMGetCharPtr(rc.msg)));
            }
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                    "Transformation from internal structure to CIM"
                    " ObjectPath failed.");
            if (rm) free_fanlist(rm);
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed"
                       ": %s", _ClassName, CMGetCharPtr(rc.msg)));
            return rc;
        }else {
            CMReturnObjectPath(rslt, op);
        }
    }
    if (rm) free_fanlist(rm);

    CMReturnDone(rslt);
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() exited", _ClassName));
    return rc;
}

CMPIStatus Linux_FanProviderEnumInstances( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char ** properties) {
    UNUSED(mi);

    CMPIInstance       * ci    = NULL;
    CMPIStatus           rc    = {CMPI_RC_OK, NULL};
    struct fanlist * lptr  = NULL;
    struct fanlist * rm    = NULL;

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

    if (enum_all_fans(&lptr)) {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_FAILED, "Could not list fans.");
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed : %s", 
                    _ClassName,CMGetCharPtr(rc.msg)));
        return rc;
    }

    // iterate fan list
    for (rm=lptr; lptr && rc.rc == CMPI_RC_OK; lptr = lptr->next) {
        // method call to create the CMPIInstance object
        ci = _makeInst_Fan(_broker, ctx, ref, properties, lptr->f, &rc);
        if (ci == NULL || rc.rc != CMPI_RC_OK ) {
            if (rc.msg != NULL ) {
                _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed :"
                           " %s",_ClassName,CMGetCharPtr(rc.msg)));
            }
            CMSetStatusWithChars( _broker, &rc,
                  CMPI_RC_ERR_FAILED, "Transformation from internal"
                 " structure to CIM Instance failed.");
            if (rm) free_fanlist(rm);
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed :"
                       " %s", _ClassName,CMGetCharPtr(rc.msg)));
            return rc;
        }else {
            CMReturnInstance(rslt, ci);
        }
    }
    if (rm) free_fanlist(rm);

    CMReturnDone(rslt);
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() exited", _ClassName));
    return rc;
}

CMPIStatus Linux_FanProviderGetInstance( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * cop,
           const char **properties) {
    UNUSED(mi);

    CMPIInstance       * ci    = NULL;
    CMPIStatus           rc    = {CMPI_RC_OK, NULL};
    struct cim_fan * sptr  = NULL;
    CMPIData             data;
    CMPIArray          * other_identifying_info = NULL;
    CMPIString         * sys_path  = NULL;
    CMPIString         * fan_name = NULL;
    CMPIString         * device_id = NULL;
    cim_fan_error_t      cmdrc;

    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

    data = CMGetKey(cop, "OtherIdentifyingInfo", &rc);
    if (  !rc.rc
       && data.type == CMPI_stringA
       && (other_identifying_info = data.value.array)
       && !rc.rc
       && (CMGetArrayCount(other_identifying_info, &rc) > 1)
       && !rc.rc)
    {
        data = CMGetArrayElementAt(other_identifying_info, 0, &rc);
        if (!rc.rc) sys_path = data.value.string;
        data = CMGetArrayElementAt(other_identifying_info, 1, &rc);
        if (!rc.rc) fan_name = data.value.string;
    }
    device_id = CMGetKey(cop, "DeviceID", &rc).value.string;
    if ((sys_path == NULL || fan_name == NULL) && (device_id == NULL)) {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_FAILED, "Could not get fan ID." );
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed : %s",
                _ClassName, CMGetCharPtr(rc.msg)));
        return rc;
    }

    if (sys_path && fan_name) {
        cmdrc = get_fan_data(CMGetCharPtr(sys_path), CMGetCharPtr(fan_name),
            &sptr);
    }
    if ((cmdrc || !sptr) && device_id) {
        cmdrc = get_fan_data_by_id(CMGetCharPtr(device_id), &sptr);
    }
    if (cmdrc || !sptr) {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_NOT_FOUND, cim_fan_strerror(cmdrc));
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() exited : %s",
                _ClassName,CMGetCharPtr(rc.msg)));
        return rc;
    }

    ci = _makeInst_Fan(_broker, ctx, cop, properties, sptr, &rc);
    if (sptr) free_fan(sptr);

    if (ci == NULL) {
        if (rc.msg != NULL) {
            _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed : %s",
                    _ClassName, CMGetCharPtr(rc.msg)));
        }else {
            _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed", _ClassName));
        }
        return rc;
    }

    CMReturnInstance(rslt, ci);
    CMReturnDone(rslt);
    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() exited", _ClassName));
    return rc;
}

CMPIStatus Linux_FanProviderCreateInstance( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * cop,
           const CMPIInstance * ci) {
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(cop); UNUSED(ci);

    CMPIStatus rc = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() called", _ClassName));
    CMSetStatusWithChars(_broker, &rc,
                    CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED");
    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() exited", _ClassName));
    return rc;
}

CMPIStatus Linux_FanProviderSetInstance( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * cop,
           const CMPIInstance * ci,
           const char ** properties) {
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(properties);

    CMPIStatus             rc    = {CMPI_RC_OK, NULL};
    CMPIData               data;
    char const           * device_id = NULL;
    struct cim_fan       * sptr;
    cim_fan_prop_value_t   cfpv;
    cim_fan_error_t        cmdrc;
    char const           * tmp = NULL;
    char buf[200];

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() called", _ClassName));
    
    data = CMGetKey(cop, "DeviceID", &rc);
    if (data.value.string == NULL) {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_FAILED, "Could not get fan ID." );
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed : %s",
                _ClassName, CMGetCharPtr(rc.msg)));
    }
    device_id = CMGetCharPtr(data.value.string);
    if (device_id) {
        cmdrc = get_fan_data_by_id(device_id, &sptr);
    }
    if (cmdrc || !sptr) {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_NOT_FOUND, cim_fan_strerror(cmdrc));
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() exited : %s",
                _ClassName,CMGetCharPtr(rc.msg)));
        return rc;
    }

    data = CMGetProperty(ci, "MinSpeed", &rc);
    if (!rc.rc && data.value.uint64 != (uint64_t) sptr->min_speed) {
        tmp = "MinSpeed";
        cfpv.min_speed = data.value.uint64;
        cmdrc = set_fan_prop_by_id(device_id, CIM_FAN_MIN_SPEED, &cfpv);
    }
    data = CMGetProperty(ci , "MaxSpeed", &rc);
    if (!cmdrc && !rc.rc && data.value.uint64 != (uint64_t) sptr->max_speed) {
        tmp = "MaxSpeed";
        cfpv.max_speed = data.value.uint64;
        set_fan_prop_by_id(device_id, CIM_FAN_MAX_SPEED, &cfpv);
    }
    data = CMGetProperty(ci, "Divisor", &rc);
    if (!cmdrc && !rc.rc && data.value.uint32 != sptr->divisor) {
        tmp = "Divisor";
        cfpv.divisor = data.value.uint32;
        set_fan_prop_by_id(device_id, CIM_FAN_DIV, &cfpv);
    }
    data = CMGetProperty(ci, "Pulses", &rc);
    if (!cmdrc && !rc.rc && data.value.uint32 != sptr->pulses) {
        tmp = "Pulses";
        cfpv.pulses = data.value.uint32;
        set_fan_prop_by_id(device_id, CIM_FAN_PULSES, &cfpv);
    }
    data = CMGetProperty(ci, "Beep", &rc);
    if (!cmdrc && !rc.rc && data.value.boolean != sptr->beep) {
        tmp = "Beep";
        cfpv.beep = data.value.uint32;
        set_fan_prop_by_id(device_id, CIM_FAN_BEEP, &cfpv);
    }

    if (cmdrc) {
        if (tmp && cmdrc == CIM_FAN_SEN_NO_ENTRY) {
            snprintf(buf, 200,
                "System interface for fan(name=\"%s\", chip=\"%s\") does not"
               " provide a way to set \"%s\".", sptr->name, sptr->chip_name,
               tmp);
            CMSetStatusWithChars(_broker, &rc,
                    CMPI_RC_ERR_FAILED, buf);
        }else {
            CMSetStatusWithChars(_broker, &rc,
                    CMPI_RC_ERR_FAILED, cim_fan_strerror(cmdrc))
        }
        _OSBASE_TRACE(2, ("--- %s CMPI SetInstance() exited : %s",
                _ClassName, CMGetCharPtr(rc.msg)));
    }else {
        rc.rc = CMPI_RC_OK;
        rc.msg = NULL;
    }

    return rc;
}

CMPIStatus Linux_FanProviderDeleteInstance( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * cop) {
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(cop);

    CMPIStatus rc = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));
    CMSetStatusWithChars( _broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" );
    _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
    return rc;
}

CMPIStatus Linux_FanProviderExecQuery( CMPIInstanceMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char * lang,
           const char * query) {
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(ref); UNUSED(lang);
    UNUSED(query);
    CMPIStatus rc = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_ClassName));

    CMSetStatusWithChars( _broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" );

    _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_ClassName));
    return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_FanProviderMethodCleanup( CMPIMethodMI * mi,
           const CMPIContext * ctx, CMPIBoolean terminate) {
    UNUSED(mi); UNUSED(ctx), UNUSED(terminate);
    _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
    _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
    CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_FanProviderInvokeMethod( CMPIMethodMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char * methodName,
           const CMPIArgs * in,
           CMPIArgs * out) {
    UNUSED(mi); UNUSED(ctx); UNUSED(in); UNUSED(out);

    CMPIString     * sys_path;
    CMPIString     * fan_name;
    CMPIValue        valrc;
    CMPIString     * class = NULL;
    CMPIStatus       rc    = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() called", _ClassName));

    class = CMGetClassName(ref, &rc);

    /* "terminate" */
    if (  strcasecmp(CMGetCharPtr(class), _ClassName) == 0
       && strcasecmp(methodName, "setspeed") == 0 ) {

        valrc.uint32 = 0;

        sys_path = CMGetKey(ref, "SysPath", &rc).value.string;
        fan_name = CMGetKey(ref, "Name", &rc).value.string;

        if (!sys_path || !fan_name) {
            CMSetStatusWithChars(_broker, &rc,
                    CMPI_RC_ERR_FAILED, "No such fan found!");
            _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() failed : %s",
                    _ClassName, CMGetCharPtr(rc.msg)));
        }else {
            _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() failed : TODO",
                        _ClassName));
        }
        CMReturnData(rslt, &valrc, CMPI_uint32);
        _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() %s exited",
                    _ClassName, methodName));
        CMReturnDone(rslt);
    }else {
        CMSetStatusWithChars(_broker, &rc,
                CMPI_RC_ERR_NOT_FOUND, methodName);
    }

    _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() exited", _ClassName));
    return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( Linux_FanProvider,
                  Linux_FanProvider,
                  _broker,
                  init_linux_fan_module());

CMMethodMIStub( Linux_FanProvider,
                Linux_FanProvider,
                _broker,
                init_linux_fan_module());

/* ---------------------------------------------------------------------------*/
/*                 end of cmpiLinux_FanProvider                      */
/* ---------------------------------------------------------------------------*/

