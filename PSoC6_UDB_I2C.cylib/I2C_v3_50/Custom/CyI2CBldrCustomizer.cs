/*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using CyDesigner.Extensions.Common;
using CyDesigner.Extensions.Gde;
using System.Windows.Forms;
using System.IO;
using System.Collections;

namespace I2C_v3_50
{
    partial class CyCustomizer : ICyBootLoaderSupport
    {
        #region ICyBootLoaderSupport Members
        /// <summary>
        /// The bootloader requires that the communication component is configured for
        /// both transfer in and out of the PSoC device. This method lets the implementing
        /// component inform PSoC Creator if it is currently configured to handle input and
        /// output.
        /// <param name="query">The ICyInstQuery for the relevant instance of the component.</param>
        /// </summary>
        public CyCustErr IsBootloaderReady(ICyInstQuery_v1 query)
        {
            Debug.Assert(query != null);
            CyEModeType mode = (CyEModeType)byte.Parse(query.GetCommittedParam(CyParamNames.MODE).Value);
            CyCustErr supportBootloader = CyCustErr.OK;
            if (mode == CyEModeType.Master || mode == CyEModeType.MultiMaster_revA)
            {
                supportBootloader = new CyCustErr(Resource.I2CNotContainSlave, query.InstanceName);
            }
            return supportBootloader;
        }
        #endregion
    }
}
