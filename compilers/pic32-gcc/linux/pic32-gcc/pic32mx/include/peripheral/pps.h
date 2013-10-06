#if !defined(__PPS_H)
#define      __PPS_H

#include "p32xxxx.h"

/*
Example Code:

/* ********************* Assign UART 2 signals onto pins using PPS ***********************************
    PPSInput(2,U2RX, RPA9);  //Assign U2RX to pin RPA09
    PPSInput(3,U2CTS,RPC3);  //Assign U2CTS to pin RPC3
    PPSOutput(4,RPC4,U2TX);   //Assign U2TX to pin RPC4
    PPSOutput(1,RPB15,U2RTS); //Assign U2RTS to pin RPB15

 */

/*---------------------------------------------------------------------------*/
/*Section:  Remappable Peripheral Inputs :                                   */
/*---------------------------------------------------------------------------*/
/* Input Pins Group 1 */
#define IN_FN_PPS1_INT4          INT4Rbits.INT4R
#define IN_FN_PPS1_T2CK          T2CKRbits.T2CKR
#define IN_FN_PPS1_IC4           IC4Rbits.IC4R
#define IN_FN_PPS1_SS1           SS1Rbits.SS1R
#define IN_FN_PPS1_REFCLKI       REFCLKIRbits.REFCLKIR

#define IN_PIN_PPS1_RPA0         0   /* Assign RPA0  as Input Pin */
#define IN_PIN_PPS1_RPB3         1   /* Assign RPB3  as Input Pin */
#define IN_PIN_PPS1_RPB4         2   /* Assign RPB4  as Input Pin */
#define IN_PIN_PPS1_RPB15        3   /* Assign RPB15 as Input Pin */
#define IN_PIN_PPS1_RPB7         4   /* Assign RPB7  as Input Pin */
#define IN_PIN_PPS1_RPC7         5   /* Assign RPC4  as Input Pin */
#define IN_PIN_PPS1_RPC0         6   /* Assign RPC0  as Input Pin */
#define IN_PIN_PPS1_RPC5         7   /* Assign RPC5  as Input Pin */


/* Input Pins Group 2 */
#define IN_FN_PPS2_INT3          INT3Rbits.INT3R
#define IN_FN_PPS2_T3CK          T3CKRbits.T3CKR
#define IN_FN_PPS2_IC3           IC3Rbits.IC3R
#define IN_FN_PPS2_U1CTS         U1CTSRbits.U1CTSR
#define IN_FN_PPS2_U2RX          U2RXRbits.U2RXR
#define IN_FN_PPS2_SDI1          SDI1Rbits.SDI1R

#define IN_PIN_PPS2_RPA1         0   /* Assign RPA1  as Input Pin */
#define IN_PIN_PPS2_RPB5         1   /* Assign RPB5  as Input Pin */
#define IN_PIN_PPS2_RPB1         2   /* Assign RPB1  as Input Pin */
#define IN_PIN_PPS2_RPB11        3   /* Assign RPB11 as Input Pin */
#define IN_PIN_PPS2_RPB8         4   /* Assign RPB8  as Input Pin */
#define IN_PIN_PPS2_RPA8         5   /* Assign RPA8  as Input Pin */
#define IN_PIN_PPS2_RPC8         6   /* Assign RPC8  as Input Pin */
#define IN_PIN_PPS2_RPA9         7   /* Assign RPA9  as Input Pin */


/* Input Pins Group 3 */
#define IN_FN_PPS3_INT2          INT2Rbits.INT2R
#define IN_FN_PPS3_T4CK          T4CKRbits.T4CKR
#define IN_FN_PPS3_IC1           IC1Rbits.IC1R
#define IN_FN_PPS3_IC5           IC5Rbits.IC5R
#define IN_FN_PPS3_U1RX          U1RXRbits.U1RXR
#define IN_FN_PPS3_U2CTS         U2CTSRbits.U2CTSR
#define IN_FN_PPS3_SDI2          SDI2Rbits.SDI2R
#define IN_FN_PPS3_OCFB          OCFBRbits.OCFBR

#define IN_PIN_PPS3_RPA2         0   /* Assign RPA2  as Input Pin */
#define IN_PIN_PPS3_RPB6         1   /* Assign RPB6  as Input Pin */
#define IN_PIN_PPS3_RPA4         2   /* Assign RPA4  as Input Pin */
#define IN_PIN_PPS3_RPB13        3   /* Assign RPB13 as Input Pin */
#define IN_PIN_PPS3_RPB2         4   /* Assign RPB2  as Input Pin */
#define IN_PIN_PPS3_RPC6         5   /* Assign RPC6  as Input Pin */
#define IN_PIN_PPS3_RPC1         6   /* Assign RPC1  as Input Pin */
#define IN_PIN_PPS3_RPC3         7   /* Assign RPC3  as Input Pin */


