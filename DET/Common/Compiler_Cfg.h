/*

Written by : Bebo
DATE : 7/2/2019
AUTDETAR Version : 4.3.1
DOC Name : AUTDETAR_SWS_CompilerAbstraction.pdf
Target : ARM TIVA_C TM4C123GH6PM

*/


#ifndef COMPILER_CFG_H

#define COMPILER_CFG_H


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

/* 7.1.2 */
/* this is compiler keywords (not C keywords) must by used with function, variables and, pointers to work in prober way */
/* for ARM C (GCC, ADS) compilers no need for thDETe*/

#define DET_CODE                 /* used for ordinary code */
#define DET_CONFIG_DATA			/* used for cused for module configuration constants */
#define REGSPACE		 		/* used for staitc Pointer to Reg (ptr to volatile) carry adress of value that changed in any time by interrupts */
#define DET_VAR_INIT             /* for variables that are initialized with values after every reset */
#define DET_CONST                /* used for global or static constants */
#define DET_APPL_CONST           /* used for global or static pointer to constants */
#define DET_VAR_CLEARED          /* for variables that are cleared to zero after every reset */
#define DET_VAR_INIT             /* for variables that are initialized with values after every reset */





#endif 	/* COMPILER_CFG_H */
