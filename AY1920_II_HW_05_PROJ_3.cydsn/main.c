/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
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
*   \brief Hex value to set high resolution mode at 100 Hz to the accelerator
*/
#define LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG1 0x57

/**
*   \brief Address of the Control register 4
*/
#define LIS3DH_CTRL_REG4 0x23

/*brief Hex value to set high resolution mode at 100 Hz to the accelerator and ±4.0 g FSR.*/

#define LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG4 0x98
//The BDU bit is set to 1

/**
*   \brief Address of the x-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_X_L 0x028

/**
*   \brief Address of the y-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_Y_L 0x02A

/**
*   \brief Address of the z-axis acceleration data output LSB register
*/
#define LIS3DH_OUT_Z_L 0x02C

/*Thanks to the multiread function, we don't need to specify the MSB register address */


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

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
               Control Register 1             */
    /******************************************/
    
        
    UART_Debug_PutString("\r\nWriting new values..\r\n");
    
    if (ctrl_reg1 != LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG1;
    
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
    /*            I2C Writing 
               Control Register 4             */
    /******************************************/
    
    UART_Debug_PutString("\r\nWriting new values..\r\n");
    
    if (ctrl_reg4 != LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG4)
    {
        ctrl_reg4 = LIS3DH_HIGH_RESOLUTION_MODE_100HZ_CTRL_REG4;
    
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
    
 
    uint8_t header = 0xA0;
    uint8_t footer = 0xC0;
    uint8_t ValueArray[14]; 
    uint8_t AccData[6];
    uint8_t status_register;
    int16_t ValueX, ValueY, ValueZ;
    int32 IntX, IntY, IntZ;
    float32 FloatX, FloatY, FloatZ;
    
    ValueArray[0] = header;
    ValueArray[13] = footer;
    
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
           //Checking if ZYXDA is set to 1. This condition means that a new set of data is available.
              if((status_register & 1<<3) == 8)
              { 
             //It is used a multiread function because the registers are consecutive.
                error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                         LIS3DH_OUT_X_L,
                                                         6,
                                                         &AccData[0]);
        
                 if (error==NO_ERROR)
                 {    
                    ValueX = (int16)((AccData[0] | (AccData[1]<<8)))>>4;
                //We need to multiply ValueX by 2 because the sensitivity in this case is of 2mg/digit. Then, in order to
                //convert the X axial output of the Accelerometer to a floating point in m/s2 units, we need to multiply
                // by 9.806* 0.001, that is the equivalent value of an mg in m/s2.
                    FloatX = (ValueX*2*9.806*0.001); // The final result is set in a float variable.
                //Cast the floating point values to an int variable without losing information through the
                    IntX= FloatX * 1000; //multiplication by 1000.
                    ValueArray[1] = (uint8_t)(IntX & 0xFF);
                    ValueArray[2] = (uint8_t)(IntX >> 8);
                    ValueArray[3] = (uint8_t)(IntX >> 16);
                    ValueArray[4] = (uint8_t)(IntX >> 24);
            
            
                    ValueY = (int16)((AccData[2] | (AccData[3]<<8)))>>4;
               //We need to multiply ValueY by 2 because the sensitivity in this case is of 2mg/digit. Then, in order to
              //convert the Y axial output of the Accelerometer to a floating point in m/s2 units, we need to multiply
              // by 9.806* 0.001, that is the equivalent value of an mg in m/s2.        
                    FloatY = (ValueY*2*9.806*0.001); //The final result is set in a float variable.
              //Cast the floating point values to an int variable without losing information through the
                    IntY= FloatY * 1000;  //multiplication by 1000.
                    ValueArray[5] = (uint8_t)(IntY & 0xFF);
                    ValueArray[6] = (uint8_t)(IntY >> 8);
                    ValueArray[7] = (uint8_t)(IntY >> 16);
                    ValueArray[8] = (uint8_t)(IntY >> 24);
                
                    
                    ValueZ = (int16)((AccData[4] | (AccData[5]<<8)))>>4;
            //We need to multiply ValueZ by 2 because the sensitivity in this case is of 2mg/digit. Then, in order to
           //convert the Z axial output of the Accelerometer to a floating point in m/s2 units, we need to multiply
           // by 9.806* 0.001, that is the equivalent value of an mg in m/s2.   
                    FloatZ = (ValueZ*2*9.806*0.001); //The final result is set in a float variable.
           //Cast the floating point values to an int variable without losing information through the
                    IntZ= FloatZ * 1000;  //multiplication by 1000.
                    ValueArray[9] = (uint8_t)(IntZ & 0xFF);
                    ValueArray[10] = (uint8_t)(IntZ >> 8);
                    ValueArray[11] = (uint8_t)(IntZ >> 16);
                    ValueArray[12] = (uint8_t)(IntZ >> 24);
                    
                    UART_Debug_PutArray(ValueArray, 14); // Sending the informations to the UART
                    
                    FlagIsr = 0; // Set the FlagIsr to 0
                    }
                }
            }
         
        }
    }    
}

/* [] END OF FILE */