/* Input Pins Group 4 */
#define _PPS_INPUT_BASE_ADDRESS  INT1R // Happens to be the first one
#define IN_FN_PPS4_INT1          INT1Rbits.INT1R
#define IN_FN_PPS4_T5CK          T5CKRbits.T5CKR
#define IN_FN_PPS4_IC2           IC2Rbits.IC2R
#define IN_FN_PPS4_SS2           SS2Rbits.SS2R
#define IN_FN_PPS4_OCFA          OCFARbits.OCFAR

#define IN_PIN_PPS4_RPA3         0   /* Assign RPA3  as Input Pin */
#define IN_PIN_PPS4_RPB14        1   /* Assign RPB14 as Input Pin */
#define IN_PIN_PPS4_RPB0         2   /* Assign RPB0  as Input Pin */
#define IN_PIN_PPS4_RPB10        3   /* Assign RPB10 as Input Pin */
#define IN_PIN_PPS4_RPB9         4   /* Assign RPB9  as Input Pin */
#define IN_PIN_PPS4_RPC9         5   /* Assign RPC9  as Input Pin */
#define IN_PIN_PPS4_RPC2         6   /* Assign RPC2  as Input Pin */
#define IN_PIN_PPS4_RPC4         7   /* Assign RPC4  as Input Pin */


/*********************************************************************
 Macro:             PPSInput(fn,pin)

 PreCondition:     None

 Side Effects:     None

 Overview:          The macro assigns given pin as input pin by configuring register RPINRx.

 Parameters:    fn - function to be assigned for particular pin
            *   IN_FN_PPS_TxCK
            *   IN_FN_PPS_INTx
            *   IN_FN_PPS_ICx
            *   IN_FN_PPS_OCFx
            *   IN_FN_PPS_UxRX
            *   IN_FN_PPS_UxCTS
            *   IN_FN_PPS_SDIx
            *   IN_FN_PPS_SCKxIN
            *   IN_FN_PPS_SSxIN

            pin - pin number(x) for which functionality has to be assigned
            *   IN_PIN_PPS_RPx

 Returns:       None

 Note:              None
 ********************************************************************/
#define PPSInput(grp,fn,pin) IN_FN_PPS##grp##_##fn = IN_PIN_PPS##grp##_##pin

/*---------------------------------------------------------------------------*/
/*Section:  Remappable Peripheral Outputs:                                   */
/*---------------------------------------------------------------------------*/
/* Output Pin Group 1 */
#define _PPS_OUTPUT_BASE_ADDRESS RPA0R // Happens to be the first one
#define OUT_PIN_PPS1_RPA0        RPA0Rbits.RPA0R
#define OUT_PIN_PPS1_RPB3        RPB3Rbits.RPB3R
#define OUT_PIN_PPS1_RPB4        RPB4Rbits.RPB4R
#define OUT_PIN_PPS1_RPB15       RPB15Rbits.RPB15R
#define OUT_PIN_PPS1_RPB7        RPB7Rbits.RPB7R
#define OUT_PIN_PPS1_RPC7        RPC7Rbits.RPC7R
#define OUT_PIN_PPS1_RPC0        RPC0Rbits.RPC0R
#define OUT_PIN_PPS1_RPC5        RPC5Rbits.RPC5R

#define OUT_FN_PPS1_NULL         0  /* No Connect */
#define OUT_FN_PPS1_U1TX         1  /* RPn tied to U1TX */
#define OUT_FN_PPS1_U2RTS        2  /* RPn tied to U2RTS_BAR */
#define OUT_FN_PPS1_SS1          3  /* RPn tied to SS1 */
//                               4  - RESERVED
#define OUT_FN_PPS1_OC1          5  /* RPn tied to OC1 */
//                               6  - RESERVED
#define OUT_FN_PPS1_C2OUT        7  /* RPn tied to C2OUT */


/* Output Pin Group 2 */
#define OUT_PIN_PPS2_RPA1        RPA1Rbits.RPA1R
#define OUT_PIN_PPS2_RPB5        RPB5Rbits.RPB5R
#define OUT_PIN_PPS2_RPB1        RPB1Rbits.RPB1R
#define OUT_PIN_PPS2_RPB11       RPB11Rbits.RPB11R
#define OUT_PIN_PPS2_RPB8        RPB8Rbits.RPB8R
#define OUT_PIN_PPS2_RPA8        RPA8Rbits.RPA8R
#define OUT_PIN_PPS2_RPC8        RPC8Rbits.RPC8R
#define OUT_PIN_PPS2_RPA9        RPA9Rbits.RPA9R

