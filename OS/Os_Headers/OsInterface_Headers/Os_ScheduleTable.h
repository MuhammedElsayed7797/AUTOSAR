/*

    Written by : Bakr
    DATE : 18/2/2020
    AUTOSAR Version : 4.3.1
    DOC Name : AUTOSAR_SWS_OS.pdf
    Target : ARM TIVA_C TM4C123GH6PM

*/

#ifndef OS_HEADERS_OSINTERFACE_HEADERS_OS_SCHEDULETABLE_H_
#define OS_HEADERS_OSINTERFACE_HEADERS_OS_SCHEDULETABLE_H_

/*********************************************************************************************************/


typedef VAR( uint8, TYPEDEF ) ScheduleTableType ;


/* describes the status of a schedule. The status can be one of the following */
#define SCHEDULETABLE_STOPPED   0x00u   /* SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS */
#define SCHEDULETABLE_NEXT      0x44u   /* will be started after the end of currently running schedule table */
#define SCHEDULETABLE_RUNNING   0xCCu   /* table is running, but is currently not synchronous to a global time source*/

typedef VAR( uint8, TYPEDEF ) ScheduleTableStatusType ;

/* data type points to a variable of the data type ScheduleTableStatusType */
typedef P2VAR( ScheduleTableStatusType, TYPEDEF, TYPEDEF ) ScheduleTableStatusRefType ;


/*********************************************************************************************************/

FUNC(StatusType, OS_CODE) StartScheduleTableRel( ScheduleTableType ScheduleTableID, TickType Offset );

FUNC( StatusType, OS_CODE ) StartScheduleTableAbs( ScheduleTableType ScheduleTableID, TickType Start );

FUNC( StatusType, OS_CODE ) StopScheduleTable( ScheduleTableType ScheduleTableID );

FUNC( StatusType, OS_CODE ) NextScheduleTable( ScheduleTableType ScheduleTableID_From, ScheduleTableType ScheduleTableID_To );

FUNC( StatusType, OS_CODE ) GetScheduleTableStatus( ScheduleTableType ScheduleTableID, ScheduleTableStatusRefType ScheduleStatus );
#endif /* OS_HEADERS_OSINTERFACE_HEADERS_OS_SCHEDULETABLE_H_ */
