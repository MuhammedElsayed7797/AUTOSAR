/*
 * Mcu.c
 *
 *  Created on: Nov 19, 2019
 *  Author: Bebo
 *  AUTOSAR Version: 4.3.1
 *  DOC Name: AUTOSAR_SWS_MCUDriver.pdf
 *  Target:  ARM TivaC TM4C123GH6PM
 *
 */

#include <MCU/Common/Mcu_MemMap.h>
#include <MCU/Common/SchM_Mcu.h>
#include <MCU/Mcu.h>
#include <MCU/Mcu_Cbk.h>
#include "..\DET\Det.h"


/*********************************************************************************************************/

/* services IDs */
#define MCU_INIT_ID                 0x00u
#define MCU_INIT_RAM_ID             0x01u
#define MCU_INIT_CLOCK_ID           0x02u
#define MCU_DISTRIBUTE_PLL_CLOCK_ID 0x03u
#define MCU_GET_PLL_STATUS_ID       0x04u
#define MCU_GET_RESET_REASON_ID     0x05u
#define MCU_GET_RESET_RAW_VALUE_ID  0x06u
#define MCU_PERFORM_RESET_ID        0x07u
#define MCU_SET_MODE_ID             0x08u
#define MCU_GET_VERSION_INFO_ID     0x09u
#define MCU_GET_RAM_STATUS_ID       0x0Au

/********************************************/

/* errors IDs */
#define MCU_E_PARAM_CONFIG      0x0Au
#define MCU_E_PARAM_CLOCK       0x0Bu
#define MCU_E_PARAM_MODE        0x0Cu
#define MCU_E_PARAM_RAMSECTION  0x0Du
#define MCU_E_PLL_NOT_LOCKED    0x0Eu
#define MCU_E_UNINIT            0x0Fu
#define MCU_E_PARAM_POINTER     0x10u
#define MCU_E_INIT_FAILED       0x11u

/********************************************/

/* note: must in startup code make RCC override RCC2  and make xtal value with 16MHz */
#define MCU_RCC         (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE060 ))
#define MCU_RCC2        (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE070 ))
#define MCU_PLLSTAT     (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE168 ))
#define ADC_CLOCK_SOURCE_BASE       (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x40038FC8 ))
#define SSI_BAUD_SOURCE_BASE        (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x40008FC8 ))
#define UART_BAUD_SOURCE_BASE       (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x4000CFC8 ))
#define SLEEP_MODE_LDO              (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE1B4 ))
#define DEEP_SLEEP_MODE_LDO         (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE1BC ))
#define SLEEP_MODE_POWER            (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE188 ))
#define DEEP_SLEEP_MODE_POWER       (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE18C ))
#define DEEP_SLEEP_MODE_CONF        (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE144 ))
#define SYSTEM_CONTROL_REG          (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0xE000ED10 ))  /* This register can only be accessed from privileged mode */
#define RESET_REASON_REG            (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE05C ))
#define PERFORM_RESET_REG           (*(( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0xE000ED0C ))




/********************************************/

/* first register adress in run, sleep, deep sleep modes clock gating registers (for watchdog module )*/
#define RUN_CLOCK_REGS_START_REG    ((( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE600 ))
#define SLEEP_CLOCK_REGS_START_REG    ((( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE700 ))
#define DEEP_SLEEP_CLOCK_REGS_START_REG    ((( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE800 ))


#define MCU_CLOCK_REGS_OFFSET       0x01u            /* offset between registers that responsible for peripherals clock enable (4 bytes) */
#define MCU_NUMBER_OF_PERIPHERALS   16u

#define XTAL_BIT_POSITION           6u
#define LOW_POWER_MODE_BIT          2u
#define LDO_DEFAULT_ENABLE_BIT      31u
#define SRAM_POWER_FIRST_BIT        0u
#define FLASH_POWER_FIRST_BIT       4u
#define DEEP_SLEEP_PRESCALER_BIT    23u
#define DEEP_SLEEP_OSCILLATOR_BIT   4u
#define DEEP_SLEEP_PIOSC_DOWN       1u
#define LDO_CLEAR_MASK              0xFFu
#define MEMORY_POWER_CLEAR_MASK     0b00110011u
#define DEEP_SLEEP_CONF_CLEAR_MASK  0x1F800072u
#define RESC_CLEAR_MASK             0x0001003Fu
#define PERFORM_RESET_CLEAR_MASK    0xFFFF0004u
#define SLEEP_GATING_REGS_OFFSET    0x040u      /* offset of 0x100 due to increment on uint32 pointer */
#define DEEP_GATING_REGS_OFFSET     0x080u      /* offset of 0x200 due to increment on uint32 pointer */

/********************************************/


/* offset of each module base adress from perivous one */
#define ADC_CLOCK_SORUCE_OFFSET     0x0400u
#define SSI_BAUD_SORUCE_OFFSET      0x0400u
#define UART_BAUD_SORUCE_OFFSET     0x0400u


/********************************************/


#define OSC_CHOSSE_BIT_FILED_START      4u
#define USB_PLL_PWRDN_BIT_POISITION     14u
#define MAIN_PLL_PWRDN_BIT_POISITION    13u
#define BYBASS_BIT_POISITION            11u  /* used to choose source of system clock */
#define PWM_CLOCK_SOURCE_BIT_POISITION  20u
#define PWM_PRESCALER_BIT_FILED_START   17u
#define SYS_DIV_BIT_POSITION            22u  /* determine if system clock used undivided or not */
#define SYS_DIVISOR_BIT_FIELD_START     23u  /* pll or osc divisor start bit POSITION in case MainPLL_preDiv is FALSE */
#define PLL_DIVISOR_BIT_FIELD_START     22u  /* pll is used as source for sys clock and this is start bit of its 7-bit divisor */
#define AUTO_CLOCK_BIT                  27u  /* position of auto clock gating bit in RCC reg */
#define MAIN_PLL_PRE_DIVIDE_BIT         30u
#define RCC2_OVERRIDE_RCC_BIT           31u

/*********************************************************************************************************/


/* Addressing values for RESET_CAUSE register bit positions */

#define EXT         0u
#define POR         1u
#define BOR         2u
#define WDT0        3u
#define SW          4u
#define WDT1        5u
#define MOSCFAIL    16u

#define RAW_RESET_UNIQUE_VALUE  0xFFFFFFFFu
#define VECTKEY_VALUE           0x05FAu
#define RESET_BIT_POSITION      2u
#define HALF_WORD_SIZE          16u


/*********************************************************************************************************/

/* this variable used to check of pll status inside Mcu_DistributePllClock function, variable modified using Mcu_GetPllStatus,
 *  and it set to MCU_PLL_UNLOCKED when using Mcu_InitClock or Mcu_Init functions  */

static VAR( Mcu_PllStatusType, MCU_VAR_INIT ) pllStatusCheck = MCU_PLL_STATUS_UNDEFINED ;

static VAR( Mcu_ClockType, MCU_VAR_INIT ) Curren_ClockSetting = 0 ;        /* to pass current clock setting ID to Mcu_DistributePllClock function */

/* used for get number of clock settings available in configuration structure to check Mcu_ClockInit function i/p */
 P2CONST( uint8_least, MCU_CONFIG_DATA, MCU_APPL_CONST ) ptr_ClockSetting_Number = NULL_PTR ;

/* used for get number of RAM sectors settings available in configuration structure to check Mcu_InitRamSection function i/p */
static P2CONST( uint8_least, MCU_CONFIG_DATA, MCU_APPL_CONST ) ptr_RAM_Sectors_Number = NULL_PTR ;

