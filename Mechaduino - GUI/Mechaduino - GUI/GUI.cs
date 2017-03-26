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

        
        int y = 0;
        int r = 0;
        int e = 0;
        int u = 0;
        int electrical_angle = 0;
        int enabled = 0;

        int raw =0;
        const int uMax = 233;
        int Pa = 0;
        int Torque = 0;

        int wrap_pointer = 0;

        int streaming = 0;

        bool wasOpen = false;
        bool changing_size = false;
        bool savetoCSV = false;
        String CSVFileName = "";

       
        
        



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
                    String[] substrings = value.Split(',');

                    if (substrings.Length == 7)
                    {
                        streaming = Convert.ToInt16(substrings[0]);

                        if (streaming == 1)
                        {
                            try
                            {
                                //Data Stream is received here
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

                                if (savetoCSV)
                                {
                                    File.AppendAllText(CSVFileName, value);
                                }
                            }
                            catch{
                            }

                            if (!changing_size)
                            {
                                // update line plots
                                uValues[wrap_pointer] = Convert.ToInt32((u * 1000 * 3.3) / (512 * 10 * 0.15));
                                yValues[wrap_pointer] = (y / 100.0);
                                rValues[wrap_pointer] = (r / 100.0);
                                eValues[wrap_pointer] = (e / 100.0);

                                wrap_pointer += 1;
                                wrap_pointer = wrap_pointer % (yValues.Length - 1);
                            }
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
                    else
                    {
                        streaming = 0;
                        txtReceived.AppendText(value + "\n");
                        txtReceived.ScrollToCaret();
                    }
                }
            }
        }



        



        private void changeYScala(object chart)
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

                for (int i = 0; i < yValues.Length - 1; i++)
                {
                    int pointer = wrap_pointer + i;
                    pointer = pointer % (yValues.Length - 1);

                    pltCurrent.Series[0].Points.AddY(uValues[pointer]);
                    pltPosition.Series[0].Points.AddY(yValues[pointer]);
                    pltPosition.Series[1].Points.AddY(rValues[pointer]);
                    pltError.Series[0].Points.AddY(eValues[pointer]);
                }

                // Update y axis scaling 
                changeYScala(pltCurrent);
                changeYScala(pltPosition);
                changeYScala(pltError);

                // update bar plots
                panelTorque.Height = 240 - Torque;

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

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (!savetoCSV)
            {
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    CSVFileName = saveFileDialog1.FileName;
                    savetoCSV = true;
                    Debug.Print(CSVFileName);
                    File.WriteAllText(CSVFileName, "");
                    File.AppendAllText(CSVFileName, "streaming,position,target,error,effort,electrical_angle,enabled \n");
                }
            }
            else
            {
                checkBox1.Checked = false;
                savetoCSV = false;
            }
        }
    }


}

