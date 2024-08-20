/*******************************************************************************
* Copyright 2011-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

namespace I2C_v3_50
{
    public class CyHexConvertor
    {
        public static string HEX_PREFIX = "0x";

        public static int HexToInt(string hexString)
        {
            int result = -1;
            try
            {
                result = Convert.ToInt32(hexString, 16);
            }
            catch (Exception) { }
            return result;
        }

        public static string IntToHex(int intValue)
        {
            return (HEX_PREFIX + intValue.ToString("X"));
        }
    }
}
