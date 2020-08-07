/*

Written by : Bebo
DATE : 16/9/2019
AUTOSAR Version : 4.3.1
DOC Name : AUTOSAR_SRS_DIODriver.pdf
Target : ARM TIVA_C TM4C123GH6PM
Module ID : 020
*/

#include <DIO/Common/Dio_MemMap.h>
#include <DIO/Common/SchM_Dio.h>
#include <DIO/Dio.h>
#include <DIO/Dio_Lcfg.h>
#include "..\DET\Det.h"



/***********************************************************************************************************/
#define GPIOHBCTL   *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE06C )

#define MAX_REG_BITS    (8)
#define AHB_OFFSET      (8)
#define AHB_MASK        (0xF000)
#define GPIOHBCTL_MASK  (0x3F)
#define PORT_EF_AHB_OFFSET  (0x00038000)
#define PORT_ABCD_AHB_OFFSET  (0x00054000)
#define PORT_D_NUMBER   (3)

#define CHANNEL_GROUP_MASK  (0xFFFFFF00)

#define BASE_PORT_COMMON_ADDRESS_MASKING 0x400003FC
#define BASE_PORT_COMMON_ADDRESS 0x40000000

#define MASK_ADDRESS_SHIFT_BITS  (2)
#define TEST_PARITY_CONSTANT    (2)

#define DIO_READ_CHANNEL_ID 0x00
#define DIO_WRITE_CHANNEL_ID 0x01
#define DIO_READ_PORT_ID 0x02
#define DIO_WRITE_PORT_ID 0x03
#define DIO_READ_CHANNEL_GROUP_ID 0x04
#define DIO_WRITE_CHANNEL_GROUP_ID 0x05
#define DIO_GET_VERSION_INFO_ID 0x12
#define DIO_FILP_CHANNEL_ID 0x11

#define DIO_E_PARAM_INVALID_CHANNEL_ID  0x0A
#define DIO_E_PARAM_INVALID_PORT_ID 0x014
#define DIO_E_PARAM_INVALID_GROUP 0x1F
#define DIO_E_PARAM_POINTER 0x20

/***********************************************************************************************************/

/* array of channels groups provided by configuration tools */

/***********************************************************************************************************/

/* symbolic names provided for ports and channels is for AHB bus so if APB pus is activated those values must be modified */
uint32 Dio_Prvate_CheckBusType ( uint32 ChannelId )
{

    if ( STD_LOW == ( ( ( GPIOHBCTL & GPIOHBCTL_MASK ) >> ( ( ( ChannelId & AHB_MASK ) >> 12 ) - AHB_OFFSET ) ) & 0x01 ) )
    {
        /* case APB mask was active */
        if (( ( ( ChannelId & AHB_MASK ) >> 12 ) - AHB_OFFSET ) > PORT_D_NUMBER )
        {
            ChannelId -= PORT_EF_AHB_OFFSET ;
        }
        else
        {
            ChannelId -= PORT_ABCD_AHB_OFFSET ;
        }
    }
    else
    {
    }
    return ChannelId ;

}


/***********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x00
    - Returns the value of the specified DIO channel Passed via ChannelId
    - functions shall provide the real pin level or the value of the output, when they are used on a channel which is configured as an output channel
    - ensure for all services, that the data is consistent (Interruptible read-modify-write sequences are not allowed) (no critical sections or disable for interrupts)
    - if development errors are enabled and an error ocurred the Dio module’s read functions shall return with the value '0'.
    - These upper layer modules may access the driver concurrently
    - read functions shall work on input and output channels
    - shall not buffer data when providing read and write services.

*/
FUNC( Dio_LevelType, DIO_CODE)  Dio_ReadChannel( Dio_ChannelType ChannelId )
{

    ChannelId = Dio_Prvate_CheckBusType( ChannelId ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    Dio_LevelType ReadChannelReturnResult = STD_LOW ;
    /*  this will access one bit if it read high will return non zero value if not it will be zero
        other bits even if they are 1 according to hardware masking feature it will be return 0
        so values of symbolic name assigned to "Dio_ChannelType must" be use address of port and masking feature */
    if ( STD_LOW == *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) )
    {
    }
    else
    {
        ReadChannelReturnResult = STD_HIGH ;
    }

    return ReadChannelReturnResult ;

    #endif // ERROR
    /*____________________________________________________________________*/

    /* for masking one bit the channel id must has the base address of data register + even number so
       check them by sub the base and module by 2  */

    #if DIO_DEV_ERROR_DETECT == STD_ON

    Dio_LevelType ReadChannelReturnResult = STD_LOW ;

    if ( STD_LOW == ( ( ( ChannelId & BASE_PORT_COMMON_ADDRESS_MASKING ) - BASE_PORT_COMMON_ADDRESS ) % TEST_PARITY_CONSTANT ) )
    {
        if ( STD_LOW == *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) )
        {
        }
        else
        {
            ReadChannelReturnResult = STD_HIGH ;
        }
    }
    else
    {
        /* invalid i/p return STD_LOW and report error " DIO_E_PARAM_INVALID_CHANNEL_ID = 0x0A " */
         Det_ReportError (DIO_MODULE_ID, 0, DIO_READ_CHANNEL_ID , DIO_E_PARAM_INVALID_CHANNEL_ID) ;
    }

    return ReadChannelReturnResult ;

    #endif // ERROR

}


