using System;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Threading;
using System.IO;


namespace Mechaduino
{
   

    public partial class GUI : Form
    {
        int length = 500;

        double[] uValues = new double[500];
        double[] yValues = new double[500];
        double[] rValues = new double[500];
        double[] eValues = new double[500];
        double[] tValues = new double[1500];
        double[] omegaValues = new double[1500];
        double[] omega_targetValues = new double[1500];


        int[] response_position = new int[1500];
        int[] response_target = new int[1500];


        int y = 0;
        int r = 0;
        int e = 0;
        int u = 0;
        int electrical_angle = 0;
        int enabled = 0;
        int int_frequency = 250;
        int omega_target = 0;
        int omega = 0;
        double fifo_workload = 0;

        int uMax = 0;
        int Torque = 0;
        double time = 0;
        double dt = 0.0;

        int wrap_pointer = 0;

        int streaming = 0;

        bool changing_size = false;
        bool wasOpen = false;
        bool savetoCSV = false;
        String CSVFileName = "";

        bool response_income = false;
        int response_command_Length = 0;
        

        bool Ramp;


        int Counter = 0;
        String value = "";

        String[] buffer = new String[100];
        int buffer_counter = 0;



        public GUI()
        {
            InitializeComponent();
        }

        
        

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            if (ports.Length > 0)
            {
                serial_box.Items.AddRange(ports);
                serial_box.SelectedIndex = 0;
            }
            btnOpen.Enabled = true;
            btnStream.Enabled = false;
            btnEnable.Enabled = false;
            
            ((Control)this.tabPlots).Enabled = false;
            ((Control)this.tabParameter).Enabled = false;
            ((Control)this.tabTuning).Enabled = false;


            saveFileDialog1.Filter = "CSV File|*.csv|Text File|*.txt";
            saveFileDialog1.Title = "Save as CSV File";


            saveFileDialogConfig.Filter = "Configuration File|*.cpp";
            saveFileDialogConfig.Title = "Save as Configuration File";
            saveFileDialogConfig.FileName = "Configuration.cpp";


            response_plot_variable.SelectedIndex = 0;

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



            // position plot
            pltPosition.Series[0].Points.Clear();
            pltPosition.ChartAreas[0].BorderWidth = 200;
            pltPosition.ChartAreas[0].AxisX.LabelStyle.Enabled = true;
            pltPosition.ChartAreas[0].AxisX.LabelStyle.Format = "{0:0.00}";
            pltPosition.ChartAreas[0].AxisX.MajorGrid.Enabled = true;
            pltPosition.ChartAreas[0].AxisX.MinorGrid.Enabled = true;
            pltPosition.ChartAreas[0].AxisX.MajorGrid.LineColor = Color.LightGray;
            pltPosition.ChartAreas[0].AxisX.MinorGrid.LineColor = Color.LightGray;
            pltPosition.ChartAreas[0].AxisX.MajorTickMark.Enabled = true;
            pltPosition.ChartAreas[0].AxisX.MinorTickMark.Enabled = true;

            pltPosition.ChartAreas[0].AxisY.MajorGrid.Enabled = true;
            pltPosition.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            pltPosition.ChartAreas[0].AxisY.MajorGrid.LineColor = Color.LightGray;
            pltPosition.ChartAreas[0].AxisY.MinorGrid.LineColor = Color.LightGray;
            pltPosition.Series[0].Color = Color.Red;
            pltPosition.Series[1].Color = Color.Blue;


            // Error plot
            pltError.Series[0].Points.Clear();
            pltError.ChartAreas[0].BorderWidth = 200;
            pltError.ChartAreas[0].AxisX.LabelStyle.Enabled = true;
            pltError.ChartAreas[0].AxisX.LabelStyle.Format = "{0:0.00}";
            pltError.ChartAreas[0].AxisX.MajorGrid.Enabled = true;
            pltError.ChartAreas[0].AxisX.MinorGrid.Enabled = true;
            pltError.ChartAreas[0].AxisX.MajorGrid.LineColor = Color.LightGray;
            pltError.ChartAreas[0].AxisX.MinorGrid.LineColor = Color.LightGray;
            pltError.ChartAreas[0].AxisX.MajorTickMark.Enabled = true;
            pltError.ChartAreas[0].AxisX.MinorTickMark.Enabled = true;

