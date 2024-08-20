namespace I2C_v3_50
{
    partial class CyAdvancedConfTab
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.chbExternalOEBuffer = new System.Windows.Forms.CheckBox();
            this.lblExternalIOBufferDesc = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // chbExternalOEBuffer
            // 
            this.chbExternalOEBuffer.AutoSize = true;
            this.chbExternalOEBuffer.Location = new System.Drawing.Point(9, 15);
            this.chbExternalOEBuffer.Name = "chbExternalOEBuffer";
            this.chbExternalOEBuffer.Size = new System.Drawing.Size(112, 17);
            this.chbExternalOEBuffer.TabIndex = 0;
            this.chbExternalOEBuffer.Text = "External OE buffer";
            this.chbExternalOEBuffer.UseVisualStyleBackColor = true;
            this.chbExternalOEBuffer.CheckedChanged += new System.EventHandler(this.chbExternalIOBuffer_CheckedChanged);
            // 
            // lblExternalIOBufferDesc
            // 
            this.lblExternalIOBufferDesc.AutoSize = true;
            this.lblExternalIOBufferDesc.Location = new System.Drawing.Point(25, 38);
            this.lblExternalIOBufferDesc.Name = "lblExternalIOBufferDesc";
            this.lblExternalIOBufferDesc.Size = new System.Drawing.Size(35, 13);
            this.lblExternalIOBufferDesc.TabIndex = 1;
            this.lblExternalIOBufferDesc.Text = "label1";
            // 
            // CyAdvancedConfTab
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lblExternalIOBufferDesc);
            this.Controls.Add(this.chbExternalOEBuffer);
            this.Name = "CyAdvancedConfTab";
            this.Size = new System.Drawing.Size(699, 488);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox chbExternalOEBuffer;
        private System.Windows.Forms.Label lblExternalIOBufferDesc;
    }
}
