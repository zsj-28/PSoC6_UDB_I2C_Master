/*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
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
using System.Runtime.InteropServices;

namespace I2C_v3_50
{
    [CyCompDevCustomizer()]
    public partial class CyCustomizer : ICyAPICustomize_v1, ICyParamEditHook_v1, ICyDRCProvider_v1
    {
        public const string BASIC_TAB_NAME = "Basic Configuration";
        public const string ADVANCED_CONFIGURATION_TAB_NAME = "Advanced Configuration";
        public const string PSOC5A = "PSoC5A";

        #region ICyAPICustomize_v1 Members
        public IEnumerable<CyAPICustomizer> CustomizeAPIs(ICyInstQuery_v1 query, ICyTerminalQuery_v1 termQuery,
            IEnumerable<CyAPICustomizer> apis)
        {
            byte clkDiv = 0;
            byte byteDefaultClockDiv = 0;

            List<CyAPICustomizer> customizers = new List<CyAPICustomizer>(apis);
            Dictionary<string, string> paramDict = new Dictionary<string, string>();

            if (customizers.Count > 0) paramDict = customizers[0].MacroDictionary;

            CyI2CParameters parameters = new CyI2CParameters(query as ICyInstEdit_v1, termQuery);

            // Remove TMOUT APIs
            if ((parameters.SdaTimeoutEnabled == false) && (parameters.SclTimeoutEnabled == false))
            {
                List<int> itemsToRemove = new List<int>();
                for (int i = 0; i < customizers.Count; i++)
                {
                    if (customizers[i].OutputName.Equals(parameters.m_instanceName + "_TMOUT.c") ||
                        customizers[i].OutputName.Equals(parameters.m_instanceName + "_TMOUT_INT.c") ||
                        customizers[i].OutputName.Equals(parameters.m_instanceName + "_TMOUT.h"))
                    {
                        itemsToRemove.Add(i);
                    }
                }

                for (int i = itemsToRemove.Count - 1; i >= 0; i--)
                {
                    customizers.RemoveAt(itemsToRemove[i]);
                }
            }

            clkDiv = CyDividerCalculator.GetES2Divider(query, termQuery);
            byteDefaultClockDiv = CyDividerCalculator.GetES3Divider(query, termQuery);

            paramDict.Add("ClkDiv", clkDiv.ToString());
            paramDict.Add("ClkDiv1", byteDefaultClockDiv.ToString());
            for (int i = 0; i < customizers.Count; i++)
            {
                customizers[i].MacroDictionary = paramDict;
            }

            return customizers;
        }
        #endregion

        #region ICyParamEditHook_v1 Members
        public DialogResult EditParams(ICyInstEdit_v1 edit, ICyTerminalQuery_v1 termQuery, ICyExpressMgr_v1 mgr)
        {
            CyI2CParameters parameters = new CyI2CParameters(edit, termQuery);

            ICyTabbedParamEditor editor = edit.CreateTabbedParamEditor();

            CyParamExprDelegate configureExpressionViewDataChanged =
            delegate(ICyParamEditor custEditor, CyCompDevParam param)
            {
                parameters.LoadParameters();
            };

            editor.AddCustomPage(Resource.TabBasicConfDisplayName, new CyI2CBasic(parameters, edit, termQuery),
                configureExpressionViewDataChanged, BASIC_TAB_NAME);
            editor.AddCustomPage(Resource.TabAdvancedConfigDisplayName, new CyAdvancedConfTab(parameters),
                configureExpressionViewDataChanged, ADVANCED_CONFIGURATION_TAB_NAME);
            editor.AddDefaultPage(Resource.TabBuiltInDisplayName, "Built-in");
            edit.NotifyWhenDesignUpdates(new CyDesignUpdated_v1(parameters.UpdateClock));

            parameters.LoadParameters();

            parameters.m_bGlobalEditMode = true;
            return editor.ShowDialog();
        }

        public bool EditParamsOnDrop
        {
            get
            {
                return false;
            }
        }

        public CyCompDevParamEditorMode GetEditorMode()
        {
            return CyCompDevParamEditorMode.COMPLETE;
        }
        #endregion

        #region ICyDRCProvider_v1 Members
        public IEnumerable<CyDRCInfo_v1> GetDRCs(ICyDRCProviderArgs_v1 args)
        {
            List<CyCustErr> err = new List<CyCustErr>();
            err.Add(VerifyImplementation(args.InstQueryV1));
            for (int i = 0; i < err.Count; i++)
            {
                if (err[i].IsOk == false)
                    yield return new CyDRCInfo_v1(CyDRCInfo_v1.CyDRCType_v1.Error, err[i].Message);
            }
        }

        CyCustErr VerifyImplementation(ICyInstQuery_v1 instQuery)
        {
            if (instQuery.DeviceQuery.IsPSoC4)
            {
                CyEImplementationType fixedFunction;
                try
                {
                    fixedFunction = (CyEImplementationType)Convert.ToByte(instQuery.GetCommittedParam(
                        CyParamNames.IMPLEMENTATION).Value);
                }
                catch (Exception)
                {
                    fixedFunction = CyEImplementationType.UDB;
                }

                if (fixedFunction == CyEImplementationType.FixedFunction)
                    return new CyCustErr(Resource.ImplementationDrcError);
            }
            return CyCustErr.OK;
        }
        #endregion
    }
}