            pltError.ChartAreas[0].AxisY.MajorGrid.Enabled = true;
            pltError.ChartAreas[0].AxisY.MinorGrid.Enabled = false;
            pltError.ChartAreas[0].AxisY.MajorGrid.LineColor = Color.LightGray;
            pltError.ChartAreas[0].AxisY.MinorGrid.LineColor = Color.LightGray;
            pltError.Series[0].Color = Color.Red;
            pltError.Series[1].Color = Color.Blue;


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

                    if (substrings.Length == 4)
                    {
                        response_target[response_command_Length] = Convert.ToInt32(substrings[0]);
                        response_position[response_command_Length] = Convert.ToInt32(substrings[1]);
                        omega_targetValues[response_command_Length] = Convert.ToInt32(substrings[2]);
                        omegaValues[response_command_Length] = Convert.ToInt32(substrings[3]);

                        response_command_Length = response_command_Length + 1;
                        if (response_command_Length >= 990)
                        {
                            response_income = true;
                        }


                    }
                    else if (substrings.Length == 10)
                    {
                        if (!changing_size)
                        {
                            //Data Stream is received here
                            streaming = Convert.ToInt16(substrings[1]);
                            electrical_angle = Math.Abs(Convert.ToInt32(substrings[8]));
                            fifo_workload = (Convert.ToDouble(substrings[9]));

                            Torque = (Math.Abs(u) * 240) / uMax;

                            if (savetoCSV)
                            {
                                buffer[buffer_counter] = value;
                                buffer_counter += 1;
                            }

                            if ((u+ Convert.ToInt32(substrings[7])) != 0)
                            {
                                enabled = 1;
                            }
                            else
                            {
                                enabled = 0;
                            }
                            r = Convert.ToInt32(substrings[2]);
                            y = Convert.ToInt32(substrings[3]);
                            e = Convert.ToInt32(substrings[4]);
                            u = Convert.ToInt32(substrings[7]);
                            dt = Convert.ToInt32(substrings[0]) / 1000000.0;
                            omega_target = (Convert.ToInt32(substrings[5]));
                            omega = (Convert.ToInt32(substrings[6]));



                            time = time + dt;

                            tValues[wrap_pointer] = time;

                            rValues[wrap_pointer] = r / 100.0;
                            yValues[wrap_pointer] = y / 100.0;
                            eValues[wrap_pointer] = e / 100.0;
                            omega_targetValues[wrap_pointer] = 50.0 * omega_target;
                            omegaValues[wrap_pointer] = 50.0 * omega;
                            uValues[wrap_pointer] = (u * 1000.0 * 3.3) / (512.0 * 10.0 * 0.15);


                            wrap_pointer += 1;
                            wrap_pointer = wrap_pointer % yValues.Length;
                        }
                        
                    }
                    else if (substrings.Length == 17)
                    {
                        txtIdentifier.Text = substrings[0];
                        txtFullstep.Text = substrings[1];
                        txtMicrostep.Text = substrings[2];
                        txtCurrent.Text = substrings[3];
                        uMax = Convert.ToInt32((Convert.ToInt32(substrings[3]) * 0.15 * 10.0 * 512.0) / (1000.0 * 3.3));
                        txtMaxM.Text = substrings[4];
                        txtMaxI.Text = substrings[5];
                        txtKp.Text = substrings[6];
                        txtKi.Text = substrings[7];
                        txtKd.Text = substrings[8];
                        txtPLPF.Text = substrings[9];
                        txtmmRev.Text = substrings[10];
                        txtMaxE.Text = substrings[11];
                        if (Convert.ToInt16(substrings[12]) == 1)
                        {
                            checkEnable.Checked = true;
                        }
                        else
                        {
                            checkEnable.Checked = false;
                        }
                        if (Convert.ToInt16(substrings[13]) == 1)
                        {
                            checkInvert.Checked = true;
                        }
                        else
                        {
                            checkInvert.Checked = false;
                        }
                        txtuLPF.Text = substrings[14];
                        txtKff.Text = substrings[15];
                        txtKacc.Text = substrings[16];
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
                /*DataPoint data = tmpChart.Series[s].Points.FindMaxByValue("Y");
                Debug.Print(Convert.ToString(data));*/
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

            temp_min = temp_min - diff * (percent / 100);
            temp_max = temp_max + diff * (percent / 100);
            
            temp_min = Math.Floor(temp_min * 100) / 100.0;
            temp_max = Math.Ceiling(temp_max * 100) / 100.0;          

            if (temp_min == temp_max)
            {
                temp_min = temp_min - 1;
                temp_max = temp_max + 1;
            }
            if (temp_max< temp_min)
            {
                temp_max = temp_min + 1;
            }
            if (temp_min > temp_max)
            {
                temp_min = temp_max - 1;
            }

            tmpChart.ChartAreas[0].AxisY.Maximum = temp_max;
            tmpChart.ChartAreas[0].AxisY.Minimum = temp_min;
        }







