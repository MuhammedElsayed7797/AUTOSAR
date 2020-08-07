/*

Written by : Bebo
DATE : 16/9/2019
AUTOSAR Version : 4.3.1
DOC Name : AUTOSAR_SRS_DIODriver.pdf
Target : ARM TIVA_C TM4C123GH6PM
Module ID : 020
*/

#include <DIO/Common/Dio_MemMap.h>
#include <DIO/Dio.h>

 /*********************************************************************/

 /* 10.1.6 */
 /* those array is used to link time configuration of channel groups created by user using configuration tool */

CONST( Dio_ChannelGroupType, DIO_CONFIG_DATA )  MyDioGroupArray [DIO_GROUPS_NUMBER]  =
{
 {
  .port = DioConf_PortA
  ,
  .mask = 0x0F
  ,
  .offset = 0
 }
 ,
 {
  .port = DioConf_PortB
  ,
  .mask = 0x1E
  ,
  .offset = 1
 }
 ,
 {
  .port = DioConf_PortC
  ,
  .mask = 0x3C
  ,
  .offset = 2
 }
 ,
 {
  .port = DioConf_PortD
  ,
  .mask = 0x78
  ,
  .offset = 3
 }
 ,
 {
  .port = DioConf_PortE
  ,
  .mask = 0x30
  ,
  .offset = 4
 }
 ,
 {
  .port = DioConf_PortF
  ,
  .mask = 0x1F
  ,
  .offset = 0
 }
};


/*********************************************************************/
