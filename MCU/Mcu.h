/*
 * Mcu.h
 *
 *  Created on: Nov 19, 2019
 *  Author: Bebo
 *  AUTOSAR Version: 4.3.1
 *  DOC Name: AUTOSAR_SWS_MCUDriver.pdf
 *  Target:  ARM TivaC TM4C123GH6PM
 *
 */

#ifndef MCU_H_
#define MCU_H_

#include "../Common/Std_Types.h"
#include "Mcu_Cfg.h"

/*********************************************************************/


/* Published information */
#define MCU_MODULE_ID 30u
#define MCU_VENDOR_ID 0x002Bu   /* vendor id of NXP */

#define MCU_AR_RELEASE_MAJOR_VERSION 0x01u
#define MCU_AR_RELEASE_MINOR_VERSION 0x00u
#define MCU_AR_RELEASE_REVISION_VERSION 0x00u

#define MCU_SW_MAJOR_VERSION 0x04u
#define MCU_SW_MINOR_VERSION 0x03u
#define MCU_SW_PATCH_VERSION 0x01u


/*********************************************************************/

/* status value returned by the function Mcu_GetPllStatus of the MCU module
 * enumeration with the following values with #define */

typedef VAR( uint8, TYPEDEF )   Mcu_PllStatusType ;
#define MCU_PLL_STATUS_UNDEFINED    (0x00u)
#define MCU_PLL_LOCKED              (0x01u)
#define MCU_PLL_UNLOCKED            (0x02u)

/*********************************************************************/


/* this struct used for configuration of modes clock gating (run mode, sleep mode, deep sleep mode )
 * used in mcu_init to set setting for all modes and to switch between mode use Mcu_setMode function
 * run mode is default and process back to it after return from sleep or deep sleep mode by default */
typedef struct
{
    /* clock gate determine clock is enabled or disabled for the peripheral has more than one module
     * for examble UART has UART0, UART1 .. UART7 every bit in UART_ClockGate correspond to one module, LSB for UART0
     * 1bfor enable clock and 0b for disable clock */
    VAR( uint8, AUTOMATIC ) Watchdog_ClockGate ;
    VAR( uint8, AUTOMATIC ) GPT_16_32_ClockGate ;
    VAR( uint8, AUTOMATIC ) GPIO_ClockGate ;
    VAR( uint8, AUTOMATIC ) DMA_ClockGate ;
    VAR( uint8, AUTOMATIC ) Hibernation_ClockGate ;
    VAR( uint8, AUTOMATIC ) UART_ClockGate ;
    VAR( uint8, AUTOMATIC ) SSI_ClockGate ;
    VAR( uint8, AUTOMATIC ) I2C_ClockGate ;
    VAR( uint8, AUTOMATIC ) USB_ClockGate ;
    VAR( uint8, AUTOMATIC ) CAN_ClockGate ;
    VAR( uint8, AUTOMATIC ) ADC_ClockGate ;
    VAR( uint8, AUTOMATIC ) AnalogCompartor_ClockGate ;
    VAR( uint8, AUTOMATIC ) PWM_ClockGate ;
    VAR( uint8, AUTOMATIC ) QEI_ClockGate ;
    VAR( uint8, AUTOMATIC ) EEPROM_ClockGate ;
    VAR( uint8, AUTOMATIC ) GPT_32_64_ClockGate ;

} str_ModeClosckSetting ;

/* used for sleep mode and deep sleep mode for power settings and clock setting in deep sleep */

/* LDO value o/p in milli volt default is 1200 mv (run,sleep) and 900 in deep sleep
 * used for LDO element in str_ModePowerSetting
 * note for 900 mv max freq is 20Mhz */
#define LDO_900_MV      0x12
#define LDO_950_MV      0x13
#define LDO_1000_MV     0x14
#define LDO_1050_MV     0x15
#define LDO_1100_MV     0x16
#define LDO_1150_MV     0x17
#define LDO_1200_MV     0x18

/* used for SRAM_Power and Flash_power in str_ModePowerSetting
 * decrease power consumption but increase wake-up time */
#define FLASH_ACTIVE_MODE       0x00
#define FLASH_LOW_POWER_MODE    0x02

#define SRAM_ACTIVE_MODE        0x00
#define SRAM_STANDBY_MODE       0x01
#define SRAM_LOW_POWER_MODE     0x03

