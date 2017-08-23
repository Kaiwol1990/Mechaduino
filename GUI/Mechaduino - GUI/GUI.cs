using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;


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
        double[] electrical_angleValues = new double[1500];
        double[] omegaValues = new double[1500];
        double[] omega_targetValues = new double[1500];


        int[] response_position = new int[1500];
        int[] response_target = new int[1500];

        int[] tuning_position = new int[1500];
        int[] tuning_target = new int[1500];
        int[] tuning_omega_target = new int[1500];
        int[] tuning_omega = new int[1500];

        int tuning_command_Length = 0;
        bool tuning_income = false;




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

        bool listen_to_stream = false;

        bool wasOpen = false;
        bool savetoCSV = false;
        string CSVFileName = "";

        bool response_income = false;
        int response_command_Length = 0;
        


        string value = "";

        string[] buffer = new string[100];
        int buffer_counter = 0;







        public GUI()
        {
            InitializeComponent();
        }

        double max_Kp = 1.0;
        double max_Ki = 0.05;
        double max_Kd = 20.0;
        double max_Kff = 1;
        double max_Kacc = 5;

        double Kp_gain = 100.0;
        double Ki_gain = 2000.0;
        double Kd_gain = 5.0;
        double Kff_gain = 200.0;
        double Kacc_gain = 50.0;

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
            ((Control)this.tabAutotune).Enabled = false;



            txtmaxKp.Text = Convert.ToString(max_Kp, System.Globalization.CultureInfo.InvariantCulture);
            txtmaxKi.Text = Convert.ToString(max_Ki, System.Globalization.CultureInfo.InvariantCulture);
            txtmaxKd.Text = Convert.ToString(max_Kd, System.Globalization.CultureInfo.InvariantCulture);
            txtmaxKff.Text = Convert.ToString(max_Kff, System.Globalization.CultureInfo.InvariantCulture);
            txtmaxKacc.Text = Convert.ToString(max_Kacc, System.Globalization.CultureInfo.InvariantCulture);
            //Calculate_gains_tune_settings();




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
            pltPosition.ChartAreas[0].AxisX.LabelStyle.Format = "#.#";
            pltPosition.ChartAreas[0].AxisY.LabelStyle.Format = "#.##";

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
            pltresponse.ChartAreas[0].AxisX.LabelStyle.Format = "#.#";
            pltresponse.ChartAreas[0].AxisY.LabelStyle.Format = "#.##";


            // autotune plot
            pltAutotune_1.ChartAreas[0].CursorX.IsUserEnabled = Enabled;
            pltAutotune_1.ChartAreas[0].CursorX.IsUserSelectionEnabled = Enabled;
            pltAutotune_1.ChartAreas[0].CursorX.Interval = 0;
            pltAutotune_1.ChartAreas[0].AxisX.ScaleView.Zoomable = Enabled;
            pltAutotune_1.ChartAreas[0].AxisX.ScrollBar.IsPositionedInside = true;
            pltAutotune_1.ChartAreas[0].AxisX.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltAutotune_1.ChartAreas[0].AxisX.ScaleView.SmallScrollMinSize = 0;

            pltAutotune_1.ChartAreas[0].CursorY.IsUserEnabled = Enabled;
            pltAutotune_1.ChartAreas[0].CursorY.IsUserSelectionEnabled = Enabled;
            pltAutotune_1.ChartAreas[0].CursorY.Interval = 0;
            pltAutotune_1.ChartAreas[0].AxisY.ScaleView.Zoomable = Enabled;
            pltAutotune_1.ChartAreas[0].AxisY.ScrollBar.IsPositionedInside = true;
            pltAutotune_1.ChartAreas[0].AxisY.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltAutotune_1.ChartAreas[0].AxisY.ScaleView.SmallScrollMinSize = 0;
            pltAutotune_1.Series[1].Color = Color.Red;
            pltAutotune_1.ChartAreas[0].AxisX.LabelStyle.Format = "#.#";
            pltAutotune_1.ChartAreas[0].AxisY.LabelStyle.Format = "#.##";

            // autotune plot
            pltAutotune_2.ChartAreas[0].CursorX.IsUserEnabled = Enabled;
            pltAutotune_2.ChartAreas[0].CursorX.IsUserSelectionEnabled = Enabled;
            pltAutotune_2.ChartAreas[0].CursorX.Interval = 0;
            pltAutotune_2.ChartAreas[0].AxisX.ScaleView.Zoomable = Enabled;
            pltAutotune_2.ChartAreas[0].AxisX.ScrollBar.IsPositionedInside = true;
            pltAutotune_2.ChartAreas[0].AxisX.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltAutotune_2.ChartAreas[0].AxisX.ScaleView.SmallScrollMinSize = 0;

            pltAutotune_2.ChartAreas[0].CursorY.IsUserEnabled = Enabled;
            pltAutotune_2.ChartAreas[0].CursorY.IsUserSelectionEnabled = Enabled;
            pltAutotune_2.ChartAreas[0].CursorY.Interval = 0;
            pltAutotune_2.ChartAreas[0].AxisY.ScaleView.Zoomable = Enabled;
            pltAutotune_2.ChartAreas[0].AxisY.ScrollBar.IsPositionedInside = true;
            pltAutotune_2.ChartAreas[0].AxisY.ScrollBar.ButtonStyle = System.Windows.Forms.DataVisualization.Charting.ScrollBarButtonStyles.SmallScroll;
            pltAutotune_2.ChartAreas[0].AxisY.ScaleView.SmallScrollMinSize = 0;
            pltAutotune_2.Series[1].Color = Color.Red;
            pltAutotune_2.ChartAreas[0].AxisX.LabelStyle.Format = "#.#";
            pltAutotune_2.ChartAreas[0].AxisY.LabelStyle.Format = "#.##";
        }



        private delegate void processValueDelegate(string value);

        private void port_DataReceived_1(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string InputData = serialPort1.ReadLine();
                if (InputData != string.Empty)
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
                if (!string.IsNullOrEmpty(value))
                {
                    string[] substrings_command = value.Split(',');

                    if (substrings_command.Length > 1)
                    {
                        string Command = substrings_command[0];

                        substrings_command[1] = substrings_command[1].Replace("\r", "");
                        substrings_command[1] = substrings_command[1].Replace("\n", "");

                        string argument = substrings_command[1];

                        string[] substrings = argument.Split(';');
                        try
                        {
                            Handle_Command(value, Command, substrings);
                        }
                        catch
                        {
                            txtReceived.AppendText(argument + "\n");
                            txtReceived.ScrollToCaret();
                        }


                    }
                    else
                    {
                        txtReceived.AppendText(value + "\n");
                        txtReceived.ScrollToCaret();
                    }
                }
            }
        }

        private void Handle_Command(string value, string Command, string[] substrings)
        {
            switch (Command)
            {
                // Stream received -> send to stream tab
                case "s":

                    if (listen_to_stream == true)
                    {
                        streaming = 1;
                    }

                    dt = Convert.ToInt32(substrings[0]) / 1000000.0;

                    Torque = (Math.Abs(u) * 240) / uMax;

                    if (savetoCSV)
                    {
                        buffer[buffer_counter] = value;
                        buffer_counter += 1;
                    }

                    r = Convert.ToInt32(substrings[1]);
                    y = Convert.ToInt32(substrings[2]);
                    e = Convert.ToInt32(substrings[3]);
                    omega_target = (Convert.ToInt32(substrings[4]));
                    omega = (Convert.ToInt32(substrings[5]));


                    if (((0.9 * u) + (0.1 * Convert.ToInt32(substrings[6]))) != 0.0)
                    {
                        enabled = 1;
                    }
                    else
                    {
                        enabled = 0;
                    }

                    u = Convert.ToInt32(substrings[6]);

                    electrical_angle = (Convert.ToInt32(substrings[7]));
                    fifo_workload = (Convert.ToDouble(substrings[8]));


                    time = time + dt;

                    tValues[wrap_pointer] = time;

                    rValues[wrap_pointer] = r / 100.0;
                    yValues[wrap_pointer] = y / 100.0;
                    eValues[wrap_pointer] = e / 100.0;
                    electrical_angleValues[wrap_pointer] = electrical_angle / 100.0;
                    omega_targetValues[wrap_pointer] = omega_target / 100.0;
                    omegaValues[wrap_pointer] = omega / 100.0;
                    uValues[wrap_pointer] = (u * 1000.0 * 3.3) / (512.0 * 10.0 * 0.15);


                    wrap_pointer += 1;
                    wrap_pointer = wrap_pointer % yValues.Length;
                    break;

                // Paramter received -> send to parameter tab
                case "p":
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
                    txterrorLPF.Text = substrings[9];
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

                    txtKff.Text = substrings[14];
                    txtKacc.Text = substrings[15];


                    TxttuneKp.Text = txtKp.Text;
                    txttuneKi.Text = txtKi.Text;
                    txttuneKd.Text = txtKd.Text;
                    txttuneKff.Text = txtKff.Text;
                    txttuneKacc.Text = txtKacc.Text;
                    

                    trackBarKp.Value = Convert.ToInt32((Kp_gain * Convert.ToDouble(txtKp.Text, System.Globalization.CultureInfo.InvariantCulture)));
                    trackBarKi.Value = Convert.ToInt32((Ki_gain * Convert.ToDouble(txtKi.Text, System.Globalization.CultureInfo.InvariantCulture)));
                    trackBarKd.Value = Convert.ToInt32((Kd_gain * Convert.ToDouble(txtKd.Text, System.Globalization.CultureInfo.InvariantCulture)));
                    trackBarKff.Value = Convert.ToInt32((Kff_gain * Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture)));
                    trackBarKacc.Value = Convert.ToInt32((Kacc_gain * Convert.ToDouble(txtKacc.Text, System.Globalization.CultureInfo.InvariantCulture)));


                    break;


                // Tuning Values received
                case "t":
                    response_target[response_command_Length] = Convert.ToInt32(substrings[0]);
                    response_position[response_command_Length] = Convert.ToInt32(substrings[1]);
                    omega_targetValues[response_command_Length] = Convert.ToInt32(substrings[2]);
                    omegaValues[response_command_Length] = Convert.ToInt32(substrings[3]);

                    response_command_Length = response_command_Length + 1;
                    if (response_command_Length == 1000)
                    {
                        response_income = true;
                    }

                    break;

                case "a":
                    tuning_target[tuning_command_Length] = Convert.ToInt32(substrings[0]);
                    tuning_position[tuning_command_Length] = Convert.ToInt32(substrings[1]);
                    tuning_omega_target[tuning_command_Length] = Convert.ToInt32(substrings[2]);
                    tuning_omega[tuning_command_Length] = Convert.ToInt32(substrings[3]);

                    tuning_command_Length = tuning_command_Length + 1;
                    if (tuning_command_Length == 1000)
                    {
                        tuning_income = true;
                    }

                    break;


                // Human information received -> append to serial txtbox
                default:
                    txtReceived.AppendText(value + "\n");
                    txtReceived.ScrollToCaret();
                    break;
            }
        }



        private void changeYScala(object chart, double percent)
        {
            double temp_max = double.MinValue;
            double temp_min = double.MaxValue;

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

            temp_min = temp_min - diff * (percent / 100);
            temp_max = temp_max + diff * (percent / 100);

            temp_min = Math.Floor(temp_min * 100) / 100.0;
            temp_max = Math.Ceiling(temp_max * 100) / 100.0;

            if (temp_min == temp_max)
            {
                temp_min = temp_min - 1;
                temp_max = temp_max + 1;
            }
            if (temp_max < temp_min)
            {
                temp_max = temp_min + 1;
            }
            if (temp_min > temp_max)
            {
                temp_min = temp_max - 1;
            }

            tmpChart.ChartAreas[0].AxisY.Maximum = temp_max;
            tmpChart.ChartAreas[0].AxisY.Minimum = temp_min;
            tmpChart.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
        }







        private void btnStream_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                if (streaming == 0)
                {
                    listen_to_stream = true;

                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Stop stream";
                    btnEnable.Enabled = true;

                    pltPosition.Series[0].Points.Clear();
                    pltPosition.Series[1].Points.Clear();
                    pltError.Series[0].Points.Clear();
                    pltError.Series[1].Points.Clear();

                    serialPort1.Write("stream -on -f " + Convert.ToString(int_frequency) + " \r");
                }
                else
                {
                    listen_to_stream = false;
                    label_Enabled.Text = "Unknown";
                    label_Enabled.ForeColor = Color.Black;
                    buffer_state.Text = "Unknown";
                    buffer_state.ForeColor = Color.Black;

                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Start stream";
                    btnEnable.Enabled = false;
                    serialPort1.Write("stream -off\r");
                    streaming = 0;
                }
            }
        }

        private int mod(int xMod, int mMod)
        {
            return (xMod % mMod + mMod) % mMod;
        }

        private void btnEnable_Click(object sender, EventArgs e)
        {
            if (enabled == 1)
            {
                serialPort1.Write("state -off\r");
            }
            else
            {
                serialPort1.Write("state -on\r");
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
                ((Control)this.tabAutotune).Enabled = false;
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
                    ((Control)this.tabAutotune).Enabled = true;
                    Thread.Sleep(100);

                    serialPort1.DiscardInBuffer();
                    txtReceived.Text = "";
                    wasOpen = true;

                    btnStream.Enabled = true;
                    btnOpen.Text = "Close";

                    txtConsole.Text = "Serial port open";
                    txtConsole.BackColor = Color.White;
                    serialPort1.Write("stream -off\r");
                    Get_parameter();
                    txtReceived.Clear();
                }
                else
                {
                    txtConsole.Text = "unable to open serial port!";
                    txtConsole.BackColor = Color.Red;
                }
            }

        }

        private void Get_parameter()
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("parameter -gui\r");
            }
        }


        private void BtnSend_Click_1(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(txtSend.Text + " \r");
                txtSend.Clear();
            }
            else
            {
                txtConsole.Text = "No Serial Port avaiable to send to";
                txtConsole.BackColor = Color.Red;
            }
        }





        private void TimerCheck_Tick(object sender, EventArgs e)
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











        private void TimerPlot_Tick(object sender, EventArgs e)
        {
            if (streaming == 1)
            {
                //buffer_state.Text = Convert.ToString(fifo_workload, 3) + " %";
                buffer_state.Text = string.Format("{0:0.00}", fifo_workload) + " %";
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
                else if (Variable_1_pltPosition.Text == "Electrical Angle")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[0].Points.AddXY(tValues[pointer], electrical_angleValues[pointer]);
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
                else if (Variable_2_pltPosition.Text == "Electrical Angle")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltPosition.Series[1].Points.AddXY(tValues[pointer], electrical_angleValues[pointer]);
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
                else if (Variable_1_pltError.Text == "Electrical Angle")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[0].Points.AddXY(tValues[pointer], electrical_angleValues[pointer]);
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
                else if (Variable_2_pltError.Text == "Electrical Angle")
                {
                    Variable_1_unit_pltPosition.Text = "[deg]";
                    for (int i = 0; i <= yValues.Length - 1; i++)
                    {
                        int pointer = wrap_pointer + i;
                        pointer = pointer % yValues.Length;
                        pltError.Series[1].Points.AddXY(tValues[pointer], electrical_angleValues[pointer]);
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
                changeYScala(pltPosition, 5);
                changeYScala(pltError, 5);

                // update bar plots
                panelTorque.Height = 240 - Torque;
            }

            if (response_income)
            {
                Response_plot_variable_SelectedIndexChanged(sender, e);
            }

            if (tuning_income)
            {
                Plot_tune(sender, e);
            }
        }



        private void BtnSetTimeframe_Click(object sender, EventArgs ev)
        {
            double frequency = Convert.ToDouble(txtFrequencyFrame.Text);
            if (frequency < 1.0)
            {
                frequency = 1.0;
            }
            else if (frequency > 500.0)
            {
                frequency = 500.0;
            }

            int_frequency = Convert.ToInt32(frequency);
            int max_serial_counter = Convert.ToInt32((5000.0 / int_frequency) + 0.5);
            int_frequency = Convert.ToInt32(5000.0 / max_serial_counter) + 1;

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


            time = Convert.ToInt32(10 * time) / 10.0;

            txtTimeframe.Text = Convert.ToString(time);
            txtFrequencyFrame.Text = Convert.ToString(frequency);

            length = Convert.ToInt32(int_frequency * time);




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

            serialPort1.Write("stream -f " + Convert.ToString(int_frequency) + " \r");

            wrap_pointer = 1;

        }



        private void Serial_box_DropDown(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            serial_box.Items.Clear();
            if (ports.Length > 0)
            {
                serial_box.Items.AddRange(ports);
                serial_box.SelectedIndex = 0;
            }
        }

        private void Btnp100_Click(object sender, EventArgs e)
        {
            Setpoint(100);
        }

        private void Btnp10_Click(object sender, EventArgs e)
        {
            Setpoint(10);
        }

        private void Btnp1_Click(object sender, EventArgs e)
        {
            Setpoint(1);
        }

        private void Btnp01_Click(object sender, EventArgs e)
        {
            Setpoint(0.1);
        }

        private void Btnn01_Click(object sender, EventArgs e)
        {
            Setpoint(-0.1);
        }
        private void Btnn1_Click(object sender, EventArgs e)
        {
            Setpoint(-1);
        }

        private void Btnn10_Click(object sender, EventArgs e)
        {
            Setpoint(-10);
        }

        private void Btnn100_Click(object sender, EventArgs e)
        {
            Setpoint(-100);
        }



        private void Setpoint(double angle)
        {
            if (serialPort1.IsOpen)
            {
                string cmd = Convert.ToString(((Convert.ToDouble(r) + (angle * 100.0)) / 100.0), System.Globalization.CultureInfo.InvariantCulture);
                serialPort1.Write("setpoint -set " + cmd + " \r");
            }


        }

        private void BtnSetpoint_Click(object sender, EventArgs e)
        {
            string cmd = Convert.ToString(Convert.ToDouble(txtSetpoint.Text), System.Globalization.CultureInfo.InvariantCulture);

            if (serialPort1.IsOpen)
            {
                serialPort1.Write("setpoint -set " + cmd + " \r");
            }
        }

        private void BtnResetSetpoint_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                if (enabled == 1)
                {
                    serialPort1.Write("state -off\r");
                    Thread.Sleep(50);
                    serialPort1.Write("state -on\r");
                }
            }

        }

        private void BtnGetpoint_Click(object sender, EventArgs e)
        {
            txtSetpoint.Text = Convert.ToString(r / 100.0);

        }

        private void BtnJump_Click(object sender, EventArgs e)
        {
            string cmd = Convert.ToString((Convert.ToDouble(r) + 100 * Convert.ToDouble(txtJump.Text)) / 100.0, System.Globalization.CultureInfo.InvariantCulture);
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("setpoint -set " + cmd + " \r");
            }
        }

        public void CheckBoxCSV_CheckedChanged(object sender, EventArgs e)
        {
            if (!savetoCSV)
            {

                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    CSVFileName = saveFileDialog1.FileName;
                    File.WriteAllText(CSVFileName, "");
                    File.AppendAllText(CSVFileName, "streaming;position;target;error;effort;electrical_angle;enabled\r");
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

        private void BtnAutotuneRun_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {

                DialogResult result = MessageBox.Show("The motor will vibrate violently, continue?", "Warning", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
                if (result == DialogResult.Yes)
                {
                    int cycles = Convert.ToInt32(txtAutotuneCycles.Text, System.Globalization.CultureInfo.InvariantCulture);
                    string cmd = Convert.ToString(cycles, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write("autotune -c " + cmd + " -gui \r");
                }


            }
        }

        private void BtnRunCalibration_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                string cmd = "calibrate";
                serialPort1.Write(cmd + " \r");
            }
        }

        private void BtnSendAll_Click(object sender, EventArgs e)
        {

            if (serialPort1.IsOpen)
            {
                try
                {
                    string cmd = "";

                    string identifier = (txtIdentifier.Text);
                    cmd = "parameter -set -identifier " + Convert.ToString(identifier, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kp = Convert.ToDouble(txtKp.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kp " + Convert.ToString(Kp, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Ki = Convert.ToDouble(txtKi.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Ki " + Convert.ToString(Ki, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kd = Convert.ToDouble(txtKd.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kd " + Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kff " + Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kacc = Convert.ToDouble(txtKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kacc " + Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int steps_per_revolution = Convert.ToInt32(txtFullstep.Text);
                    cmd = "parameter -set -steps_per_revolution " + Convert.ToString(steps_per_revolution, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int microstepping = Convert.ToInt32(txtMicrostep.Text);
                    cmd = "parameter -set -microstepping " + Convert.ToString(microstepping, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int iMax = Convert.ToInt32(txtCurrent.Text);
                    cmd = "parameter -set -iMax " + Convert.ToString(iMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double max_e = Convert.ToDouble(txtMaxE.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -error_led_value " + Convert.ToString(max_e, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double mMax = Convert.ToDouble(txtMaxM.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -M_Max " + Convert.ToString(mMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int i_rated = Convert.ToInt32(txtMaxI.Text);
                    cmd = "parameter -set -I_rated " + Convert.ToString(i_rated, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int pLPF = Convert.ToInt32(txterrorLPF.Text);
                    cmd = "parameter -set -D_Term_LPF " + Convert.ToString(pLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                    
                    int mm_rev = Convert.ToInt32(txtmmRev.Text);
                    cmd = "parameter -set -mm_rev " + Convert.ToString(mm_rev, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int enable = Convert.ToInt32(checkEnable.Checked);
                    cmd = "parameter -set -USE_ENABLE_PIN " + Convert.ToString(enable, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int invert = Convert.ToInt32(checkInvert.Checked);
                    cmd = "parameter -set -INVERT " + Convert.ToString(invert, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                }
                catch
                {
                }
            }
        }



        private void BtnSendIdentifier_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    string identifier = (txtIdentifier.Text);
                    string cmd = "parameter -set -identifier " + Convert.ToString(identifier, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }







        private void BtnSendStepSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    int steps_per_revolution = Convert.ToInt32(txtFullstep.Text);
                    string cmd = "parameter -set -steps_per_revolution " + Convert.ToString(steps_per_revolution, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int microstepping = Convert.ToInt32(txtMicrostep.Text);
                    cmd = "parameter -set -microstepping " + Convert.ToString(microstepping, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }






        private void BtnSendMotorSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {

                    int iMax = Convert.ToInt32(txtCurrent.Text);
                    string cmd = "parameter -set -iMAX " + Convert.ToString(iMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double mMax = Convert.ToDouble(txtMaxM.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set - M_max " + Convert.ToString(mMax, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int i_rated = Convert.ToInt32(txtMaxI.Text);
                    cmd = "parameter -set -I_rated " + Convert.ToString(i_rated, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }






        private void BtnSendPins_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    int enable = Convert.ToInt32(checkEnable.Checked);
                    string cmd = "parameter -set -USE_ENABLE_PIN " + Convert.ToString(enable, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int invert = Convert.ToInt32(checkInvert.Checked);
                    cmd = "parameter -set -INVERT " + Convert.ToString(invert, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }







        private void BtnSendKinematics_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {

                    double max_e = Convert.ToDouble(txtMaxE.Text, System.Globalization.CultureInfo.InvariantCulture);
                    string cmd = "parameter -set -error_led_value " + Convert.ToString(max_e, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    int mm_rev = Convert.ToInt32(txtmmRev.Text);
                    cmd = "parameter -set -mm_rev " + Convert.ToString(mm_rev, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }





        private void BtnSendControllerSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    double Kp = Convert.ToDouble(txtKp.Text, System.Globalization.CultureInfo.InvariantCulture);
                    string cmd = "parameter -set -Kp " + Convert.ToString(Kp, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Ki = Convert.ToDouble(txtKi.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Ki " + Convert.ToString(Ki, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kd = Convert.ToDouble(txtKd.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kd " + Convert.ToString(Kd, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kff = Convert.ToDouble(txtKff.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kff " + Convert.ToString(Kff, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                    double Kacc = Convert.ToDouble(txtKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
                    cmd = "parameter -set -Kacc " + Convert.ToString(Kacc, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");
                }
                catch
                {
                }
            }
        }





        private void BtnSendFilterSettings_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    int errorLPF = Convert.ToInt32(txterrorLPF.Text);
                    string cmd = "parameter -set -error_LPF " + Convert.ToString(errorLPF, System.Globalization.CultureInfo.InvariantCulture);
                    serialPort1.Write(cmd + " \r");

                }
                catch
                {
                }
            }
        }









        private void BtnLoad_Click(object sender, EventArgs e)
        {
            Get_parameter();
        }


        private void BtnSave_Click(object sender, EventArgs e)
        {
            if (saveFileDialogConfig.ShowDialog() == DialogResult.OK)
            {
                CSVFileName = saveFileDialogConfig.FileName;
                File.WriteAllText(CSVFileName, "");
                File.AppendAllText(CSVFileName, "#include \"Configuration.h\" \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//-------------------------------------------------- Identifier -------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------------------------------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "// String to identify the mechaduino with the Serial monitor\r");
                File.AppendAllText(CSVFileName, "String identifier = \"");
                File.AppendAllText(CSVFileName, txtIdentifier.Text);
                File.AppendAllText(CSVFileName, "\";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------- Hardware Section ----------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------------------------------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)\r");
                File.AppendAllText(CSVFileName, "int iMAX = ");
                File.AppendAllText(CSVFileName, txtCurrent.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//set to 1 if you want to use a enable pin\r");
                if (checkEnable.Checked)
                {
                    File.AppendAllText(CSVFileName, "int USE_ENABLE_PIN = 1;\r");
                }
                else
                {
                    File.AppendAllText(CSVFileName, "int USE_ENABLE_PIN = 0;\r");
                }
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "// microstepping setting for step input\r");
                File.AppendAllText(CSVFileName, "int microstepping = ");
                File.AppendAllText(CSVFileName, txtMicrostep.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "// fullsteps for 360 degrees\r");
                File.AppendAllText(CSVFileName, "int steps_per_revolution = ");
                File.AppendAllText(CSVFileName, txtFullstep.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "// mm per revolution\r");
                File.AppendAllText(CSVFileName, "int mm_rev  =");
                File.AppendAllText(CSVFileName, txtmmRev.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "// max error in mm, if the error gets bigger the led turns off\r");
                File.AppendAllText(CSVFileName, "float error_led_value = ");
                File.AppendAllText(CSVFileName, txtMaxE.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//set to 1 to invert your motor direction\r");
                if (checkInvert.Checked)
                {
                    File.AppendAllText(CSVFileName, "int INVERT = 1;\r");
                }
                else
                {
                    File.AppendAllText(CSVFileName, "int INVERT = 0;\r");
                }
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//------------------------------------------------ Motor Section ------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------------------------------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "// max moment in Nm\r");
                File.AppendAllText(CSVFileName, "float M_max = ");
                File.AppendAllText(CSVFileName, txtMaxM.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "// rated current for max moment in mA\r");
                File.AppendAllText(CSVFileName, "int I_rated = ");
                File.AppendAllText(CSVFileName, txtMaxI.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------- Controller Section ----------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------------------------------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---- PID Values current control -----\r");
                File.AppendAllText(CSVFileName, "//");
                File.AppendAllText(CSVFileName, txtCurrent.Text);
                File.AppendAllText(CSVFileName, " mA coil current\r");
                File.AppendAllText(CSVFileName, "float Kp = ");
                File.AppendAllText(CSVFileName, txtKp.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, "float Ki = ");
                File.AppendAllText(CSVFileName, txtKi.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, "float Kd = ");
                File.AppendAllText(CSVFileName, txtKd.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "float Kff = ");
                File.AppendAllText(CSVFileName, txtKff.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "float Kacc = ");
                File.AppendAllText(CSVFileName, txtKacc.Text);
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, "//----------------------------------------------- Filter  Section -----------------------------------------------\r");
                File.AppendAllText(CSVFileName, "//---------------------------------------------------------------------------------------------------------------\r");
                File.AppendAllText(CSVFileName, "// break frequency in hertz for the error filter\r");
                File.AppendAllText(CSVFileName, "int error_LPF = ");
                double errorLPF = Convert.ToDouble(txterrorLPF.Text, System.Globalization.CultureInfo.InvariantCulture);
                File.AppendAllText(CSVFileName, Convert.ToString(errorLPF));
                File.AppendAllText(CSVFileName, ";\r");
                File.AppendAllText(CSVFileName, " \r");
                File.AppendAllText(CSVFileName, " \r");
            }
        }


        
        private void Btnmp100_Click(object sender, EventArgs e)
        {
            Setpoint(100 * 1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void Btnmp10_Click(object sender, EventArgs e)
        {
            Setpoint(10 * 1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void Btnmp1_Click(object sender, EventArgs e)
        {
            Setpoint(1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void Btnmn1_Click(object sender, EventArgs e)
        {
            Setpoint(-1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void Btnmn10_Click(object sender, EventArgs e)
        {
            Setpoint(-10 * 1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void Btnmn100_Click(object sender, EventArgs e)
        {
            Setpoint(-100 * 1.8 / Convert.ToInt32(txtMicrostep.Text));
        }

        private void BtnStepResponse_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                int steps = Convert.ToInt32(txtStepResponse.Text);
                string cmd_response = Convert.ToString(steps, System.Globalization.CultureInfo.InvariantCulture);
                int frequency = Convert.ToInt32(txtfrequency.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);

                saveFileDialog1.FileName = "step_response_" + cmd_frequency + "Hz";
                saveFileDialog1.Title = "Save step response data";

                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();

                response_command_Length = 0;

                serialPort1.Write("stream -off\r");
                Thread.Sleep(100);
                serialPort1.Write("response -f " + cmd_frequency + " -s " + cmd_response + " \r");


            }
        }
        
        private void BtnDirac_Click(object sender, EventArgs e)
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

                serialPort1.Write("dirac -f " + cmd_frequency + " \r");
            }

        }

        private void TimerCSV_Tick(object sender, EventArgs e)
        {


            if (savetoCSV)
            {
                using (StreamWriter sw = File.AppendText(CSVFileName))
                {
                    for (int i = 0; i < buffer_counter; i++)
                    {
                        sw.Write(buffer[i] + " \r");
                    }
                    buffer_counter = 0;
                }
            }
        }



        private void BtnExport_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {

                CSVFileName = saveFileDialog1.FileName;
                File.WriteAllText(CSVFileName, "");
                File.AppendAllText(CSVFileName, "position;target\r");

                using (FileStream fs = new FileStream(CSVFileName, FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))

                    if (response_position.Length > 0)
                    {

                        for (int i = 0; i < response_position.Length; i++)
                        {
                            sw.Write(Convert.ToString(response_position[i]) + ";" + Convert.ToString(response_target[i]) + " \r");
                        }

                    }

            }

        }

        
        private void BtnSend_Click(object sender, EventArgs e)
        {


            if (serialPort1.IsOpen)
            {
                int frequency = Convert.ToInt32(txtfrequency.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);


                int velocity = Convert.ToInt32(txtStepVelocity.Text);
                string cmd_velocity = Convert.ToString(velocity, System.Globalization.CultureInfo.InvariantCulture);

                saveFileDialog1.FileName = "test_move_" + cmd_frequency + "Hz";
                saveFileDialog1.Title = "Save test move data";

                pltresponse.Series[0].Points.Clear();
                pltresponse.Series[1].Points.Clear();

                response_command_Length = 0;

                serialPort1.Write("testmove -f " + cmd_frequency + " -v " + cmd_velocity + " -o\r");
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
                pltError.Series[1].Color = MyDialog.Color;
            }

        }

        private void Response_plot_variable_SelectedIndexChanged(object sender, EventArgs e)
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

                for (int j = 0; j < response_command_Length; j++)
                {
                    pltresponse.Series[0].Points.AddXY(j * dt, (omegaValues[j]) / 100.0);
                    pltresponse.Series[1].Points.AddXY(j * dt, (omega_targetValues[j]) / 100.0);
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
                    pltresponse.Series[1].Points.AddXY(j * dt, (response_target[j] - start_position) / 100.0);
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
                    pltresponse.Series[0].Points.AddXY(j * dt, Convert.ToInt32(txtfrequency.Text) * ((omega_targetValues[j] - omegaValues[j]) - start_position) / 100.0);
                }

            }


            changeYScala(pltresponse, 5);
        }


        // gets called if a set of tuning data is received 
        private void Plot_tune(object sender, EventArgs e)
        {

            tuning_income = false;

            pltAutotune_1.Series[0].Points.Clear();
            pltAutotune_1.Series[1].Points.Clear();
            pltAutotune_2.Series[0].Points.Clear();
            pltAutotune_2.Series[1].Points.Clear();

            double start_position = 0;
            double dt = 1 / 5000;

            for (int j = 0; j < 50; j++)
            {
                start_position = start_position + tuning_position[j];
            }
            start_position = start_position / 50;


            for (int j = 0; j < tuning_command_Length; j++)
            {
                pltAutotune_1.Series[0].Points.AddXY(j * dt, (tuning_position[j] - start_position) / 100.0);
                pltAutotune_1.Series[1].Points.AddXY(j * dt, (tuning_target[j] - start_position) / 100.0);
            }


            for (int j = 0; j < tuning_command_Length; j++)
            {
                pltAutotune_2.Series[0].Points.AddXY(j * dt, (tuning_omega[j]) / 100.0);
                pltAutotune_2.Series[1].Points.AddXY(j * dt, (tuning_omega_target[j]) / 100.0);
            }

            tuning_command_Length = 0;

            changeYScala(pltAutotune_1, 5);

        }

        private void BtnDownhill_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {

                string cmd = "parameter -set -Kp " + TxttuneKp.Text + " -Ki " + txttuneKi.Text + " -Kd " + txttuneKd.Text;
                serialPort1.Write(cmd + " \r");
                cmd = "parameter -set -Kff " + txttuneKff.Text + " -Kacc " + txttuneKacc.Text;
                serialPort1.Write(cmd + " \r");

                int frequency = Convert.ToInt32(txttunefrequency.Text);
                int velocity = Convert.ToInt32(txttunevelocity.Text);
                string cmd_frequency = Convert.ToString(frequency, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_velocity = Convert.ToString(velocity, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_max_Kp = Convert.ToString(max_Kp, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_max_Ki = Convert.ToString(max_Ki, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_max_Kd = Convert.ToString(max_Kd, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_max_Kff = Convert.ToString(max_Kff, System.Globalization.CultureInfo.InvariantCulture);
                string cmd_max_Kacc = Convert.ToString(max_Kacc, System.Globalization.CultureInfo.InvariantCulture);

                cmd = "downhill -f " + cmd_frequency + " -v " + cmd_velocity + " -gui -p " + cmd_max_Kp + " -i " + cmd_max_Ki + " -d " + cmd_max_Kd + " -kf " + cmd_max_Kff + " -ka " + cmd_max_Kacc;

                serialPort1.Write(cmd + "\r");
            }
        }

        private void BtnCancle_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Write("c \r");
            }

        }


        private void TrackBarKp_Scroll(object sender, EventArgs e)
        {
            TxttuneKp.Text = Convert.ToString(Convert.ToDouble(trackBarKp.Value) / Kp_gain, System.Globalization.CultureInfo.InvariantCulture);
        }

        private void TrackBarKi_Scroll(object sender, EventArgs e)
        {
            txttuneKi.Text = Convert.ToString(Convert.ToDouble(trackBarKi.Value) / Ki_gain, System.Globalization.CultureInfo.InvariantCulture);
        }

        private void TrackBarKd_Scroll(object sender, EventArgs e)
        {
            txttuneKd.Text = Convert.ToString(Convert.ToDouble(trackBarKd.Value) / Kd_gain, System.Globalization.CultureInfo.InvariantCulture);
        }

        private void TrackBarKff_Scroll(object sender, EventArgs e)
        {
            txttuneKff.Text = Convert.ToString(Convert.ToDouble(trackBarKff.Value) / Kff_gain, System.Globalization.CultureInfo.InvariantCulture);
        }

        private void TrackBarKacc_Scroll(object sender, EventArgs e)
        {
            txttuneKacc.Text = Convert.ToString(Convert.ToDouble(trackBarKacc.Value) / Kacc_gain, System.Globalization.CultureInfo.InvariantCulture);
        }
        

        private void Pltresponse_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            pltresponse.ChartAreas[0].AxisX.ScaleView.ZoomReset(0);
            pltresponse.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
        }

        private void TxtSend_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                BtnSend_Click_1(sender, e);
            }
        }

        private void TxtFrequencyFrame_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                BtnSetTimeframe_Click(sender, e);
            }
            
        }

        private void TxtTimeframe_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                BtnSetTimeframe_Click(sender, e);
            }
        }

        private void PltAutotune_1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            pltAutotune_1.ChartAreas[0].AxisX.ScaleView.ZoomReset(0);
            pltAutotune_1.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
        }

        private void PltAutotune_2_Click(object sender, EventArgs e)
        {
            pltAutotune_2.ChartAreas[0].AxisX.ScaleView.ZoomReset(0);
            pltAutotune_2.ChartAreas[0].AxisY.ScaleView.ZoomReset(0);
        }

        private void BtnReload_Click(object sender, EventArgs e)
        {
            Get_parameter();
        }

        private void Calculate_gains_tune_settings()
        {
            double Kp_val = Convert.ToDouble(TxttuneKp.Text, System.Globalization.CultureInfo.InvariantCulture);
            double Ki_val = Convert.ToDouble(txttuneKi.Text, System.Globalization.CultureInfo.InvariantCulture);
            double Kd_val = Convert.ToDouble(txttuneKd.Text, System.Globalization.CultureInfo.InvariantCulture);
            double Kff_val = Convert.ToDouble(txttuneKff.Text, System.Globalization.CultureInfo.InvariantCulture);
            double Kacc_val = Convert.ToDouble(txttuneKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
          

            max_Kp = Convert.ToDouble(txtmaxKp.Text, System.Globalization.CultureInfo.InvariantCulture);
            max_Ki = Convert.ToDouble(txtmaxKi.Text, System.Globalization.CultureInfo.InvariantCulture);
            max_Kd = Convert.ToDouble(txtmaxKd.Text, System.Globalization.CultureInfo.InvariantCulture);
            max_Kff = Convert.ToDouble(txtmaxKff.Text, System.Globalization.CultureInfo.InvariantCulture);
            max_Kacc = Convert.ToDouble(txtmaxKacc.Text, System.Globalization.CultureInfo.InvariantCulture);
            
            Kp_gain = 100.0 / max_Kp;
            Ki_gain = 100.0 / max_Ki;
            Kd_gain = 100.0 / max_Kd;
            Kff_gain = 100.0 / max_Kff;
            Kacc_gain = 100.0 / max_Kacc;
            

            trackBarKp.Value = Convert.ToInt16(Kp_gain * Kp_val);
            trackBarKi.Value = Convert.ToInt16(Ki_gain * Ki_val);
            trackBarKd.Value = Convert.ToInt16(Kd_gain * Kd_val);
            trackBarKff.Value = Convert.ToInt16(Kff_gain * Kff_val);
            trackBarKacc.Value = Convert.ToInt16(Kacc_gain * Kacc_val);

        }

        private void Calculate_gains(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Calculate_gains_tune_settings();
            }
        }

        private void txtmaxKi_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Calculate_gains_tune_settings();
            }
        }

        private void txtmaxKd_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Calculate_gains_tune_settings();
            }
        }

        private void txtmaxKff_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Calculate_gains_tune_settings();
            }
        }

        private void txtmaxKacc_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Calculate_gains_tune_settings();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Calculate_gains_tune_settings();
        }
    }
}

