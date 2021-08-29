 /******************************************************************************
 *
 * Module: Spi
 *
 * File Name: Spi_Cfg.h
 *
 * Description: Pre-Compile Configuration Header file for TM4C123GH6PM Microcontroller - Spi Driver
 *
 * Author: Mohamed Tarek
 ******************************************************************************/

#ifndef SPI_CFG_H
#define SPI_CFG_H

/*
 * Module Version 1.0.0
 */
#define SPI_CFG_SW_MAJOR_VERSION              (1U)
#define SPI_CFG_SW_MINOR_VERSION              (0U)
#define SPI_CFG_SW_PATCH_VERSION              (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define SPI_CFG_AR_RELEASE_MAJOR_VERSION     (4U)
#define SPI_CFG_AR_RELEASE_MINOR_VERSION     (0U)
#define SPI_CFG_AR_RELEASE_PATCH_VERSION     (3U)

/* Pre-compile option for Development Error Detect */
#define SPI_DEV_ERROR_DETECT                (STD_ON)

/* Pre-compile option for Version Info API */
#define SPI_VERSION_INFO_API                (STD_OFF)

/* Pre-compile option for Get SPI Status API */
#define SPI_HW_STATUS_API                   (STD_ON)

   
/* Number of the configured Spi Channels */
#define SPI_PINS_CONFIGURED_NUMBER              (4U)

/* Channel Index in the array of structures in SSI_PBcfg.c */
#define SpiConf_0_CHANNEL_ID_INDEX          (uint8)0x00
#define SpiConf_1_CHANNEL_ID_INDEX          (uint8)0x01
#define SpiConf_2_CHANNEL_ID_INDEX          (uint8)0x02
#define SpiConf_3_CHANNEL_ID_INDEX          (uint8)0x03   

 /*SPI ID*/  
#define SPI_0   (uint8)0x00
#define SPI_1   (uint8)0x01
#define SPI_2   (uint8)0x02
#define SPI_3   (uint8)0x03   
      
/*SPI Data Width*/
#define DATA_LENGTH_4_BITS    (0x3)
#define DATA_LENGTH_5_BITS    (0x4)
#define DATA_LENGTH_6_BITS    (0x5)
#define DATA_LENGTH_7_BITS    (0x6)
#define DATA_LENGTH_8_BITS    (0x7)
#define DATA_LENGTH_9_BITS    (0x8)
#define DATA_LENGTH_10_BITS   (0x9)
#define DATA_LENGTH_11_BITS   (0xA)
#define DATA_LENGTH_12_BITS   (0xB)
#define DATA_LENGTH_13_BITS   (0xC)
#define DATA_LENGTH_14_BITS   (0xD)
#define DATA_LENGTH_15_BITS   (0xE)
#define DATA_LENGTH_16_BITS   (0xF)
   
/*SPI Baud Rate*/   
#define Default_Baud_Rate             (9600.0)
   
/*SPI System Clock Source*/
#define SPI_System_Clock_Source       (16000000.0)   

#endif /* SPI_CFG_H */