        private void btnStream_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                if (streaming == 0)
                {
                    
                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Stop stream";
                    btnEnable.Enabled = true;
                    
                    pltPosition.Series[0].Points.Clear();
                    pltPosition.Series[1].Points.Clear();
                    pltError.Series[0].Points.Clear();
                    pltError.Series[1].Points.Clear();

                    serialPort1.WriteLine("stream\n");
                    Thread.Sleep(100);
                    serialPort1.Write(Convert.ToString(int_frequency) + "\n");
                }
                else
                {
                    label_Enabled.Text = "Unknown";
                    label_Enabled.ForeColor = Color.Black;
                    buffer_state.Text = "Unknown";
                    buffer_state.ForeColor = Color.Black;

                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Start stream";
                    btnEnable.Enabled = false;
                    serialPort1.WriteLine("stream\n");
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
                ((Control)this.tabTuning).Enabled = false;
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
                    ((Control)this.tabTuning).Enabled = true;
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
                    if (ports.Length > 0)
                    {
                        serial_box.Items.AddRange(ports);
                        serial_box.SelectedIndex = 0;
                    }
                }
            }
        }





     





        private void timerPlot_Tick(object sender, EventArgs e)
        {
            if (streaming == 1)
            {
                //buffer_state.Text = Convert.ToString(fifo_workload, 3) + " %";
                buffer_state.Text = string.Format("{0:0.00}",fifo_workload) + " %";
                if (fifo_workload > 90.0)
                {
                    buffer_state.ForeColor = Color.Red;
                }
                if (fifo_workload > 70.0)
                {
                    buffer_state.ForeColor = Color.Orange;
                }
                else
                {
                    buffer_state.ForeColor = Color.Lime;
                }
                if (enabled == 1)
                {
                    label_Enabled.ForeColor = Color.Lime;
                    label_Enabled.Text = "Enabled";
                    btnEnable.Text = "Disable";
                }
                else
                {
                    label_Enabled.ForeColor = Color.Red;
                    label_Enabled.Text = "Disabled";
                    btnEnable.Text = "Enable";
                }

                // update angle plot
                anglePlot.Series[0].Points.Clear();
                anglePlot.Series[0].Points.AddXY(0, 0);
                anglePlot.Series[0].Points.AddXY(Math.Cos((y / 100.0) * Math.PI / 180), Math.Sin((y / 100.0) * Math.PI / 180));

                anglePlot.Series[1].Points.Clear();
                anglePlot.Series[1].Points.AddXY(0, 0);
                anglePlot.Series[1].Points.AddXY(Math.Cos((electrical_angle / 100.0) * Math.PI / 180), Math.Sin((electrical_angle / 100.0) * Math.PI / 180));

                pltPosition.Series[0].Points.Clear();
                pltPosition.Series[1].Points.Clear();
                pltError.Series[0].Points.Clear();
                pltError.Series[1].Points.Clear();


                if (Variable_1_pltPosition.Text == "Position")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], yValues[pointer]);
                    }
                }
                else if (Variable_1_pltPosition.Text == "Target")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], rValues[pointer]);
                    }
                }
                else if (Variable_1_pltPosition.Text == "Error")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], eValues[pointer]);
                    }
                }
                else if (Variable_1_pltPosition.Text == "Target Velocity")
                {
                    Variable_1_unit_pltPosition.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], omega_targetValues[pointer]);
                    }
                }
                else if (Variable_1_pltPosition.Text == "Velocity")
                {
                    Variable_1_unit_pltPosition.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], omegaValues[pointer]);
                    }
                }
                else if (Variable_1_pltPosition.Text == "Effort")
                {
                    Variable_1_unit_pltPosition.Text = "[mA]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], uValues[pointer]);
                    }
                }




                if (Variable_2_pltPosition.Text == "Position")
                {
                    Variable_2_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], yValues[pointer]);
                    }
                }
                else if (Variable_2_pltPosition.Text == "Target")
                {
                    Variable_2_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], rValues[pointer]);
                    }
                }
                else if (Variable_2_pltPosition.Text == "Error")
                {
                    Variable_2_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], eValues[pointer]);
                    }
                }
                else if (Variable_2_pltPosition.Text == "Target Velocity")
                {
                    Variable_2_unit_pltPosition.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], omega_targetValues[pointer]);
                    }
                }
                else if (Variable_2_pltPosition.Text == "Velocity")
                {
                    Variable_2_unit_pltPosition.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], omegaValues[pointer]);
                    }
                }
                else if (Variable_2_pltPosition.Text == "Effort")
                {
                    Variable_2_unit_pltPosition.Text = "[mA]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], uValues[pointer]);
                    }
                }




                if (Variable_1_pltError.Text == "Position")
                {
                    Variable_1_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], yValues[pointer]);
                    }
                }
                else if (Variable_1_pltError.Text == "Target")
                {
                    Variable_1_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], rValues[pointer]);
                    }
                }
                else if (Variable_1_pltError.Text == "Error")
                {
                    Variable_1_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], eValues[pointer]);
                    }
                }
                else if (Variable_1_pltError.Text == "Target Velocity")
                {
                    Variable_1_unit_pltError.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], omega_targetValues[pointer]);
                    }
                }
                else if (Variable_1_pltError.Text == "Velocity")
                {
                    Variable_1_unit_pltError.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], omegaValues[pointer]);
                    }
                }
                else if (Variable_1_pltError.Text == "Effort")
                {
                    Variable_1_unit_pltError.Text = "[mA]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], uValues[pointer]);
                    }
                }





                if (Variable_2_pltError.Text == "Position")
                {
                    Variable_2_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], yValues[pointer]);
                    }
                }
                else if (Variable_2_pltError.Text == "Target")
                {
                    Variable_2_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], rValues[pointer]);
                    }
                }
                else if (Variable_2_pltError.Text == "Error")
                {
                    Variable_2_unit_pltError.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], eValues[pointer]);
                    }
                }
                else if (Variable_2_pltError.Text == "Target Velocity")
                {
                    Variable_2_unit_pltError.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], omega_targetValues[pointer]);
                    }
                }
                else if (Variable_2_pltError.Text == "Velocity")
                {
                    Variable_2_unit_pltError.Text = "[deg/s]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], omegaValues[pointer]);
                    }
                }
                else if (Variable_2_pltError.Text == "Effort")
                {
                    Variable_2_unit_pltError.Text = "[mA]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], uValues[pointer]);
                    }
                }

                // Update y axis scaling 
                changeYScala(pltPosition,5);
                changeYScala(pltError,5);

                // update bar plots
                panelTorque.Height = 240 - Torque;
            }

            if (response_income)
            {
                response_plot_variable_SelectedIndexChanged(sender, e);
            }
        }



        private void btnSetTimeframe_Click(object sender,EventArgs ev)
        {
            changing_size = true;
            double frequency = Convert.ToDouble(txtFrequencyFrame.Text);
            if (frequency < 1.0)
            {
                frequency = 1.0;
            }
            else if (frequency > 300.0)
            {
                frequency = 300.0;
            }

            int_frequency = Convert.ToInt32(frequency);
            int max_serial_counter = Convert.ToInt32((5000.0 / int_frequency) + 0.5);
            int_frequency = Convert.ToInt32(5000.0/ max_serial_counter)+1;

            double max_time = 2000.0 / int_frequency;

            double time = Convert.ToDouble(txtTimeframe.Text);
            if (time < 0.1)
            {
                time = 0.1;
            }
            else if (time > max_time)
            {
                time = max_time;
            }


            time = Convert.ToInt32(10 * time)/10.0;

            txtTimeframe.Text = Convert.ToString(time);
            txtFrequencyFrame.Text = Convert.ToString(frequency);

            length = Convert.ToInt32(int_frequency * time);

            Debug.Print(Convert.ToString(length));
            

            
            Array.Clear(uValues, 0, uValues.Length);
            Array.Clear(yValues, 0, yValues.Length);
            Array.Clear(rValues, 0, rValues.Length);
            Array.Clear(eValues, 0, eValues.Length);
            Array.Clear(tValues, 0, tValues.Length);
            Array.Clear(omegaValues, 0, omegaValues.Length);
            Array.Clear(omega_targetValues, 0, omega_targetValues.Length);


            Array.Resize(ref uValues, length);
            Array.Resize(ref yValues, length);
            Array.Resize(ref rValues, length);
            Array.Resize(ref eValues, length);
            Array.Resize(ref tValues, length);
            Array.Resize(ref omegaValues, length);
            Array.Resize(ref omega_targetValues, length);


            for (int i = 0; i < length; i++)
            {
                uValues[i] = Convert.ToInt32((u * 1000.0 * 3.3) / (512.0 * 10.0 * 0.15));
                yValues[i] = y / 100.0;
                rValues[i] = r / 100.0;
                eValues[i] = e / 100.0;
                tValues[i] = time;
                omegaValues[i] = omega;
                omega_targetValues[i] = omega_target;
            }
            if (streaming == 1)
            {

                serialPort1.WriteLine("stream\n");
                Thread.Sleep(500);
                serialPort1.WriteLine("stream\n");
                Thread.Sleep(100);
                serialPort1.Write(Convert.ToString(int_frequency) + "\n");
            }

            wrap_pointer = 1;
            changing_size = false;

        }
        


        private void serial_box_DropDown(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            serial_box.Items.Clear();
            if (ports.Length > 0)
            {
                serial_box.Items.AddRange(ports);
                serial_box.SelectedIndex = 0;
            }
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
                    File.AppendAllText(CSVFileName, "streaming;position;target;error;effort;electrical_angle;enabled \n");
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

                DialogResult result = MessageBox.Show("The motor will vibrate violently, continue?", "Warning",MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
                if (result == DialogResult.Yes)
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("autotune\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("y");
                    int cycles = Convert.ToInt32(txtAutotuneCycles.Text, System.Globalization.CultureInfo.InvariantCulture);
                    String cmd = Convert.ToString(cycles, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");
                    serialPort1.Write(" \n");
                    serialPort1.Write(" \n");
                    serialPort1.Write("load_param\n");
                }
                else if (result == DialogResult.No)
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("autotune\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("n");
                }
                else if (result == DialogResult.Cancel)
                {
                    Thread.Sleep(10);
                    serialPort1.WriteLine("autotune\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("n");
                }


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
                    serialPort1.WriteLine("f\n");
                    Thread.Sleep(10);
                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("a\n");
                    Thread.Sleep(10);
                    double Kacc = Convert.ToDouble(txtKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
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
            if (saveFileDialogConfig.ShowDialog() == DialogResult.OK)
            {
                CSVFileName = saveFileDialogConfig.FileName;
                File.WriteAllText(CSVFileName, "");
                File.AppendAllText(CSVFileName, "#include \"Configuration.h\" \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//-------------------------------------------------- Identifier ------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "//--------------------------------------------------------------------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "// char to identify the mechaduino with the Serial monitor \n");
                File.AppendAllText(CSVFileName, "char identifier = '");
                File.AppendAllText(CSVFileName, txtIdentifier.Text);
                File.AppendAllText(CSVFileName, "'; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//---------------------------------------------- Hardware Section ---------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "//--------------------------------------------------------------------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions) \n");
                File.AppendAllText(CSVFileName, "int iMAX = ");
                File.AppendAllText(CSVFileName, txtCurrent.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//set to 1 if you want to use a enable pin \n");
                if (checkEnable.Checked)
                {
                    File.AppendAllText(CSVFileName, "int USE_ENABLE_PIN = 1; \n");
                }
                else
                {
                    File.AppendAllText(CSVFileName, "int USE_ENABLE_PIN = 0; \n");
                }
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// microstepping setting for step input \n");
                File.AppendAllText(CSVFileName, "int microstepping = ");
                File.AppendAllText(CSVFileName, txtMicrostep.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// fullsteps for 360 degrees \n");
                File.AppendAllText(CSVFileName, "int steps_per_revolution = ");
                File.AppendAllText(CSVFileName, txtFullstep.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// mm per revolution \n");
                File.AppendAllText(CSVFileName, "int mm_rev  =");
                File.AppendAllText(CSVFileName, txtmmRev.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// max error in mm, if the error gets bigger the led turns off \n");
                File.AppendAllText(CSVFileName, "float error_led_value = ");
                File.AppendAllText(CSVFileName, txtMaxE.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//set to 1 to invert your motor direction \n");
                if (checkInvert.Checked)
                {
                    File.AppendAllText(CSVFileName, "int INVERT = 1; \n");
                }
                else
                {
                    File.AppendAllText(CSVFileName, "int INVERT = 0; \n");
                }
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//------------------------------------------------ Motor Section ------------------------------------------------ \n");
                File.AppendAllText(CSVFileName, "//--------------------------------------------------------------------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "// max moment in Nm \n");
                File.AppendAllText(CSVFileName, "float M_max = ");
                File.AppendAllText(CSVFileName, txtMaxM.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// rated current for max moment in mA \n");
                File.AppendAllText(CSVFileName, "int I_rated = ");
                File.AppendAllText(CSVFileName, txtMaxI.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//---------------------------------------------- Controller Section ---------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "//--------------------------------------------------------------------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "//---- PID Values current control ----- \n");
                File.AppendAllText(CSVFileName, "//");
                File.AppendAllText(CSVFileName, txtCurrent.Text);
                File.AppendAllText(CSVFileName, " mA coil current \n");
                File.AppendAllText(CSVFileName, "float Kp = ");
                File.AppendAllText(CSVFileName, txtKp.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, "float Ki = ");
                File.AppendAllText(CSVFileName, txtKi.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, "float Kd = ");
                File.AppendAllText(CSVFileName, txtKd.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "float Kff = ");
                File.AppendAllText(CSVFileName, txtKff.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "float Kacc = ");
                File.AppendAllText(CSVFileName, txtKacc.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "//----------------------------------------------- Filter  Section ----------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "//--------------------------------------------------------------------------------------------------------------- \n");
                File.AppendAllText(CSVFileName, "// break frequency in hertz for DTerm \n");
                File.AppendAllText(CSVFileName, "int D_Term_LPF = ");
                File.AppendAllText(CSVFileName, txtPLPF.Text);
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, "// break frequency in hertz for the effort filter \n");
                File.AppendAllText(CSVFileName, "int u_LPF = ");
                double ulpf = Convert.ToDouble(txtuLPF.Text, System.Globalization.CultureInfo.InvariantCulture);
                File.AppendAllText(CSVFileName, Convert.ToString(ulpf));
                File.AppendAllText(CSVFileName, "; \n");
                File.AppendAllText(CSVFileName, " \n");
                File.AppendAllText(CSVFileName, " \n");
            }
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
                    serialPort1.Write("e\n");
                    Thread.Sleep(10);
                    double max_e = Convert.ToDouble(txtMaxE.Text, System.Globalization.CultureInfo.InvariantCulture);
                    String cmd = Convert.ToString(max_e, System.Globalization.CultureInfo.InvariantCulture);
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

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("f\n");
                    Thread.Sleep(10);
                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + "\n");

                    Thread.Sleep(10);
                    serialPort1.WriteLine("editparam d\n");
                    Thread.Sleep(10);
                    serialPort1.WriteLine("a\n");
                    Thread.Sleep(10);
                    double Kacc = Convert.ToDouble(txtKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
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
                    serialPort1.WriteLine("x\n");
                    Thread.Sleep(10);
                    int uLPF = Convert.ToInt32(txtuLPF.Text);
                    cmd = Convert.ToString(uLPF, System.Globalization.CultureInfo.InvariantCulture);
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
                int frequency = Convert.ToInt32(txtfrequency.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);

                saveFileDialog1.FileName = "step_response_" + cmd_frequency + "Hz";
                saveFileDialog1.Title = "Save xtep response data";

                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();

                response_command_Length = 0;

                serialPort1.Write("stop_stream \n");
                Thread.Sleep(100);
                serialPort1.Write("response \n");
                Thread.Sleep(100);
                serialPort1.Write(cmd_response + "\n");
                Thread.Sleep(100);
                serialPort1.Write(cmd_frequency + "\n");


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
                int frequency = Convert.ToInt32(txtfrequency.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);

                saveFileDialog1.FileName = "dirac_" + cmd_frequency + "Hz";
                saveFileDialog1.Title = "Save dirac data";

                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();

                response_command_Length = 0;

                serialPort1.Write("dirac \n");
                Thread.Sleep(100);
                serialPort1.Write(cmd_frequency + "\n");
            }

        }

        private void timerCSV_Tick(object sender, EventArgs e)
        {


            if (savetoCSV)
            {
                using (StreamWriter sw = File.AppendText(CSVFileName))
                {
                    for (int i = 0; i < buffer_counter; i++)
                    {
                        sw.Write(buffer[i] + "\n");
                    }
                    buffer_counter = 0;
                }
            }
        }

        private void btnRamp_Click(object sender, EventArgs e)
        {
            Ramp = true;
        }

       
        private void btnExport_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {

                CSVFileName = saveFileDialog1.FileName;
                File.WriteAllText(CSVFileName, "");
                File.AppendAllText(CSVFileName, "position;target\n");

                using (FileStream fs = new FileStream(CSVFileName, FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))

                    if (response_position.Length > 0)
                    {

                        for (int i = 0; i < response_position.Length; i++)
                        {
                            sw.Write(Convert.ToString(response_position[i]) + ";" + Convert.ToString(response_target[i]) + "\n");
                        }

                    }

            }

        }
       

        private void button1_Click(object sender, EventArgs e)
        {


            if (serialPort1.IsOpen)
            {
                int frequency = Convert.ToInt32(txtfrequency.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);

                saveFileDialog1.FileName = "test_move_" + cmd_frequency + "Hz";
                saveFileDialog1.Title = "Save test move data";

                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();

                response_command_Length = 0;

                serialPort1.Write("test \n");
                Thread.Sleep(100);
                serialPort1.Write(cmd_frequency + "\n");
            }

        }

       

        private void Variable_1_pltPosition_color_Click(object sender, EventArgs e)
        {
            ColorDialog MyDialog = new ColorDialog();
            // Keeps the user from selecting a custom color.
            MyDialog.AllowFullOpen = false;
            // Allows the user to get help. (The default is false.)
            MyDialog.ShowHelp = true;
            // Sets the initial color select to the current text color.
            MyDialog.Color = Variable_1_pltPosition_color.BackColor;

            // Update the text box color if the user clicks OK 
            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                Variable_1_pltPosition_color.ForeColor = MyDialog.Color;
                pltPosition.Series[0].Color = MyDialog.Color;
            }

        }

        private void Variable_2_pltPosition_color_Click(object sender, EventArgs e)
        {
            ColorDialog MyDialog = new ColorDialog();
            // Keeps the user from selecting a custom color.
            MyDialog.AllowFullOpen = false;
            // Allows the user to get help. (The default is false.)
            MyDialog.ShowHelp = true;
            // Sets the initial color select to the current text color.
            MyDialog.Color = Variable_2_pltPosition_color.ForeColor;

            // Update the text box color if the user clicks OK 
            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                Variable_2_pltPosition_color.ForeColor = MyDialog.Color;
                pltPosition.Series[1].Color = MyDialog.Color;
            }

        }

        private void Variable_1_pltError_color_Click(object sender, EventArgs e)
        {
            ColorDialog MyDialog = new ColorDialog();
            // Keeps the user from selecting a custom color.
            MyDialog.AllowFullOpen = false;
            // Allows the user to get help. (The default is false.)
            MyDialog.ShowHelp = true;
            // Sets the initial color select to the current text color.
            MyDialog.Color = Variable_1_pltError_color.ForeColor;

            // Update the text box color if the user clicks OK 
            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                Variable_1_pltError_color.ForeColor = MyDialog.Color;
                pltError.Series[0].Color = MyDialog.Color;
            }

        }

        private void Variable_2_pltError_color_Click(object sender, EventArgs e)
        {
            ColorDialog MyDialog = new ColorDialog();
            // Keeps the user from selecting a custom color.
            MyDialog.AllowFullOpen = false;
            // Allows the user to get help. (The default is false.)
            MyDialog.ShowHelp = true;
            // Sets the initial color select to the current text color.
            MyDialog.Color = Variable_2_pltError_color.ForeColor;

            // Update the text box color if the user clicks OK 
            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                Variable_2_pltError_color.ForeColor = MyDialog.Color;
                pltError.Series[0].Color = MyDialog.Color;
            }

        }

        private void response_plot_variable_SelectedIndexChanged(object sender, EventArgs e)
        {
            response_income = false;
            pltresponse.Series[0].Points.Clear();
            pltresponse.Series[1].Points.Clear();

            double start_position = 0;
            double start_target = 0;

            int frequency = Convert.ToInt32(txtfrequency.Text);
            double dt = 1 / frequency;

            if (response_plot_variable.Text == "velocity")
            {

                pltresponse.Series[0].Name = "position [deg/s]";
                pltresponse.Series[1].Name = "target [deg/s]";

                for (int j = 0; j < 50; j++)
                {
                    start_position = start_position + omegaValues[j];
                    start_target = start_target + omega_targetValues[j];
                }
                start_position = start_position / 50;
                start_target = start_target / 50;

                for (int j = 0; j < response_command_Length; j++)
                {
                    pltresponse.Series[0].Points.AddXY(j * dt, 5000 * (omegaValues[j] - start_position) / 100.0);
                    pltresponse.Series[1].Points.AddXY(j * dt, 5000 * (omega_targetValues[j] - start_target) / 100.0);
                }
                
            }
            else if (response_plot_variable.Text == "angle")
            {

                pltresponse.Series[0].Name = "position [deg]";
                pltresponse.Series[1].Name = "target [deg]";

                for (int j = 0; j < 50; j++)
                {
                    start_position = start_position + response_position[j];
                    start_target = start_target + response_target[j];
                }
                start_position = start_position / 50;
                start_target = start_target / 50;

                for (int j = 0; j < response_command_Length; j++)
                {
                    pltresponse.Series[0].Points.AddXY(j * dt, (response_position[j] - start_position) / 100.0);
                    pltresponse.Series[1].Points.AddXY(j * dt, (response_target[j] - start_target) / 100.0);
                }
                
            }
            else if (response_plot_variable.Text == "angle error")
            {

                pltresponse.Series[0].Name = "angle error [deg]";
                pltresponse.Series[1].Name = " ";

                for (int j = 0; j < 50; j++)
                {
                    start_position = start_position + (response_target[j] - response_position[j]);
                }
                start_position = start_position / 50;

                for (int j = 0; j < response_command_Length; j++)
                {
                    pltresponse.Series[0].Points.AddXY(j * dt, ((response_target[j] - response_position[j]) - start_position) / 100.0);
                }
                
            }
            else if (response_plot_variable.Text == "velocity error")
            {

                pltresponse.Series[0].Name = "velocity error [deg/s]";
                pltresponse.Series[1].Name = " ";

                for (int j = 0; j < 50; j++)
                {
                    start_position = start_position + (omega_targetValues[j] - omegaValues[j]);
                }
                start_position = start_position / 50;

                for (int j = 0; j < response_command_Length; j++)
                {
                    pltresponse.Series[0].Points.AddXY(j * dt, 5000 * ((omega_targetValues[j] - omegaValues[j]) - start_position) / 100.0);
                }
                
            }


            //response_command_Length = 0;
            changeYScala(pltresponse, 5);
        }
    }


}