typedef struct
{
    /* used for sleep and deep sleep modes*/

    /* if true then the LDO setting will be set as factory recommended value */
    VAR( boolean, AUTOMATIC ) UseRecommendedSetting_LDO ;

    /* if true then software will request to power down PIOSC in deep sleep mode if not used by any hardware module */
    VAR( boolean, AUTOMATIC ) PIOSC_DeepSleep_PowerDown ;

    VAR( uint8, AUTOMATIC ) LDO ;   /* as it decrease system clock frequency must so */
    VAR( uint8, AUTOMATIC ) SRAM_Power ;    /* decrease power consumption but increase wake-up time*/
    VAR( uint8, AUTOMATIC ) Flash_Power ;

    /* this applied  in case of Deep sleep mode 0x00~0x3F divisor = DeepSleep_Prescaler + 1 :: max -> 64 */
    VAR( uint8, AUTOMATIC ) DeepSleep_Prescaler ;

    /* this determine oscillator used for system clock just apllied in deep sleep mode
     * DeepSleep_ClockSource take values as of  Oscillator_Source in str_ClockSetting except for MCU_PIOSC_4 */
    VAR( uint8, AUTOMATIC ) DeepSleep_ClockSource ;


} str_ModePowerSetting ;

/*********************************************************************/

#define UART_MODULES_NUMBER 8
#define SSI_MODULES_NUMBER 4
#define ADC_MODULES_NUMBER 2
#define OSCILLATORS_RCC2_BITS 3
#define PWM_PRESCALER_BITS 3


/* used for Oscillator_Source inside str_clockSetting */
#define MCU_MOSC        0x00u   /* main oscillator 16MHz according to fixed crystal on tiva kit */
#define MCU_PIOSC       0x01u   /* precision internal oscillator 16Mhz */
#define MCU_PIOSC_4     0x02u   /* PIOSC/4 -> 4Mhz */
#define MCU_LFIOSC      0x03u   /* low frequency internal oscillator 30 ~ 90 kHz not accurate freq normal 30kHz */
#define MCU_32_768      0x07u   /* 32.768kHz according to hibernation module crystal on tiva kit*/

/* used for MainCrystal_Value clockSetting  */
#define MCU_MAIN_CRYSTAL_16     0x15u   /* crystal connected to launch pad  support usb and pll */


typedef struct
{
    VAR( boolean, AUTOMATIC ) MainOSC_poweDown;      /* enable and disable of main oscillator, must be enabled to use USB_PLL (TRUE->Disable) */

    /* apllied only if oscillator o/p is PIOSC or enabled MOSC */
    VAR( boolean, AUTOMATIC) MainPLL_powerDown ;      /* enable and disable of main pll used to sysclock and ADC_clock (TRUE->Disable )*/

    /* applied only if MOSC is enabled */
    VAR( boolean, AUTOMATIC ) USB_PLL_powerDown ;     /* enable and disable of usb pllr, must be enabled to use USB (TRUE->Disable) */

    /*applied only if pll is source of sysclock and if true that make new prescaler optoins */
    VAR( boolean, AUTOMATIC ) MainPLL_preDiv ;    /* determine i/p of sysclock prescaler from pll 200 or 400 MHz (TRUE->400) */

    /* 2 bits for ADC 0,1 LSB for ADC0 */
    VAR( uint8, AUTOMATIC ) ADC_PIOSC   : ADC_MODULES_NUMBER ;         /* determine sourc of ADC clock PIOSC or (pll if powered up, sysclock if pll is down) (1->PIOSC) */

    /* 4 bits for SSI 0,1,2,3 LSB for SSI0 */
    VAR( uint8, AUTOMATIC ) BaudSSI_PIOSC : SSI_MODULES_NUMBER ;     /* determine source of SSI baud rate (PIOSC or sysclock) (1->PIOSC) */

    /* 8 bits for UART 0~7 LSB for UART0 */
    VAR( uint8, AUTOMATIC ) BaudUART_PIOSC : UART_MODULES_NUMBER ;    /* determine source of UART baud rate (PIOSC or sysclock) (1->PIOSC) */

    VAR( boolean, AUTOMATIC ) PWM_SysClock ;      /* determine source of PWM clock sysclock or divided sysclock (FALSE->sysclock)*/

    /* applied only if PWM_SysClock is TRUE*/
    /* 3 bits value of divisor 2^(PWM_Prescaler+1) */
    VAR( uint8, AUTOMATIC ) PWM_Prescaler : PWM_PRESCALER_BITS ;       /* determine value of divisor to sysclock o/p will clock of PWM */

    /* for MainPLL_powerDown is false in case of MainPLL_preDiv is FALSE (0~0x3F) is available (for PLL 0x00 & 0x01 is not used) */
    /* for MainPLL_powerDown is false in case of MainPLL_preDiv is TRUE (0~0x7F) is available and pll is used (0x00 0x01, 0x02, 0x03, 0x06) not used*/
    /* divisor = SysClock_Prescaler+1 */
    VAR( uint8, AUTOMATIC ) SysClock_Prescaler ;  /* determine value of divisor of osc or pll & result will be sysclock */

    /* only MOSC and PIOSC can by source for PLL */
    VAR( uint8, AUTOMATIC ) Oscillator_Source : OSCILLATORS_RCC2_BITS ;   /* choose which oscillator will be source of both sysclock and PLL from following enumerations */

    VAR( uint8, AUTOMATIC ) MainCrystal_Value   ;       /* choose the value of crystal drive the Main oscillator not all crystal value support PLL and USB */

} str_ClockSetting ;


