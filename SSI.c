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

STATIC const Spi_ConfigType * SPI_Configuration_Ptr = NULL_PTR;
STATIC uint8 Spi_Init_Status =   SPI_NOT_INITIALIZED;

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
  
#if (SPI_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		     SPI_E_PARAM_POINTER_ID);
	}
        if(Spi_Init_Status == SPI_INITIALIZED )
        {
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		     SPI_E_ALREADY_INITIALIZED_ID);
        }
	else
#endif
	{
		
                SPI_Configuration_Ptr = ConfigPtr;
		Spi_Init_Status       = SPI_INITIALIZED;
             
                 switch( SPI_Configuration_Ptr->SpiChannelID )
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
                 SET_BIT(*(volatile uint32 *)((volatile uint8 *)RCGCSSI_BASE_ADDRESS + RCGCSSI_OFFSET) , ((SPI_Configuration_Ptr->SpiChannelID) & 0x0F));
                 
                 
                 /*Disable SSI operation*/
                 CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 1 );
                 
            
                 /*Select Mode*/
                 if(SPI_Configuration_Ptr->Spi_Mode == Master_Mode)
                 {
                   CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 2 );
                 }
                 else if(SPI_Configuration_Ptr->Spi_Mode == Slave_Mode)
                 {
                   SET_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 2 );
                 }
                 else
                 {
                   
                 }
                 
                 /*SSI Clock Source*/
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICC_OFFSET) = SPI_Configuration_Ptr->Spi_Source_Clock;
                 
                 
                 /*Clock Prescaler*/
                  /*Initial value of SCR , Frame Style , SPO , SPH , Data_Size =0*/
                 *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) = 0;
                 
                 if( (uint32)((SPI_System_Clock_Source)/(SPI_Configuration_Ptr->BaudRate)) > 254 )
                 {
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICPSR_OFFSET) = 254;
               
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= ( (uint32)((((SPI_System_Clock_Source)/(SPI_Configuration_Ptr->BaudRate)) / 254) -1 ) << 8 );
                 }
                 else 
                 {
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICPSR_OFFSET) = (uint32)((SPI_System_Clock_Source)/(SPI_Configuration_Ptr->BaudRate));
                 }
                 
                 /*Frame Format*/
                 switch(SPI_Configuration_Ptr->Spi_Frame_Format)
                 {
                 case Freescale_SPI:
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 4 );
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) , 5 );
                    *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (SPI_Configuration_Ptr->Spi_Clock_Polarity)<<6;
                    *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (SPI_Configuration_Ptr->Spi_Clock_Phase)<<7;
                    
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
                         
                 if( (SPI_Configuration_Ptr->SpiDataWidth)<(0x3) || (SPI_Configuration_Ptr->SpiDataWidth)>(0xF))
                 {
                       Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_Init_SID,
		          SPI_E_PARAM_LENGTH_ID);
                 }  
                 else 
                 #endif
                 {    
                   *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR0_OFFSET) |= (SPI_Configuration_Ptr->SpiDataWidth);
                 }
                 /*Enable SSI Operation*/
                 CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSICR1_OFFSET) , 1 );
  
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
Std_ReturnType Spi_DeInit(void)
{
  #if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Init_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_DeInit_SID,
		     Spi_DeInit_SID);
        }
	else 
#endif
        {
          volatile uint32 * SPIn_BaseAddress_Ptr = NULL_PTR;

          if( SPI_Configuration_Ptr ->Spi_Status != SPI_BUSY)
          {
            switch( SPI_Configuration_Ptr->SpiChannelID )
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
                 SPI_Configuration_Ptr->Spi_Status = SPI_UNINIT;                 
                 Spi_Init_Status =   SPI_NOT_INITIALIZED;
                 
                 return E_OK;       
          }
         
          else
          {
            return E_NOT_OK;  
          }
      

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
        if(Spi_Init_Status == SPI_NOT_INITIALIZED )
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

/************************************************************************************
* Service Name: Spi_GetStatus
* Service ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: Spi_StatusType
* Description: Service returns the SPI Handler/Driver software module status.
************************************************************************************/

#if(SPI_HW_STATUS_API == STD_ON)
Spi_StatusType Spi_GetStatus(void)
{
  #if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Init_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_GetStatus_SID,
		     SPI_E_UNINIT_ID);
        }
	else
#endif /* (SPI_DEV_ERROR_DETECT == STD_ON) */
{  

      return SPI_Configuration_Ptr->Spi_Status ;
     
}
}
#endif

/************************************************************************************
* Service Name: Spi_WriteIB
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): DataBufferPtr - Pointer to source data buffer
* Parameters (inout): None
* Parameters (out): None
* Return value: Std_ReturnType
* Description: Service for writing one or more data to an IB SPI Handler/Driver Channel specified
by parameter.
************************************************************************************/

#if(SPI_CHANNEL_BUFFERS_ALLOWED_API == STD_ON)
Std_ReturnType Spi_WriteIB(Spi_ChannelType Channel,const Spi_DataBufferType* DataBufferPtr)
{
  
#if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Init_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_WriteIB_SID,
		     SPI_E_UNINIT_ID);
        }
        
        if (NULL_PTR == DataBufferPtr)
	{
		Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_WriteIB_SID,
		     SPI_E_PARAM_POINTER_ID);
	}
        
        if( (Channel != 0x00) || (Channel != 0x01) || (Channel != 0x02) || (Channel != 0x03) )
        {
                Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_WriteIB_SID,
		      SPI_E_PARAM_CHANNEL_ID);
        }
        
	else
#endif /* (SPI_DEV_ERROR_DETECT == STD_ON) */
        {
            volatile uint32 * SPIn_BaseAddress_Ptr = NULL_PTR;

            switch( Channel )
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
            
          /*Write Data*/  
           *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSIDATA_OFFSET) = *DataBufferPtr;
          /*Busy wait as soon as Transmit FIFO is not empty*/
           while( ((*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSISR_OFFSET)) & (1<<0)) == 0 ); 
            
           return E_OK;  
        }
         
}
#endif


#if(SPI_CHANNEL_BUFFERS_ALLOWED_API == STD_ON)
Std_ReturnType Spi_ReadIB(Spi_ChannelType Channel,Spi_DataBufferType* DataBufferPointer)
{
  
#if (SPI_DEV_ERROR_DETECT == STD_ON)

        if(Spi_Init_Status == SPI_NOT_INITIALIZED )
        {
          
          Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_ReadIB_SID,
		     SPI_E_UNINIT_ID);
        }
                
        if( (Channel != 0x00) || (Channel != 0x01) || (Channel != 0x02) || (Channel != 0x03) )
        {
                Det_ReportError(SPI_MODULE_ID, SPI_INSTANCE_ID, Spi_ReadIB_SID,
		      SPI_E_PARAM_CHANNEL_ID);
        }
        
	else
#endif /* (SPI_DEV_ERROR_DETECT == STD_ON) */
        {
            volatile uint32 * SPIn_BaseAddress_Ptr = NULL_PTR;

            switch( Channel )
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
           
            /*Busy wait as soon as Receive FIFO is not full*/
            while( ((*(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSISR_OFFSET)) & (1<<3)) == 0 ); 
           /*Read Data*/  
           *DataBufferPtr = *(volatile uint32 *)((volatile uint8 *)SPIn_BaseAddress_Ptr + SSIDATA_OFFSET);
           
           return E_OK;  
        }
         
}
#endif