/* pointer to array of  constant str_ClockSetting structures this pointer is initialized by Mcu_init to make configuration data
 * of clock setting visible to Mcu_InitClock function to one from this setting to apply according to i/p which is setthing ID */

static const str_ClockSetting (* ptr_ClockSetting_array ) [] = NULL_PTR ;

/* pointer to array of  constant str_RAM_SectorSetting structures this pointer is initialized by Mcu_init to make configuration data
 * of RAM sector setting visible to Mcu_InitRamSection function to one from this setting to apply according to i/p which is setthing ID */

static const str_RAM_SectorSetting (* ptr_RAM_SectorSetting_array ) [] = NULL_PTR;


/* used for Mcu_PerformReset function to determine reset type */
static P2CONST( uint8, MCU_CONFIG_DATA, MCU_APPL_CONST ) ptr_ResetSetting = NULL_PTR;


/* used to check if clock disabled for those peripherals to not access their register avoiding traps */
static VAR( uint8, MCU_VAR_INIT) ADC_State = 0 ;

static VAR( uint8, MCU_VAR_INIT) UART_State = 0 ;

static VAR( uint8, MCU_VAR_INIT) SSI_State = 0 ;



/************************************************************************************/

/*********************************************************************************************************/

/*
    - Synchronous, Non Reentrant, Service ID : 0x00
    - shall initialize the MCU module, i.e. make the configuration settings for power down, clock and RAM sections visible within the MCU module.
    - After execution Mcu_Init, the configuration data are accessible and can be used by the MCU module functions

*/

FUNC( void , MUC_CODE ) Mcu_Init( P2CONST( Mcu_ConfigType, AUTOMATIC, AUTOMATIC ) ConfigPtr )
{
    CS_ON ;

    VAR( uint8, AUTOMATIC ) localCounter = 0 ;
    VAR( uint32, AUTOMATIC ) tempVar = 0 ;
    P2VAR( uint8, AUTOMATIC, AUTOMATIC ) tempPtr_2 = NULL_PTR ;     /* to carry clock gating values */
    volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ClockGating_array [ MCU_NUMBER_OF_PERIPHERALS ] =
    {
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE600,    /* watch dog timer run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE604,    /* 16/32 GPT timer run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE608,    /* GPIO  run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE60C,    /* uDMA  run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE614,    /* HIB  run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE618,    /* UART run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE61C,    /* SSI run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE620,    /* 12C run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE628,    /* USB run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE634,    /* CAN run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE638,    /* ADC run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE63C,    /* ACMP run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE640,    /* PWM run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE644,    /* QEI run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE658,    /* EEPROM run mode clock gating register adress */
     ( volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) ) 0x400FE65C     /* 32/64 GPT run mode clock gating register adress */
    };

    #if MCU_DEV_ERROR_DETECT == STD_OFF

    /* initialize ptr_ResetSetting used Mcu_PerforReset function (make configuration visible to function) */
    ptr_ResetSetting = &( ConfigPtr->ResetSetting ) ;

    /* initialize ptr_ClockSetting_Number used Mcu_InitClock function (make configuration visible to function) */
    ptr_ClockSetting_Number = &( ConfigPtr->ClockSetting_Number ) ;

    /* initialize ptr_ClockSetting_array used Mcu_InitClock function (make configuration visible to function) */
    ptr_ClockSetting_array = &(ConfigPtr->ClockSetting_array) ;

    /* initialize ptr_RAM_Sectors_Number used Mcu_InitRamSection function (make configuration visible to function) */
    ptr_RAM_Sectors_Number = &( ConfigPtr->RAM_Sectors_Number ) ;

    /* initialize ptr_RAM_SectorSetting_array used Mcu_InitRamSection function (make configuration visible to function) */
    ptr_RAM_SectorSetting_array = &(ConfigPtr->RAM_SectorSetting_array) ;

    /* make RCC2 reg override RCC in similar function bit fields */
    MCU_RCC2 |=  ( 0x01u << RCC2_OVERRIDE_RCC_BIT ) ;

    /* initialize run mode clock settings */

    tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->RunMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

    for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
    {
        /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
        * ( ClockGating_array[localCounter] ) = *tempPtr_2 ;
        tempPtr_2 ++ ;
    }


    /* used to check if clock disabled for those peripherals to not access their register avoiding traps */

    ADC_State = ConfigPtr->RunMode_ClockSetting.ADC_ClockGate ;
    UART_State = ConfigPtr->RunMode_ClockSetting.UART_ClockGate ;
    SSI_State = ConfigPtr->RunMode_ClockSetting.SSI_ClockGate ;


    if ( TRUE == (ConfigPtr->AutoClockGting) ) /* check if clockGating is enabled */
    {
        MCU_RCC &= ~( 0x01 << AUTO_CLOCK_BIT ) ;
    }
    else
    {
        MCU_RCC |= ( 0x01 << AUTO_CLOCK_BIT ) ;

        /* initialize sleep mode clock settings */
        tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->SleepMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

        for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
        {
            /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
            * ( ClockGating_array[localCounter] + SLEEP_GATING_REGS_OFFSET ) = *tempPtr_2 ;
            tempPtr_2 ++ ;
        }

        /* initialize deep sleep mode clock settings */
        tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->DeepSleepMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

        for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
        {
            /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
            * ( ClockGating_array[localCounter] + DEEP_GATING_REGS_OFFSET ) = *tempPtr_2 ;
            tempPtr_2 ++ ;

        } /* for*/

    } /* else*/


    /* initialize sleep mode power settings */

    if ( TRUE == ConfigPtr->SleepMode_PowerSetting.UseRecommendedSetting_LDO ) /* use factory setting for LDO */
    {
        SLEEP_MODE_LDO &= ~(0x01u << LDO_DEFAULT_ENABLE_BIT) ;
    }
    else    /* use configuration vaule */
    {
        /* read modify write process */
        tempVar = SLEEP_MODE_LDO ;
        tempVar |= (0x01u << LDO_DEFAULT_ENABLE_BIT) ;
        tempVar &= ~( LDO_CLEAR_MASK ) ;
        tempVar |= ConfigPtr->SleepMode_PowerSetting.LDO ;
        SLEEP_MODE_LDO = tempVar ;
    }

    /* clear bits of memory power configuration in sleep mode */
    SLEEP_MODE_POWER &= ~( MEMORY_POWER_CLEAR_MASK ) ;
    /* set power level for sram and flash in sleep mode */
    SLEEP_MODE_POWER |= ( (ConfigPtr->SleepMode_PowerSetting.SRAM_Power) << SRAM_POWER_FIRST_BIT ) ;
    SLEEP_MODE_POWER |= ( (ConfigPtr->SleepMode_PowerSetting.Flash_Power) << FLASH_POWER_FIRST_BIT ) ;



    /* initialize deep sleep mode power settings */

    if ( TRUE == ConfigPtr->DeepSleepMode_PowerSetting.UseRecommendedSetting_LDO ) /* use factory setting for LDO */
    {
        DEEP_SLEEP_MODE_LDO &= ~(0x01 << LDO_DEFAULT_ENABLE_BIT) ;
    }
    else    /* use configuration vaule */
    {
        /* read modify write process */
        tempVar = SLEEP_MODE_LDO ;
        tempVar |= (0x01u << LDO_DEFAULT_ENABLE_BIT) ;
        tempVar &= ~( LDO_CLEAR_MASK ) ;
        tempVar |= ConfigPtr->DeepSleepMode_PowerSetting.LDO ;
        DEEP_SLEEP_MODE_LDO = tempVar ;
    }

    /* clear bits of memory power configuration in sleep mode */
    DEEP_SLEEP_MODE_POWER &= ~( MEMORY_POWER_CLEAR_MASK ) ;
    /* set power level for sram and flash in sleep mode */
    DEEP_SLEEP_MODE_POWER |= ( (ConfigPtr->DeepSleepMode_PowerSetting.SRAM_Power) << SRAM_POWER_FIRST_BIT ) ;
    DEEP_SLEEP_MODE_POWER |= ( (ConfigPtr->DeepSleepMode_PowerSetting.Flash_Power) << FLASH_POWER_FIRST_BIT ) ;

    /* set clock setting in deep sleep mode clock source, prescaler, PIOSC power-down request*/
    /* read modify write operation */
    tempVar = DEEP_SLEEP_MODE_CONF ;
    tempVar &= ~( DEEP_SLEEP_CONF_CLEAR_MASK ) ;
    /* set divisor */
    tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.DeepSleep_Prescaler) << DEEP_SLEEP_PRESCALER_BIT ) ;
    /* set oscillator source */
    tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.DeepSleep_ClockSource) << DEEP_SLEEP_OSCILLATOR_BIT ) ;
    /* set request for power down the PIOSC */
    tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.PIOSC_DeepSleep_PowerDown) << DEEP_SLEEP_PIOSC_DOWN ) ;
    DEEP_SLEEP_MODE_CONF = tempVar ;

    #endif

