using System;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Threading;
using System.IO;
using System.Windows.Forms.DataVisualization.Charting;


namespace Mechaduino
{
   

    public partial class GUI : Form
    {
        int length = 500;

        int[] uValues = new int[500];
        double[] yValues = new double[500];
        double[] rValues = new double[500];
        double[] eValues = new double[500];

        int[] response_position = new int[1000];
        int[] response_target = new int[1000];

        double omeaga = 0;
        double last_y = 0;



        int y = 0;
        int r = 0;
        int e = 0;
        int u = 0;
        int electrical_angle = 0;
        int enabled = 0;

        int raw =0;
        int uMax = 233;
        int Pa = 0;
        int Torque = 0;

        int wrap_pointer = 0;

        int response_counter = 0;

        int streaming = 0;

        bool wasOpen = false;
        bool changing_size = false;
        bool savetoCSV = false;
        String CSVFileName = "";

        bool response_income = false;
        int response_command_Length = 0;

        int k = 0;

        bool Ramp;


        int Counter = 0;
        String value = "";



        public GUI()
        {
            InitializeComponent();
        }

        
        

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            serial_box.Items.AddRange(ports);
            serial_box.SelectedIndex = 0;
            btnOpen.Enabled = true;
            btnStream.Enabled = false;
            btnEnable.Enabled = false;

            ((Control)this.tabPlots).Enabled = false;
            ((Control)this.tabParameter).Enabled = false;
            ((Control)this.tabResponse).Enabled = false;


            saveFileDialog1.Filter = "CSV File|*.csv|Text File|*.txt";
            saveFileDialog1.Title = "Save as CSV File";

            Thread.Sleep(300);
            

            anglePlot.Series[0].Points.Clear();
            anglePlot.Series[1].Points.Clear();
            anglePlot.ChartAreas[0].AxisX.Minimum = -1.1;
            anglePlot.ChartAreas[0].AxisX.Maximum = 1.1;
            anglePlot.ChartAreas[0].AxisY.Minimum = -1.1;
            anglePlot.ChartAreas[0].AxisY.Maximum = 1.1;

            anglePlot.ChartAreas[0].AxisX.LineWidth = 0;
            anglePlot.ChartAreas[0].AxisY.LineWidth = 0;
            anglePlot.ChartAreas[0].AxisX.LabelStyle.Enabled = false;
            anglePlot.ChartAreas[0].AxisY.LabelStyle.Enabled = false;
            anglePlot.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            anglePlot.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
            anglePlot.ChartAreas[0].AxisX.MinorGrid.Enabled = false;
            anglePlot.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            anglePlot.ChartAreas[0].AxisX.MajorTickMark.Enabled = false;
            anglePlot.ChartAreas[0].AxisY.MajorTickMark.Enabled = false;
            anglePlot.ChartAreas[0].AxisX.MinorTickMark.Enabled = false;
            anglePlot.ChartAreas[0].AxisY.MinorTickMark.Enabled = false;

            var backImage = new NamedImage("bgImg", Mechaduino___GUI.Properties.Resources.anglePlotBackgroundPicture);
            anglePlot.Images.Add(backImage);
            anglePlot.BackImage = "bgImg";



            // current plot
            pltCurrent.Series[0].Points.Clear();
            pltCurrent.ChartAreas[0].AxisX.LabelStyle.Enabled = false;
            pltCurrent.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            pltCurrent.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
            pltCurrent.ChartAreas[0].AxisX.MinorGrid.Enabled = false;
            pltCurrent.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            pltCurrent.ChartAreas[0].AxisX.MajorTickMark.Enabled = false;
            pltCurrent.ChartAreas[0].AxisX.MinorTickMark.Enabled = false;


            // position plot
            pltPosition.Series[0].Points.Clear();
            pltPosition.ChartAreas[0].AxisX.LabelStyle.Enabled = false;
            pltPosition.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            pltPosition.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
            pltPosition.ChartAreas[0].AxisX.MinorGrid.Enabled = false;
            pltPosition.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            pltPosition.ChartAreas[0].AxisX.MajorTickMark.Enabled = false;
            pltPosition.ChartAreas[0].AxisX.MinorTickMark.Enabled = false;
            pltPosition.Series[1].Color = Color.Red;


