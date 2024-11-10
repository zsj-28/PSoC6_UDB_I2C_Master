/*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;
using System.Windows.Forms;
using System.Drawing;

namespace I2C_v3_50
{
    #region Component Parameter Names
    public class CyParamNames
    {
        public const string MODE = "I2C_Mode";
        public const string DATA_RATE = "BusSpeed_kHz";
        public const string IMPLEMENTATION = "Implementation";
        public const string ADDRESS_DECODE = "Address_Decode";
        public const string SLAVE_ADDRESS = "Slave_Address";
        public const string BUS_PORT = "I2cBusPort";
        public const string ENABLE_WAKEUP = "EnableWakeup";
        public const string UDB_INTERNAL_CLOCK = "UdbInternalClock";
        public const string SLAVE_CLOCK_MINUS_TOLERANCE = "SlaveClockMinusTolerance";
        public const string SLAVE_CLOCK_PLUS_TOLERANCE = "SlaveClockPlusTolerance";
        public const string NOT_SLAVE_CLOCK_MINUS_TOLERANCE = "NotSlaveClockMinusTolerance";
        public const string NOT_SLAVE_CLOCK_PLUS_TOLERANCE = "NotSlaveClockPlusTolerance";
        public const string UDB_SLAVE_FIXED_PLACEMENT_ENABLE = "UdbSlaveFixedPlacementEnable";

        public const string EXTERNAL_BUFFER = "ExternalBuffer";
        public const string SDA_TIMEOUT_ENABLED = "SdaTimeoutEnabled";
        public const string SCL_TIMEOUT_ENABLED = "SclTimeoutEnabled";
        public const string TIME_OUT = "TimeOutms";

        // Parameters for internal use
        public const string HEX = "Hex";
        public const string UDB_REQUIRED_CLOCK = "UdbRequiredClock";
    }
    #endregion

    #region Constants For Parameters Range
    public class CyParamRange
    {
        public const int DATA_RATE_MIN = 1;
        public const int DATA_RATE_MAX = 1000;
        public const int SLAVE_ADDRESS_MIN = 0;
        public const int SLAVE_ADDRESS_MAX = 127;
        public const double SLAVE_TOLERANCE_MIN = -5.0;
        public const double SLAVE_TOLERANCE_MAX = 50.0;
        public const double NOT_SLAVE_TOLERANCE_MIN = -25.0;
        public const double NOT_SLAVE_TOLERANCE_MAX = 5.0;
    }
    #endregion

    #region Component Enums
    public enum CyEModeType { Slave = 1, Master = 2, MultiMaster_revA = 6, MultiMaster_Slave_revA = 7 }
    public enum CyEImplementationType { UDB, FixedFunction }
    public enum CyEAddressDecodeType { Software, Hardware }
    public enum CyEBusPortType { Any, I2C0, I2C1 };
    #endregion

    public class CyI2CParameters
    {
        #region Declaring public variables
        public ICyInstEdit_v1 m_inst;
        public CyI2CBasic m_basicTab;
        public CyAdvancedConfTab m_advancedConfigTab;

        // During first getting of parameters this variable is false, what means that assigning
        // values to form controls will not immediatly overwrite parameters with the same values.
        public bool m_bGlobalEditMode = false;
        // The clock connected to the symbol
        public CyClockData m_extClock;
        //public double m_requiredClock;
        public string m_instanceName;

        // Lists contain display names of types taken from symbol,
        // lists are used to fill comboboxes
        public List<string> m_modesList;
        public List<string> m_portsList;

        // Dictionary is used to keep display names of types taken from symbol,
        // and associate them with the enum fields to simplify access to diaplay names
        public Dictionary<object, string> m_dnDict = new Dictionary<object, string>();
        #endregion

        #region Constructor(s)
        public CyI2CParameters(ICyInstEdit_v1 inst, ICyTerminalQuery_v1 termQuery)
        {
            this.m_inst = inst;
            m_instanceName = inst.GetCommittedParam("INSTANCE_NAME").Value;
            m_modesList = new List<string>(inst.GetPossibleEnumValues(CyParamNames.MODE));
            m_extClock = CyClockReader.GetExternalClock(termQuery);

            CyDictParser.FillDictionary(ref m_dnDict, typeof(CyEModeType), m_modesList);
        }
        #endregion

        #region Class properties
        public CyEModeType Mode
        {
            get
            {
                return GetValue<CyEModeType>(CyParamNames.MODE);
            }
            set
            {//MultiMaster
                SetValue(CyParamNames.MODE, value);
            }
        }

        public UInt16 DataRate
        {
            get
            {
                return GetValue<UInt16>(CyParamNames.DATA_RATE);
            }
            set
            {
                SetValue(CyParamNames.DATA_RATE, value);
            }
        }

        public CyEImplementationType Implementation
        {
            get
            {
                return GetValue<CyEImplementationType>(CyParamNames.IMPLEMENTATION);
            }
            set
            {
                SetValue(CyParamNames.IMPLEMENTATION, value);
            }
        }

        public CyEAddressDecodeType AddressDecode
        {
            get
            {
                return GetValue<CyEAddressDecodeType>(CyParamNames.ADDRESS_DECODE);
            }
            set
            {
                SetValue(CyParamNames.ADDRESS_DECODE, value);
            }
        }

        public UInt16 SlaveAddress
        {
            get
            {
                return GetValue<UInt16>(CyParamNames.SLAVE_ADDRESS);
            }
            set
            {
                SetValue(CyParamNames.SLAVE_ADDRESS, value);
            }
        }

        public CyEBusPortType BusPort
        {
            get
            {
                return GetValue<CyEBusPortType>(CyParamNames.BUS_PORT);
            }
            set
            {
                SetValue(CyParamNames.BUS_PORT, value);
            }
        }

        public bool EnableWakeup
        {
            get
            {
                return GetValue<bool>(CyParamNames.ENABLE_WAKEUP);
            }
            set
            {
                SetValue(CyParamNames.ENABLE_WAKEUP, value);
            }
        }

        public bool UdbInternalClock
        {
            get
            {
                return GetValue<bool>(CyParamNames.UDB_INTERNAL_CLOCK);
            }
            set
            {
                SetValue(CyParamNames.UDB_INTERNAL_CLOCK, value);
            }
        }

        public double MinusTolerance
        {
            get
            {
                return GetValue<double>((Mode == CyEModeType.Slave)
                    ? CyParamNames.SLAVE_CLOCK_MINUS_TOLERANCE : CyParamNames.NOT_SLAVE_CLOCK_MINUS_TOLERANCE);
            }
            set
            {
                SetValue((Mode == CyEModeType.Slave)
                    ? CyParamNames.SLAVE_CLOCK_MINUS_TOLERANCE : CyParamNames.NOT_SLAVE_CLOCK_MINUS_TOLERANCE, value);
            }
        }

        public double PlusTolerance
        {
            get
            {
                return GetValue<double>((Mode == CyEModeType.Slave)
                    ? CyParamNames.SLAVE_CLOCK_PLUS_TOLERANCE : CyParamNames.NOT_SLAVE_CLOCK_PLUS_TOLERANCE);
            }
            set
            {
                SetValue((Mode == CyEModeType.Slave)
                    ? CyParamNames.SLAVE_CLOCK_PLUS_TOLERANCE : CyParamNames.NOT_SLAVE_CLOCK_PLUS_TOLERANCE, value);
            }
        }

        public bool UdbSlaveFixedPlacementEnable
        {
            get
            {
                return GetValue<bool>(CyParamNames.UDB_SLAVE_FIXED_PLACEMENT_ENABLE);
            }
            set
            {
                SetValue(CyParamNames.UDB_SLAVE_FIXED_PLACEMENT_ENABLE, value);
            }
        }

        // Parameters for internal use
        public bool Hex
        {
            get
            {
                return GetValue<bool>(CyParamNames.HEX);
            }
            set
            {
                SetValue(CyParamNames.HEX, value);
            }
        }

        public double UdbRequiredClock
        {
            get
            {
                return GetValue<double>(CyParamNames.UDB_REQUIRED_CLOCK);
            }
        }

        public bool ExternalBuffer
        {
            get
            {
                return GetValue<bool>(CyParamNames.EXTERNAL_BUFFER);
            }
            set
            {
                SetValue(CyParamNames.EXTERNAL_BUFFER, value);
            }
        }

        public bool SdaTimeoutEnabled
        {
            get
            {
                return GetValue<bool>(CyParamNames.SDA_TIMEOUT_ENABLED);
            }
        }

        public bool SclTimeoutEnabled
        {
            get
            {
                return GetValue<bool>(CyParamNames.SCL_TIMEOUT_ENABLED);
            }
        }

        public int TimeOut
        {
            get
            {
                return GetValue<int>(CyParamNames.TIME_OUT);
            }
        }
        #endregion

        #region Getting Parameters
        private T GetValue<T>(string paramName)
        {
            T value;
            CyCustErr err = m_inst.GetCommittedParam(paramName).TryGetValueAs<T>(out value);
            if (err != null && err.IsOK)
            {
                return value;
            }
            else
            {
                return default(T);
            }
        }

        public void LoadParameters()
        {
            m_bGlobalEditMode = false;
            m_basicTab.UpdateUI();
            m_advancedConfigTab.UpdateUI();
            m_bGlobalEditMode = true;
        }
        #endregion

        #region Setting Parameters
        private void SetValue<T>(string paramName, T value)
        {
            if (m_bGlobalEditMode)
            {
                string valueToSet = value.ToString();
                if (value is bool)
                    valueToSet = valueToSet.ToLower();
                if (m_inst.SetParamExpr(paramName, valueToSet.ToString()))
                {
                    m_inst.CommitParamExprs();
                }
            }
        }
        #endregion

        public void UpdateClock(ICyInstQuery_v1 edit, ICyTerminalQuery_v1 termQuery)
        {
            m_extClock = CyClockReader.GetExternalClock(termQuery);
            m_basicTab.UpdateCalculator(m_extClock);
        }
    }
}