/********************************************************************/

    #if MCU_DEV_ERROR_DETECT == STD_ON

    if ( NULL_PTR == ConfigPtr )
    {
        /* wrong i/p parameter (null pointer)*/
        Det_ReportError (MCU_MODULE_ID, 0, MCU_INIT_ID , MCU_E_PARAM_POINTER) ;
    }
    else
    {
        /* initialize ptr_ResetSetting used Mcu_PerforReset function (make configuration visible to function) */
        ptr_ResetSetting = &( ConfigPtr->ResetSetting ) ;

        /* initialize ptr_ClockSetting_Number used Mcu_InitClock function (make configuration visible to function) */
        ptr_ClockSetting_Number = &( ConfigPtr->ClockSetting_Number ) ;

        /* initialize ptr_ClockSetting_array used Mcu_InitClock function (make configuration visible to function) */
        ptr_ClockSetting_array = &(ConfigPtr->ClockSetting_array) ;

        /* initialize ptr_RAM_Sectors_Number used Mcu_InitRamSection function (make configuration visible to function) */
        ptr_RAM_Sectors_Number = &( ConfigPtr->RAM_Sectors_Number ) ;

        /* initialize ptr_RAM_SectorSetting_array used Mcu_InitRamSection function (make configuration visible to function) */
        ptr_RAM_SectorSetting_array = &(ConfigPtr->RAM_SectorSetting_array) ;

        /* make RCC2 reg override RCC in similar function bit fields */
        MCU_RCC2 |=  ( 0x01u << RCC2_OVERRIDE_RCC_BIT ) ;

        /* initialize run mode clock settings */

        tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->RunMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

        for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
        {
            /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
            * ( ClockGating_array[localCounter] ) = *tempPtr_2 ;
            tempPtr_2 ++ ;
        }


        /* used to check if clock disabled for those peripherals to not access their register avoiding traps */

        ADC_State = ConfigPtr->RunMode_ClockSetting.ADC_ClockGate ;
        UART_State = ConfigPtr->RunMode_ClockSetting.UART_ClockGate ;
        SSI_State = ConfigPtr->RunMode_ClockSetting.SSI_ClockGate ;


        if ( TRUE == (ConfigPtr->AutoClockGting) ) /* check if clockGating is enabled */
        {
            MCU_RCC &= ~( 0x01 << AUTO_CLOCK_BIT ) ;
        }
        else
        {
            MCU_RCC |= ( 0x01 << AUTO_CLOCK_BIT ) ;

            /* initialize sleep mode clock settings */
            tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->SleepMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

            for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
            {
                /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
                * ( ClockGating_array[localCounter] + SLEEP_GATING_REGS_OFFSET ) = *tempPtr_2 ;
                tempPtr_2 ++ ;
            }

            /* initialize deep sleep mode clock settings */
            tempPtr_2 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) &(ConfigPtr->DeepSleepMode_ClockSetting) ;   /* point to first clock gating value (watchdog) */

            for( localCounter = 0 ; localCounter < MCU_NUMBER_OF_PERIPHERALS ; localCounter++ )
            {
                /* in case of future modification on reg reserved bits it will increas number of modules so writing in this way won't be wrong */
                * ( ClockGating_array[localCounter] + DEEP_GATING_REGS_OFFSET ) = *tempPtr_2 ;
                tempPtr_2 ++ ;

            } /* for*/

        } /* else*/


        /* initialize sleep mode power settings */

        if ( TRUE == ConfigPtr->SleepMode_PowerSetting.UseRecommendedSetting_LDO ) /* use factory setting for LDO */
        {
            SLEEP_MODE_LDO &= ~(0x01u << LDO_DEFAULT_ENABLE_BIT) ;
        }
        else    /* use configuration vaule */
        {
            /* read modify write process */
            tempVar = SLEEP_MODE_LDO ;
            tempVar |= (0x01u << LDO_DEFAULT_ENABLE_BIT) ;
            tempVar &= ~( LDO_CLEAR_MASK ) ;
            tempVar |= ConfigPtr->SleepMode_PowerSetting.LDO ;
            SLEEP_MODE_LDO = tempVar ;
        }

        /* clear bits of memory power configuration in sleep mode */
        SLEEP_MODE_POWER &= ~( MEMORY_POWER_CLEAR_MASK ) ;
        /* set power level for sram and flash in sleep mode */
        SLEEP_MODE_POWER |= ( (ConfigPtr->SleepMode_PowerSetting.SRAM_Power) << SRAM_POWER_FIRST_BIT ) ;
        SLEEP_MODE_POWER |= ( (ConfigPtr->SleepMode_PowerSetting.Flash_Power) << FLASH_POWER_FIRST_BIT ) ;



        /* initialize deep sleep mode power settings */

        if ( TRUE == ConfigPtr->DeepSleepMode_PowerSetting.UseRecommendedSetting_LDO ) /* use factory setting for LDO */
        {
            DEEP_SLEEP_MODE_LDO &= ~(0x01 << LDO_DEFAULT_ENABLE_BIT) ;
        }
        else    /* use configuration vaule */
        {
            /* read modify write process */
            tempVar = SLEEP_MODE_LDO ;
            tempVar |= (0x01u << LDO_DEFAULT_ENABLE_BIT) ;
            tempVar &= ~( LDO_CLEAR_MASK ) ;
            tempVar |= ConfigPtr->DeepSleepMode_PowerSetting.LDO ;
            DEEP_SLEEP_MODE_LDO = tempVar ;
        }

        /* clear bits of memory power configuration in sleep mode */
        DEEP_SLEEP_MODE_POWER &= ~( MEMORY_POWER_CLEAR_MASK ) ;
        /* set power level for sram and flash in sleep mode */
        DEEP_SLEEP_MODE_POWER |= ( (ConfigPtr->DeepSleepMode_PowerSetting.SRAM_Power) << SRAM_POWER_FIRST_BIT ) ;
        DEEP_SLEEP_MODE_POWER |= ( (ConfigPtr->DeepSleepMode_PowerSetting.Flash_Power) << FLASH_POWER_FIRST_BIT ) ;

        /* set clock setting in deep sleep mode clock source, prescaler, PIOSC power-down request*/
        /* read modify write operation */
        tempVar = DEEP_SLEEP_MODE_CONF ;
        tempVar &= ~( DEEP_SLEEP_CONF_CLEAR_MASK ) ;
        /* set divisor */
        tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.DeepSleep_Prescaler) << DEEP_SLEEP_PRESCALER_BIT ) ;
        /* set oscillator source */
        tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.DeepSleep_ClockSource) << DEEP_SLEEP_OSCILLATOR_BIT ) ;
        /* set request for power down the PIOSC */
        tempVar |= ( (ConfigPtr->DeepSleepMode_PowerSetting.PIOSC_DeepSleep_PowerDown) << DEEP_SLEEP_PIOSC_DOWN ) ;
        DEEP_SLEEP_MODE_CONF = tempVar ;

    } /* else */

    #endif

    pllStatusCheck = MCU_PLL_UNLOCKED ; /* as indication for function must start after McuInit */

    CS_OFF ;

    return  ;
}


