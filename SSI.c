 /******************************************************************************
 *
 * Module: SSI
 *
 * File Name: SSI.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - SSI Driver
 *
 * Author: Mohamed Tarek
 ******************************************************************************/

#include "SSI.h"
#include "SSI_Regs.h"  
#include "tm4c123gh6pm_registers.h"

STATIC uint8 Spi_Status =   SPI_NOT_INITIALIZED;
STATIC const Spi_ConfigChannel * Spi_ChannelPtr = NULL_PTR;

#if (SPI_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != SSI_AR_RELEASE_MAJOR_VERSION)\
 || (DET_AR_MINOR_VERSION != SSI_AR_RELEASE_MINOR_VERSION)\
 || (DET_AR_PATCH_VERSION != SSI_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif

/************************************************************************************
* Service Name: SSI_Init
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the SSI module.
************************************************************************************/
void Spi_Init(const Spi_ConfigType* ConfigPtr)
{  
  volatile uint32 * SPIn_BaseAddress_Ptr = NULL_PTR;
  volatile uint8 Spi_Pin_Index_Iterator =0;
    
#if (SPI_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		     SPI_E_PARAM_POINTER_ID);
	}
        if(Spi_Status == SPI_INITIALIZED )
        {
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		     SPI_E_ALREADY_INITIALIZED_ID);
        }
	else
#endif
	{
		
		Spi_Status       = SPI_INITIALIZED;
                Spi_ChannelPtr    = ConfigPtr->SpiPins; /* address of the first SpiPins structure --> SpiPins[0] */
                
               
                while( Spi_Pin_Index_Iterator != SPI_PINS_CONFIGURED_NUMBER )
                {
                  
                 switch( Spi_ChannelPtr->SpiChannelID )
                 {
                 case 0x00 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI0_BASE_ADDRESS; /* SPI0 Base Address */
                   break;
                 case 0x01 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI1_BASE_ADDRESS; /* SPI1 Base Address */
                   break;
                 case 0x02 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI2_BASE_ADDRESS; /* SPI2 Base Address */
                   break;
                 case 0x03 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI3_BASE_ADDRESS; /* SPI3 Base Address */
                   break;
                 default:
                   #if (SPI_DEV_ERROR_DETECT == STD_ON)
                       Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		          SPI_E_PARAM_CHANNEL_ID);
                   #endif
                 }
    

                 /*Enable SSIn module clock*/
                 SET_BIT(*(volatile uint32 *)((volatile uint8 *)RCGCSSI_BASE_ADDRESS + RCGCSSI_OFFSET) , ((Spi_ChannelPtr->SpiChannelID) & 0x0F));
                 
                 
                 /*Disable SSI operation*/
                 CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 1 );
                 
            
                 /*Select Mode*/
                 if(Spi_ChannelPtr->Spi_Mode == Master_Mode)
                 {
                   CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 2 );
                 }
                 else if(Spi_ChannelPtr->Spi_Mode == Slave_Mode)
                 {
                   SET_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 2 );
                 }
                 else
                 {
                   
                 }
                 
                 /*SSI Clock Source*/
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICC_OFFSET) = Spi_ChannelPtr->Spi_Source_Clock;
                 
                 
                 /*Clock Prescaler*/
                  /*Initial value of SCR , Frame Style , SPO , SPH , Data_Size =0*/
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) = 0;
                 
                 if( (uint32)((SPI_System_Clock_Source)/(Spi_ChannelPtr->BaudRate)) > 254 )
                 {
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICPSR_OFFSET) = 254;
               
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= ( (uint32)((((SPI_System_Clock_Source)/(Spi_ChannelPtr->BaudRate)) / 254) -1 ) << 8 );
                 }
                 else 
                 {
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICPSR_OFFSET) = (uint32)((SPI_System_Clock_Source)/(Spi_ChannelPtr->BaudRate));
                 }
                 
                 /*Frame Format*/
                 switch(Spi_ChannelPtr->Spi_Frame_Format)
                 {
                 case Freescale_SPI:
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 4 );
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 5 );
                    *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (Spi_ChannelPtr->Spi_Clock_Polarity)<<6;
                    *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (Spi_ChannelPtr->Spi_Clock_Phase)<<7;
                    
                   break;
                 case TI_Sync_Serial:
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 5 );
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) ,  4);
                   break;
                 case MicroWire:
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 4 );
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 5 );
                    
                   break;
                 }
     
    
                 /*Data Size*/
                 #if (SPI_DEV_ERROR_DETECT == STD_ON)
                         
                 if( (Spi_ChannelPtr->SpiDataWidth)<(0x3) || (Spi_ChannelPtr->SpiDataWidth)>(0xF))
                 {
                       Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		          SPI_E_PARAM_LENGTH_ID);
                 }  
                 else 
                 #endif
                 {    
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (Spi_ChannelPtr->SpiDataWidth);
                 }
                 /*Enable SSI Operation*/
                 CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 1 );
                 
     ++Spi_Pin_Index_Iterator;
     ++Spi_ChannelPtr;
     }
     
     Spi_ChannelPtr       = ConfigPtr->SpiPins; /* address of the first SpiPins structure --> SpiPins[0] */
     
   }
}



