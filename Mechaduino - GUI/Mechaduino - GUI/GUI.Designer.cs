namespace Mechaduino
{
    partial class GUI
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title3 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GUI));
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
            this.serial_box = new System.Windows.Forms.ComboBox();
            this.btnOpen = new System.Windows.Forms.Button();
            this.tabPlots = new System.Windows.Forms.TabPage();
            this.groupBox10 = new System.Windows.Forms.GroupBox();
            this.btnResetSetpoint = new System.Windows.Forms.Button();
            this.btnSetpoint = new System.Windows.Forms.Button();
            this.txtSetpoint = new System.Windows.Forms.TextBox();
            this.btnp100 = new System.Windows.Forms.Button();
            this.btnp10 = new System.Windows.Forms.Button();
            this.btnp1 = new System.Windows.Forms.Button();
            this.btnn100 = new System.Windows.Forms.Button();
            this.btnn10 = new System.Windows.Forms.Button();
            this.btnn1 = new System.Windows.Forms.Button();
            this.groupBox9 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txtTimeframe = new System.Windows.Forms.TextBox();
            this.btnSetTimeframe = new System.Windows.Forms.Button();
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
            this.timerCheck = new System.Windows.Forms.Timer(this.components);
            this.timerPlot = new System.Windows.Forms.Timer(this.components);
            this.btnp01 = new System.Windows.Forms.Button();
            this.btnn01 = new System.Windows.Forms.Button();
            this.btnGetpoint = new System.Windows.Forms.Button();
            this.tabControl1.SuspendLayout();
            this.tabConnection.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.tabPlots.SuspendLayout();
            this.groupBox10.SuspendLayout();
            this.groupBox9.SuspendLayout();
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
            this.tabControl1.Controls.Add(this.tabPlots);
            this.tabControl1.Controls.Add(this.tabParameter);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Top;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(2);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(950, 824);
            this.tabControl1.TabIndex = 6;
            this.tabControl1.Selecting += new System.Windows.Forms.TabControlCancelEventHandler(this.tabControl1_Selecting);
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
            this.tabConnection.Size = new System.Drawing.Size(942, 646);
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
            this.groupBox8.Size = new System.Drawing.Size(928, 515);
            this.groupBox8.TabIndex = 19;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Serial Communication";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(840, 488);
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
            this.txtSend.Size = new System.Drawing.Size(905, 20);
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
            this.txtReceived.Size = new System.Drawing.Size(906, 432);
            this.txtReceived.TabIndex = 13;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.txtConsole);
            this.groupBox7.Location = new System.Drawing.Point(9, 62);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(925, 58);
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
            this.txtConsole.Size = new System.Drawing.Size(906, 23);
            this.txtConsole.TabIndex = 12;
            this.txtConsole.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.serial_box);
            this.groupBox6.Controls.Add(this.btnOpen);
            this.groupBox6.Location = new System.Drawing.Point(9, 5);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(308, 51);
            this.groupBox6.TabIndex = 17;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "COM Port";
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
            this.serial_box.DropDown += new System.EventHandler(this.serial_box_DropDown);
            // 
            // btnOpen
            // 
            this.btnOpen.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btnOpen.Location = new System.Drawing.Point(211, 18);
            this.btnOpen.Margin = new System.Windows.Forms.Padding(2);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(80, 21);
            this.btnOpen.TabIndex = 17;
            this.btnOpen.Text = "Open";
            this.btnOpen.UseVisualStyleBackColor = true;
            this.btnOpen.Click += new System.EventHandler(this.btnOpen_Click);
            // 
            // tabPlots
            // 
            this.tabPlots.Controls.Add(this.groupBox10);
            this.tabPlots.Controls.Add(this.groupBox9);
            this.tabPlots.Controls.Add(this.groupBox5);
            this.tabPlots.Controls.Add(this.groupBox4);
            this.tabPlots.Controls.Add(this.groupBox3);
            this.tabPlots.Controls.Add(this.groupBox2);
            this.tabPlots.Controls.Add(this.groupBox1);
            this.tabPlots.Location = new System.Drawing.Point(4, 22);
            this.tabPlots.Margin = new System.Windows.Forms.Padding(2);
            this.tabPlots.Name = "tabPlots";
            this.tabPlots.Size = new System.Drawing.Size(942, 798);
            this.tabPlots.TabIndex = 2;
            this.tabPlots.Text = "Control";
            this.tabPlots.UseVisualStyleBackColor = true;
            // 
            // groupBox10
            // 
            this.groupBox10.Controls.Add(this.btnGetpoint);
            this.groupBox10.Controls.Add(this.btnn01);
            this.groupBox10.Controls.Add(this.btnp01);
            this.groupBox10.Controls.Add(this.btnResetSetpoint);
            this.groupBox10.Controls.Add(this.btnSetpoint);
            this.groupBox10.Controls.Add(this.txtSetpoint);
            this.groupBox10.Controls.Add(this.btnp100);
            this.groupBox10.Controls.Add(this.btnp10);
            this.groupBox10.Controls.Add(this.btnp1);
            this.groupBox10.Controls.Add(this.btnn100);
            this.groupBox10.Controls.Add(this.btnn10);
            this.groupBox10.Controls.Add(this.btnn1);
            this.groupBox10.Location = new System.Drawing.Point(553, 55);
            this.groupBox10.Name = "groupBox10";
            this.groupBox10.Size = new System.Drawing.Size(253, 223);
            this.groupBox10.TabIndex = 26;
            this.groupBox10.TabStop = false;
            this.groupBox10.Text = "Move";
            // 
            // btnResetSetpoint
            // 
            this.btnResetSetpoint.Location = new System.Drawing.Point(147, 190);
            this.btnResetSetpoint.Name = "btnResetSetpoint";
            this.btnResetSetpoint.Size = new System.Drawing.Size(100, 20);
            this.btnResetSetpoint.TabIndex = 13;
            this.btnResetSetpoint.Text = "Reset";
            this.btnResetSetpoint.UseVisualStyleBackColor = true;
            this.btnResetSetpoint.Click += new System.EventHandler(this.btnResetSetpoint_Click);
            // 
            // btnSetpoint
            // 
            this.btnSetpoint.Location = new System.Drawing.Point(147, 40);
            this.btnSetpoint.Name = "btnSetpoint";
            this.btnSetpoint.Size = new System.Drawing.Size(100, 20);
            this.btnSetpoint.TabIndex = 11;
            this.btnSetpoint.Text = "Set";
            this.btnSetpoint.UseVisualStyleBackColor = true;
            this.btnSetpoint.Click += new System.EventHandler(this.btnSetpoint_Click);
            // 
            // txtSetpoint
            // 
            this.txtSetpoint.Location = new System.Drawing.Point(147, 16);
            this.txtSetpoint.Name = "txtSetpoint";
            this.txtSetpoint.Size = new System.Drawing.Size(100, 20);
            this.txtSetpoint.TabIndex = 10;
            this.txtSetpoint.Text = "0,0";
            this.txtSetpoint.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btnp100
            // 
            this.btnp100.Location = new System.Drawing.Point(10, 15);
            this.btnp100.Name = "btnp100";
            this.btnp100.Size = new System.Drawing.Size(100, 20);
            this.btnp100.TabIndex = 9;
            this.btnp100.Text = "+100";
            this.btnp100.UseVisualStyleBackColor = true;
            this.btnp100.Click += new System.EventHandler(this.btnp100_Click);
            // 
            // btnp10
            // 
            this.btnp10.Location = new System.Drawing.Point(20, 40);
            this.btnp10.Name = "btnp10";
            this.btnp10.Size = new System.Drawing.Size(80, 20);
            this.btnp10.TabIndex = 8;
            this.btnp10.Text = "+ 10";
            this.btnp10.UseVisualStyleBackColor = true;
            this.btnp10.Click += new System.EventHandler(this.btnp10_Click);
            // 
            // btnp1
            // 
            this.btnp1.Location = new System.Drawing.Point(30, 65);
            this.btnp1.Name = "btnp1";
            this.btnp1.Size = new System.Drawing.Size(60, 20);
            this.btnp1.TabIndex = 7;
            this.btnp1.Text = "+ 1";
            this.btnp1.UseVisualStyleBackColor = true;
            this.btnp1.Click += new System.EventHandler(this.btnp1_Click);
            // 
            // btnn100
            // 
            this.btnn100.Location = new System.Drawing.Point(10, 190);
            this.btnn100.Name = "btnn100";
            this.btnn100.Size = new System.Drawing.Size(100, 20);
            this.btnn100.TabIndex = 5;
            this.btnn100.Text = "-100";
            this.btnn100.UseVisualStyleBackColor = true;
            this.btnn100.Click += new System.EventHandler(this.btnn100_Click);
            // 
            // btnn10
            // 
            this.btnn10.Location = new System.Drawing.Point(20, 165);
            this.btnn10.Name = "btnn10";
            this.btnn10.Size = new System.Drawing.Size(80, 20);
            this.btnn10.TabIndex = 4;
            this.btnn10.Text = "- 10";
            this.btnn10.UseVisualStyleBackColor = true;
            this.btnn10.Click += new System.EventHandler(this.btnn10_Click);
            // 
            // btnn1
            // 
            this.btnn1.Location = new System.Drawing.Point(30, 140);
            this.btnn1.Name = "btnn1";
            this.btnn1.Size = new System.Drawing.Size(60, 20);
            this.btnn1.TabIndex = 3;
            this.btnn1.Text = "- 1";
            this.btnn1.UseVisualStyleBackColor = true;
            this.btnn1.Click += new System.EventHandler(this.btnn1_Click);
            // 
            // groupBox9
            // 
            this.groupBox9.Controls.Add(this.label1);
            this.groupBox9.Controls.Add(this.txtTimeframe);
            this.groupBox9.Controls.Add(this.btnSetTimeframe);
            this.groupBox9.Location = new System.Drawing.Point(385, 107);
            this.groupBox9.Name = "groupBox9";
            this.groupBox9.Size = new System.Drawing.Size(155, 123);
            this.groupBox9.TabIndex = 25;
            this.groupBox9.TabStop = false;
            this.groupBox9.Text = "Timeframe";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(128, 26);
            this.label1.TabIndex = 2;
            this.label1.Text = "Set the plot timeframe in s\r\n[min = 0.5 s, max = 20 s]\r\n";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // txtTimeframe
            // 
            this.txtTimeframe.Location = new System.Drawing.Point(27, 50);
            this.txtTimeframe.Name = "txtTimeframe";
            this.txtTimeframe.Size = new System.Drawing.Size(100, 20);
            this.txtTimeframe.TabIndex = 1;
            this.txtTimeframe.Text = "5,0";
            this.txtTimeframe.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btnSetTimeframe
            // 
            this.btnSetTimeframe.Location = new System.Drawing.Point(36, 76);
            this.btnSetTimeframe.Name = "btnSetTimeframe";
            this.btnSetTimeframe.Size = new System.Drawing.Size(86, 19);
            this.btnSetTimeframe.TabIndex = 0;
            this.btnSetTimeframe.Text = "Set";
            this.btnSetTimeframe.UseVisualStyleBackColor = true;
            this.btnSetTimeframe.Click += new System.EventHandler(this.btnSetTimeframe_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.panelEnabled);
            this.groupBox5.Location = new System.Drawing.Point(385, 11);
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
            this.groupBox4.Location = new System.Drawing.Point(285, 10);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(95, 268);
            this.groupBox4.TabIndex = 23;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Torque";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(55, 133);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(33, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "- 50%";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(55, 250);
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
            this.panelTorque.Size = new System.Drawing.Size(35, 240);
            this.panelTorque.TabIndex = 6;
            // 
            // panelGreen
            // 
            this.panelGreen.BackColor = System.Drawing.Color.Lime;
            this.panelGreen.Location = new System.Drawing.Point(15, 20);
            this.panelGreen.Name = "panelGreen";
            this.panelGreen.Size = new System.Drawing.Size(35, 241);
            this.panelGreen.TabIndex = 1;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnEnable);
            this.groupBox3.Controls.Add(this.btnStream);
            this.groupBox3.Location = new System.Drawing.Point(553, 10);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox3.Size = new System.Drawing.Size(378, 40);
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
            this.anglePlot.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.anglePlot.BackImage = "D:\\Dropbox\\Drucker\\Servo\\Visual Studio\\Mechaduino\\Mechaduino - GUI\\background_2.p" +
    "ng";
            this.anglePlot.BackImageAlignment = System.Windows.Forms.DataVisualization.Charting.ChartImageAlignmentStyle.Center;
            this.anglePlot.BackImageWrapMode = System.Windows.Forms.DataVisualization.Charting.ChartImageWrapMode.Scaled;
            this.anglePlot.BackSecondaryColor = System.Drawing.Color.Transparent;
            this.anglePlot.BorderlineColor = System.Drawing.Color.Transparent;
            this.anglePlot.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea1.BackColor = System.Drawing.Color.Transparent;
            chartArea1.Name = "ChartArea1";
            this.anglePlot.ChartAreas.Add(chartArea1);
            this.anglePlot.Cursor = System.Windows.Forms.Cursors.No;
            this.anglePlot.Location = new System.Drawing.Point(9, 19);
            this.anglePlot.Margin = new System.Windows.Forms.Padding(2);
            this.anglePlot.Name = "anglePlot";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series1.Name = "Series1";
            series2.ChartArea = "ChartArea1";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series2.Name = "Series2";
            this.anglePlot.Series.Add(series1);
            this.anglePlot.Series.Add(series2);
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
            this.groupBox1.Size = new System.Drawing.Size(930, 513);
            this.groupBox1.TabIndex = 20;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Live plots";
            // 
            // pltError
            // 
            this.pltError.BorderlineColor = System.Drawing.Color.Black;
            chartArea2.Name = "ChartArea1";
            this.pltError.ChartAreas.Add(chartArea2);
            this.pltError.Location = new System.Drawing.Point(9, 293);
            this.pltError.Margin = new System.Windows.Forms.Padding(2);
            this.pltError.Name = "pltError";
            series3.ChartArea = "ChartArea1";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series3.Name = "Series1";
            this.pltError.Series.Add(series3);
            this.pltError.Size = new System.Drawing.Size(915, 106);
            this.pltError.TabIndex = 25;
            this.pltError.Text = "chart1";
            title1.Name = "Title1";
            title1.Text = "Error [deg]";
            this.pltError.Titles.Add(title1);
            // 
            // pltPosition
            // 
            this.pltPosition.BorderlineColor = System.Drawing.Color.Black;
            chartArea3.Name = "ChartArea1";
            this.pltPosition.ChartAreas.Add(chartArea3);
            this.pltPosition.Location = new System.Drawing.Point(9, 17);
            this.pltPosition.Margin = new System.Windows.Forms.Padding(2);
            this.pltPosition.Name = "pltPosition";
            series4.ChartArea = "ChartArea1";
            series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series4.Legend = "Legend1";
            series4.Name = "Series1";
            series5.ChartArea = "ChartArea1";
            series5.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series5.Legend = "Legend1";
            series5.Name = "Series2";
            this.pltPosition.Series.Add(series4);
            this.pltPosition.Series.Add(series5);
            this.pltPosition.Size = new System.Drawing.Size(915, 272);
            this.pltPosition.TabIndex = 24;
            this.pltPosition.Text = "chart1";
            title2.Name = "Title1";
            title2.Text = "Angles [deg]";
            this.pltPosition.Titles.Add(title2);
            // 
            // pltCurrent
            // 
            this.pltCurrent.BorderlineColor = System.Drawing.Color.Black;
            chartArea4.Name = "ChartArea1";
            this.pltCurrent.ChartAreas.Add(chartArea4);
            this.pltCurrent.Location = new System.Drawing.Point(9, 403);
            this.pltCurrent.Margin = new System.Windows.Forms.Padding(2);
            this.pltCurrent.Name = "pltCurrent";
            series6.ChartArea = "ChartArea1";
            series6.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series6.Name = "Series1";
            this.pltCurrent.Series.Add(series6);
            this.pltCurrent.Size = new System.Drawing.Size(915, 106);
            this.pltCurrent.TabIndex = 23;
            this.pltCurrent.Text = "chart1";
            title3.Name = "Title1";
            title3.Text = "Current [mA]";
            this.pltCurrent.Titles.Add(title3);
            // 
            // tabParameter
            // 
            this.tabParameter.Location = new System.Drawing.Point(4, 22);
            this.tabParameter.Margin = new System.Windows.Forms.Padding(2);
            this.tabParameter.Name = "tabParameter";
            this.tabParameter.Size = new System.Drawing.Size(942, 646);
            this.tabParameter.TabIndex = 3;
            this.tabParameter.Text = "Paramter";
            this.tabParameter.UseVisualStyleBackColor = true;
            // 
            // timerCheck
            // 
            this.timerCheck.Enabled = true;
            this.timerCheck.Interval = 1000;
            this.timerCheck.Tick += new System.EventHandler(this.timerCheck_Tick);
            // 
            // timerPlot
            // 
            this.timerPlot.Enabled = true;
            this.timerPlot.Interval = 40;
            this.timerPlot.Tick += new System.EventHandler(this.timerPlot_Tick);
            // 
            // btnp01
            // 
            this.btnp01.Location = new System.Drawing.Point(40, 90);
            this.btnp01.Name = "btnp01";
            this.btnp01.Size = new System.Drawing.Size(40, 20);
            this.btnp01.TabIndex = 14;
            this.btnp01.Text = "+ 0.1";
            this.btnp01.UseVisualStyleBackColor = true;
            this.btnp01.Click += new System.EventHandler(this.btnp01_Click);
            // 
            // btnn01
            // 
            this.btnn01.Location = new System.Drawing.Point(40, 115);
            this.btnn01.Name = "btnn01";
            this.btnn01.Size = new System.Drawing.Size(40, 20);
            this.btnn01.TabIndex = 15;
            this.btnn01.Text = "- 0.1";
            this.btnn01.UseVisualStyleBackColor = true;
            this.btnn01.Click += new System.EventHandler(this.btnn01_Click);
            // 
            // btnGetpoint
            // 
            this.btnGetpoint.Location = new System.Drawing.Point(148, 65);
            this.btnGetpoint.Name = "btnGetpoint";
            this.btnGetpoint.Size = new System.Drawing.Size(100, 20);
            this.btnGetpoint.TabIndex = 16;
            this.btnGetpoint.Text = "Get";
            this.btnGetpoint.UseVisualStyleBackColor = true;
            this.btnGetpoint.Click += new System.EventHandler(this.btnGetpoint_Click);
            // 
            // GUI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.ClientSize = new System.Drawing.Size(950, 835);
            this.Controls.Add(this.tabControl1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "GUI";
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
            this.groupBox10.ResumeLayout(false);
            this.groupBox10.PerformLayout();
            this.groupBox9.ResumeLayout(false);
            this.groupBox9.PerformLayout();
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
        private System.Windows.Forms.ComboBox serial_box;
        private System.Windows.Forms.Button btnOpen;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.TextBox txtSend;
        private System.Windows.Forms.TextBox txtReceived;
        private System.Windows.Forms.Timer timerCheck;
        private System.Windows.Forms.Timer timerPlot;
        private System.Windows.Forms.GroupBox groupBox9;
        private System.Windows.Forms.TextBox txtTimeframe;
        private System.Windows.Forms.Button btnSetTimeframe;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox10;
        private System.Windows.Forms.Button btnp100;
        private System.Windows.Forms.Button btnp10;
        private System.Windows.Forms.Button btnp1;
        private System.Windows.Forms.Button btnn100;
        private System.Windows.Forms.Button btnn10;
        private System.Windows.Forms.Button btnn1;
        private System.Windows.Forms.Button btnSetpoint;
        private System.Windows.Forms.TextBox txtSetpoint;
        private System.Windows.Forms.Button btnResetSetpoint;
        private System.Windows.Forms.Button btnp01;
        private System.Windows.Forms.Button btnn01;
        private System.Windows.Forms.Button btnGetpoint;
    }
}

