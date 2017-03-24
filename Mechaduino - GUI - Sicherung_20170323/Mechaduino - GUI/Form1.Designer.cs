namespace Mechaduino___GUI
{
    partial class Form1
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea5 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series7 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series8 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea6 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series9 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title4 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea7 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series10 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series11 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title5 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea8 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series12 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title6 = new System.Windows.Forms.DataVisualization.Charting.Title();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabConnection = new System.Windows.Forms.TabPage();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.txtSend = new System.Windows.Forms.TextBox();
            this.txtReceived = new System.Windows.Forms.TextBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.txtConsole = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.serial_box = new System.Windows.Forms.ComboBox();
            this.btnOpen = new System.Windows.Forms.Button();
            this.tabControl = new System.Windows.Forms.TabPage();
            this.tabPlots = new System.Windows.Forms.TabPage();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.panelEnabled = new System.Windows.Forms.Panel();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.panelTorque = new System.Windows.Forms.Panel();
            this.panelGreen = new System.Windows.Forms.Panel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnEnable = new System.Windows.Forms.Button();
            this.btnStream = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.anglePlot = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.pltError = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.pltPosition = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.pltCurrent = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.tabParameter = new System.Windows.Forms.TabPage();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.tabControl1.SuspendLayout();
            this.tabConnection.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.tabPlots.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.anglePlot)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pltError)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pltPosition)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pltCurrent)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabConnection);
            this.tabControl1.Controls.Add(this.tabControl);
            this.tabControl1.Controls.Add(this.tabPlots);
            this.tabControl1.Controls.Add(this.tabParameter);
            this.tabControl1.Location = new System.Drawing.Point(1, 3);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(2);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(556, 672);
            this.tabControl1.TabIndex = 6;
            // 
            // tabConnection
            // 
            this.tabConnection.Controls.Add(this.groupBox8);
            this.tabConnection.Controls.Add(this.groupBox7);
            this.tabConnection.Controls.Add(this.groupBox6);
            this.tabConnection.Location = new System.Drawing.Point(4, 22);
            this.tabConnection.Margin = new System.Windows.Forms.Padding(2);
            this.tabConnection.Name = "tabConnection";
            this.tabConnection.Padding = new System.Windows.Forms.Padding(2);
            this.tabConnection.Size = new System.Drawing.Size(548, 646);
            this.tabConnection.TabIndex = 0;
            this.tabConnection.Text = "Connection";
            this.tabConnection.UseVisualStyleBackColor = true;
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.btnSend);
            this.groupBox8.Controls.Add(this.txtSend);
            this.groupBox8.Controls.Add(this.txtReceived);
            this.groupBox8.Location = new System.Drawing.Point(9, 126);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(533, 515);
            this.groupBox8.TabIndex = 19;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Serial Communication";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(444, 488);
            this.btnSend.Margin = new System.Windows.Forms.Padding(2);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(80, 21);
            this.btnSend.TabIndex = 17;
            this.btnSend.Text = "Send";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click_1);
            // 
            // txtSend
            // 
            this.txtSend.Location = new System.Drawing.Point(15, 464);
            this.txtSend.Margin = new System.Windows.Forms.Padding(2);
            this.txtSend.Name = "txtSend";
            this.txtSend.Size = new System.Drawing.Size(510, 20);
            this.txtSend.TabIndex = 16;
            // 
            // txtReceived
            // 
            this.txtReceived.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.txtReceived.Location = new System.Drawing.Point(14, 18);
            this.txtReceived.Margin = new System.Windows.Forms.Padding(2);
            this.txtReceived.Multiline = true;
            this.txtReceived.Name = "txtReceived";
            this.txtReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtReceived.Size = new System.Drawing.Size(510, 432);
            this.txtReceived.TabIndex = 13;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.txtConsole);
            this.groupBox7.Location = new System.Drawing.Point(9, 62);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(533, 58);
            this.groupBox7.TabIndex = 18;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Console";
            // 
            // txtConsole
            // 
            this.txtConsole.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.txtConsole.Location = new System.Drawing.Point(14, 18);
            this.txtConsole.Margin = new System.Windows.Forms.Padding(2);
            this.txtConsole.Name = "txtConsole";
            this.txtConsole.Size = new System.Drawing.Size(510, 23);
            this.txtConsole.TabIndex = 12;
            this.txtConsole.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.btnRefresh);
            this.groupBox6.Controls.Add(this.serial_box);
            this.groupBox6.Controls.Add(this.btnOpen);
            this.groupBox6.Location = new System.Drawing.Point(9, 5);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(533, 51);
            this.groupBox6.TabIndex = 17;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "COM Port";
            // 
            // btnRefresh
            // 
            this.btnRefresh.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btnRefresh.Location = new System.Drawing.Point(211, 18);
            this.btnRefresh.Margin = new System.Windows.Forms.Padding(2);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(80, 21);
            this.btnRefresh.TabIndex = 19;
            this.btnRefresh.Text = "Refresh";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // serial_box
            // 
            this.serial_box.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.serial_box.FormattingEnabled = true;
            this.serial_box.Location = new System.Drawing.Point(15, 18);
            this.serial_box.Margin = new System.Windows.Forms.Padding(2);
            this.serial_box.Name = "serial_box";
            this.serial_box.Size = new System.Drawing.Size(192, 21);
            this.serial_box.TabIndex = 18;
            // 
            // btnOpen
            // 
            this.btnOpen.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btnOpen.Location = new System.Drawing.Point(444, 18);
            this.btnOpen.Margin = new System.Windows.Forms.Padding(2);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(80, 21);
            this.btnOpen.TabIndex = 17;
            this.btnOpen.Text = "Open";
            this.btnOpen.UseVisualStyleBackColor = true;
            this.btnOpen.Click += new System.EventHandler(this.btnOpen_Click);
            // 
            // tabControl
            // 
            this.tabControl.Location = new System.Drawing.Point(4, 22);
            this.tabControl.Margin = new System.Windows.Forms.Padding(2);
            this.tabControl.Name = "tabControl";
            this.tabControl.Padding = new System.Windows.Forms.Padding(2);
            this.tabControl.Size = new System.Drawing.Size(548, 646);
            this.tabControl.TabIndex = 1;
            this.tabControl.Text = "Control";
            this.tabControl.UseVisualStyleBackColor = true;
            // 
            // tabPlots
            // 
            this.tabPlots.Controls.Add(this.groupBox5);
            this.tabPlots.Controls.Add(this.groupBox4);
            this.tabPlots.Controls.Add(this.groupBox3);
            this.tabPlots.Controls.Add(this.groupBox2);
            this.tabPlots.Controls.Add(this.groupBox1);
            this.tabPlots.Location = new System.Drawing.Point(4, 22);
            this.tabPlots.Margin = new System.Windows.Forms.Padding(2);
            this.tabPlots.Name = "tabPlots";
            this.tabPlots.Size = new System.Drawing.Size(548, 646);
            this.tabPlots.TabIndex = 2;
            this.tabPlots.Text = "Plots";
            this.tabPlots.UseVisualStyleBackColor = true;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.panelEnabled);
            this.groupBox5.Location = new System.Drawing.Point(385, 55);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(155, 93);
            this.groupBox5.TabIndex = 24;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "State";
            // 
            // panelEnabled
            // 
            this.panelEnabled.BackColor = System.Drawing.Color.Red;
            this.panelEnabled.Location = new System.Drawing.Point(50, 20);
            this.panelEnabled.Name = "panelEnabled";
            this.panelEnabled.Size = new System.Drawing.Size(60, 60);
            this.panelEnabled.TabIndex = 1;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Controls.Add(this.panelTorque);
            this.groupBox4.Controls.Add(this.panelGreen);
            this.groupBox4.Location = new System.Drawing.Point(285, 55);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(95, 223);
            this.groupBox4.TabIndex = 23;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Torque";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(55, 107);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(33, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "- 50%";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(55, 200);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(27, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "- 0%";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(55, 15);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(39, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "- 100%";
            // 
            // panelTorque
            // 
            this.panelTorque.BackColor = System.Drawing.Color.White;
            this.panelTorque.Location = new System.Drawing.Point(15, 20);
            this.panelTorque.Name = "panelTorque";
            this.panelTorque.Size = new System.Drawing.Size(35, 190);
            this.panelTorque.TabIndex = 6;
            // 
            // panelGreen
            // 
            this.panelGreen.BackColor = System.Drawing.Color.Lime;
            this.panelGreen.Location = new System.Drawing.Point(15, 20);
            this.panelGreen.Name = "panelGreen";
            this.panelGreen.Size = new System.Drawing.Size(35, 190);
            this.panelGreen.TabIndex = 1;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnEnable);
            this.groupBox3.Controls.Add(this.btnStream);
            this.groupBox3.Location = new System.Drawing.Point(285, 10);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox3.Size = new System.Drawing.Size(255, 40);
            this.groupBox3.TabIndex = 22;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Control";
            // 
            // btnEnable
            // 
            this.btnEnable.Location = new System.Drawing.Point(148, 13);
            this.btnEnable.Margin = new System.Windows.Forms.Padding(2);
            this.btnEnable.Name = "btnEnable";
            this.btnEnable.Size = new System.Drawing.Size(86, 19);
            this.btnEnable.TabIndex = 17;
            this.btnEnable.Text = "Enable";
            this.btnEnable.UseVisualStyleBackColor = true;
            this.btnEnable.Click += new System.EventHandler(this.btnEnable_Click);
            // 
            // btnStream
            // 
            this.btnStream.Location = new System.Drawing.Point(24, 13);
            this.btnStream.Margin = new System.Windows.Forms.Padding(2);
            this.btnStream.Name = "btnStream";
            this.btnStream.Size = new System.Drawing.Size(86, 19);
            this.btnStream.TabIndex = 15;
            this.btnStream.Text = "Start stream";
            this.btnStream.UseVisualStyleBackColor = true;
            this.btnStream.Click += new System.EventHandler(this.btnStream_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.anglePlot);
            this.groupBox2.Location = new System.Drawing.Point(5, 10);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox2.Size = new System.Drawing.Size(255, 268);
            this.groupBox2.TabIndex = 21;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Shaft angle";
            // 
            // anglePlot
            // 
            this.anglePlot.BackColor = System.Drawing.Color.Transparent;
            this.anglePlot.BackImage = "C:\\Users\\Wolter\\Dropbox\\Drucker\\Servo\\Visual Studio\\Mechaduino\\Mechaduino - GUI\\b" +
    "ackground_2.png";
            this.anglePlot.BackImageAlignment = System.Windows.Forms.DataVisualization.Charting.ChartImageAlignmentStyle.Center;
            this.anglePlot.BackImageWrapMode = System.Windows.Forms.DataVisualization.Charting.ChartImageWrapMode.Scaled;
            this.anglePlot.BorderlineColor = System.Drawing.Color.Black;
            chartArea5.BackColor = System.Drawing.Color.Transparent;
            chartArea5.Name = "ChartArea1";
            this.anglePlot.ChartAreas.Add(chartArea5);
            this.anglePlot.Location = new System.Drawing.Point(12, 17);
            this.anglePlot.Margin = new System.Windows.Forms.Padding(2);
            this.anglePlot.Name = "anglePlot";
            series7.ChartArea = "ChartArea1";
            series7.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series7.Name = "Series1";
            series8.ChartArea = "ChartArea1";
            series8.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series8.Name = "Series2";
            this.anglePlot.Series.Add(series7);
            this.anglePlot.Series.Add(series8);
            this.anglePlot.Size = new System.Drawing.Size(232, 244);
            this.anglePlot.TabIndex = 17;
            this.anglePlot.Text = "chart1";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.pltError);
            this.groupBox1.Controls.Add(this.pltPosition);
            this.groupBox1.Controls.Add(this.pltCurrent);
            this.groupBox1.Location = new System.Drawing.Point(5, 283);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox1.Size = new System.Drawing.Size(541, 351);
            this.groupBox1.TabIndex = 20;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Live plots";
            // 
            // pltError
            // 
            this.pltError.BorderlineColor = System.Drawing.Color.Black;
            chartArea6.Name = "ChartArea1";
            this.pltError.ChartAreas.Add(chartArea6);
            this.pltError.Location = new System.Drawing.Point(11, 239);
            this.pltError.Margin = new System.Windows.Forms.Padding(2);
            this.pltError.Name = "pltError";
            series9.ChartArea = "ChartArea1";
            series9.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series9.Name = "Series1";
            this.pltError.Series.Add(series9);
            this.pltError.Size = new System.Drawing.Size(502, 106);
            this.pltError.TabIndex = 25;
            this.pltError.Text = "chart1";
            title4.Name = "Title1";
            title4.Text = "Error [deg]";
            this.pltError.Titles.Add(title4);
            // 
            // pltPosition
            // 
            this.pltPosition.BorderlineColor = System.Drawing.Color.Black;
            chartArea7.Name = "ChartArea1";
            this.pltPosition.ChartAreas.Add(chartArea7);
            this.pltPosition.Location = new System.Drawing.Point(11, 128);
            this.pltPosition.Margin = new System.Windows.Forms.Padding(2);
            this.pltPosition.Name = "pltPosition";
            series10.ChartArea = "ChartArea1";
            series10.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series10.Legend = "Legend1";
            series10.Name = "Series1";
            series11.ChartArea = "ChartArea1";
            series11.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series11.Legend = "Legend1";
            series11.Name = "Series2";
            this.pltPosition.Series.Add(series10);
            this.pltPosition.Series.Add(series11);
            this.pltPosition.Size = new System.Drawing.Size(502, 106);
            this.pltPosition.TabIndex = 24;
            this.pltPosition.Text = "chart1";
            title5.Name = "Title1";
            title5.Text = "Angles [deg]";
            this.pltPosition.Titles.Add(title5);
            // 
            // pltCurrent
            // 
            this.pltCurrent.BorderlineColor = System.Drawing.Color.Black;
            chartArea8.Name = "ChartArea1";
            this.pltCurrent.ChartAreas.Add(chartArea8);
            this.pltCurrent.Location = new System.Drawing.Point(11, 18);
            this.pltCurrent.Margin = new System.Windows.Forms.Padding(2);
            this.pltCurrent.Name = "pltCurrent";
            series12.ChartArea = "ChartArea1";
            series12.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series12.Name = "Series1";
            this.pltCurrent.Series.Add(series12);
            this.pltCurrent.Size = new System.Drawing.Size(502, 106);
            this.pltCurrent.TabIndex = 23;
            this.pltCurrent.Text = "chart1";
            title6.Name = "Title1";
            title6.Text = "Current [mA]";
            this.pltCurrent.Titles.Add(title6);
            // 
            // tabParameter
            // 
            this.tabParameter.Location = new System.Drawing.Point(4, 22);
            this.tabParameter.Margin = new System.Windows.Forms.Padding(2);
            this.tabParameter.Name = "tabParameter";
            this.tabParameter.Size = new System.Drawing.Size(548, 646);
            this.tabParameter.TabIndex = 3;
            this.tabParameter.Text = "Paramter";
            this.tabParameter.UseVisualStyleBackColor = true;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDark;
            this.ClientSize = new System.Drawing.Size(559, 678);
            this.Controls.Add(this.tabControl1);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "Form1";
            this.Text = "Mechaduino GUI";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabConnection.ResumeLayout(false);
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.tabPlots.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.anglePlot)).EndInit();
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pltError)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pltPosition)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pltCurrent)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabConnection;
        private System.Windows.Forms.TabPage tabControl;
        private System.Windows.Forms.TabPage tabPlots;
        private System.Windows.Forms.TabPage tabParameter;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.DataVisualization.Charting.Chart pltError;
        private System.Windows.Forms.DataVisualization.Charting.Chart pltPosition;
        private System.Windows.Forms.DataVisualization.Charting.Chart pltCurrent;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnStream;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.DataVisualization.Charting.Chart anglePlot;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Panel panelGreen;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panelTorque;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Panel panelEnabled;
        private System.Windows.Forms.Button btnEnable;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox txtConsole;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.ComboBox serial_box;
        private System.Windows.Forms.Button btnOpen;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.TextBox txtSend;
        private System.Windows.Forms.TextBox txtReceived;
        private System.Windows.Forms.Timer timer1;
    }
}