/*********************************************************************************************************/


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


FUNC( Std_ReturnType, MCU_CODE)  Mcu_InitRamSection( Mcu_RamSectionType RamSection )
{

    CS_ON ;

    /* case error detection is off */
    /* with Mcu_InitRamSection() function we better no use development error to prevent stack using while we're initializing the entire RAM */

    VAR( Std_ReturnType, AUTOMATIC ) returnResult = E_NOT_OK ;
    VAR( uint32, AUTOMATIC ) InitData = 0u ;
    VAR( uint8, AUTOMATIC )  WriteSize = 0u ;
    VAR( uint16, AUTOMATIC ) SectionSize = 0u ;
    VAR (uint16, AUTOMATIC) LocalCounter = 0u ;
    P2VAR( uint8, AUTOMATIC, AUTOMATIC )  RamBaseAddressPtr_1 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) ( &InitData ) ;
    P2VAR( uint16, AUTOMATIC, AUTOMATIC ) RamBaseAddressPtr_2 = NULL_PTR ;
    P2VAR( uint32, AUTOMATIC, AUTOMATIC ) RamBaseAddressPtr_4 = NULL_PTR ;


    #if MCU_DEV_ERROR_DETECT == STD_OFF

    InitData = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamDefaultValue ;
    RamBaseAddressPtr = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionBaseAddress ;
    SectionSize = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionSize ;
    WriteSize = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionWriteSize ;

    for( LocalCounter = 0u; LocalCounter < SectionSize/WriteSize ; LocalCounter++ )
    {

        *RamBaseAddressPtr = InitData ;

        RamBaseAddressPtr++ ; /* jump (write size) bytes at a time */

    }

    #endif


/********************************************************************/

    /* case error detection is on */
    #if MCU_DEV_ERROR_DETECT == STD_ON



    if( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_INIT_RAM_ID , MCU_E_UNINIT) ;

    }
    else if ( RamSection >= (*ptr_RAM_Sectors_Number) ) /* check for valid section */
    {

        Det_ReportError (MCU_MODULE_ID, 0, MCU_INIT_RAM_ID , MCU_E_PARAM_RAMSECTION) ;

    }
    else
    {

        returnResult = E_NOT_OK ;

        (*RamBaseAddressPtr_1) = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamDefaultValue ;
        RamBaseAddressPtr_1 ++ ;
        (*RamBaseAddressPtr_1) = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamDefaultValue ;
        RamBaseAddressPtr_1 ++ ;
        (*RamBaseAddressPtr_1) = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamDefaultValue ;
        RamBaseAddressPtr_1 ++ ;
        (*RamBaseAddressPtr_1) = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamDefaultValue ;

        SectionSize = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionSize ;
        WriteSize = ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionWriteSize ;

        switch ( WriteSize )
        {
            case ONE_BYTE :
            {
                RamBaseAddressPtr_1 = ( P2VAR( uint8, AUTOMATIC, AUTOMATIC ) ) ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionBaseAddress ;

                for(LocalCounter = 0u; LocalCounter < SectionSize/WriteSize; LocalCounter++)
                {

                    *RamBaseAddressPtr_1 = ( VAR( uint8, AUTOMATIC ) ) InitData;
                    RamBaseAddressPtr_1 ++;     /* jump (write size) bytes at a time */

                }

                break ;
            }

            case TWO_BYTE :
            {
                RamBaseAddressPtr_2 =  ( P2VAR( uint16, AUTOMATIC, AUTOMATIC ) ) ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionBaseAddress ;

                for(LocalCounter = 0u; LocalCounter < SectionSize/WriteSize; LocalCounter++)
                {

                    *RamBaseAddressPtr_2 = ( VAR( uint16, AUTOMATIC ) ) InitData;
                    RamBaseAddressPtr_2 ++;     /* jump (write size) bytes at a time */

                }

                break ;
            }

            case FOUR_BYTE :
            {
                RamBaseAddressPtr_4 =  ( P2VAR( uint32, AUTOMATIC, AUTOMATIC ) ) ( (*ptr_RAM_SectorSetting_array) [RamSection] ).RamSectionBaseAddress ;

                for(LocalCounter = 0u; LocalCounter < SectionSize/WriteSize; LocalCounter++)
                {

                    *RamBaseAddressPtr_4 = ( VAR( uint32, AUTOMATIC ) ) InitData;
                    RamBaseAddressPtr_4 ++;     /* jump (write size) bytes at a time */

                }

                break ;
            }   /* case */
        }   /* switch */
    }   /* else */
    #endif

    CS_OFF ;

    return returnResult ;

}

/*********************************************************************************************************/

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

