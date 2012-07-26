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

static char const * _ClassName = "Linux_FanAssociatedSensor";
static char const * _RefLeft = "Antecedent";
static char const * _RefRight = "Dependent";
static char const * _RefLeftClass = "Linux_FanSensor";
static char const * _RefRightClass = "Linux_Fan";

#define UNUSED(x) ((void) (x))
static const CMPIBroker * _broker;
#ifdef CMPI_VER_100
    #define Linux_FanAssociatedSensorProviderSetInstance \
        Linux_FanAssociatedSensorProviderModifyInstance
#endif

/* ---------------------------------------------------------------------------*/
/*                       Instance Provider Interface                          */
/* ---------------------------------------------------------------------------*/

CMPIStatus Linux_FanAssociatedSensorProviderCleanup(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx,
        CMPIBoolean terminate)
{ 
    UNUSED(mi); UNUSED(ctx); UNUSED(terminate);
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_FanAssociatedSensorProviderEnumInstanceNames(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * ref)
{ 
    UNUSED(mi);
  CMPIStatus rc    = {CMPI_RC_OK, NULL};
  int        refrc = 0;
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));
  
  refrc = _assoc_create_inst_1toN( _broker,ctx,rslt,ref,
				   _ClassName,_RefLeftClass,_RefRightClass,
				   _RefLeft,_RefRight,
				   1,0,&rc);
  if( refrc != 0 ) { 
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed : %s",
                  _ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed",_ClassName));
    }
    return rc;
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderEnumInstances(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * ref, 
        const char ** properties)
{ 
    UNUSED(mi); UNUSED(properties);
  CMPIStatus rc    = {CMPI_RC_OK, NULL};
  int        refrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

  refrc = _assoc_create_inst_1toN( _broker,ctx,rslt,ref,
				   _ClassName,_RefLeftClass,_RefRightClass,
				   _RefLeft,_RefRight,
				   1,1,&rc);
  if( refrc != 0 ) { 
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed : %s",
                  _ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed",_ClassName));
    }
    return rc;
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderGetInstance(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * cop, 
        const char ** properties)
{
    UNUSED(mi); UNUSED(properties);
  const CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));
  
  ci = _assoc_get_inst( _broker,ctx,cop,_ClassName,_RefLeft,_RefRight,&rc);

  if( ci == NULL ) { 
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed : %s",
                  _ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed",_ClassName));
    }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);
  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderCreateInstance(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * cop, 
        const CMPIInstance * ci)
{
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(cop); UNUSED(ci);
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderSetInstance(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * cop,
        const CMPIInstance * ci, 
        const char ** properties)
{
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(cop); UNUSED(ci);
    UNUSED(properties);
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" );

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName)); 
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderDeleteInstance(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * cop)
{
    UNUSED(mi); UNUSED(ctx); UNUSED(rslt); UNUSED(cop);
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
  return rc;
}

CMPIStatus Linux_FanAssociatedSensorProviderExecQuery(
        CMPIInstanceMI * mi, 
        const CMPIContext * ctx, 
        const CMPIResult * rslt, 
        const CMPIObjectPath * ref, 
        const char * lang, 
        const char * query)
{
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
/*                    Associator Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_FanAssociatedSensorProviderAssociationCleanup(
        CMPIAssociationMI * mi,
        const CMPIContext * ctx,
        CMPIBoolean terminate)
{
    UNUSED(mi); UNUSED(ctx); UNUSED(terminate);
    _OSBASE_TRACE(1,("--- %s CMPI AssociationCleanup() called",_ClassName));
    _OSBASE_TRACE(1,("--- %s CMPI AssociationCleanup() exited",_ClassName));
    CMReturn(CMPI_RC_OK);
}

/* 
 * The intention of associations is to show the relations between different
 * classes and their instances. Therefore an association has two properties. 
 * Each one representing a reference to a certain instance of the specified
 * class. We can say, that an association has a left and the right "end". 
 *
 * Linux_RunningOS : 
 *    < role >   -> < class >
 *    Antecedent -> Linux_Fan
 *    Dependent  -> Linux_FanSensor
 *
 */

/*
 * general API information
 *
 * cop :
 *    Specifies the CMPIObjectPath to the CMPIInstance of the known end of 
 *    the association. Its absolutelly necessary to define this anchor point,
 *    from where the evaluation of the association starts.
 * assocPath : 
 *    If not NULL, it contains the name of the association the caller is 
 *    looking for. The provider should only return values, when the assocPath 
 *    contains the name(s) of the association(s) he is responsible for !
 * role :
 *    The caller can specify that the source instance ( cop ) has to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, role has to contain
 *    the same value as the <role> the source instance plays in this assoc.
 *    If this requirement is not true, the provider returns nothing.
 */

/*
 * specification of associators() and associatorNames()
 *
 * These methods return CMPIInstance ( in the case of associators() ) or 
 * CMPIObjectPath ( in the case of associatorNames() ) object(s) of the 
 * opposite end of the association.
 *
 * resultRole :
 *    The caller can specify that the target instance(s) has/have to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, resultRole has to 
 *    contain the same value as the <role> the target instance(s) plays
 *    in this assoc. If this requirement is not true, the provider returns 
 *    nothing.
 * resultClass :
 *    The caller can specify that the target instance(s) has/have to be 
 *    instances of a certain <class>. The <class> is specified by the
 *    association definition (see above). That means, resultClass has to 
 *    contain the same value as the <class> of the target instance(s).
 *    If this requirement is not true, the provider returns nothing.
 */

CMPIStatus Linux_FanAssociatedSensorProviderAssociators(
        CMPIAssociationMI * mi,
        const CMPIContext * ctx,
        const CMPIResult * rslt,
        const CMPIObjectPath * cop,
        const char * assocClass,
        const char * resultClass,
        const char * role,
        const char * resultRole,
        const char ** propertyList)
{
    UNUSED(mi); UNUSED(propertyList);
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI Associators() called",_ClassName));

  if( assocClass ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,&rc)),
			  _ClassName, &rc );
    if( op==NULL ) {
      CMSetStatusWithChars( _broker, &rc,
            CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
      _OSBASE_TRACE(2,
              ("--- %s CMPI Associators() failed : %s",CMGetCharPtr(rc.msg)));
      return rc;
    }
  }

  if (  (assocClass == NULL)
     || (CMClassPathIsA(_broker,op,assocClass,&rc) == 1))
  {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      resultClass,role,resultRole, 
				      &rc ) == 0 ) { goto exit; }

    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    1, 1, &rc);
    if( refrc != 0 ) {
      if( rc.msg != NULL ) {
	_OSBASE_TRACE(1, ("--- %s CMPI Associators() failed : %s",
                    _ClassName,CMGetCharPtr(rc.msg)));
      }
      else {
	_OSBASE_TRACE(1,("--- %s CMPI Associators() failed",_ClassName));
      }
      return rc;
    }
  }
  
 exit:
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI Associators() exited",_ClassName));  
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_FanAssociatedSensorProviderAssociatorNames(
        CMPIAssociationMI * mi,
        const CMPIContext * ctx,
        const CMPIResult * rslt,
        const CMPIObjectPath * cop,
        const char * assocClass,
        const char * resultClass,
        const char * role,
        const char * resultRole)
{
    UNUSED(mi);
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI AssociatorNames() called",_ClassName));  

  if( assocClass ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,&rc)),
			  _ClassName, &rc );
    if( op==NULL ) {
      CMSetStatusWithChars( _broker, &rc,
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
      _OSBASE_TRACE(2, ("--- %s CMPI AssociatorNames() failed : %s",
                  CMGetCharPtr(rc.msg)));
      return rc;
    }
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {

    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      resultClass,role,resultRole, 
				      &rc ) == 0 ) { goto exit; }

    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    0, 1, &rc);
    if( refrc != 0 ) { 
      if( rc.msg != NULL ) {
	_OSBASE_TRACE(1, ("--- %s CMPI AssociatorNames() failed : %s",
                    _ClassName,CMGetCharPtr(rc.msg)));
      }
      else {
	_OSBASE_TRACE(1,("--- %s CMPI AssociatorNames() failed",_ClassName));
      }
      return rc;
    }
  }

 exit:
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI AssociatorNames() exited",_ClassName));  
  CMReturn(CMPI_RC_OK);
}


