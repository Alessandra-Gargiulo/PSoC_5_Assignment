/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "InterruptRoutines.h"

uint8 FlagIsr = 0;   //Inizialization of FlagIsr

CY_ISR(Custom_ISR)
{
    Timer_1_ReadStatusRegister();
    FlagIsr = 1; //Set the Flag to 1 every 10 ms
    
}

/* [] END OF FILE */