/************************************************************************************
* Service Name: Spi_DeInit
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: Std_ReturnType
* Description: Function to deinitialize the SSI module.
************************************************************************************/
Std_ReturnType* Spi_DeInit(void)
{
  #if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_DeInit_SID,
		     Spi_DeInit_SID);
        }
	else 
#endif
        {
          volatile uint32 * SPIn_BaseAddress_Ptr = NULL_PTR;
          volatile uint8 Spi_Pin_Index_Iterator =0;
          volatile Std_ReturnType Spi_DeInit_Success_Status[SPI_PINS_CONFIGURED_NUMBER];
        while( Spi_Pin_Index_Iterator != SPI_PINS_CONFIGURED_NUMBER )
        {
          if( Spi_ChannelPtr ->Spi_Status != SPI_BUSY)
          {
            switch( Spi_ChannelPtr->SpiChannelID )
                 {
                 case 0x00 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI0_BASE_ADDRESS; /* SPI0 Base Address */
                   break;
                 case 0x01 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI1_BASE_ADDRESS; /* SPI1 Base Address */
                   break;
                 case 0x02 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI2_BASE_ADDRESS; /* SPI2 Base Address */
                   break;
                 case 0x03 :SPIn_BaseAddress_Ptr = (volatile uint32 *)SPI3_BASE_ADDRESS; /* SPI3 Base Address */
                   break;
                 }
                 *(volatile uint32 *)((volatile uint8 *)RCGCSSI_BASE_ADDRESS + RCGCSSI_OFFSET)=0;
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET)=0;
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICC_OFFSET)=0;
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) = 0;
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICPSR_OFFSET)=0;
                 Spi_DeInit_Success_Status[Spi_Pin_Index_Iterator]= E_OK;
                 Spi_ChannelPtr->Spi_Status = SPI_UNINIT;
          }
         
          else
          {
            Spi_DeInit_Success_Status[Spi_Pin_Index_Iterator]= E_NOT_OK;  
          }
       
          ++Spi_Pin_Index_Iterator;
          ++Spi_ChannelPtr;
        }
        
        return &Spi_DeInit_Success_Status; 
         /*
           Bug: If Deinit Fnc is called after Init then Calling any fnc. the unint error will not be fired
                It is difficult to handle now because if SPI0 is deinit well but SPI1 isnot so why shall I make the SPI_Status to be Uninitalized 
         */
         

        }

}



/************************************************************************************
* Service Name: Spi_GetVersionInfo
* Service ID[hex]: 0x09
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): VersionInfo - Pointer to where to store the version information of this module.
* Return value: None
* Description: Function to get the version information of this module.
************************************************************************************/
#if (SPI_VERSION_INFO_API == STD_ON)
void Spi_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
#if (SPI_DEV_ERROR_DETECT == STD_ON)
	/* Check if input pointer is not Null pointer */
	if(NULL_PTR == versioninfo)
	{
		/* Report to DET  */
		Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_GetVersionInfo_SID,
		     SPI_E_PARAM_POINTER_ID);
	}
        if(Spi_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_GetVersionInfo_SID,
		     SPI_E_UNINIT_ID);
        }
	else
#endif /* (SPI_DEV_ERROR_DETECT == STD_ON) */
	{
		/* Copy the vendor Id */
		versioninfo->vendorID = (uint16)SPI_VENDOR_ID;
		/* Copy the module Id */
		versioninfo->moduleID = (uint16)SPI_MODULE_ID;
		/* Copy Software Major Version */
		versioninfo->sw_major_version = (uint8)SPI_SW_MAJOR_VERSION;
		/* Copy Software Minor Version */
		versioninfo->sw_minor_version = (uint8)SPI_SW_MINOR_VERSION;
		/* Copy Software Patch Version */
		versioninfo->sw_patch_version = (uint8)SPI_SW_PATCH_VERSION;
	}
}
#endif

#if(SPI_HW_STATUS_API == STD_ON)
Spi_StatusType* Spi_GetStatus(void)
{
  #if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_GetStatus_SID,
		     SPI_E_UNINIT_ID);
        }
	else
#endif /* (SPI_DEV_ERROR_DETECT == STD_ON) */
{  
   volatile uint8 Spi_Pin_Index_Iterator =0;
   volatile Spi_StatusType Spis_Status[SPI_PINS_CONFIGURED_NUMBER];
   while( Spi_Pin_Index_Iterator != SPI_PINS_CONFIGURED_NUMBER )
   {
   
      Spis_Status[Spi_Pin_Index_Iterator]=Spi_ChannelPtr->Spi_Status;
     
     ++Spi_Pin_Index_Iterator;
     ++Spi_ChannelPtr;
   }
  return &Spis_Status;
}
}
#endif

