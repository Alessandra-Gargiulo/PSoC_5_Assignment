/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
*
* \date , 2020
*/

// Include required header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"
#include "InterruptRoutines.h"

/**
*   \brief 7-bit I2C address of the slave device.
*/
#define LIS3DH_DEVICE_ADDRESS 0x18

/**
*   \brief Address of the WHO AM I register
*/
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

/**
*   \brief Address of the Status register
*/
#define LIS3DH_STATUS_REG 0x27

/**
*   \brief Address of the Control register 1
*/
#define LIS3DH_CTRL_REG1 0x20

/**
*   \brief Hex value to set normal mode at 100 Hz to the accelerator
*/
#define LIS3DH_NORMAL_MODE_100HZ_CTRL_REG1 0x57

/**
*   \brief Address of the Control register 4
*/
#define LIS3DH_CTRL_REG4 0x23

// For the normale mode at 100 Hz and ±2.0 g FSR, Hex value is set to 0x80 because BDU is set to 1

#define LIS3DH_NORMAL_MODE_100HZ_CTRL_REG4 0x80
/**
*   \brief Address of the x-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_X_L 0x28

/**
*   \brief Address of the y-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_Y_L 0x2A

/**
*   \brief Address of the z-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_Z_L 0x2C

/*Thanks to the multiread function, we don't need to specify the MSB register address */


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // String to print out messages on the UART
    char message[50];

    // Check which devices are present on the I2C bus
    for (int i = 0 ; i < 128; i++)
    {
        if (I2C_Peripheral_IsDeviceConnected(i))
        {
            // print out the address is hex format
            sprintf(message, "Device 0x%02X is connected\r\n", i);
            UART_Debug_PutString(message); 
        }
        
    }
    
    /******************************************/
    /*            I2C Reading                 */
    /******************************************/
    
    /* Read WHO AM I REGISTER register */
    uint8_t who_am_i_reg;
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                  LIS3DH_WHO_AM_I_REG_ADDR, 
                                                  &who_am_i_reg);
    if (error == NO_ERROR)
    {
        sprintf(message, "WHO AM I REG: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm\r\n");   
    }
    
    /******************************************/
    /*        Read Control Register 1         */
    /******************************************/
    uint8_t ctrl_reg1; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register 1\r\n");   
    }
    
    /******************************************/
    /*            I2C Writing  
                 Control Register 1           */
    /******************************************/
    
        
    UART_Debug_PutString("\r\nWriting new values..\r\n");
    
    if (ctrl_reg1 != LIS3DH_NORMAL_MODE_100HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_NORMAL_MODE_100HZ_CTRL_REG1;
    
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", ctrl_reg1);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
    }
    
     /******************************************/
    /*        Read Control Register 4        */
    /******************************************/
    uint8_t ctrl_reg4; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register 4\r\n");   
    }
    
    /******************************************/
    /*            I2C Writing 
               Control Register 4             */
    /******************************************/
    
    UART_Debug_PutString("\r\nWriting new values..\r\n");
    
    if (ctrl_reg4 != LIS3DH_NORMAL_MODE_100HZ_CTRL_REG4)
    {
        ctrl_reg4 = LIS3DH_NORMAL_MODE_100HZ_CTRL_REG4;
    
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG4,
                                             ctrl_reg4);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 4 successfully written as: 0x%02X\r\n", ctrl_reg4);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to set control register 4\r\n");   
        }
    }
    
    
    int16_t ValueX, ValueY, ValueZ;
    uint8_t header = 0xA0;
    uint8_t footer = 0xC0;
    uint8_t ValueArray[8]; 
    uint8_t AccData[6];
    uint8_t status_register;
    
    ValueArray[0] = header;
    ValueArray[7] = footer;
    
    Timer_1_Start();
    isr_10_StartEx(Custom_ISR);
    
    for(;;)
    {
        if(FlagIsr != 0)
        {
            //Reading of the status register
             error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                 LIS3DH_STATUS_REG,
                                                 &status_register);
        
             if(error==NO_ERROR) 
            { 
                //Checking if ZYXDA is set to 1. This condition that means that a new set of data is avaiable.
               if((status_register & 1<<3) == 8)
                { 
                
                 //It is used a multiread function because the registers X, Y and Z are consecutives.
            
                 error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                          LIS3DH_OUT_X_L,
                                                          6,
                                                          &AccData[0]);
        
                 if (error==NO_ERROR)
                {    
                   ValueX = (int16)((AccData[0] | (AccData[1]<<8)))>>6;
                   ValueX = (ValueX*4); //Operation needed because the sensitity is of 4 mg/digit
                   ValueArray[1] = (uint8_t)(ValueX & 0xFF);
                   ValueArray[2] = (uint8_t)(ValueX >> 8);
            
            
                   ValueY = (int16)((AccData[2] | (AccData[3]<<8)))>>6;
                   ValueY = (ValueY*4); //Operation needed because the sensitity is of 4 mg/digit
                   ValueArray[3] = (uint8_t)(ValueY & 0xFF);
                   ValueArray[4] = (uint8_t)(ValueY >> 8);
            
            
                   ValueZ = (int16)((AccData[4] | (AccData[5]<<8)))>>6;
                   ValueZ = (ValueZ*4); //Operation needed because the sensitity is of 4 mg/digit
                   ValueArray[5] = (uint8_t)(ValueZ & 0xFF);
                   ValueArray[6] = (uint8_t)(ValueZ >> 8);
                
                   UART_Debug_PutArray(ValueArray, 8); //Sending the values to UART
                
                   FlagIsr =0; //Set FlagIsr to 0 again
                }
                }
              
            }
        }
    }
}
    

/* [] END OF FILE */