#if MCU_INIT_CLOCK == STD_ON
FUNC( Std_ReturnType, MCU_CODE ) Mcu_InitClock( VAR( Mcu_ClockType, AUTOMATIC ) ClockSetting )
{
    CS_ON ;

    VAR( Std_ReturnType, AUTOMATIC ) returnResult = E_NOT_OK ;
    VAR( uint8, AUTOMATIC ) localCounter = 0 ;
    VAR( uint8, AUTOMATIC ) tempVar = 0 ;
    VAR( uint32, AUTOMATIC ) tempVar_2 = 0 ;
    volatile P2VAR( uint32, AUTOMATIC, REGSPACE ) tempPtr = NULL_PTR ;

    /* case error report is off */
    #if MCU_DEV_ERROR_DETECT == STD_OFF


    returnResult = E_OK ;

/**************************************************/

    /* make oscillator is source of sysclock, in case want pll use Mcu_DistributePllClock function
     * and ignore system clock divisor (prescaler) (make it = 1 ) */

    MCU_RCC2 |= ( 0x01u << BYBASS_BIT_POISITION ) ;  /* set bypass bit to make oscillators is source for system clock of MCU */
    MCU_RCC &= ~(0x01u << SYS_DIV_BIT_POSITION ) ;  /* system clock is used undivided (ignore divisor )*/

/**************************************************/
    /* power of main oscillator */
    if( (*ptr_ClockSetting_array)[ClockSetting].MainOSC_poweDown )
    {
        /* case True disable MOSC */
        MCU_RCC |= (0x01u) ;
    }
    else
    {
        /* case false enable MOSC */
        MCU_RCC &= ~(0x01u) ;
    }

/**************************************************/

    /* power of USB PLL */
    if( (*ptr_ClockSetting_array)[ClockSetting].USB_PLL_powerDown )
    {
        /* case true disable USB PLL */
        MCU_RCC2 |= ( 0x01u << USB_PLL_PWRDN_BIT_POISITION ) ;
    }
    else
    {
        /* case false enable USB PLL */
        MCU_RCC2 &= ~( 0x01u << USB_PLL_PWRDN_BIT_POISITION ) ;
    }

/**************************************************/

    /* configure the oscillator o/p */
    MCU_RCC2 &= ~( 0x7  << OSC_CHOSSE_BIT_FILED_START ) ; /* clear all oscillators bits*/
    MCU_RCC2 |= ( ( (*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source ) << OSC_CHOSSE_BIT_FILED_START ) ;

/**************************************************/

    /* power of main PLL*/
    if( (*ptr_ClockSetting_array)[ClockSetting].MainPLL_powerDown )
    {
        /* case true disable main PLL */
        MCU_RCC2 |= ( 0x01u << MAIN_PLL_PWRDN_BIT_POISITION ) ;
    }
    else if ( ( MCU_MOSC == ((*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source) ) || ( MCU_PIOSC == ( (*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source) ) )
    {
        /* case true enable main PLL check first if oscillator source is MOSC or PIOSC */
        MCU_RCC2 &= ~( 0x01u << MAIN_PLL_PWRDN_BIT_POISITION ) ;
    }

/**************************************************/

    /* set clock source of pwm either sysclock or divided sysclock by pwm prescaler */
    if ((*ptr_ClockSetting_array)[ClockSetting].PWM_SysClock)
    {
        /* case true clock source is divided sys clock */
        MCU_RCC |= (0x01 << PWM_CLOCK_SOURCE_BIT_POISITION) ;

        /* set the value of pwm prescaler */
        MCU_RCC &= ~(0x7 << PWM_PRESCALER_BIT_FILED_START ) ; /* clear PWM divisor bits */
        MCU_RCC |= ( ((*ptr_ClockSetting_array)[ClockSetting].PWM_Prescaler) << PWM_PRESCALER_BIT_FILED_START ) ;
    }
    else
    {
        /* case false clock source is  sys clock */
        MCU_RCC &= ~(0x01 << PWM_CLOCK_SOURCE_BIT_POISITION) ;
    }

/**************************************************/


    /* set ADC modules clock source */
    tempVar = (*ptr_ClockSetting_array)[ClockSetting].ADC_PIOSC ;   /* copy data which indicates clock source of each module */
    tempPtr = &(ADC_CLOCK_SOURCE_BASE) ;    /* copy adress of first module reg, ohter modules regs shifted by same offset from each other */

    for( localCounter = 0 ; localCounter < ADC_MODULES_NUMBER ; localCounter++ )
    {

        if( TRUE == ( (ADC_State >> localCounter) & 0x01 ) )    /* to avoid writing in peripheral reg while it's disabled */
        {


            *(tempPtr + ADC_CLOCK_SORUCE_OFFSET * localCounter ) &= ~(0xF) ;    /* clear bits responsible of determine clock source in module reg (sys clock is clock source )*/
            if( (tempVar >> localCounter) & 0x01 )
            {
                /* case bit corresponding of module is set PIOSC is clock source*/
                *( tempPtr + ADC_CLOCK_SORUCE_OFFSET * localCounter ) |= ~(0x1) ;

            }
            else
            {
                /* case bit corresponding of module is clear sysclock is clock source*/
            }
        }

        else
        {

        }

    }

/**************************************************/

    /* set SSI modules clock source same idea as ADC */
    tempVar = (*ptr_ClockSetting_array)[ClockSetting].BaudSSI_PIOSC ;
    tempPtr = &(SSI_BAUD_SOURCE_BASE) ;

    for( localCounter = 0 ; localCounter < SSI_MODULES_NUMBER ; localCounter++ )
    {

        if( TRUE == (( SSI_State >> localCounter ) & 0x01) )
        {
            *(tempPtr + SSI_BAUD_SORUCE_OFFSET * localCounter ) &= ~(0xF) ;
            if( (tempVar >> localCounter) & 0x01 )
            {
                /* case bit corresponding of module is set PIOSC is clock source*/
                *(tempPtr + SSI_BAUD_SORUCE_OFFSET * localCounter ) |= ~(0x5) ;

            }
            else
            {
                /* case bit corresponding of module is clear sysclock is clock source*/
            }
        }

        else
        {

        }
    }

/**************************************************/

    /* set UART modules clock source same idea as ADC */
    tempVar = (*ptr_ClockSetting_array)[ClockSetting].BaudUART_PIOSC ;
    tempPtr = &(UART_BAUD_SOURCE_BASE) ;

    for( localCounter = 0 ; localCounter < UART_MODULES_NUMBER ; localCounter++ )
    {

        if( TRUE == (( UART_State >> localCounter ) & 0x01) )
        {
            *(tempPtr + UART_BAUD_SOURCE_BASE * localCounter ) &= ~(0xF) ;
            if( (tempVar >> localCounter) & 0x01 )
            {
                /* case bit corresponding of module is set PIOSC is clock source*/
                *(tempPtr + UART_BAUD_SORUCE_OFFSET * localCounter) |= ~(0x5) ;

            }
            else
            {
                /* case bit corresponding of module is clear sysclock is clock source*/
            }
        }
        else
        {

        }
    }

/**************************************************/

    /* set sysclock divisor */
    tempVar = ( ((*ptr_ClockSetting_array)[ClockSetting].SysClock_Prescaler) & 0x3F ) ;
    /* use first 6 bits assume MainPLL_preDiv is FALSE preDiv option is executed in Mcu_DistributePllClock function after pll is locked and used for sysclock */

    if ( tempVar )
    {
        /* case  SysClock_Prescaler is not 0 */
        MCU_RCC2 &= ~(0x3F << SYS_DIVISOR_BIT_FIELD_START) ;
        MCU_RCC2 |= (tempVar << SYS_DIVISOR_BIT_FIELD_START) ;
        MCU_RCC |= (0x01u << SYS_DIV_BIT_POSITION ) ;   /* system clock is divided by divisor in SysClock_Prescaler */
    }
    else
    {
        /* case  SysClock_Prescaler is  0 system clock is undivided (divisor = 1 ) */
    }

/**************************************************/

    #endif  /* DET_OFF */

/********************************************************************/

    /* case error report is on */
    #if MCU_DEV_ERROR_DETECT == STD_ON
    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_INIT_CLOCK_ID , MCU_E_UNINIT) ;
    }
    else
    {

        if( (*ptr_ClockSetting_Number) < ClockSetting )
        {
            /* ClockSetting not within the range defined in the configuration data structure */
            Det_ReportError (MCU_MODULE_ID, 0, MCU_INIT_CLOCK_ID , MCU_E_PARAM_CLOCK) ;
        }
        else
        {

            returnResult = E_OK ;

/**************************************************/

            /* make oscillator is source of sysclock, in case want pll use Mcu_DistributePllClock function
             * and ignore system clock divisor (prescaler) (make it = 1 ) */

            MCU_RCC2 |= ( 0x01u << BYBASS_BIT_POISITION ) ;     /* set bypass bit to make oscillators is source for system clock of MCU */
            MCU_RCC &= ~(0x01u << SYS_DIV_BIT_POSITION ) ;      /* system clock is used undivided (ignore divisor )*/

            /* write XTAL bit filed represent crystal value of main oscillator (read write modify) */
            tempVar_2 = MCU_RCC ;
            tempVar_2 &= ~( 0x1Fu << XTAL_BIT_POSITION ) ;
            tempVar_2 |= ( (*ptr_ClockSetting_array)[ClockSetting].MainCrystal_Value << XTAL_BIT_POSITION ) ;
            MCU_RCC = tempVar_2 ;

/**************************************************/
            /* power of main oscillator */
            if( TRUE == (*ptr_ClockSetting_array)[ClockSetting].MainOSC_poweDown )
            {
                /* case True disable MOSC */
                MCU_RCC |= (0x01u) ;
            }
            else
            {
                /* case false enable MOSC */
                MCU_RCC &= ~(0x01u) ;
            }

/**************************************************/

            /* power of USB PLL */
            if( TRUE == (*ptr_ClockSetting_array)[ClockSetting].USB_PLL_powerDown )
            {
                /* case true disable USB PLL */
                MCU_RCC2 |= ( 0x01u << USB_PLL_PWRDN_BIT_POISITION ) ;
            }
            else
            {
                /* case false enable USB PLL */
                MCU_RCC2 &= ~( 0x01u << USB_PLL_PWRDN_BIT_POISITION ) ;
            }

/**************************************************/

            /* configure the oscillator o/p  ( read modify write operation ) */
            tempVar_2 = MCU_RCC2 ;
            tempVar_2 &= ~( 0x7  << OSC_CHOSSE_BIT_FILED_START ) ; /* clear all oscillators bits*/
            tempVar_2 |= ( ( (*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source ) << OSC_CHOSSE_BIT_FILED_START ) ;
            MCU_RCC2 = tempVar_2 ;

/**************************************************/

            /* power of main PLL*/
            if( TRUE == (*ptr_ClockSetting_array)[ClockSetting].MainPLL_powerDown )
            {
                /* case true disable main PLL */
                MCU_RCC2 |= ( 0x01u << MAIN_PLL_PWRDN_BIT_POISITION ) ;
            }
            else if ( ( MCU_MOSC == ((*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source) ) || ( MCU_PIOSC == ( (*ptr_ClockSetting_array)[ClockSetting].Oscillator_Source) ) )
            {
                /* case true enable main PLL check first if oscillator source is MOSC or PIOSC */
                MCU_RCC2 &= ~( 0x01u << MAIN_PLL_PWRDN_BIT_POISITION ) ;
            }
            else
            {

            }

/**************************************************/



            /* set sysclock divisor */
            tempVar = ( ((*ptr_ClockSetting_array)[ClockSetting].SysClock_Prescaler) & 0x3F ) ;
            /* use first 6 bits assume MainPLL_preDiv is FALSE preDiv option is executed in Mcu_DistributePllClock function after pll is locked and used for sysclock */

            if ( 0 != tempVar )
            {
                /* case  SysClock_Prescaler is not 0 */
                MCU_RCC2 &= ~(0x3F << SYS_DIVISOR_BIT_FIELD_START) ;
                MCU_RCC2 |= (tempVar << SYS_DIVISOR_BIT_FIELD_START) ;
                MCU_RCC |= (0x01u << SYS_DIV_BIT_POSITION ) ;   /* system clock is divided by divisor in SysClock_Prescaler */
            }
            else
            {
                /* case  SysClock_Prescaler is  0 system clock is undivided (divisor = 1 ) */
            }

/**************************************************/

            /* set clock source of pwm either sysclock or divided sysclock by pwm prescaler */
            if ( TRUE == (*ptr_ClockSetting_array)[ClockSetting].PWM_SysClock )
            {
                /* case true clock source is divided sys clock */
                MCU_RCC |= (0x01 << PWM_CLOCK_SOURCE_BIT_POISITION) ;

                /* set the value of pwm prescaler */
                MCU_RCC &= ~(0x7 << PWM_PRESCALER_BIT_FILED_START ) ; /* clear PWM divisor bits */
                MCU_RCC |= ( ((*ptr_ClockSetting_array)[ClockSetting].PWM_Prescaler) << PWM_PRESCALER_BIT_FILED_START ) ;
            }
            else
            {
                /* case false clock source is  sys clock */
                MCU_RCC &= ~(0x01 << PWM_CLOCK_SOURCE_BIT_POISITION) ;
            }

/**************************************************/

            /* set ADC modules clock source */
            tempVar = (*ptr_ClockSetting_array)[ClockSetting].ADC_PIOSC ;   /* copy data which indicates clock source of each module */
            tempPtr = &(ADC_CLOCK_SOURCE_BASE) ;    /* copy address of first module reg, other modules regs shifted by same offset from each other */

            for( localCounter = 0 ; localCounter < ADC_MODULES_NUMBER ; localCounter++ )
            {

                if( TRUE == ( (ADC_State >> localCounter) & 0x01 ) )    /* to avoid writing in peripheral reg while it's disabled */
                {


                    *(tempPtr + ADC_CLOCK_SORUCE_OFFSET * localCounter ) &= ~(0xF) ;    /* clear bits responsible of determine clock source in module reg (sys clock is clock source )*/
                    if( (tempVar >> localCounter) & 0x01 )
                    {
                        /* case bit corresponding of module is set PIOSC is clock source*/
                        *( tempPtr + ADC_CLOCK_SORUCE_OFFSET * localCounter ) |= (0x1) ;

                    }
                    else
                    {
                        /* case bit corresponding of module is clear sysclock is clock source*/
                    }
                }
                else
                {

                }
            }

/**************************************************/

            /* set SSI modules clock source same idea as ADC */
            tempVar = (*ptr_ClockSetting_array)[ClockSetting].BaudSSI_PIOSC ;
            tempPtr = &(SSI_BAUD_SOURCE_BASE) ;

            for( localCounter = 0 ; localCounter < SSI_MODULES_NUMBER ; localCounter++ )
            {

                if( TRUE == (( SSI_State >> localCounter ) & 0x01) )    /* to avoid writing in peripheral reg while it's disabled */
                {
                    *(tempPtr + SSI_BAUD_SORUCE_OFFSET * localCounter ) &= ~(0xF) ;
                    if( (tempVar >> localCounter) & 0x01 )
                    {
                        /* case bit corresponding of module is set PIOSC is clock source*/
                        *(tempPtr + SSI_BAUD_SORUCE_OFFSET * localCounter ) |= (0x5) ;

                    }
                    else
                    {
                        /* case bit corresponding of module is clear sysclock is clock source*/
                    }
                }
                else
                {

                }
            }

/**************************************************/

            /* set UART modules clock source same idea as ADC */
            tempVar = (*ptr_ClockSetting_array)[ClockSetting].BaudUART_PIOSC ;
            tempPtr = &(UART_BAUD_SOURCE_BASE) ;

            for( localCounter = 0 ; localCounter < UART_MODULES_NUMBER ; localCounter++ )
            {

                if( TRUE == (( UART_State >> localCounter ) & 0x01) )   /* to avoid writing in peripheral reg while it's disabled */
                {
                    *(tempPtr + UART_BAUD_SOURCE_BASE * localCounter ) &= ~(0xF) ;
                    if( TRUE == ( (tempVar >> localCounter) & 0x01 ) )
                    {
                        /* case bit corresponding of module is set PIOSC is clock source*/
                        *(tempPtr + UART_BAUD_SORUCE_OFFSET * localCounter) |= (0x5) ;

                    }
                    else
                    {
                        /* case bit corresponding of module is clear sysclock is clock source*/
                    }
                }
                else
                {

                }
            }

/**************************************************/

        }   /* else */

    }   /* else */
    #endif  /* DET_ON */

    Curren_ClockSetting = ClockSetting ; /* to pass current clock setting ID to Mcu_DistributePllClock function */

    CS_OFF ;

    return returnResult ;

}
#endif

/*********************************************************************************************************/

/*
    - Synchronous, Non Reentrant, Service ID : 0x03
    - shall activate the PLL clock to the MCU clock distribution
    - shall remove the current clock source (for example internal oscillator clock) from MCU clock distribution
    - shall only call the function after the status of the PLL has been detected as locked by the function Mcu_GetPllStatus
    - If the function is called before PLL has locked, this function shall return E_NOT_OK immediately, without any further action
    - The function Mcu_DistributePllClock shall be available if the pre-compile parameter McuNoPll is set to STD_OFF. Otherwise, this Api has to be disabled
    - this function just make PLL is source for system clock that happen after PLL is locked any change to PLL setting is within MCU_initClock function


*/

#if MCU_NO_PLL == STD_OFF

FUNC(Std_ReturnType, MCU_CODE) Mcu_DistributePllClock( void )
{
    CS_ON ;

    VAR(Std_ReturnType, AUTOMATIC) returnResult = E_NOT_OK ;


    /* case error report is off */
    #if MCU_DEV_ERROR_DETECT == STD_OFF

    if ( MCU_PLL_LOCKED == pllStatusCheck )
    {
        returnResult = E_OK ;
        MCU_RCC2 &= ~( 0x01u << BYBASS_BIT_POISITION ) ;  /* clear bypass bit to make PLL is source for system clock of MCU */
        if( (*ptr_ClockSetting_array)[Curren_ClockSetting].MainPLL_preDiv )
        {
            /* case true divisor is applied for 400Mhz & divisor is consist of 7 bits */
            MCU_RCC2 |= (0x01 << MAIN_PLL_PRE_DIVIDE_BIT) ;
            MCU_RCC2 |= ( 0x7F << PLL_DIVISOR_BIT_FIELD_START ) ; /* clear divisor bit field */
            MCU_RCC2 |= ( ((*ptr_ClockSetting_array)[Curren_ClockSetting].SysClock_Prescaler) << PLL_DIVISOR_BIT_FIELD_START ) ; /* assign divisor value */
         }
         else
         {
             /* case false divisor of pll is same as configured in Mcu_ClockInit function */
             MCU_RCC2 &= ~(0x01 << MAIN_PLL_PRE_DIVIDE_BIT) ;

          }
    }
    else
    {

    }
    #endif  /* DET_OFF */

/********************************************************************/

    /* case error report is on */
    #if MCU_DEV_ERROR_DETECT == STD_ON

    if ( MCU_PLL_LOCKED == pllStatusCheck )
    {
        returnResult = E_OK ;

        MCU_RCC2 &= ~(0x01u << BYBASS_BIT_POISITION) ;  /* clear bypass bit to make PLL is source for system clock of MCU */

        if( (*ptr_ClockSetting_array)[Curren_ClockSetting].MainPLL_preDiv )
        {
            /* case true divisor is applied for 400Mhz & divisor is consist of 7 bits */
            MCU_RCC2 |= (0x01 << MAIN_PLL_PRE_DIVIDE_BIT) ;
            MCU_RCC2 &= ~( 0x7F << PLL_DIVISOR_BIT_FIELD_START ) ; /* clear divisor bit field */
            MCU_RCC2 |= ( ((*ptr_ClockSetting_array)[Curren_ClockSetting].SysClock_Prescaler) << PLL_DIVISOR_BIT_FIELD_START ) ; /* assign divisor value */
        }
        else
        {
            /* case false divisor of pll is same as configured in Mcu_ClockInit function */
            MCU_RCC2 &= ~(0x01 << MAIN_PLL_PRE_DIVIDE_BIT) ;
        }

    }
    else if ( MCU_PLL_UNLOCKED == pllStatusCheck )
    {
        /* pll is not locked return NOT_OK and report for error */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_DISTRIBUTE_PLL_CLOCK_ID , MCU_E_PLL_NOT_LOCKED) ;
    }
    else
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_DISTRIBUTE_PLL_CLOCK_ID , MCU_E_UNINIT) ;
    }

    #endif  /* DET_ON */

    CS_OFF ;

    return returnResult ;
}

