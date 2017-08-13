using System;
using System.Windows.Forms;
using System.IO;
namespace Mechaduino
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(true);
            Application.Run(new GUI());
            
        }
    }

}
