/*******************************************************************************
* Copyright 2011-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;

namespace I2C_v3_50
{
    public class CyDividerCalculator
    {
        public static byte GetES3Divider(ICyInstQuery_v1 query, ICyTerminalQuery_v1 termQuery)
        {
            // Get Implementation
            CyEImplementationType implementation = new CyEImplementationType();
            try
            {
                implementation = (CyEImplementationType)byte.Parse(query.GetCommittedParam(
                    CyParamNames.IMPLEMENTATION).Value);
            }
            catch (Exception)
            {
                Debug.Assert(false);
            }

            // Get Mode
            CyEModeType mode = new CyEModeType();
            try
            {
                mode = (CyEModeType)byte.Parse(query.GetCommittedParam(CyParamNames.MODE).Value);
            }
            catch (Exception)
            {
                Debug.Assert(false);
            }

            // Get DataRate
            UInt32 dataRate = 0;
            UInt32.TryParse(query.GetCommittedParam(CyParamNames.DATA_RATE).Value, out dataRate);

            // Get Bus Clock value
            CyClockData clock = CyClockReader.GetInternalClock(termQuery, implementation);
            double busClock = CyClockReader.ConvertFreqToKHz(clock);

            byte oversampleRate = (dataRate <= 50) ? (byte)32 : (byte)16;
            double defaultClockDiv = (double)(busClock / (dataRate * oversampleRate));

            return (mode == CyEModeType.Slave) ? (byte)defaultClockDiv : (byte)Math.Ceiling(defaultClockDiv);
        }

        public static byte GetES2Divider(ICyInstQuery_v1 query, ICyTerminalQuery_v1 termQuery)
        {
            // Get Implementation
            CyEImplementationType implementation = new CyEImplementationType();
            try
            {
                implementation = (CyEImplementationType)byte.Parse(query.GetCommittedParam(
                    CyParamNames.IMPLEMENTATION).Value);
            }
            catch (Exception)
            {
                Debug.Assert(false);
            }

            // Get Mode
            CyEModeType mode = new CyEModeType();
            try
            {
                mode = (CyEModeType)byte.Parse(query.GetCommittedParam(CyParamNames.MODE).Value);
            }
            catch (Exception)
            {
                Debug.Assert(false);
            }

            // Get Data Rate
            UInt32 dataRate = 0;
            UInt32.TryParse(query.GetCommittedParam(CyParamNames.DATA_RATE).Value, out dataRate);

            // Calculate Over Sample Rate
            byte oversampleRate = (dataRate <= 50) ? (byte)32 : (byte)16;

            // Get Bus Clock value
            CyClockData clock = CyClockReader.GetInternalClock(termQuery, implementation);
            double busClock = CyClockReader.ConvertFreqToKHz(clock);

            // Value for saving power of Divide Factor
            int power = 0;

            // Create possible Baud Rate values
            List<double> baudRate = new List<double>();
            baudRate.Add(busClock / (1 * oversampleRate));
            baudRate.Add(busClock / (2 * oversampleRate));
            baudRate.Add(busClock / (4 * oversampleRate));
            baudRate.Add(busClock / (8 * oversampleRate));
            baudRate.Add(busClock / (16 * oversampleRate));
            baudRate.Add(busClock / (32 * oversampleRate));
            baudRate.Add(busClock / (64 * oversampleRate));

            if (mode == CyEModeType.Slave)
            {
                // Find most close value (greater or equal) to Data Rate
                for (int i = baudRate.Count - 1; i >= 0; i--)
                {
                    if (baudRate[i] >= dataRate)
                    {
                        power = i;
                        break;
                    }
                }
            }
            else
            {
                // Find most close value (less or equal) to Data Rate
                for (int i = 0; i < baudRate.Count; i++)
                {
                    if (baudRate[i] <= dataRate)
                    {
                        power = i;
                        break;
                    }
                }
            }

            return (byte)power;
        }
    }
}