#endif /* MCU_NO_PLL */


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x04
    - shall return the lock status of the PLL
    - shall return MCU_PLL_STATUS_UNDEFINED if this function is called prior to calling of the function Mcu_Init.
    - shall return MCU_PLL_STATUS_UNDEFINED if the pre-compile parameter McuNoPll is set to STD_ON

*/

FUNC( Mcu_PllStatusType, MCU_CODE ) Mcu_GetPllStatus( void )
{
    VAR( Mcu_PllStatusType, AUTOMATIC ) returnResult = MCU_PLL_STATUS_UNDEFINED ;

    #if MCU_NO_PLL == STD_ON



    #endif /* no PLL*/

/********************************/

    #if MCU_NO_PLL == STD_OFF

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {

        #if MCU_DEV_ERROR_DETECT == STD_ON
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_GET_PLL_STATUS_ID , MCU_E_UNINIT) ;

        #endif  /* DET_ON */

        /* in case DET is off then it will return undefined state without report */
    }
    else
    {

        if ( FALSE == (MCU_PLLSTAT & 0x01) ) /* LOCK bit = 0 */
        {
            returnResult = MCU_PLL_UNLOCKED ;
        }
        else    /* LOCK bit = 1 */
        {
            returnResult = MCU_PLL_LOCKED ;
        }

        pllStatusCheck = returnResult ;
    }

    #endif


    return returnResult ;

}


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x05
    - shall read the reset reason from the hardware and return this reason
    - If the hardware does not support hardware detection  the return value shall always be MCU_POWER_ON_RESET
    - shall return MCU_RESET_UNDEFINED if this function is called prior to calling of the function Mcu_Init
    - User should ensure that the reset reason is cleared once it has been read out to avoid multiple reset reasons
    - In case of multiple calls to this function the return value should always be the same

