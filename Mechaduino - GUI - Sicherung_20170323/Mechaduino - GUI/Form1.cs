using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;



namespace Mechaduino___GUI
{


    public partial class Form1 : Form
    {
        const int length = 750;
        int[] uValues = new int[length];
        double[] yValues = new double[length];
        double[] rValues = new double[length];
        double[] eValues = new double[length];

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
        int plot_counter = 0;

        int streaming = 0;

        bool wasOpen = false;


        public Form1()
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
                                Torque = Math.Abs((Pa * u * 189) / (180 * uMax));
                            }
                            catch{
                            }

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

                            // update line plots
                            uValues[wrap_pointer] = Convert.ToInt32((u*1000*3.3)/(512*10*0.15));
                            yValues[wrap_pointer] = (y / 100.0);
                            rValues[wrap_pointer] = (r / 100.0);
                            eValues[wrap_pointer] = (e / 100.0);
                            plot_counter += 1;

                            if (plot_counter == 5)
                            {
                                // update angle plot
                                anglePlot.Series[0].Points.Clear();
                                anglePlot.Series[0].Points.AddXY(0, 0);
                                anglePlot.Series[0].Points.AddXY(Math.Cos((y / 100.0) * Math.PI / 180), Math.Sin((y / 100.0) * Math.PI / 180));

                                anglePlot.Series[1].Points.Clear();
                                anglePlot.Series[1].Points.AddXY(0, 0);
                                anglePlot.Series[1].Points.AddXY(Math.Cos((electrical_angle / 100.0) * Math.PI / 180), Math.Sin((electrical_angle / 100.0) *  Math.PI/ 180));
                            }

                            if (plot_counter >= 9)
                            {
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
                                panelTorque.Height = 189 - Torque;

                                plot_counter = 0;

                            }
                            wrap_pointer += 1;
                            wrap_pointer = wrap_pointer % (yValues.Length - 1);
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
            double max = Double.MinValue;
            double min = Double.MaxValue;

            Chart tmpChart = (Chart)chart;

            double leftLimit = tmpChart.ChartAreas[0].AxisX.Minimum;
            double rightLimit = tmpChart.ChartAreas[0].AxisX.Maximum;

            for (int s = 0; s < tmpChart.Series.Count(); s++)
            {
                foreach (DataPoint dp in tmpChart.Series[s].Points)
                {
                    if (dp.XValue >= leftLimit && dp.XValue <= rightLimit)
                    {
                        min = Math.Min(min, dp.YValues[0]);
                        max = Math.Max(max, dp.YValues[0]);
                    }
                }
            }
            
            if (min == 0)
            {
                min = -1;
            }
            if (max == 0)
            {
                max = 1;
            }

            min = Math.Floor(min*10)/10.0;
            max = Math.Ceiling(max*10)/10.0;
            
            tmpChart.ChartAreas[0].AxisY.Maximum = max;
            tmpChart.ChartAreas[0].AxisY.Minimum = min;
        }







        private void btnStream_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                Debug.Print(Convert.ToString(streaming));
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
                    
                    serialPort1.WriteLine("start_stream\r\n");
                }
                else
                {
                    serialPort1.DiscardInBuffer();
                    btnStream.Text = "Start stream";
                    btnEnable.Enabled = false;
                    serialPort1.WriteLine("stop_stream\r\n");
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
                serialPort1.WriteLine("disable\r\n");
            }
            else
            {
                serialPort1.WriteLine("enable\r\n");
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
                catch (Exception ex)
                {

                }
                //serialPort1.DiscardInBuffer();
                btnOpen.Text = "Open";
                wasOpen = false;
                txtConsole.Text = "Serial port closed";
                txtConsole.BackColor = Color.White;
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
                catch (Exception ex)
                {
                    // unable to open serial port
                }

                if (serialPort1.IsOpen)
                {
                    serialPort1.DiscardInBuffer();
                    txtReceived.Text = "";
                    wasOpen = true;
                    
                    btnStream.Enabled = true;
                    btnOpen.Text = "Close";

                    txtConsole.Text = "Serial port open";
                    txtConsole.BackColor = Color.White;
                    serialPort1.WriteLine("stop_stream\n\r");
                    serialPort1.DiscardInBuffer();
                    serialPort1.WriteLine("help\n\r");
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
                serialPort1.WriteLine(txtSend.Text + "\n\r");
                txtSend.Clear();
            }
            else
            {
                txtConsole.Text = "No Serial Port avaiable to send to";
                txtConsole.BackColor = Color.Red;
            }
        }





        private void timer1_Tick(object sender, EventArgs e)
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





        private void btnRefresh_Click(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            serial_box.Items.Clear();
            serial_box.Items.AddRange(ports);
            serial_box.SelectedIndex = 0;
        }
    }


}

