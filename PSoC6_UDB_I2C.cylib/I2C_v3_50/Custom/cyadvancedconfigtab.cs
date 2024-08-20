/*******************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace I2C_v3_50
{
    public partial class CyAdvancedConfTab : UserControl, ICyParamEditingControl
    {
        CyI2CParameters m_params;

        #region Constructor(s)
        public CyAdvancedConfTab(CyI2CParameters parameters)
        {
            m_params = parameters;
            m_params.m_advancedConfigTab = this;
            InitializeComponent();
            this.Dock = DockStyle.Fill;
            lblExternalIOBufferDesc.Text = Resource.ExternalIODescription;
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
                if (param.TabName.Equals(CyCustomizer.ADVANCED_CONFIGURATION_TAB_NAME))
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
            chbExternalOEBuffer.Checked = m_params.ExternalBuffer;
        }
        #endregion

        private void chbExternalIOBuffer_CheckedChanged(object sender, EventArgs e)
        {
            m_params.ExternalBuffer = chbExternalOEBuffer.Checked;
        }
    }
}