*/

FUNC(Mcu_ResetType, MCU_CODE) Mcu_GetResetReason( void )
{
    static VAR ( Mcu_ResetType, MCU_VAR_INIT ) Static_ReturnResult = MCU_RESET_UNDEFINED ;

    #if  MCU_DEV_ERROR_DETECT == STD_OFF


    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
    }
    else
    {

        if (MCU_RESET_UNDEFINED == Static_ReturnResult)
        {

            Static_ReturnResult = RESET_REASON_REG ;

            /* clear reserved bits */
            Static_ReturnResult &= ( RESC_CLEAR_MASK) ;

            /* clear reset reason */
            RESET_REASON_REG ^= Static_ReturnResult ;

        } /* if */

        else
        {
        } /* else*/

    } /* else */
lse*/


    #endif

/********************************/

    #if MCU_DEV_ERROR_DETECT == STD_ON

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_GET_RESET_REASON_ID , MCU_E_UNINIT) ;
    }
    else
    {

        if (MCU_RESET_UNDEFINED == Static_ReturnResult)
        {

            Static_ReturnResult = RESET_REASON_REG ;

            /* clear reserved bits */
            Static_ReturnResult &= ( RESC_CLEAR_MASK) ;

            /* clear reset reason */
            RESET_REASON_REG ^= Static_ReturnResult ;

        } /* if */

        else
        {
        } /* else*/

    } /* else */

    #endif

    return Static_ReturnResult ;


    /*


    */

}


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x06
    - shall return an implementation specific value does not correspond to a valid value of the reset status register and not equal to 0
      if this function is called prior to calling of the function Mcu_Init
    - shall read the reset raw value from the hardware register if the hardware supports this
    - User should ensure that the reset reason is cleared once it has been read out to avoid multiple reset reasons
    - In case of multiple calls to this function the return value should always be the same

*/

