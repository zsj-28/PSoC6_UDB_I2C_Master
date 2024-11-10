/*******************************************************************************
* Copyright 2011-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.ComponentModel;

namespace I2C_v3_50
{
    static class CyDictParser
    {
        /// <summary>
        /// Gets dictionary key by value
        /// </summary>
        public static object GetDictKey(Dictionary<object, string> dict, string value)
        {
            foreach (object key in dict.Keys)
            {
                if (dict[key] == value)
                {
                    return key;
                }
            }
            return null;
        }

        /// <summary>
        /// Gets dictionary value by key
        /// </summary>
        public static string GetDictValue(Dictionary<object, string> dict, object value)
        {
            string newValue = string.Empty;
            dict.TryGetValue(value, out newValue);
            return newValue;
        }

        /// <summary>
        /// Fills dictionary object with corresponding values from list
        /// </summary>
        public static void FillDictionary(ref Dictionary<object, string> dict, Type type, List<string> list)
        {
            int i = 0;
            foreach (System.Reflection.FieldInfo item in type.GetFields())
            {
                if (item.FieldType == type)
                {
                    object obj = Enum.Parse(type, item.Name);
                    dict.Add(obj, list[i++]);
                }
            }
        }
    }
}
