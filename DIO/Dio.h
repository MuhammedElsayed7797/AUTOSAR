/*

Written by : Bebo
DATE : 16/9/2019
AUTOSAR Version : 4.3.1
DOC Name : AUTOSAR_SRS_DIODriver.pdf
Target : ARM TIVA_C TM4C123GH6PM
Module ID : 020
*/

#ifndef DIO_H

#define DIO_H


#include "../Common/Std_Types.h"
#include "Dio_Cfg.h"


/*********************************************************************************************************/

/* Published information */
#define DIO_MODULE_ID 0x020u
#define DIO_VENDOR_ID 0x002Bu   /* vendor id of NXP */

#define DIO_AR_RELEASE_MAJOR_VERSION 0x01u
#define DIO_AR_RELEASE_MINOR_VERSION 0x00u
#define DIO_AR_RELEASE_REVISION_VERSION 0x00u

#define DIO_SW_MAJOR_VERSION 0x04u
#define DIO_SW_MINOR_VERSION 0x03u
#define DIO_SW_PATCH_VERSION 0x01u

/*********************************************************************************************************/

/* 8.2 Type definitions */

/* contain the numeric ID of a DIO channel take value from symbolic name provided to it in configuration description */
/* total number of channel -> 86 (43 for AHB) , (43 for APB) */
typedef VAR( uint32, TYPEDEF )     Dio_ChannelType ;

/* total number of ports -> 12 (6 for AHB) , (6 for APB) */
/* contain the numeric ID of a DIO port take value from symbolic name provided to it in configuration description */
typedef VAR( uint32, TYPEDEF )    Dio_PortType ;

/* type for the definition of a channel group take value from symbolic name provided to it in configuration description */
typedef struct
{

    VAR( uint8, TYPEDEF ) mask ;      /* defines the positions of the channel group */
    VAR( uint8_least, TYPEDEF ) offset ;    /* position of the Channel Group on the port, counted from the LSB */
    Dio_PortType port ;     /*  the port on which the Channel group is defined */

}Dio_ChannelGroupType ;

/* These are the possible levels a DIO channel can have (input or output) take values STD_HIGH, STD_LOW*/
typedef VAR( boolean, TYPEDEF ) Dio_LevelType ;

/* type for the value of a DIO port */
typedef VAR( uint8, TYPEDEF ) Dio_PortLevelType ;


/*********************************************************************************************************/


/*
    - Synchronous, Reentrant
    - Returns the value of the specified DIO channel Passed via ChannelId
    - functions shall provide the real pin level or the value of the output, when they are used on a channel which is configured as an output channel
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s read functions shall return with the value '0'.
    - These upper layer modules may access the driver concurrently
    - read functions shall work on input and output channels

*/
FUNC( Dio_LevelType, DIO_CODE)  Dio_ReadChannel( Dio_ChannelType ChannelId ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x01
    - Service to set a level of a channel which has ChannelId to value of Level
    - function shall set the specified Level for the specified channel
    - If configured as an input channel, the Dio_WriteChannel function shall have no influence on the physical outputو no influence on the result of the next Read-Service.
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.

*/
FUNC( void, DIO_CODE ) Dio_WriteChannel( Dio_ChannelType ChannelId, Dio_LevelType Level ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x02
    - Returns the level of all channels of that port.
    - When reading a port which is smaller than the Dio_PortType, function shall set the bits corresponding to undefined port pins to 0 like PORTF , PORTE.
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s read functions shall return with the value '0'.
    - These upper layer modules may access the driver concurrently
    - read functions shall work on input and output channels
    - shall not buffer data when providing read and write services.

*/
FUNC( Dio_PortLevelType, DIO_CODE ) Dio_ReadPort( Dio_PortType PortId ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x03
    - Service to set a value of the port.
    - DIO Channels that are configured as input shall remain unchanged, no effect on channels within this port which are configured as input channels
    - writing a port which is smaller than the Dio_PortType , the function shall ignore the MSBs
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.
*/

FUNC( void, DIO_CODE ) Dio_WritePort( Dio_PortType PortId, Dio_PortLevelType Level ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x04
    - Service reads a subset of the adjoining bits of a port, function shall do the masking of the channel group
    - function shall do the shifting so that the values read by the function are aligned to the LSB.
    - If configured as an input channel, the Dio_WriteChannel function shall have no influence on the physical outputو no influence on the result of the next Read-Service.
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.

*/
FUNC( Dio_PortLevelType, DIO_CODE ) Dio_ReadChannelGroup( const Dio_ChannelGroupType * ChannelGroupIdPtr ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x05
    - set a subset of the adjoining bits (inside port)(starting from (offset) and masked by (mask) ) of a port to a specified level
    - shall not change the remaining channels of the port and channels which are configured as input
    - shall do the masking of the channel group, shall do the shifting so that the values written by the function are aligned to the LSB
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.

*/
FUNC( void, DIO_CODE ) Dio_WriteChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level ) ;


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x12
    - get the version information of this module
    - If DET is enabled, the function shall raise DIO_E_PARAM_POINTER, if the argument is NULL pointer and return without any action.
*/

#if DIO_VERSION_INFO_API == STD_ON

FUNC( void, DIO_CODE ) Dio_GetVersionInfo( Std_VersionInfoType* VersionInfo ) ;

#endif // GET_VERSION


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x11
    - flip (change from 1 to 0 or from 0 to 1) the level of a channel and return the level of the channel after flip.
    - If channel is configured as output, the function shall read level of the channel and invert it, write the inverted level to the channel
    - If configured as an input channel, the Dio_FlipChannel function shall have no influence on the physical outputو no influence on the result of the next Read-Service.
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.

*/
#if DIO_FLIP_CHANNEL_API == STD_ON

FUNC( Dio_LevelType, DIO_CODE ) Dio_FlipChannel( Dio_ChannelType ChannelId ) ;


#endif // FLIP_CHANNEL

/*********************************************************************************************************/

/* array of channel groups configured by user in configuration tool */

extern CONST( Dio_ChannelGroupType, DIO_CONFIG_DATA )  MyDioGroupArray [DIO_GROUPS_NUMBER] ;


/*********************************************************************************************************/

#endif