            // Error plot
            pltError.Series[0].Points.Clear();
            pltError.ChartAreas[0].AxisX.LabelStyle.Enabled = false;
            pltError.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            pltError.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
            pltError.ChartAreas[0].AxisX.MinorGrid.Enabled = false;
            pltError.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            pltError.ChartAreas[0].AxisX.MajorTickMark.Enabled = false;
            pltError.ChartAreas[0].AxisX.MinorTickMark.Enabled = false;



            // response plot
            pltresponse.ChartAreas[0].CursorX.IsUserEnabled = Enabled;
            pltresponse.ChartAreas[0].CursorX.IsUserSelectionEnabled = Enabled;
            pltresponse.ChartAreas[0].CursorX.Interval = 0;
            pltresponse.ChartAreas[0].AxisX.ScaleView.Zoomable = Enabled;
            pltresponse.ChartAreas[0].AxisX.ScrollBar.IsPositionedInside = true;
            pltresponse.ChartAreas[0].AxisX.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltresponse.ChartAreas[0].AxisX.ScaleView.SmallScrollMinSize = 0;

            pltresponse.ChartAreas[0].CursorY.IsUserEnabled = Enabled;
            pltresponse.ChartAreas[0].CursorY.IsUserSelectionEnabled = Enabled;
            pltresponse.ChartAreas[0].CursorY.Interval = 0;
            pltresponse.ChartAreas[0].AxisY.ScaleView.Zoomable = Enabled;
            pltresponse.ChartAreas[0].AxisY.ScrollBar.IsPositionedInside = true;
            pltresponse.ChartAreas[0].AxisY.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltresponse.ChartAreas[0].AxisY.ScaleView.SmallScrollMinSize = 0;
            pltresponse.Series[1].Color = Color.Red;
        }



        private delegate void processValueDelegate(string value);