FUNC(Mcu_RawResetType, MCU_CODE) Mcu_GetResetRawValue( void )
{
    VAR ( Mcu_RawResetType, AUTOMATIC ) Static_Raw_ReturnResult = RAW_RESET_UNIQUE_VALUE ;

    #if  MCU_DEV_ERROR_DETECT == STD_OFF

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
    }
    else
    {
        if( RAW_RESET_UNIQUE_VALUE == Static_Raw_ReturnResult )
        {
            Static_Raw_ReturnResult = RESET_REASON_REG ;
            RESET_REASON_REG ^= RESET_REASON_REG ;
        }
        else
        {

        }

    }



    #endif

    #if  MCU_DEV_ERROR_DETECT == STD_ON

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_GET_RESET_RAW_VALUE_ID , MCU_E_UNINIT) ;
    }
    else
    {
        if( RAW_RESET_UNIQUE_VALUE == Static_Raw_ReturnResult )
        {
            Static_Raw_ReturnResult = RESET_REASON_REG ;
            RESET_REASON_REG ^= RESET_REASON_REG ;
        }
        else
        {

        }

    }

    #endif

    return Static_Raw_ReturnResult ;
}


/*********************************************************************************************************/

/*
    - Synchronous, nonReentrant, Service ID : 0x07
    - shall perform a microcontroller reset by using the hardware feature of the microcontroller
    - shall perform the reset type which is configured in the configuration set
    - shall only call the function after the MCU module has been initialized by the function Mcu_Init
    - is only available if the pre-compile parameter McuPerformResetApi is set to STD_ON

*/

#if MCU_PERFORM_RESET_API ==STD_ON

FUNC(void, MCU_CODE) Mcu_PerformReset( void )
{
    CS_ON ;

    VAR ( uint32, AUTOMATIC ) tempVar = 0 ;
    #if  MCU_DEV_ERROR_DETECT == STD_OFF

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_PERFORM_RESET_ID , MCU_E_UNINIT) ;
    }
    else
    {

        if ( MCU_RESET_SETTING == *(ptr_ResetSetting) )
        {

            /* read modify write operation */
            tempVar = PERFORM_RESET_REG ;
            tempVar &= ~(PERFORM_RESET_CLEAR_MASK);

            /* vectkey is protection value to prevent accediant write to rgister */
            tempVar |= ( (VECTKEY_VALUE << HALF_WORD_SIZE) | (1<<RESET_BIT_POSITION) ) ;

            PERFORM_RESET_REG = tempVar ;        }

        else
        {

        }

    }

    #endif


    #if  MCU_DEV_ERROR_DETECT == STD_ON

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_PERFORM_RESET_ID , MCU_E_UNINIT) ;
    }
    else
    {

        if ( MCU_RESET_SETTING == *(ptr_ResetSetting) )
        {

            /* read modify write operation */
            tempVar = PERFORM_RESET_REG ;
            tempVar &= ~(PERFORM_RESET_CLEAR_MASK);

            /* vectkey is protection value to prevent accediant write to rgister */
            tempVar |= ( (VECTKEY_VALUE << HALF_WORD_SIZE) | (1<<RESET_BIT_POSITION) ) ;

            PERFORM_RESET_REG = tempVar ;        }

        else
        {

        }

    }

    #endif

    CS_OFF ;

    return ;
}

#endif

/*********************************************************************************************************/


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


FUNC ( void, MCU_CODE ) Mcu_SetMode( Mcu_ModeType McuMode )
{

    #if  MCU_DEV_ERROR_DETECT == STD_OFF


    /* determine low power mode sleep or deep sleep */
    if ( McuConf_SleepMode == McuMode ) /* sleep mode */
    {
        SYSTEM_CONTROL_REG &= ~( 0x01u << LOW_POWER_MODE_BIT ) ;
    }
    else
    {
        /* set deep sleep mode */
        SYSTEM_CONTROL_REG |= ( 0x01u << LOW_POWER_MODE_BIT ) ;
    }

    /*WFI instruction*/
    __asm( " WFI " ) ;

    #endif  /* DET_OFF */

/********************************/
    #if  MCU_DEV_ERROR_DETECT == STD_ON

    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {

        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_SET_MODE_ID , MCU_E_UNINIT) ;

    }
    else
    {

        /* McuMode has invalid value */
        if(McuMode > McuConf_DeepSleepMode)
        {
            Det_ReportError (MCU_MODULE_ID, 0, MCU_SET_MODE_ID , MCU_E_PARAM_MODE) ;
        }
        else
        {

            /* determine low power mode sleep or deep sleep */
            if ( McuConf_SleepMode == McuMode ) /* sleep mode */
            {
                SYSTEM_CONTROL_REG &= ~( 0x01u << LOW_POWER_MODE_BIT ) ;
            }
            else
            {
                /* set deep sleep mode */
                SYSTEM_CONTROL_REG |= ( 0x01u << LOW_POWER_MODE_BIT ) ;
            }

            /*WFI instruction if any enabled interrupt have event after calling this function then wfi condition will be false and won't enter sleep mode */
            __asm( " WFI " ) ;

        } /* else */

    } /* else */

    #endif  /* DET_ON */

    return ;
}


/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x0A
    - provides the actual status of the microcontroller Ram. (if supported)
    - MCU modules environment shall call this function only if the MCU module has been already initialized
    - function shall be available to the Mcu_GetRamState user if the pre-compile parameter is set to STD_ON McuGetRamStateApi
    - tiva c don't support ram state so if this service is enabled will always return MCU_RAMSTATE_VALID

*/

#if MCU_GET_RAM_STATE_API == STD_ON

FUNC( Mcu_RamStateType, MCU_CODE ) Mcu_GetRamState( void )
{

    #if MCU_DEV_ERROR_DETECT == STD_ON
    if ( MCU_PLL_STATUS_UNDEFINED == pllStatusCheck )
    {
        /* function called before Mcu_Init function return NOT_OK and report error  */
        Det_ReportError (MCU_MODULE_ID, 0, MCU_GET_RAM_STATUS_ID , MCU_E_UNINIT) ;
    }
    else
    {

    }
    #endif

/********************************/

    #if MCU_DEV_ERROR_DETECT == STD_OFF


    #endif

    return MCU_RAMSTATE_VALID;
}

#endif




/*********************************************************************************************************/

/*
    - Synchronous, Reentrant, Service ID : 0x09
    - get the version information of this module
    - If DET is enabled, the function shall raise MCU_E_PARAM_POINTER, if the argument is NULL pointer and return without any action.
*/

#if MCU_VERSION_INFO_API == STD_ON

FUNC( void, MCU_CODE ) Mcu_GetVersionInfo( Std_VersionInfoType* VersionInfo )
{
    #if MCU_DEV_ERROR_DETECT == STD_OFF

        VersionInfo->moduleID = MCU_MODULE_ID ;
        VersionInfo->vendorID = MCU_VENDOR_ID ;
        VersionInfo->sw_major_version = MCU_SW_MAJOR_VERSION ;
        VersionInfo->sw_minor_version = MCU_SW_MINOR_VERSION ;
        VersionInfo->sw_patch_version = MCU_SW_PATCH_VERSION ;

    return  ;

    #endif // ERROR

    /*____________________________________________________________________*/

    #if MCU_DEV_ERROR_DETECT == STD_ON

    if ( NULL_PTR == VersionInfo)
    {
        /* invalid i/p return and report error " MCU_E_PARAM_POINTER = 0x20 " */
        Det_ReportError ( MCU_MODULE_ID, 0, MCU_E_PARAM_POINTER , MCU_GET_VERSION_INFO_ID ) ;
    }
    else
    {
        VersionInfo->moduleID = MCU_MODULE_ID ;
        VersionInfo->vendorID = MCU_VENDOR_ID ;
        VersionInfo->sw_major_version = MCU_SW_MAJOR_VERSION ;
        VersionInfo->sw_minor_version = MCU_SW_MINOR_VERSION ;
        VersionInfo->sw_patch_version = MCU_SW_PATCH_VERSION ;
    }
    return ;

    #endif // ERROR


}
#endif // GET_VERSION

/***********************************************************************************************************/
