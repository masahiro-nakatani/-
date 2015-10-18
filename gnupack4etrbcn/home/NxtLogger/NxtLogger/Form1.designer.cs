namespace NxtLogger
{
    partial class Form1
    {
        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナで生成されたコード

        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.btnQuit = new System.Windows.Forms.Button();
            this.chkConnect = new System.Windows.Forms.CheckBox();
            this.boxDataView = new System.Windows.Forms.TextBox();
            this.portNoBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.labelLogFile = new System.Windows.Forms.Label();
            this.textLogFile = new System.Windows.Forms.TextBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.labelCTS = new System.Windows.Forms.Label();
            this.labelDSR = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnQuit
            // 
            resources.ApplyResources(this.btnQuit, "btnQuit");
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.UseVisualStyleBackColor = true;
            this.btnQuit.Click += new System.EventHandler(this.BtnQuitClick);
            // 
            // chkConnect
            // 
            resources.ApplyResources(this.chkConnect, "chkConnect");
            this.chkConnect.Name = "chkConnect";
            this.chkConnect.UseVisualStyleBackColor = true;
            this.chkConnect.CheckedChanged += new System.EventHandler(this.ChkConnectCheckedChanged);
            // 
            // boxDataView
            // 
            this.boxDataView.AcceptsReturn = true;
            this.boxDataView.BackColor = System.Drawing.SystemColors.HighlightText;
            resources.ApplyResources(this.boxDataView, "boxDataView");
            this.boxDataView.Name = "boxDataView";
            this.boxDataView.ReadOnly = true;
            // 
            // portNoBox
            // 
            this.portNoBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.portNoBox.FormattingEnabled = true;
            resources.ApplyResources(this.portNoBox, "portNoBox");
            this.portNoBox.Name = "portNoBox";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // labelLogFile
            // 
            resources.ApplyResources(this.labelLogFile, "labelLogFile");
            this.labelLogFile.Name = "labelLogFile";
            // 
            // textLogFile
            // 
            this.textLogFile.BackColor = System.Drawing.SystemColors.HighlightText;
            resources.ApplyResources(this.textLogFile, "textLogFile");
            this.textLogFile.Name = "textLogFile";
            this.textLogFile.ReadOnly = true;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.Timer1Tick);
            // 
            // labelCTS
            // 
            resources.ApplyResources(this.labelCTS, "labelCTS");
            this.labelCTS.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.labelCTS.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelCTS.Name = "labelCTS";
            // 
            // labelDSR
            // 
            resources.ApplyResources(this.labelDSR, "labelDSR");
            this.labelDSR.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.labelDSR.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelDSR.Name = "labelDSR";
            // 
            // Form1
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.labelDSR);
            this.Controls.Add(this.labelCTS);
            this.Controls.Add(this.textLogFile);
            this.Controls.Add(this.labelLogFile);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.portNoBox);
            this.Controls.Add(this.boxDataView);
            this.Controls.Add(this.chkConnect);
            this.Controls.Add(this.btnQuit);
            this.Name = "Form1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1FormClosed);
            this.Load += new System.EventHandler(this.Form1Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnQuit;
        private System.Windows.Forms.CheckBox chkConnect;
        private System.Windows.Forms.TextBox boxDataView;
        private System.Windows.Forms.ComboBox portNoBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label labelLogFile;
        private System.Windows.Forms.TextBox textLogFile;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label labelCTS;
        private System.Windows.Forms.Label labelDSR;
    }
}

