/*
 * Mcu_PBcfg.c
 *
 *  Created on: Nov 19, 2019
 *  Author: Bebo
 *  AUTOSAR Version: 4.3.1
 *  DOC Name: AUTOSAR_SWS_MCUDriver.pdf
 *  Target:  ARM TivaC TM4C123GH6PM
 *
 */

#ifndef CODES_MCU_COMMON_SCHM_MCU_H_
#define CODES_MCU_COMMON_SCHM_MCU_H_


/* set and clear PRIMASK register*/
#define CS_ON       __asm ( " SVC #0x00 " ) ; \
                    __asm ( " CPSID i " ) ;


#define CS_OFF      __asm ( " CPSIE i " ) ; \
                    __asm ( " MRS R10, CONTROL " ) ;\
                    __asm ( " ORR R10, R10, #0x01 " ) ;\
                    __asm ( " MSR CONTROL, R10 " )

#endif /* CODES_MCU_COMMON_SCHM_MCU_H_ */