/***********************************************************************************************************/

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
FUNC( void, DIO_CODE ) Dio_WriteChannel( Dio_ChannelType ChannelId, Dio_LevelType Level )
{
    ChannelId = Dio_Prvate_CheckBusType( ChannelId ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    /* masking on bit of bit of port writing in unmasked bit has no effect */
    if ( Level )
    {
        *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0xFF ;
    }
    else
    {
        *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0x00 ;
    }

    return ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON
    /* for masking one bit the channel id must has the base address of data register + even number so
           check them by sub the base and module by 2  */
    if ( STD_LOW == ( ( ( ChannelId & BASE_PORT_COMMON_ADDRESS_MASKING ) - BASE_PORT_COMMON_ADDRESS ) % TEST_PARITY_CONSTANT ) )
    {

        if ( Level )
        {
            *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0xFF ;
        }
        else
        {
            *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0x00 ;
        }

        }
    else
    {
        /* invalid i/p report error " DIO_E_PARAM_INVALID_CHANNEL_ID = 0x0A " */
         Det_ReportError (DIO_MODULE_ID, 0, DIO_WRITE_CHANNEL_ID , DIO_E_PARAM_INVALID_CHANNEL_ID) ;
    }

    return ;
    #endif // ERROR

}

/***********************************************************************************************************/

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
FUNC( Dio_PortLevelType, DIO_CODE ) Dio_ReadPort( Dio_PortType PortId )
{
    PortId = Dio_Prvate_CheckBusType( PortId ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    /* port id must have value that masking all port bits "base address + 0x3FC" */
    return  * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) PortId ) ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON

    /* all data registers has base address with 0x000 in lower nibbles  */

    Dio_PortLevelType ReadPortReturnResult = 0x00 ;



    if ( BASE_PORT_COMMON_ADDRESS_MASKING == (  PortId & BASE_PORT_COMMON_ADDRESS_MASKING ) )
    {
        /* port id must have value that masking all port bits "base address + 0x3FC" */
        ReadPortReturnResult = *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) PortId ) ;
    }
    else
    {
        /* invalid i/p return 0x00 and report error " DIO_E_PARAM_INVALID_PORT_ID = 0x14 " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_READ_PORT_ID , DIO_E_PARAM_INVALID_PORT_ID ) ;
    }

    return ReadPortReturnResult ;
    #endif // ERROR
}

/***********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x03
    - Service to set a value of the port.
    - DIO Channels that are configured as input shall remain unchanged, no effect on channels within this port which are configured as input channels
    - writing a port which is smaller than the Dio_PortType , the function shall ignore the MSBs
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.
*/

FUNC( void, DIO_CODE ) Dio_WritePort( Dio_PortType PortId, Dio_PortLevelType Level )
{
    PortId = Dio_Prvate_CheckBusType( PortId ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    /* port id must have value that masking all port bits "base address + 0x3FC" */
     * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) PortId ) = Level ;
    return   ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON

    /* all data registers has base address with 0x000 in lower nibbles  */

    if ( BASE_PORT_COMMON_ADDRESS_MASKING == (  PortId & BASE_PORT_COMMON_ADDRESS_MASKING ) )
    {
        /* port id must have value that masking all port bits "base address + 0x3FC" */
        * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) PortId ) = Level ;
    }
    else
    {
        /* invalid i/p report error " DIO_E_PARAM_INVALID_PORT_ID = 0x14 " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_WRITE_PORT_ID , DIO_E_PARAM_INVALID_PORT_ID ) ;
    }

    return  ;
    #endif // ERROR
}

/***********************************************************************************************************/

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
FUNC( Dio_PortLevelType, DIO_CODE ) Dio_ReadChannelGroup( const Dio_ChannelGroupType * ChannelGroupIdPtr )
{

    VAR( Dio_PortType, AUTOMATIC ) temp_Port = 0x00 ;
    VAR( uint8, AUTOMATIC ) temp_mask = ChannelGroupIdPtr->mask ;
    VAR( uint8, AUTOMATIC ) temp_offset = ChannelGroupIdPtr->offset ;

    temp_Port = Dio_Prvate_CheckBusType( ChannelGroupIdPtr->port ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    /* port id must have value that masking all port bits "base address + 0x3FC" */

    return  ( * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ( ( temp_Port ) & ( ( temp_mask + CHANNEL_GROUP_MASK ) << MASK_ADDRESS_SHIFT_BITS ) ) ) ) >> ( temp_offset ) ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON

    /* all data registers has base address with 0x000 in lower nibbles */

    Dio_PortLevelType ReadPortReturnResult = 0x00 ;


    if ( BASE_PORT_COMMON_ADDRESS_MASKING != (  ( temp_Port ) & BASE_PORT_COMMON_ADDRESS_MASKING ) )	/* check for valid port */
    {
        /* invalid i/p return 0x00 and report error " DIO_E_PARAM_INVALID_GROUP = 0xIF " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_READ_CHANNEL_GROUP_ID , DIO_E_PARAM_INVALID_GROUP ) ;
    }
    else if ( STD_LOW != ( ( ( ( temp_mask ) >> ( temp_offset ) )+ 1 ) % TEST_PARITY_CONSTANT ) )	/* check for valid mask with offset */
    {
        /* invalid i/p return 0x00 and report error " DIO_E_PARAM_INVALID_GROUP = 0xIF " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_READ_CHANNEL_GROUP_ID , DIO_E_PARAM_INVALID_GROUP ) ;
    }
    else
    {
       ReadPortReturnResult = ( * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ( ( temp_Port ) & ( ( temp_mask + CHANNEL_GROUP_MASK ) << MASK_ADDRESS_SHIFT_BITS ) ) ) ) >> ( temp_offset ) ;
    }

    return ReadPortReturnResult ;

    #endif // ERROR
}

/***********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x05
    - set a subset of the adjoining bits (inside port)(starting from (offset) and masked by (mask) ) of a port to a specified level
    - shall not change the remaining channels of the port and channels which are configured as input
    - shall do the masking of the channel group, shall do the shifting so that the values written by the function are aligned to the LSB
    - if development errors are enabled and an error ocurred the Dio module’s write functions shall ignore write command.
    - These upper layer modules may access the driver concurrently
    - shall not buffer data when providing read and write services.

*/
FUNC( void, DIO_CODE ) Dio_WriteChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level )
{

    VAR( Dio_PortType, AUTOMATIC ) temp_Port = 0x00 ;
    VAR( uint8, AUTOMATIC ) temp_mask = ChannelGroupIdPtr->mask ;
    VAR( uint8, AUTOMATIC ) temp_offset = ChannelGroupIdPtr->offset ;

    temp_Port = Dio_Prvate_CheckBusType( ChannelGroupIdPtr->port ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    /* port id must have value that masking all port bits "base address + 0x3FC" */
    * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ( ( temp_Port ) & ( ( temp_mask + CHANNEL_GROUP_MASK ) << MASK_ADDRESS_SHIFT_BITS ) ) ) = ( Level << ( temp_offset ) ) ;
    return  ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON

    /* all data registers has base address with 0x000 in lower nibbles */

    if ( BASE_PORT_COMMON_ADDRESS_MASKING != (  ( temp_Port ) & BASE_PORT_COMMON_ADDRESS_MASKING ) )	/* check for valid port */
    {
        /* invalid i/p report error " DIO_E_PARAM_INVALID_GROUP = 0xIF " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_WRITE_CHANNEL_GROUP_ID , DIO_E_PARAM_INVALID_GROUP ) ;
    }
    else if ( STD_LOW != ( ( ( ( temp_mask ) >> ( temp_offset ) )+ 1 ) % TEST_PARITY_CONSTANT ) )	/* check for valid mask with offset */
    {
        /* invalid i/p and report error " DIO_E_PARAM_INVALID_GROUP = 0xIF " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_WRITE_CHANNEL_GROUP_ID , DIO_E_PARAM_INVALID_GROUP ) ;
    }
    else
    {
        * ( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ( ( temp_Port ) & ( ( temp_mask + CHANNEL_GROUP_MASK ) << MASK_ADDRESS_SHIFT_BITS ) ) ) = ( Level << ( temp_offset ) ) ;
    }

    return ;

    #endif // ERROR

}

/***********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x12
    - get the version information of this module
    - If DET is enabled, the function shall raise DIO_E_PARAM_POINTER, if the argument is NULL pointer and return without any action.
*/

#if DIO_VERSION_INFO_API == STD_ON

FUNC( void, DIO_CODE ) Dio_GetVersionInfo( Std_VersionInfoType* VersionInfo )
{
    #if DIO_DEV_ERROR_DETECT == STD_OFF

        VersionInfo->moduleID = DIO_MODULE_ID ;
        VersionInfo->vendorID = DIO_VENDOR_ID ;
        VersionInfo->sw_major_version = DIO_SW_MAJOR_VERSION ;
        VersionInfo->sw_minor_version = DIO_SW_MINOR_VERSION ;
        VersionInfo->sw_patch_version = DIO_SW_PATCH_VERSION ;

    return  ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if DIO_DEV_ERROR_DETECT == STD_ON

    if ( NULL_PTR == VersionInfo)
    {
        /* invalid i/p return and report error " DIO_E_PARAM_POINTER = 0x20 " */
        Det_ReportError ( DIO_MODULE_ID, 0, DIO_E_PARAM_POINTER , DIO_GET_VERSION_INFO_ID ) ;
    }
    else
    {
        VersionInfo->moduleID = DIO_MODULE_ID ;
        VersionInfo->vendorID = DIO_VENDOR_ID ;
        VersionInfo->sw_major_version = DIO_SW_MAJOR_VERSION ;
        VersionInfo->sw_minor_version = DIO_SW_MINOR_VERSION ;
        VersionInfo->sw_patch_version = DIO_SW_PATCH_VERSION ;
    }
    return ;

    #endif // ERROR


}
#endif // GET_VERSION

/***********************************************************************************************************/

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
/* hear shall provide a critical section to avoid interrupt this function which have read modify write sequence */
#if DIO_FLIP_CHANNEL_API == STD_ON

FUNC( Dio_LevelType, DIO_CODE ) Dio_FlipChannel( Dio_ChannelType ChannelId )
{


    ChannelId = Dio_Prvate_CheckBusType( ChannelId ) ;

    #if DIO_DEV_ERROR_DETECT == STD_OFF

    Dio_LevelType ReadChannelResult = STD_LOW ;
    /*  this will access one bit if it read high will return non zero value if not it will be zero
        other bits even if they are 1 according to hardware masking feature it will be return 0
        so values of symbolic name assigned to "Dio_ChannelType must" be use address of port and masking feature */

    CS_ON ;

    if ( STD_LOW == *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) )
    {
        *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0xFF ;
        ReadChannelResult = STD_HIGH ;
    }
    else
    {
        *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0x00 ;
        ReadChannelResult = STD_LOW ;
    }


    CS_OFF ;


    return ReadChannelResult ;

    #endif // ERROR
    /*____________________________________________________________________*/

    /*  all base addresses of reg to read data from is 0x000 at lower nibbles
        so masking single channel for target will produce address its least 3 nibbles value with next statement will produce 0 */

    #if DIO_DEV_ERROR_DETECT == STD_ON

    Dio_LevelType ReadChannelResult = STD_LOW ;

    if ( STD_LOW == ( ( ( ChannelId & BASE_PORT_COMMON_ADDRESS_MASKING ) - BASE_PORT_COMMON_ADDRESS ) % TEST_PARITY_CONSTANT ) )
    {
        CS_ON ;

        if ( STD_LOW == *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) )
        {
            *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0xFF ;
            ReadChannelResult = STD_HIGH ;
        }
        else
        {
            *( ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) ChannelId ) =  0x00 ;
            ReadChannelResult = STD_LOW ;
        }

        CS_OFF ;

    }
    else
    {
        /* invalid i/p return STD_LOW and report error " DIO_E_PARAM_INVALID_CHANNEL_ID = 0x0A " */
         Det_ReportError ( DIO_MODULE_ID, 0, DIO_FILP_CHANNEL_ID , DIO_E_PARAM_INVALID_CHANNEL_ID ) ;
    }


    return ReadChannelResult ;

    #endif // ERROR

}

#endif // FLIP_CHANNEL

/***********************************************************************************************************/
