/*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Data;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;

namespace I2C_v3_50
{
    public partial class CyI2CBasic : UserControl, ICyParamEditingControl
    {
        #region Declaring private members
        private CyI2CParameters m_params;
        private bool m_nonNumberEntered = false;
        private ICyDesignQuery_v1 m_designQuery;
        private ICyInstEdit_v1 m_instEdit;
        private ICyTerminalQuery_v1 m_termQuery;
        private string m_previousMinusTolerance;
        private string m_previousPlusTolerance;

        private const string PERCENT_SIGN = "%";
        #endregion

        #region Constructor(s)
        public CyI2CBasic(CyI2CParameters inst, ICyInstEdit_v1 edit, ICyTerminalQuery_v1 termQuery)
        {
            InitializeComponent();

            inst.m_basicTab = this;
            m_designQuery = edit.DesignQuery;
            m_instEdit = edit;
            m_termQuery = termQuery;
            this.Dock = DockStyle.Fill;
            m_params = inst;
            // Assigning ModeType possible values to ComboBox
            cbMode.DataSource = m_params.m_modesList;
            chbWakeup.Enabled = rbFixedFunction.Checked;
        }
        #endregion

        #region ICyParamEditingControl Members
        public Control DisplayControl
        {
            get { return this; }
        }

        public IEnumerable<CyCustErr> GetErrors()
        {
            List<CyCustErr> errs = new List<CyCustErr>();

            foreach (string paramName in m_params.m_inst.GetParamNames())
            {
                CyCompDevParam param = m_params.m_inst.GetCommittedParam(paramName);
                if (param.TabName.Equals(CyCustomizer.BASIC_TAB_NAME))
                {
                    if (param.ErrorCount > 0)
                    {
                        foreach (string errMsg in param.Errors)
                        {
                            errs.Add(new CyCustErr(errMsg));
                        }
                    }
                }
            }

            return errs;
        }
        #endregion

        #region Assigning parameters values to controls
        public void UpdateUI()
        {
            // I2C_Mode
            cbMode.SelectedItem = CyDictParser.GetDictValue(m_params.m_dnDict, m_params.Mode);

            // BusSpeed
            cbDataRate.Text = m_params.DataRate.ToString();

            // Implementation
            switch (m_params.Implementation)
            {
                case CyEImplementationType.UDB:
                    rbUDB.Checked = true;
                    break;
                case CyEImplementationType.FixedFunction:
                    rbFixedFunction.Checked = true;
                    break;
                default:
                    break;
            }
            // AddressDecode
            switch (m_params.AddressDecode)
            {
                case CyEAddressDecodeType.Software:
                    rbSoftware.Checked = true;
                    break;
                case CyEAddressDecodeType.Hardware:
                    rbHardware.Checked = true;
                    break;
                default:
                    break;
            }

            // SlaveAddress
            if (m_params.Hex)
                tbSlaveAddress.Text = CyHexConvertor.IntToHex(m_params.SlaveAddress);
            else
                tbSlaveAddress.Text = m_params.SlaveAddress.ToString();

            // EnableWakeup
            chbWakeup.Checked = m_params.EnableWakeup;

            // I2CBusPort
            switch (m_params.BusPort)
            {
                case CyEBusPortType.Any:
                    rbAny.Checked = true;
                    break;
                case CyEBusPortType.I2C0:
                    rbI2C0.Checked = true;
                    break;
                case CyEBusPortType.I2C1:
                    rbI2C1.Checked = true;
                    break;
                default:
                    break;
            }

            // InternalClock
            if (m_params.UdbInternalClock)
                rbInternalClock.Checked = true;
            else
                rbExternalClock.Checked = true;

            // Tolerance
            tbMinusTolerance.Text = m_params.MinusTolerance.ToString() + PERCENT_SIGN;
            m_previousMinusTolerance = tbMinusTolerance.Text;
            tbPlusTolerance.Text = m_params.PlusTolerance.ToString() + PERCENT_SIGN;
            m_previousPlusTolerance = tbPlusTolerance.Text;

            // Fixed Placement
            chbFixedPlacement.Checked = m_params.UdbSlaveFixedPlacementEnable;

            SetWakeupEnabling();
            SetToleranceEnabling();
        }
        #endregion

        #region Calculator update
        // Updates Input Frequency Based Calculations
        public void UpdateCalculator(CyClockData clock)
        {
            const string DATA_RATE_UNITS = " kbps";

            if (m_params.m_bGlobalEditMode)
            {
                // This clock depends on implementation - BusClock in FF and Internal in UDB
                CyClockData internalClock = CyClockReader.GetInternalClock(m_termQuery, m_params.Implementation);
                double internalClockFreqInKHz = CyClockReader.ConvertFreqToKHz(internalClock);

                if (m_params.Implementation == CyEImplementationType.UDB)
                {
                    if (rbInternalClock.Checked)
                    {
                        // Actual Data Rate Calculation
                        if (internalClockFreqInKHz > 0)
                            lblCalcDataRateValue.Text = Math.Round((internalClockFreqInKHz / 16), 2).ToString()
                                + DATA_RATE_UNITS;
                        else
                            lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                    }
                    else
                    {
                        try
                        {
                            if (clock.IsFrequencyKnown)
                            {
                                // Actual Data Rate Calculation
                                double externalClockFreqInKHz = CyClockReader.ConvertFreqToKHz(clock);
                                lblCalcDataRateValue.Text = Math.Round((externalClockFreqInKHz / 16), 2).ToString()
                                    + DATA_RATE_UNITS;
                            }
                            else
                            {
                                lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                            }
                        }
                        catch (Exception)
                        {
                            lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                        }
                    }
                }
                else
                {
                    if (internalClock.IsFrequencyKnown)
                    {
                        // Actual Data Rate Calculation
                        try
                        {
                            byte divider = 0;
                            string deviceArchMember = m_instEdit.DeviceQuery.ArchFamilyMember;

                            divider = CyDividerCalculator.GetES3Divider(m_instEdit, m_termQuery);

                            double oversampleRate = (m_params.DataRate <= 50) ? 32 : 16;
                            lblCalcDataRateValue.Text = Math.Round((internalClockFreqInKHz /
                                (divider * oversampleRate)), 2).ToString() + DATA_RATE_UNITS;
                        }
                        catch (Exception)
                        {
                            lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                        }
                    }
                    else
                    {
                        lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                    }
                }
            }
        }
        #endregion

        #region Event Handlers
        private void cbMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbMode.SelectedItem.ToString() == CyDictParser.GetDictValue(m_params.m_dnDict, CyEModeType.Slave)
                || cbMode.SelectedItem.ToString() == CyDictParser.GetDictValue(m_params.m_dnDict,
                CyEModeType.MultiMaster_Slave_revA))
            {
                panelAddressDecode.Enabled = true;
                lblSlaveAddress.Enabled = true;
                tbSlaveAddress.Enabled = true;
                lblHexademical.Enabled = true;
            }
            else
            {
                panelAddressDecode.Enabled = false;
                lblSlaveAddress.Enabled = false;
                tbSlaveAddress.Enabled = false;
                lblHexademical.Enabled = false;
            }
            // Set Mode value
            m_params.Mode = (CyEModeType)CyDictParser.GetDictKey(m_params.m_dnDict, cbMode.SelectedItem.ToString());
            SetWakeupEnabling();
            SetPinSelectionsEnabling();
            SetFixedPlacementEnabling();

            // Update Tolerance for Mode
            m_previousMinusTolerance = m_params.MinusTolerance.ToString() + PERCENT_SIGN;
            m_previousPlusTolerance = m_params.PlusTolerance.ToString() + PERCENT_SIGN;

            tbMinusTolerance.Text = m_previousMinusTolerance;
            tbPlusTolerance.Text = m_previousPlusTolerance;


            UpdateCalculator(m_params.m_extClock);

            // Fixed Placement error provider
            SetFixedPlacementErrorProvider();
            // EnableWakeup error provider
            SetEnableWakeupErrorProvider();
        }

        private void cbDataRate_TextChanged(object sender, EventArgs e)
        {
            CancelEventArgs ce = new CancelEventArgs();
            cbDataRate_Validating(sender, ce);
            if (ce.Cancel == false)
            {
                m_params.DataRate = UInt16.Parse(cbDataRate.Text);
                if (m_params.Implementation == CyEImplementationType.UDB && m_params.UdbInternalClock)
                {
                    lblCalcDataRateValue.Text = Resource.UnknownActualDataRate;
                }
                else
                {
                    UpdateCalculator(m_params.m_extClock);
                }
            }
        }

        private void rbFixedFunction_CheckedChanged(object sender, EventArgs e)
        {
            SetDataRateEnabling();
            m_params.Implementation = rbUDB.Checked ? CyEImplementationType.UDB : CyEImplementationType.FixedFunction;
            SetWakeupEnabling();
            UpdateCalculator(m_params.m_extClock);
            panelUDBClockSource.Enabled = rbUDB.Checked;
            SetPinSelectionsEnabling();
            SetToleranceEnabling();
            SetFixedPlacementEnabling();
            // Fixed Placement error provider
            SetFixedPlacementErrorProvider();
            // EnableWakeup error provider
            SetEnableWakeupErrorProvider();
            // Validating Tolerance
            tbTolerance_TextChanged(null, e);
        }

        private void rbHardware_CheckedChanged(object sender, EventArgs e)
        {
            m_params.AddressDecode = rbHardware.Checked ? CyEAddressDecodeType.Hardware : CyEAddressDecodeType.Software;
            SetWakeupEnabling();
            // EnableWakeup error provider
            SetEnableWakeupErrorProvider();
        }

        private void tbSlaveAddress_TextChanged(object sender, EventArgs e)
        {
            CancelEventArgs ce = new CancelEventArgs();
            tbSlaveAddress_Validating(sender, ce);
            if (ce.Cancel == false)
            {
                bool isHex = tbSlaveAddress.Text.StartsWith(CyHexConvertor.HEX_PREFIX);

                if (isHex)
                    m_params.SlaveAddress = (UInt16)CyHexConvertor.HexToInt(tbSlaveAddress.Text);
                else
                    m_params.SlaveAddress = UInt16.Parse(tbSlaveAddress.Text);

                if (isHex)
                    m_params.Hex = true;
                else
                    m_params.Hex = false;
            }
        }

        private void tbTolerance_TextChanged(object sender, EventArgs e)
        {
            string minusToleranceWithoutPercent = tbMinusTolerance.Text.Replace(PERCENT_SIGN, string.Empty);
            string plusToleranceWithoutPercent = tbPlusTolerance.Text.Replace(PERCENT_SIGN, string.Empty);

            double minusToleranceValue;
            double plusToleranceValue;

            if (double.TryParse(minusToleranceWithoutPercent, out minusToleranceValue) &&
                double.TryParse(plusToleranceWithoutPercent, out plusToleranceValue))
            {
                minusToleranceValue *= -1;

                double min = 0;
                double max = 0;

                if (m_params.Mode == CyEModeType.Slave)
                {
                    min = CyParamRange.SLAVE_TOLERANCE_MIN;
                    max = CyParamRange.SLAVE_TOLERANCE_MAX;
                }
                else
                {
                    min = CyParamRange.NOT_SLAVE_TOLERANCE_MIN;
                    max = CyParamRange.NOT_SLAVE_TOLERANCE_MAX;
                }
                if ((minusToleranceValue < min || minusToleranceValue > max || plusToleranceValue < min
                    || plusToleranceValue > max) && m_params.Implementation == CyEImplementationType.UDB
                    && m_params.UdbInternalClock == true)
                    errorProvider.SetError(tbMinusTolerance, string.Format(Resource.ToleranceValidator, min, max));
                else
                    errorProvider.SetError(tbMinusTolerance, string.Empty);
            }
            if (m_params.m_bGlobalEditMode)
            {
                try
                {
                    m_params.MinusTolerance = double.Parse(minusToleranceWithoutPercent);
                    m_params.PlusTolerance = double.Parse(plusToleranceWithoutPercent);
                }
                catch
                {
                    Debug.Assert(false);
                }
            }
        }

        private void checkBoxWakeup_CheckedChanged(object sender, EventArgs e)
        {
            m_params.EnableWakeup = chbWakeup.Checked;
            SetEnableWakeupErrorProvider();
        }

        private void rbPinConnections_CheckedChanged(object sender, EventArgs e)
        {
            if (rbAny.Checked)
                m_params.BusPort = CyEBusPortType.Any;
            else if (rbI2C0.Checked)
                m_params.BusPort = CyEBusPortType.I2C0;
            else
                m_params.BusPort = CyEBusPortType.I2C1;
            SetWakeupEnabling();
            SetEnableWakeupErrorProvider();
        }

        private void tbSlaveAddress_Validating(object sender, CancelEventArgs e)
        {
            int value = 0;
            string message = string.Format(Resource.SlaveAddressValidator, CyParamRange.SLAVE_ADDRESS_MIN,
                CyParamRange.SLAVE_ADDRESS_MAX);
            bool isHex = tbSlaveAddress.Text.StartsWith(CyHexConvertor.HEX_PREFIX);

            if (int.TryParse(tbSlaveAddress.Text, out value) || isHex)
            {
                if (isHex) value = CyHexConvertor.HexToInt(tbSlaveAddress.Text);
                if (value < CyParamRange.SLAVE_ADDRESS_MIN || value > CyParamRange.SLAVE_ADDRESS_MAX)
                {
                    errorProvider.SetError((Control)sender, string.Format(message));
                    e.Cancel = true;
                }
                else
                {
                    errorProvider.SetError((Control)sender, string.Empty);
                }
            }
            else
            {
                errorProvider.SetError((Control)sender, message);
                e.Cancel = true;
            }
        }

        private void cbDataRate_Validating(object sender, CancelEventArgs e)
        {
            if (sender == cbDataRate)
            {
                int value = 0;
                string message = string.Format(Resource.I2CDataRateValidator, CyParamRange.DATA_RATE_MIN,
                    CyParamRange.DATA_RATE_MAX);
                if (int.TryParse(cbDataRate.Text, out value))
                {
                    if (value < CyParamRange.DATA_RATE_MIN || value > CyParamRange.DATA_RATE_MAX)
                    {
                        errorProvider.SetError((Control)sender, string.Format(message));
                        e.Cancel = true;
                    }
                    else
                    {
                        errorProvider.SetError((Control)sender, string.Empty);
                    }
                }
                else
                {
                    errorProvider.SetError((Control)sender, message);
                    e.Cancel = true;
                }
            }
        }

        private void tbTolerance_Validating(object sender, CancelEventArgs e)
        {
            TextBox currentTextBox = (TextBox)sender;
            string previousTolerance = (currentTextBox == tbMinusTolerance) ? m_previousMinusTolerance
                : m_previousPlusTolerance;
            double val = 0;

            currentTextBox.Text = currentTextBox.Text.Trim();
            Match match = Regex.Match(currentTextBox.Text, @"([0-9]+\.{0,1}[0-9]*\%{0,1})");
            if (match.Groups[0].Value == currentTextBox.Text)
            {
                string tbTextWithoutPercentSign = currentTextBox.Text.Replace(PERCENT_SIGN, string.Empty);
                if (double.TryParse(tbTextWithoutPercentSign, out val))
                {
                    if (currentTextBox.Text.Contains(PERCENT_SIGN) == false)
                        currentTextBox.Text += PERCENT_SIGN;
                    if (currentTextBox == tbMinusTolerance)
                        m_previousMinusTolerance = currentTextBox.Text;
                    else
                        m_previousPlusTolerance = currentTextBox.Text;
                }
                else
                {
                    currentTextBox.Text = previousTolerance;
                }
            }
            else
            {
                currentTextBox.Text = previousTolerance;
            }
        }

        private void EditBox_KeyDown(object sender, KeyEventArgs e)
        {
            ValidateInput(e, sender);
        }

        private void EditBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (m_nonNumberEntered) e.Handled = true;
        }

        private void CyI2CBasic_Load(object sender, EventArgs e)
        {
            // Update controls
            UpdateCalculator(m_params.m_extClock);
            SetDataRateEnabling();
            SetWakeupEnabling();

            // Set UDB for PSoC 4 device
            if (m_instEdit.DeviceQuery.IsPSoC4)
            {
                rbFixedFunction.Visible = false;
                rbUDB.Left = rbFixedFunction.Left;
                rbUDB.Top = rbFixedFunction.Top;
                rbUDB.Checked = true;
                rbUDB.Enabled = false;
                chbWakeup.Checked = false;
                chbWakeup.Enabled = false;
            }
        }

        private void rbInternalClock_CheckedChanged(object sender, EventArgs e)
        {
            m_params.UdbInternalClock = rbInternalClock.Checked;
            SetDataRateEnabling();
            SetToleranceEnabling();
            tbTolerance_TextChanged(null, e);
        }

        private void chbFixedPlacement_CheckedChanged(object sender, EventArgs e)
        {
            m_params.UdbSlaveFixedPlacementEnable = ((CheckBox)sender).Checked;
            SetFixedPlacementErrorProvider();
        }
        #endregion

        #region Input validators
        private void ValidateInput(KeyEventArgs e, object sender)
        {
            m_nonNumberEntered = false;
            TextBox currentTextBox = new TextBox();
            ComboBox currentComboBox = new ComboBox();

            if (sender.GetType().Equals(typeof(TextBox)))
                currentTextBox = (TextBox)sender;
            if (sender.GetType().Equals(typeof(ComboBox)))
                currentComboBox = (ComboBox)sender;

            if (e.KeyCode != Keys.X && e.KeyCode != Keys.A && e.KeyCode != Keys.B &&
                e.KeyCode != Keys.C && e.KeyCode != Keys.D && e.KeyCode != Keys.E &&
                e.KeyCode != Keys.F)
            {
                ValidateKey(e);
            }
            else
            {
                if (sender.GetType().Equals(typeof(TextBox)))
                {
                    if (e.KeyCode == Keys.X && currentTextBox.Text.Contains("x"))
                        m_nonNumberEntered = true;
                    if (e.KeyCode == Keys.X && !currentTextBox.Text.StartsWith("0"))
                        m_nonNumberEntered = true;
                    if ((e.KeyCode == Keys.A || e.KeyCode == Keys.B || e.KeyCode == Keys.C ||
                        e.KeyCode == Keys.D || e.KeyCode == Keys.E || e.KeyCode == Keys.F) &&
                            (!currentTextBox.Text.StartsWith("0x")))
                    {
                        m_nonNumberEntered = true;
                    }
                    if (Control.ModifierKeys == Keys.Shift && e.KeyCode == Keys.X)
                        m_nonNumberEntered = true;
                }
                else
                {
                    ValidateKey(e);
                }
            }
        }

        // Validates whether a number key or other has been pressed.
        private void ValidateKey(KeyEventArgs e)
        {
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                {
                    if (e.KeyCode != Keys.Back)
                    {
                        m_nonNumberEntered = true;
                    }
                }
            }
            // Not a number, because shift pressed
            if (Control.ModifierKeys == Keys.Shift)
            {
                m_nonNumberEntered = true;
            }
        }
        #endregion

        #region Enabling/Disabling controls and activating Error Provider
        private void SetWakeupEnabling()
        {
            if (m_params.EnableWakeup == false && (m_params.BusPort == CyEBusPortType.Any
                || m_params.Implementation == CyEImplementationType.UDB
                || m_params.AddressDecode == CyEAddressDecodeType.Software
                || (m_params.Mode == CyEModeType.Master || m_params.Mode == CyEModeType.MultiMaster_revA)))
            {
                chbWakeup.Enabled = false;
            }
            else
            {
                chbWakeup.Enabled = true;
            }
        }

        private void SetEnableWakeupErrorProvider()
        {
            if (m_params.EnableWakeup
                && (m_params.BusPort == CyEBusPortType.Any
                || m_params.Implementation == CyEImplementationType.UDB
                || m_params.AddressDecode == CyEAddressDecodeType.Software
                || (m_params.Mode == CyEModeType.Master || m_params.Mode == CyEModeType.MultiMaster_revA)))
            {
                errorProvider.SetError(chbWakeup, Resource.EnableWakeupValidator);
            }
            else
            {
                errorProvider.SetError(chbWakeup, string.Empty);
            }
        }

        private void SetPinSelectionsEnabling()
        {
            bool enabling = ((cbMode.SelectedItem == (object)CyDictParser.GetDictValue(m_params.m_dnDict,
                CyEModeType.Slave) || cbMode.SelectedItem == (object)CyDictParser.GetDictValue(m_params.m_dnDict,
                CyEModeType.MultiMaster_Slave_revA)) && rbFixedFunction.Checked);
            panelPinConnections.Enabled = enabling;
        }

        private void SetDataRateEnabling()
        {
            bool enabling = (rbFixedFunction.Checked || rbInternalClock.Checked);
            lblDataRate.Enabled = enabling;
            cbDataRate.Enabled = enabling;
        }

        private void SetToleranceEnabling()
        {
            bool enabling = (m_params.Implementation == CyEImplementationType.UDB && m_params.UdbInternalClock);
            lblTolerance.Enabled = enabling;
            tbMinusTolerance.Enabled = enabling;
            tbPlusTolerance.Enabled = enabling;
        }

        private void SetFixedPlacementEnabling()
        {
            //chbFixedPlacement.Enabled = false;
            if (m_params.UdbSlaveFixedPlacementEnable == false
                && (m_params.Implementation == CyEImplementationType.FixedFunction
                || m_params.Mode != CyEModeType.Slave))
            {
                chbFixedPlacement.Enabled = false;
            }
            else
            {
                chbFixedPlacement.Enabled = true;
            }
        }

        private void SetFixedPlacementErrorProvider()
        {
            if (m_params.UdbSlaveFixedPlacementEnable
                && (m_params.Implementation != CyEImplementationType.UDB || m_params.Mode != CyEModeType.Slave))
            {
                errorProvider.SetError(chbFixedPlacement, Resource.FixedPlacementValidator);
            }
            else
            {
                errorProvider.SetError(chbFixedPlacement, string.Empty);
            }
        }
        #endregion
    }
}