        private void port_DataReceived_1(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string InputData = serialPort1.ReadLine();
                if (InputData != String.Empty)
                {
                   processValue(InputData);
                    value = InputData;
                }
                else
                {
                    serialPort1.DiscardInBuffer();
                }
            }
            catch
            {
            }
        }



        private void processValue(string value)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new processValueDelegate(processValue), new object[] { value });
            }
            else
            {
                // ... safe to update GUI in here ...
                if (!String.IsNullOrEmpty(value))
                {
                    String[] substrings = value.Split(';');

                    if (substrings.Length == 2)
                    {
                        response_position[response_command_Length] = Convert.ToInt32(substrings[0]);
                        response_target[response_command_Length] = Convert.ToInt32(substrings[1]);

                        response_command_Length = response_command_Length + 1;
                        if (response_command_Length >= 990)
                        {
                            response_income = true;
                        }


                    }
                    else if (substrings.Length == 7)
                    {
                        streaming = Convert.ToInt16(substrings[0]);

                        //Data Stream is received here
                        last_y = y;
                        y = Convert.ToInt32(substrings[1]);
                        r = Convert.ToInt32(substrings[2]);
                        e = Convert.ToInt32(substrings[3]);
                        u = Convert.ToInt32(substrings[4]);
                        electrical_angle = Math.Abs(Convert.ToInt32(substrings[5]));
                        enabled = Convert.ToInt16(substrings[6]);

                        raw = mod(y, 36000);

                        Pa = electrical_angle - raw;
                        if (Pa > 500)
                        {
                            Pa = Pa - 36000;
                        }
                        else if (Pa < -500)
                        {
                            Pa = Pa + 36000;
                        }

                        Torque = Math.Abs((Pa * u * 240) / (180 * uMax));
                        /*

                        if (savetoCSV)
                        {
                            using (StreamWriter sw = File.AppendText(CSVFileName))
                            {
                                sw.Write(value);
                            }
                        }
                        */
                        if (!changing_size)
                        {
                            // update line plots
                            uValues[wrap_pointer] = Convert.ToInt32((u * 1000 * 3.3) / (512 * 10 * 0.15));
                            yValues[wrap_pointer] = (y / 100.0);
                            rValues[wrap_pointer] = (r / 100.0);
                            eValues[wrap_pointer] = (e / 100.0);

                            omeaga = Math.Round((((y / 100.0) - (last_y / 100.0)) * 100.0), 3);

                            wrap_pointer += 1;
                            wrap_pointer = wrap_pointer % yValues.Length;
                        }

                        else
                        {
                            foreach (var substring in substrings)
                            {
                                txtReceived.AppendText(substring + "\n");
                                txtReceived.ScrollToCaret();
                            }
                        }
                    }
                    else if (substrings.Length == 19)
                    {
                        txtIdentifier.Text = substrings[0];
                        txtFullstep.Text = substrings[1];
                        txtMicrostep.Text = substrings[2];
                        txtCurrent.Text = substrings[3];
                        txtMaxM.Text = substrings[4];
                        txtMaxI.Text = substrings[5];
                        uMax = Convert.ToInt32((Convert.ToInt32(substrings[5]) * 0.15 * 10.0 * 512.0) / (1000.0 * 3.3));
                        txtRotorJ.Text = substrings[6];
                        txtloadMass.Text = substrings[7];
                        txtKp.Text = substrings[8];
                        txtKi.Text = substrings[9];
                        txtKd.Text = substrings[10];
                        txtPLPF.Text = substrings[11];
                        txtEncoderLPF.Text = substrings[12];
                        txtmmRev.Text = substrings[13];
                        txtMaxE.Text = substrings[14];
                        if (Convert.ToInt16(substrings[15]) == 1)
                        {
                            checkEnable.Checked = true;
                        }
                        else
                        {
                            checkEnable.Checked = false;
                        }
                        if (Convert.ToInt16(substrings[16]) == 1)
                        {
                            checkInvert.Checked = true;
                        }
                        else
                        {
                            checkInvert.Checked = false;
                        }
                        txtKff.Text = substrings[17];
                        txtKvff.Text = substrings[18];
                    }
                    else
                    {
                        streaming = 0;
                        txtReceived.AppendText(value + "\n");
                        txtReceived.ScrollToCaret();
                    }
                }
            }
        }



        



        private void changeYScala(object chart,double percent)
        {
            double temp_max = Double.MinValue;
            double temp_min = Double.MaxValue;

            Chart tmpChart = (Chart)chart;

            double leftLimit = tmpChart.ChartAreas[0].AxisX.Minimum;
            double rightLimit = tmpChart.ChartAreas[0].AxisX.Maximum;

            for (int s = 0; s < tmpChart.Series.Count(); s++)
            {
                foreach (DataPoint dp in tmpChart.Series[s].Points)
                {
                    if (dp.XValue >= leftLimit && dp.XValue <= rightLimit)
                    {
                        temp_min = Math.Min(temp_min, dp.YValues[0]);
                        temp_max = Math.Max(temp_max, dp.YValues[0]);
                    }
                }
            }

            double diff = temp_max - temp_min;

            /*if (temp_min < 0)
            {
            temp_min = temp_min + diff * (percent / 100);
            }
            else
            {*/
                temp_min = temp_min - diff * (percent / 100);
            /*}
             if (temp_max < 0)
             {
                 temp_max = temp_max  - diff * (percent / 100);
             }
             else
             {*/
            temp_max = temp_max + diff * (percent / 100);
            // }


            temp_min = Math.Floor(temp_min * 100) / 100.0;
            temp_max = Math.Ceiling(temp_max * 100) / 100.0;
            
            

            if (temp_min == temp_max)
            {
                temp_min = temp_min - 1;
                temp_max = temp_max + 1;
            }
            
            tmpChart.ChartAreas[0].AxisY.Maximum = temp_max;
            tmpChart.ChartAreas[0].AxisY.Minimum = temp_min;
        }







        private void btnStream_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                //Debug.Print(Convert.ToString(streaming));
                if (streaming == 0)
                {
                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Stop stream";
                    btnEnable.Enabled = true;

                    Array.Clear(yValues, 0, yValues.Length);
                    Array.Clear(rValues, 0, rValues.Length);
                    Array.Clear(eValues, 0, eValues.Length);
                    pltCurrent.Series[0].Points.Clear();
                    pltPosition.Series[0].Points.Clear();
                    pltPosition.Series[1].Points.Clear();
                    pltError.Series[0].Points.Clear();
                    
                    serialPort1.WriteLine("start_stream\n");
                }
                else
                {
                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Start stream";
                    btnEnable.Enabled = false;
                    serialPort1.WriteLine("stop_stream\n");
                }
            }
        }






        private int mod(int xMod, int mMod)
        {
            return (xMod % mMod + mMod) % mMod;
        }





        private void btnEnable_Click(object sender, EventArgs e)
        {
            if (enabled==1)
            {
                serialPort1.WriteLine("disable\n");
            }
            else
            {
                serialPort1.WriteLine("enable\n");
            }
        }





        private void btnOpen_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.DiscardInBuffer();
                try
                {
                    serialPort1.Close();
                    btnOpen.Enabled = true;
                    btnStream.Enabled = false;
                }
                catch
                {
                }
                btnOpen.Text = "Open";
                wasOpen = false;
                txtConsole.Text = "Serial port closed";
                txtConsole.BackColor = Color.White;

                ((Control)this.tabPlots).Enabled = false;
                ((Control)this.tabParameter).Enabled = false;
                ((Control)this.tabResponse).Enabled = false;
            }
            else
            {
                try
                {
                    serialPort1.BaudRate = 250000;
                    serialPort1.Parity = Parity.None;
                    serialPort1.StopBits = StopBits.One;
                    serialPort1.DataBits = 8;
                    serialPort1.Handshake = Handshake.None;
                    serialPort1.RtsEnable = true;
                    serialPort1.PortName = serial_box.Text;
                    serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(port_DataReceived_1);
                    serialPort1.Open();
                }
                catch
                {
                }

                if (serialPort1.IsOpen)
                {
                    ((Control)this.tabPlots).Enabled = true;
                    ((Control)this.tabParameter).Enabled = true;
                    ((Control)this.tabResponse).Enabled = true;
                    serialPort1.WriteLine("\n");
                    Thread.Sleep(100);

                    serialPort1.DiscardInBuffer();
                    txtReceived.Text = "";
                    wasOpen = true;
                    
                    btnStream.Enabled = true;
                    btnOpen.Text = "Close";

                    txtConsole.Text = "Serial port open";
                    txtConsole.BackColor = Color.White;
                    serialPort1.WriteLine("stop_stream \n");
                    serialPort1.DiscardInBuffer();
                    Thread.Sleep(10);
                    serialPort1.WriteLine("load_param\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("help \n");
                    txtReceived.Clear();
                }
                else
                {
                    txtConsole.Text = "unable to open serial port!";
                    txtConsole.BackColor = Color.Red;
                }
            }

        }





        private void btnSend_Click_1(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.WriteLine(txtSend.Text + "\n");
                txtSend.Clear();
            }
            else
            {
                txtConsole.Text = "No Serial Port avaiable to send to";
                txtConsole.BackColor = Color.Red;
            }
        }





        private void timerCheck_Tick(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
            {
                btnOpen.Text = "Open";
                btnStream.Enabled = false;
                btnEnable.Enabled = false;
                if (wasOpen)
                {
                    txtConsole.Text = "Lost Serial Connection";
                    txtConsole.BackColor = Color.Red;
                    wasOpen = false;
                    string[] ports = SerialPort.GetPortNames();
                    serial_box.Items.Clear();
                    serial_box.Items.AddRange(ports);
                    serial_box.SelectedIndex = 0;
                }
            }
        }





     





        private void timerPlot_Tick(object sender, EventArgs e)
        {
            if (streaming == 1)
            {
                if (enabled == 1)
                {
                    panelEnabled.BackColor = Color.Lime;
                    btnEnable.Text = "Disable";
                }
                else
                {
                    panelEnabled.BackColor = Color.Red;
                    btnEnable.Text = "Enable";
                }

                // update angle plot
                anglePlot.Series[0].Points.Clear();
                anglePlot.Series[0].Points.AddXY(0, 0);
                anglePlot.Series[0].Points.AddXY(Math.Cos((y / 100.0) * Math.PI / 180), Math.Sin((y / 100.0) * Math.PI / 180));

                anglePlot.Series[1].Points.Clear();
                anglePlot.Series[1].Points.AddXY(0, 0);
                anglePlot.Series[1].Points.AddXY(Math.Cos((electrical_angle / 100.0) * Math.PI / 180), Math.Sin((electrical_angle / 100.0) * Math.PI / 180));

                pltCurrent.Series[0].Points.Clear();
                pltPosition.Series[0].Points.Clear();
                pltPosition.Series[1].Points.Clear();
                pltError.Series[0].Points.Clear();

                txtOmega.Text = Convert.ToString(omeaga);

                for (int i = 0; i <= yValues.Length - 1; i++)
                {
                    int pointer = wrap_pointer + i;
                    pointer = pointer % yValues.Length;

                    pltCurrent.Series[0].Points.AddY(uValues[pointer]);
                    pltPosition.Series[0].Points.AddY(yValues[pointer]);
                    pltPosition.Series[1].Points.AddY(rValues[pointer]);
                    pltError.Series[0].Points.AddY(eValues[pointer]);
                }

                // Update y axis scaling 
                changeYScala(pltCurrent,5);
                changeYScala(pltPosition,5);
                changeYScala(pltError,5);

                // update bar plots
                panelTorque.Height = 240 - Torque;
            }

            if (response_income)
            {
                response_income = false;
                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();
                
                double start_position = 0;
                double start_target = 0;

                for (int j = 0; j < 50; j++)
                {
                    start_position = start_position +response_position[j];
                    start_target = start_target + response_target[j];
                }
                start_position = start_position / 50;
                start_target = start_target / 50;

                for (int j = 0; j <= response_command_Length - 1; j++)
                {
                    pltresponse.Series[0].Points.AddY((response_position[j]- start_position) / 100.0);
                    pltresponse.Series[1].Points.AddY((response_target[j] - start_target) / 100.0);
                }

                response_command_Length = 0;
                changeYScala(pltresponse,5);
            }
        }

        private void btnSetTimeframe_Click(object sender, EventArgs e)
        {
            changing_size = true;
            length = Convert.ToInt32(100*Convert.ToDouble(txtTimeframe.Text));

            if (length < 10)
            {
                length = 50;
            }


            if (streaming == 1)
            {
                streaming = 0;
                Array.Clear(uValues, 0, uValues.Length);
                Array.Clear(yValues, 0, uValues.Length);
                Array.Clear(rValues, 0, uValues.Length);
                Array.Clear(eValues, 0, uValues.Length);

                Array.Resize(ref uValues, length);
                Array.Resize(ref yValues, length);
                Array.Resize(ref rValues, length);
                Array.Resize(ref eValues, length);
                streaming = 1;
            }
            else
            {
                Array.Clear(uValues, 0, uValues.Length);
                Array.Clear(yValues, 0, uValues.Length);
                Array.Clear(rValues, 0, uValues.Length);
                Array.Clear(eValues, 0, uValues.Length);

                Array.Resize(ref uValues, length);
                Array.Resize(ref yValues, length);
                Array.Resize(ref rValues, length);
                Array.Resize(ref eValues, length);

            }

            wrap_pointer = 0;
            changing_size = false;


        }
        


        private void serial_box_DropDown(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            serial_box.Items.Clear();
            serial_box.Items.AddRange(ports);
            serial_box.SelectedIndex = 0;
        }

        private void btnp100_Click(object sender, EventArgs e)
        {
            setpoint(100);
        }

        private void btnp10_Click(object sender, EventArgs e)
        {
            setpoint(10);
        }

        private void btnp1_Click(object sender, EventArgs e)
        {
            setpoint(1);
        }

        private void btnp01_Click(object sender, EventArgs e)
        {
            setpoint(0.1);
        }

        private void btnn01_Click(object sender, EventArgs e)
        {
            setpoint(-0.1);
        }
        private void btnn1_Click(object sender, EventArgs e)
        {
            setpoint(-1);
        }

        private void btnn10_Click(object sender, EventArgs e)
        {
            setpoint(-10);
        }

        private void btnn100_Click(object sender, EventArgs e)
        {
            setpoint(-100);
        }



        private void setpoint(double angle)
        {
            if (serialPort1.IsOpen)
            {
                string cmd = Convert.ToString(((Convert.ToDouble(r) + (angle * 100.0)) / 100.0), System.Globalization.CultureInfo.InvariantCulture);
                serialPort1.Write("set " + cmd + "\n");
            }


        }

        private void btnSetpoint_Click(object sender, EventArgs e)
        {
            string cmd = Convert.ToString(Convert.ToDouble(txtSetpoint.Text), System.Globalization.CultureInfo.InvariantCulture);

            if (serialPort1.IsOpen)
            {
                serialPort1.Write("set " + cmd + "\n");
            }
        }

        private void btnResetSetpoint_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                if (enabled==1)
                {
                    serialPort1.WriteLine("disable \n");
                    Thread.Sleep(50);
                    serialPort1.WriteLine("enable \n");
                }
            }

        }

        private void btnGetpoint_Click(object sender, EventArgs e)
        {
            txtSetpoint.Text=Convert.ToString(r/100.0);

        }

        private void btnJump_Click(object sender, EventArgs e)
        {
            string cmd = Convert.ToString((Convert.ToDouble(r) + 100*Convert.ToDouble(txtJump.Text))/100.0, System.Globalization.CultureInfo.InvariantCulture);
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("set " + cmd + "\n");
            }
        }

        public void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (!savetoCSV)
            {
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    CSVFileName = saveFileDialog1.FileName;
                    File.WriteAllText(CSVFileName, "");
                    File.AppendAllText(CSVFileName, "time;streaming;position;target;error;effort;electrical_angle;enabled \n");
                    savetoCSV = true;
                }
            }
            else
            {
                checkBox1.Checked = false;
                savetoCSV = false;
                using (FileStream fs = new FileStream(CSVFileName, FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.Close();
                }
            }
        }

        private void btnAutotuneRun_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                Thread.Sleep(10);
                serialPort1.WriteLine("autotune\n");
                Thread.Sleep(10);
                int cycles = Convert.ToInt32(txtAutotuneCycles.Text, System.Globalization.CultureInfo.InvariantCulture);
                String cmd = Convert.ToString(cycles, System.Globalization.CultureInfo.InvariantCulture);
                serialPort1.Write(cmd + "\n");
                serialPort1.Write(" \n");
                serialPort1.Write(" \n");
                serialPort1.Write("load_param\n");
            }
        }

        private void btnRunCalibration_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                String cmd = "calibrate";
                serialPort1.Write(cmd + "\n");
            }
        }

        private void btnSendAll_Click(object sender, EventArgs e)
        {

            if (serialPort1.IsOpen)
            {
                try
                {
                    String cmd = "";

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("a\n");
                    Thread.Sleep(10);
                    String identifier = (txtIdentifier.Text);
                    cmd = Convert.ToString(identifier, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("n\n");
                    Thread.Sleep(10);
                    int steps_per_revolution = Convert.ToInt32(txtFullstep.Text);
                    cmd = Convert.ToString(steps_per_revolution, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("m\n");
                    Thread.Sleep(10);
                    int microstepping = Convert.ToInt32(txtMicrostep.Text);
                    cmd = Convert.ToString(microstepping, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("r\n");
                    Thread.Sleep(10);
                    int iMax = Convert.ToInt32(txtCurrent.Text);
                    cmd = Convert.ToString(iMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("e\n");
                    Thread.Sleep(10);
                    double max_e = Convert.ToDouble(txtMaxE.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(max_e, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("f\n");
                    Thread.Sleep(10);
                    double mMax = Convert.ToDouble(txtMaxM.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(mMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("g\n");
                    Thread.Sleep(10);
                    int i_rated = Convert.ToInt32(txtMaxI.Text);
                    cmd = Convert.ToString(i_rated, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("h\n");
                    Thread.Sleep(10);
                    int m_load = Convert.ToInt32(txtloadMass.Text);
                    cmd = Convert.ToString(m_load, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("j\n");
                    Thread.Sleep(10);
                    int j_rotor = Convert.ToInt32(txtRotorJ.Text);
                    cmd = Convert.ToString(j_rotor, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("p\n");
                    Thread.Sleep(10);
                    double Kp = Convert.ToDouble(txtKp.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kp, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("i\n");
                    Thread.Sleep(10);
                    double Ki = Convert.ToDouble(txtKi.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Ki, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("d\n");
                    Thread.Sleep(10);
                    double Kd = Convert.ToDouble(txtKd.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("k\n");
                    Thread.Sleep(10);
                    int pLPF = Convert.ToInt32(txtPLPF.Text);
                    cmd = Convert.ToString(pLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("l\n");
                    Thread.Sleep(10);
                    int encoderLPF = Convert.ToInt32(txtEncoderLPF.Text);
                    cmd = Convert.ToString(encoderLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("b\n");
                    Thread.Sleep(10);
                    int mm_rev = Convert.ToInt32(txtmmRev.Text);
                    cmd = Convert.ToString(mm_rev, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("o\n");
                    Thread.Sleep(10);
                    int enable = Convert.ToInt32(checkEnable.Checked);
                    cmd = Convert.ToString(enable, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("q\n");
                    Thread.Sleep(10);
                    int invert = Convert.ToInt32(checkInvert.Checked);
                    cmd = Convert.ToString(invert, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("s\n");
                    Thread.Sleep(10);
                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("t\n");
                    Thread.Sleep(10);
                    double Kvff = Convert.ToDouble(txtKvff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kvff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }







        private void btnLoad_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("load_param\n");
            }
        }






        private void btnSave_Click(object sender, EventArgs e)
        {

        }





        private void btnSendIdentifier_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    String cmd = "";

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("a\n");
                    Thread.Sleep(10);
                    String identifier = (txtIdentifier.Text);
                    cmd = Convert.ToString(identifier, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }







        private void btnSendStepSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("n\n");
                    Thread.Sleep(10);
                    int steps_per_revolution = Convert.ToInt32(txtFullstep.Text);
                    String cmd = Convert.ToString(steps_per_revolution, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("m\n");
                    Thread.Sleep(10);
                    int microstepping = Convert.ToInt32(txtMicrostep.Text);
                    cmd = Convert.ToString(microstepping, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }






        private void btnSendMotorSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("r\n");
                    Thread.Sleep(10);
                    int iMax = Convert.ToInt32(txtCurrent.Text);
                    String cmd = Convert.ToString(iMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("f\n");
                    Thread.Sleep(10);
                    double mMax = Convert.ToDouble(txtMaxM.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(mMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("g\n");
                    Thread.Sleep(10);
                    int i_rated = Convert.ToInt32(txtMaxI.Text);
                    cmd = Convert.ToString(i_rated, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }






        private void btnSendPins_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("o\n");
                    Thread.Sleep(10);
                    int enable = Convert.ToInt32(checkEnable.Checked);
                    String cmd = Convert.ToString(enable, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("q\n");
                    Thread.Sleep(10);
                    int invert = Convert.ToInt32(checkInvert.Checked);
                    cmd = Convert.ToString(invert, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }







        private void btnSendKinematics_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("h\n");
                    Thread.Sleep(10);
                    int m_load = Convert.ToInt32(txtloadMass.Text);
                    String cmd = Convert.ToString(m_load, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("j\n");
                    Thread.Sleep(10);
                    int j_rotor = Convert.ToInt32(txtRotorJ.Text);
                    cmd = Convert.ToString(j_rotor, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.Write("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.Write("e\n");
                    Thread.Sleep(10);
                    double max_e = Convert.ToDouble(txtMaxE.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(max_e, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("b\n");
                    Thread.Sleep(10);
                    int mm_rev = Convert.ToInt32(txtmmRev.Text);
                    cmd = Convert.ToString(mm_rev, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }





        private void btnSendControllerSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("p\n");
                    Thread.Sleep(10);
                    double Kp = Convert.ToDouble(txtKp.Text, System.Globalization.CultureInfo.InvariantCulture);
                    String cmd = Convert.ToString(Kp, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("i\n");
                    Thread.Sleep(10);
                    double Ki = Convert.ToDouble(txtKi.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Ki, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("d\n");
                    Thread.Sleep(10);
                    double Kd = Convert.ToDouble(txtKd.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }





        private void btnSendFilterSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("k\n");
                    Thread.Sleep(10);
                    int pLPF = Convert.ToInt32(txtPLPF.Text);
                    String cmd = Convert.ToString(pLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("l\n");
                    Thread.Sleep(10);
                    int encoderLPF = Convert.ToInt32(txtEncoderLPF.Text);
                    cmd = Convert.ToString(encoderLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }
        }


        private void btnSendFeedforward_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("s\n");
                    Thread.Sleep(10);
                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    String cmd = Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("t\n");
                    Thread.Sleep(10);
                    double Kvff = Convert.ToDouble(txtKvff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kvff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                }
                catch
                {
                }
            }

        }

        private void btnDirac_Click(object sender, EventArgs e)
        {
            double setpoint = Convert.ToDouble(r);
            double high = (setpoint + 10000)/100;
            double low = (setpoint) /100;
            string cmd_jump = Convert.ToString(high, System.Globalization.CultureInfo.InvariantCulture);
            string cmd_jump_back = Convert.ToString(low, System.Globalization.CultureInfo.InvariantCulture);
 
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("set " + cmd_jump + " \n");
                serialPort1.Write(" \n");
                Thread.Sleep(1);
                serialPort1.Write("set " + cmd_jump_back + " \n");
            }

        }

        private void btnmp100_Click(object sender, EventArgs e)
        {
            setpoint(100*1.8/Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnmp10_Click(object sender, EventArgs e)
        {
            setpoint(10*1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnmp1_Click(object sender, EventArgs e)
        {
            setpoint(1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnmn1_Click(object sender, EventArgs e)
        {
            setpoint(-1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnmn10_Click(object sender, EventArgs e)
        {
            setpoint(-10*1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnmn100_Click(object sender, EventArgs e)
        {
            setpoint(-100*1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void btnStepResponse_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {

                int steps = Convert.ToInt32(txtStepResponse.Text);
                string cmd_response = Convert.ToString(steps, System.Globalization.CultureInfo.InvariantCulture);
                serialPort1.Write("stop_stream \n");
                Thread.Sleep(100);
                serialPort1.Write("response \n");
                Thread.Sleep(100);
                serialPort1.Write(cmd_response + "\n");
            }
        }

        private void btnresetzoom_Click(object sender, EventArgs e)
        {
            pltresponse.ChartAreas[0].AxisX.ScaleView.ZoomReset(0);
            pltresponse.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
        }

        private void btnDirac_Click_1(object sender, EventArgs e)
        {

            if (serialPort1.IsOpen)
            {
                serialPort1.Write("dirac \n");
            }

        }

        private void timerCSV_Tick(object sender, EventArgs e)
        {


            if (savetoCSV)
            {
                using (StreamWriter sw = File.AppendText(CSVFileName))
                {
                    sw.Write(value);
                }
            }
        }

        private void btnRamp_Click(object sender, EventArgs e)
        {
            Ramp = true;
        }

        private void timerRamp_Tick(object sender, EventArgs e)
        {

            if (Ramp)
            {
                Counter = Counter + 1;
                
                if (serialPort1.IsOpen)
                {
                    double step = Convert.ToDouble(txtRamp.Text)/100.0;
                    double start = (Convert.ToDouble(r) / 100.0);
                    string cmd = Convert.ToString(start + step, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write("set ");
                    serialPort1.Write(cmd);
                    serialPort1.Write("\n");
                }
            }

            if (Counter == 100)
            {
                Ramp = false;
                Counter = 0;
            }

        }
    }


}