/*********************************************************************/

/* used as values of RamSectionWriteSize */
#define ONE_BYTE    0x01u
#define TWO_BYTE    0x02u
#define FOUR_BYTE   0x04u

typedef struct
{
    VAR( uint8, AUTOMATIC ) RamDefaultValue ;            /* represent the Data pre-setting to be initialized for byte */
    VAR( uint8, AUTOMATIC ) RamSectionWriteSize ;            /* define the size in bytes of data which can be written into RAM at once ( 1, 2, 3 or, 4 ) */
    VAR( uint16, AUTOMATIC ) RamSectionSize ;            /* represents the MCU RAM Section size in bytes */
    P2VAR( uint32, AUTOMATIC , AUTOMATIC) RamSectionBaseAddress ;     /* represent the MCU RAM section base address */

} str_RAM_SectorSetting ;

/*********************************************************************/



typedef struct
{

    /* if true then same clock setting used in run mode are the same in sleep and deep sleep modes */
    VAR (boolean, AUTOMATIC) AutoClockGting ;

    /* used for funtion perform reset setting
     * must has same value as MCU_RESET_SETTING parameter in configuration to perform reset */
    VAR (uint8, MCU_CONFIG_DATA) ResetSetting ;

    /* this contain clock gating for every mode ( which peripheral clock is enabled and which not )*/
    VAR (str_ModeClosckSetting, AUTOMATIC) RunMode_ClockSetting ;
    VAR (str_ModeClosckSetting, AUTOMATIC) SleepMode_ClockSetting ;
    VAR (str_ModeClosckSetting, AUTOMATIC) DeepSleepMode_ClockSetting ;

    /* this setting is mcu dependent and tiva don't support change of any clock setting in sleep mode
     * setting of sleep mode clock the same as run mode except for clock gating  */
    VAR (str_ModePowerSetting, AUTOMATIC) SleepMode_PowerSetting ;
    VAR (str_ModePowerSetting, AUTOMATIC) DeepSleepMode_PowerSetting ;


    /* number of available clock settings (elements) in ClockSetting_array */
    VAR (uint8_least, MCU_CONFIG_DATA) ClockSetting_Number ;

    /* number of available RAM sectore */
    VAR (uint8_least, MCU_CONFIG_DATA) RAM_Sectors_Number ;

    /* array contain all clock setting to use pass setting id to Mcu_InitClock to set this setting */
    const str_ClockSetting  ClockSetting_array [MCU_MAX_CLOCK_SETTING_NUMBER] ;

    /* array contain all RAM sectors setting to use pass setting id to Mcu_InitRamSection to set this setting */
    const str_RAM_SectorSetting RAM_SectorSetting_array [MCU_MAX_RAM_SECTOR_NUMBER] ;


} Mcu_ConfigType ;
/*********************************************************************/


/* Specifies the (ID) for a clock setting, which is configured in the configuration structure
 * inside configuration structure passed to McuInit function there's many setting for clock
 *every setting has id and this is type for this id
 *every type shall be uint8, uint16 or uint32, depending on uC platform*/

typedef VAR( uint8_least, TYPEDEF )   Mcu_ClockType ;

/*********************************************************************/

/* Mcu_ResetType, represents the different reset that a specified MCU can have */
/* take value form on of McuResetReasonConf container parameters */

#define MCU_EXTERNAL_RESET          0x01u
#define MCU_POWER_ON_RESET          0x02u
#define MCU_BROWN_OUT_RESET         0x04u
#define MCU_WATCHDOG_TIMER0_RESET   0x08u
#define MCU_SW_RESET                0x10u
#define MCU_WATCHDOG_TIMER1_RESET   0x20u
#define MCU_MOSCFAIL                0x00010000u
#define MCU_RESET_UNDEFINED         0xFFFFFFFFu