#  define OUT_FN_PPS2_NULL       0  /* No Connect */
//                               1  - RESERVED
//                               2  - RESERVED
#define OUT_FN_PPS2_SDO1         3  /* RPn tied to SDO1 */
#define OUT_FN_PPS2_SDO2         4  /* RPn tied to SDO2 */
#define OUT_FN_PPS2_OC2          5  /* RPn tied to OC2 */
//                               6  - RESERVED
//                               7  - RESERVED


/* Output Pin Group 3 */
#define OUT_PIN_PPS3_RPA2        RPA2Rbits.RPA2R
#define OUT_PIN_PPS3_RPB6        RPB6Rbits.RPB6R
#define OUT_PIN_PPS3_RPA4        RPA4Rbits.RPA4R
#define OUT_PIN_PPS3_RPB13       RPB13Rbits.RPB13R
#define OUT_PIN_PPS3_RPB2        RPB2Rbits.RPB2R
#define OUT_PIN_PPS3_RPC6        RPC6Rbits.RPC6R
#define OUT_PIN_PPS3_RPC1        RPC1Rbits.RPC1R
#define OUT_PIN_PPS3_RPC3        RPC3Rbits.RPC3R

#define OUT_FN_PPS3_NULL         0  /* No Connect */
//                               1  - RESERVED
//                               2  - RESERVED
#define OUT_FN_PPS3_SDO1         3  /* RPn tied to SDO1 */
#define OUT_FN_PPS3_SDO2         4  /* RPn tied to SDO2 */
#define OUT_FN_PPS3_OC4          5  /* RPn tied to OC4 */
#define OUT_FN_PPS3_OC5          6  /* RPn tied to OC5 */
#define OUT_FN_PPS3_REFCLKO      7  /* RPn tied to REFCLKO */


/* Output Pin Group 4 */
#define OUT_PIN_PPS4_RPA3        RPA3Rbits.RPA3R
#define OUT_PIN_PPS4_RPB14       RPB14Rbits.RPB14R
#define OUT_PIN_PPS4_RPB0        RPB0Rbits.RPB0R
#define OUT_PIN_PPS4_RPB10       RPB10Rbits.RPB10R
#define OUT_PIN_PPS4_RPB9        RPB9Rbits.RPB9R
#define OUT_PIN_PPS4_RPC9        RPC9Rbits.RPC9R
#define OUT_PIN_PPS4_RPC2        RPC2Rbits.RPC2R
#define OUT_PIN_PPS4_RPC4        RPC4Rbits.RPC4R

#define OUT_FN_PPS4_NULL         0  /* No Connect */
#define OUT_FN_PPS4_U1RTS        1  /* RPn tied to U1RTS */
#define OUT_FN_PPS4_U2TX         2  /* RPn tied to U2TX */
//                               3  - RESERVED
#define OUT_FN_PPS4_SS2          4  /* RPn tied to SS2_BAR */
#define OUT_FN_PPS4_OC3          5  /* RPn tied to OC31 */
//                               6  - RESERVED
#define OUT_FN_PPS4_C1OUT        7  /* RPn tied to C1OUT */


 /*********************************************************************
  Macro:            PPSOutput(pin,fn)

  PreCondition:     None

  Side Effects:     None

  Overview:          The macro assigns given pin as output pin by configuring register RPORx.

  Parameters:   pin - pin number(x) for which functionality has to be assigned
            *   OUT_PIN_PPS_RPx

                                fn - function to be assigned for particular pin
            *   OUT_FN_PPS_NULL
            *   OUT_FN_PPS_CxOUT
            *   OUT_FN_PPS_UxTX
            *   OUT_FN_PPS_UxRTS
            *   OUT_FN_PPS_SDOx
            *   OUT_FN_PPS_SCKxOUT
            *   OUT_FN_PPS_SSxOUT
            *   OUT_FN_PPS_OCx
            *   OUT_FN_PPS_CTMU

  Returns:      None

  Note:             None
 ********************************************************************/
#define PPSOutput(grp,pin,fn)   OUT_PIN_PPS##grp##_##pin = OUT_FN_PPS##grp##_##fn
/*----------------------------------------------------------------------------------------------------*/

 /*********************************************************************
  Macro:            PPSUnLock
  PreCondition:     None
  Side Effects:     None
  Overview:          The macro performs the unlocking sequence for PPS assignment.
  Parameters:   None
  Returns:      None
  Note:             None
 ********************************************************************/
#define  PPSUnLock                  //__builtin_write_OSCCONL(OSCCON & 0xbf)

 /*********************************************************************
  Macro:            PPSLock
  PreCondition:     None
  Side Effects:     None
  Overview:          The macro performs the locking sequence for PPS assignment.
  Parameters:   None
  Returns:      None
 Note:              None
 ********************************************************************/
#define  PPSLock                    //__builtin_write_OSCCONL(OSCCON | 0x40)

#endif//!defined(__PPS_H)
