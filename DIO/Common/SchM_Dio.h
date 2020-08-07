/*

Written by : Bebo
DATE : 16/9/2019
AUTOSAR Version : 4.3.1
DOC Name : AUTOSAR_SRS_DIODriver.pdf
Target : ARM TIVA_C TM4C123GH6PM
Module ID : 020
*/


#ifndef CODES_DIO_COMMON_SCHM_DIO_H_
#define CODES_DIO_COMMON_SCHM_DIO_H_



/* set and clear PRIMASK register*/
#define CS_ON       __asm ( " SVC #0x00 " ) ; \
                    __asm ( " CPSID i " ) ;


#define CS_OFF      __asm ( " CPSIE i " ) ; \
                    __asm ( " MRS R10, CONTROL " ) ;\
                    __asm ( " ORR R10, R10, #0x01 " ) ;\
                    __asm ( " MSR CONTROL, R10 " )

#endif /* CODES_DIO_COMMON_SCHM_DIO_H_ */