/*
 * specification of references() and referenceNames()
 *
 * These methods return CMPIInstance ( in the case of references() ) or 
 * CMPIObjectPath ( in the case of referenceNames() ) object(s) of th 
 * association itself.
 */

CMPIStatus Linux_FanAssociatedSensorProviderReferences(
        CMPIAssociationMI * mi,
        const CMPIContext * ctx,
        const CMPIResult * rslt,
        const CMPIObjectPath * cop,
        const char * assocClass,
        const char * role,
        const char ** propertyList)
{
    UNUSED(mi); UNUSED(propertyList);
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI References() called",_ClassName)); 

  if( assocClass ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,&rc)),
			  _ClassName, &rc );
    if( op==NULL ) {
      CMSetStatusWithChars( _broker, &rc,
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
      _OSBASE_TRACE(2,("--- %s CMPI References() failed : %s",CMGetCharPtr(rc.msg)));
      return rc;
    }
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      NULL,role,NULL, 
				      &rc ) == 0 ) { goto exit; }
    
    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    1, 0, &rc);
    if( refrc != 0 ) { 
      if( rc.msg != NULL ) {
	_OSBASE_TRACE(1, ("--- %s CMPI References() failed : %s",
                    _ClassName,CMGetCharPtr(rc.msg)));
      }
      else {
	_OSBASE_TRACE(1,("--- %s CMPI References() failed",_ClassName));
      }
      return rc;
    }
  }

 exit:
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI References() exited",_ClassName)); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_FanAssociatedSensorProviderReferenceNames(
        CMPIAssociationMI * mi,
        const CMPIContext * ctx,
        const CMPIResult * rslt,
        const CMPIObjectPath * cop,
        const char * assocClass,
        const char * role)
{
    UNUSED(mi);
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI ReferenceNames() called",_ClassName));
  
  if( assocClass ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,&rc)),
			  _ClassName, &rc );
    if( op==NULL ) {
      CMSetStatusWithChars( _broker, &rc,
			    CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
      _OSBASE_TRACE(2,("--- %s CMPI ReferenceNames() failed : %s",CMGetCharPtr(rc.msg)));
      return rc;
    }
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      NULL,role,NULL, 
				      &rc ) == 0 ) { goto exit; }
    
    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    0, 0, &rc);
    if( refrc != 0 ) { 
      if( rc.msg != NULL ) {
	_OSBASE_TRACE(1,("--- %s CMPI ReferenceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
      }
      else {
	_OSBASE_TRACE(1,("--- %s CMPI ReferenceNames() failed",_ClassName));
      }
      return rc;
    }
  }

 exit:
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI ReferenceNames() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/
CMInstanceMIStub( Linux_FanAssociatedSensorProvider,
                  Linux_FanAssociatedSensorProvider,
                  _broker,
                  CMNoHook)

CMAssociationMIStub( Linux_FanAssociatedSensorProvider, 
                     Linux_FanAssociatedSensorProvider, 
                     _broker, 
                     CMNoHook)

/* ---------------------------------------------------------------------------*/
/*                 end of cmpiLinux_FanAssociatedSensorProvider               */
/* ---------------------------------------------------------------------------*/