typedef VAR( uint32, TYPEDEF ) Mcu_ResetType;

/*********************************************************************/

/* specifies the reset reason in raw register format, read from a reset status register */

typedef VAR( uint32, AUTOMATIC ) Mcu_RawResetType ;

/*********************************************************************/



/* specifies the (ID) for a MCU mode, which is configured in the configuration structure
 * inside configuration structure (run, sleep, deep sleep modes) passed to Mcu_SetMode functions to set mode with this ID
 * no need for ID for run mode as it's default mode of mcu which applied after return form others mode
 * take types values from McuMode parameter in McuModeSettingConf container */

typedef VAR( uint8_least, TYPEDEF ) Mcu_ModeType ;


/*********************************************************************/

/* The Mcu_RamSectionType specifies the identification (ID) for a RAM section,
 *  configured via the configuration structure
 *  same as Mcu_ModeType but for ram sections */

typedef VAR( uint8_least, TYPEDEF ) Mcu_RamSectionType ;

/*********************************************************************/

/* Ram State data type returned by the function Mcu_GetRamState of the Mcu module.*/

#define MCU_RAMSTATE_INVALID    0x55
#define MCU_RAMSTATE_VALID      0xAA

typedef VAR( uint8, TYPEDEF ) Mcu_RamStateType ;

/*********************************************************************/


/*
    - Synchronous, Non Reentrant, Service ID : 0x00
    - shall initialize the MCU module, i.e. make the configuration settings for power down, clock and RAM sections visible within the MCU module.
    - After execution Mcu_Init, the configuration data are accessible and can be used by the MCU module functions

*/

FUNC( void, MUC_CODE ) Mcu_Init( P2CONST( Mcu_ConfigType, AUTOMATIC, AUTOMATIC ) ConfigPtr ) ;


/*********************************************************************/


/*
     - Synchronous, Non reentrant, Service ID: 0x01
     - call the function only after the MCU module has been initialized Mcu_InitRamSection
     - shall fill the memory from address McuRamSectionBaseAddress up to address McuRamSectionBaseAddress + McuRamSectionSize - 1
       with the byte-value contained in McuRamDefaultValue and by writing at once a number of bytes defined by McuRamSectionWriteSize

     - Shall initialized the SRAM with McuRamDefaultValue which is in our case 0
     - Takes in section ID in order to initialize it but we will consider our memory section the entire RAM so we pass ID: 0
       in the array of RAM sections.
     - This behavior is needed in order to clear the memory to prevent garbage values

 */


FUNC( Std_ReturnType, MCU_CODE)  Mcu_InitRamSection( Mcu_RamSectionType RamSection ) ;

/*********************************************************************/

/*
    - Synchronous, Non Reentrant, Service ID : 0x02
    - shall initialize the PLL and other MCU specific clock options. The clock configuration parameters are provided via the configuration structure
    - shall start the PLL lock procedure (if PLL shall be initialized) and shall return without waiting until the PLL is locked
    - shall only call the function after the MCU module has been initialized using the function Mcu_Init.
    - function Mcu_InitClock shall be disabled if the parameter McuInitClock is set to FALSE
    - enable and set the MCU clock.(i.e. Cpu clock, Peripheral Clock, Prescalers, Multipliers
    - many clock setting is available inside structure passed to McuInit function to choose one of this setting and  apply it pass this setting id to this functions
    - this clock setting are applied in run and sleep modes

*/

#if MCU_INIT_CLOCK == TRUE
FUNC( Std_ReturnType, MCU_CODE ) Mcu_InitClock( Mcu_ClockType ClockSetting ) ;
#endif

/*********************************************************************/



/*
    - Synchronous, Non Reentrant, Service ID : 0x03
    - shall activate the PLL clock to the MCU clock distribution
    - shall remove the current clock source (for example internal oscillator clock) from MCU clock distribution
    - shall only call the function after the status of the PLL has been detected as locked by the function Mcu_GetPllStatus
    - If the function is called before PLL has locked, this function shall return E_NOT_OK immediately, without any further action
    - The function Mcu_DistributePllClock shall be available if the pre-compile parameter McuNoPll is set to FALSE. Otherwise, this Api has to be disabled


*/

#if MCU_NO_PLL == FALSE

FUNC(Std_ReturnType, MCU_CODE) Mcu_DistributePllClock( void ) ;

#endif /* MCU_NO_PLL */


