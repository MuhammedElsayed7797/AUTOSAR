/*

Written by : Bebo
DATE : 1/9/2019
AUTOSAR Version : 4.3.1
DOC Name : AUTOSAR_SWS_CompilerAbstraction.pdf
Target : ARM TIVA_C TM4C123GH6PM

*/


#ifndef COMPILER_CFG_H

#define COMPILER_CFG_H


/*********************************************************************/


/* Published information */
#define DIO_MODULE_ID 0x020u
#define DIO_VENDOR_ID 0x002Bu   /* vendor id of NXP */

#define DIO_AR_RELEASE_MAJOR_VERSION 0x01u
#define DIO_AR_RELEASE_MINOR_VERSION 0x00u
#define DIO_AR_RELEASE_REVISION_VERSION 0x00u

#define DIO_SW_MAJOR_VERSION 0x04u
#define DIO_SW_MINOR_VERSION 0x03u
#define DIO_SW_PATCH_VERSION 0x01u


/*********************************************************************/

/* 7.1.2 */
/* this is compiler keywords (not C keywords) must by used with function, variables and, pointers to work in prober way */
/* for ARM C (GCC, ADS) compilers no need for those*/

#define DIO_CODE                /* used for ordinary code */
#define DIO_CONFIG_DATA			/* used for cused for module configuration constants */
#define REGSPACE		 		/* used for staitc Pointer to Reg (ptr to volatile) carry adress of value that changed in any time by interrupts */

#endif 	/* COMPILER_CFG_H */
