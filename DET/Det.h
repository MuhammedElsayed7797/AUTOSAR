#ifndef DET_H

#define DET_H

/*********************************************************************/


/* Published information */
#define DET_MODULE_ID 30u
#define DET_VENDOR_ID 0x002Bu   /* vendor id of NXP */

#define DET_AR_RELEASE_MAJOR_VERSION 0x01u
#define DET_AR_RELEASE_MINOR_VERSION 0x00u
#define DET_AR_RELEASE_REVISION_VERSION 0x00u

#define DET_SW_MAJOR_VERSION 0x04u
#define DET_SW_MINOR_VERSION 0x03u
#define DET_SW_PATCH_VERSION 0x01u


/*********************************************************************/

#include "../Common\Std_Types.h"


FUNC(Std_ReturnType, DET_CODE ) Det_ReportError( uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId ) ;


#endif // DET_H
