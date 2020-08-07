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


#ifndef MCU_PBFG_H_
#define MCU_PBFG_H_


#include <MCU/Common/Mcu_MemMap.h>
#include <MCU/Mcu.h>


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

CONST (Mcu_ConfigType, OS_CONFIG_DATA)  MCU_ConfigurationStructure [MCU_CONFIGURATION_SETTING_NUMBER] =
{
    {
         .AutoClockGting = FALSE
         ,
         .ClockSetting_Number = 1
         ,
         .ClockSetting_array =
         {
              {
                   .MainOSC_poweDown = FALSE    /* enable  MOSC */
                   ,
                   .MainPLL_powerDown = FALSE    /* enable */
                   ,
                   .USB_PLL_powerDown = TRUE    /* enable */
                   ,
                   .MainPLL_preDiv = FALSE       /* 400 MHz */
                   ,
                   .ADC_PIOSC = 0b0            /* sysclock or pll/25 for ADC 0 , and PIOSC for ADC 1 */
                   ,
                   .BaudSSI_PIOSC = 0      /* sysclock or pll/25 for SSI 0,1 , and PIOSC for SSI 2,3 */
                   ,
                   .BaudUART_PIOSC = 0 /* same as SSI and ADC */
                   ,
                   .PWM_SysClock = TRUE         /* use divided sys clock for PWM clock */
                   ,
                   .PWM_Prescaler = 0b100           /* divide sysclock by 16 and use it for PWM */
                   ,
                   .SysClock_Prescaler = 4          /* prescaler of o/p of pll (o/p freq = 8 MHz) */
                   ,
                   .Oscillator_Source = MCU_PIOSC    /* choose main oscillator for drive pll and sys clock */
                   ,
                   .MainCrystal_Value = MCU_MAIN_CRYSTAL_16
              }
         }
         ,
         .DeepSleepMode_ClockSetting =
         {
              .SSI_ClockGate = 0
              ,
              .ADC_ClockGate = 0
              ,
              .UART_ClockGate = 0
         }
         ,
         .DeepSleepMode_PowerSetting =
         {
              .UseRecommendedSetting_LDO = TRUE
              ,
              .PIOSC_DeepSleep_PowerDown = TRUE
              ,
              .SRAM_Power = SRAM_ACTIVE_MODE
              ,
              .Flash_Power = FLASH_ACTIVE_MODE
              ,
              .DeepSleep_Prescaler = 03u
              ,
              .DeepSleep_ClockSource = MCU_32_768
         }
         ,
         .RAM_SectorSetting_array =
         {
              {
                   .RamDefaultValue = 0x42u
                   ,
                   .RamSectionWriteSize = FOUR_BYTE
                   ,
                   .RamSectionSize = 2048
                   ,
                   .RamSectionBaseAddress = ( P2VAR( uint32, AUTOMATIC , AUTOMATIC) ) 0x20004000u
              }
         }
         ,
         .RAM_Sectors_Number = 1
         ,
         .ResetSetting = MCU_RESET_SETTING
         ,
         .RunMode_ClockSetting =
         {
              .Watchdog_ClockGate = 0xFF
              ,
              .GPT_16_32_ClockGate = 0xFF
              ,
              .GPIO_ClockGate = 0xFF
              ,
              .DMA_ClockGate = 0xFF
              ,
              .Hibernation_ClockGate = 0xFF
              ,
              .UART_ClockGate = 0x00
              ,
              .SSI_ClockGate = 0xFF
              ,
              .I2C_ClockGate = 0xFF
              ,
              .USB_ClockGate = 0xFF
              ,
              .CAN_ClockGate = 0xFF
              ,
              .ADC_ClockGate = 0xFF
              ,
              .AnalogCompartor_ClockGate = 0xFF
              ,
              .PWM_ClockGate = 0xFF
              ,
              .QEI_ClockGate = 0xFF
              ,
              .EEPROM_ClockGate = 0xFF
              ,
              .GPT_32_64_ClockGate = 0xFF
         }
         ,
         .SleepMode_ClockSetting =
         {

          .Watchdog_ClockGate = 0xFF
          ,
          .GPT_16_32_ClockGate = 0xFF
          ,
          .GPIO_ClockGate = 0xFF
          ,
          .DMA_ClockGate = 0xFF
          ,
          .Hibernation_ClockGate = 0x00
          ,
          .UART_ClockGate = 0xFF
          ,
          .SSI_ClockGate = 0xFF
          ,
          .I2C_ClockGate = 0xFF
          ,
          .USB_ClockGate = 0xFF
          ,
          .CAN_ClockGate = 0xFF
          ,
          .ADC_ClockGate = 0xFF
          ,
          .AnalogCompartor_ClockGate = 0xFF
          ,
          .PWM_ClockGate = 0xFF
          ,
          .QEI_ClockGate = 0xFF
          ,
          .EEPROM_ClockGate = 0xFF
          ,
          .GPT_32_64_ClockGate = 0xFF
         }
         ,
         .DeepSleepMode_ClockSetting =
         {
             .Watchdog_ClockGate = 0xFF
             ,
             .GPT_16_32_ClockGate = 0x00
             ,
             .GPIO_ClockGate = 0xFF
             ,
             .DMA_ClockGate = 0x00
             ,
             .Hibernation_ClockGate = 0xFF
             ,
             .UART_ClockGate = 0x00
             ,
             .SSI_ClockGate = 0xFF
             ,
             .I2C_ClockGate = 0x00
             ,
             .USB_ClockGate = 0xFF
             ,
             .CAN_ClockGate = 0x00
             ,
             .ADC_ClockGate = 0xFF
             ,
             .AnalogCompartor_ClockGate = 0x00
             ,
             .PWM_ClockGate = 0xFF
             ,
             .QEI_ClockGate = 0x00
             ,
             .EEPROM_ClockGate = 0xFF
             ,
             .GPT_32_64_ClockGate = 0x00
         }
         ,
         .SleepMode_PowerSetting =
         {
              .UseRecommendedSetting_LDO = FALSE
              ,
              .LDO = LDO_1100_MV
              ,
              .SRAM_Power = SRAM_LOW_POWER_MODE
              ,
              .Flash_Power = FLASH_ACTIVE_MODE
         }
         ,
         .DeepSleepMode_PowerSetting =
         {

          .UseRecommendedSetting_LDO = FALSE
          ,
          .LDO = LDO_1100_MV
          ,
          .SRAM_Power = SRAM_STANDBY_MODE
          ,
          .Flash_Power = FLASH_LOW_POWER_MODE
          ,
          .PIOSC_DeepSleep_PowerDown = TRUE
          ,
          .DeepSleep_Prescaler = 0xF
          ,
          .DeepSleep_ClockSource =MCU_32_768
         }

    }
} ;



#endif /* MCU_PBFG_H_ */