/*********************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x04
    - shall return the lock status of the PLL
    - shall return MCU_PLL_STATUS_UNDEFINED if this function is called prior to calling of the function Mcu_Init.
    - shall return MCU_PLL_STATUS_UNDEFINED if the pre-compile parameter McuNoPll is set to TRUE

*/

FUNC( Mcu_PllStatusType, MCU_CODE ) Mcu_GetPllStatus( void ) ;

/*********************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x05
    - shall read the reset reason from the hardware and return this reason
    - If the hardware does not support hardware detection  the return value shall always be MCU_POWER_ON_RESET
    - shall return MCU_RESET_UNDEFINED if this function is called prior to calling of the function Mcu_Init
    - User should ensure that the reset reason is cleared once it has been read out to avoid multiple reset reasons
    - In case of multiple calls to this function the return value should always be the same

*/

FUNC(Mcu_ResetType, MCU_CODE) Mcu_GetResetReason( void ) ;

/*********************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x05
    - shall return an implementation specific value does not correspond to a valid value of the reset status register and not equal to 0
      if this function is called prior to calling of the function Mcu_Init
    - shall read the reset raw value from the hardware register if the hardware supports this
    - User should ensure that the reset reason is cleared once it has been read out to avoid multiple reset reasons
    - In case of multiple calls to this function the return value should always be the same

*/

FUNC(Mcu_RawResetType, MCU_CODE) Mcu_GetResetRawValue( void ) ;

/*********************************************************************/

/*
    - Synchronous, nonReentrant, Service ID : 0x07
    - shall perform a microcontroller reset by using the hardware feature of the microcontroller
    - shall perform the reset type which is configured in the configuration set
    - shall only call the function after the MCU module has been initialized by the function Mcu_Init
    - is only available if the pre-compile parameter McuPerformResetApi is set to TRUE

*/

#if MCU_PERFORM_RESET_API ==TRUE

FUNC(void, MCU_CODE) Mcu_PerformReset( void ) ;

#endif

/*********************************************************************/


/*
    - Synchronous, Reentrant, Service ID : 0x08
    - shall set the MCU power mode. In case of CPU power down mode, the function Mcu_SetMode returns after it has performed a wake-up
    - shall only call the function after the MCU module has been initialized by the function Mcu_Init
    - MCU module shall provide a service to activate MCU reduced power modes
    - this function just activate reduced power modes sleep or deep sleep mode depend on McuMode parameter
    - sleep mode clock setting the same as run mode so if any setting need to be changed , do it before set the sleep mode
    - deep sleep mode has its oscillator setting and prescaler, and after wake up run mode setting automatically applied again
    - LDO value changed by change of mode and max frequency can be achieved change according to this LDO value
      so take care that clock setting is matched with LDO value
    - main pll is disabled in deep sleep mode
    - The environment of the function Mcu_SetMode has to ensure that the ECU is ready for reduced power mode activation
    - API Mcu_SetMode assumes that all interrupts are disabled prior the call of the API by the calling instance.
      The implementation has to take care that no wake-up interrupt event is lost.
      This could be achieved by a check whether pending wakeup interrupts already have occurred even
      if Mcu_SetMode has not set the controller to power down mode yet.
    -  this function access registers need privileged level
    - all interrupts must be masked before calling of this function
    - this function must called in thread mode not handler mode (in handler mode not all interrupts will cause wake up)
    - in case a masked interrupt happen after calling this function and befor to execute sleep instruction, sleep mode won't be entered and will return
*/


FUNC ( void, MCU_CODE ) Mcu_SetMode( Mcu_ModeType McuMode ) ;

/*********************************************************************/


/*
    - Synchronous, Reentrant, Service ID : 0x0A
    - provides the actual status of the microcontroller Ram. (if supported)
    - MCU modules environment shall call this function only if the MCU module has been already initialized
    - function shall be available to the Mcu_GetRamState user if the pre-compile parameter is set to TRUE McuGetRamStateApi
    - tiva c don't support ram state so if this service is enabled will always return MCU_RAMSTATE_VALID

*/

#if MCU_VERSION_INFO_API == STD_ON

FUNC( Mcu_RamStateType, MCU_CODE ) Mcu_GetRamState( void ) ;

#endif

/*********************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x09
    - get the version information of this module
    - If DET is enabled, the function shall raise MCU_E_PARAM_POINTER, if the argument is NULL pointer and return without any action.
*/

#if MCU_VERSION_INFO_API == STD_ON

FUNC( void, MCU_CODE ) Mcu_GetVersionInfo( Std_VersionInfoType* VersionInfo ) ;

#endif


#endif /* MCU_H_ */




