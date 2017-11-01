using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SSC
{
    partial class SquidstatCalibrator
    {
        static SerialPort _keithley485;
        static SerialPort _hp34401a;
        static SerialPort _squidstat;

        public static bool ConnectToKeithley485(string portName)
        {
            _keithley485 = new SerialPort();
            _keithley485.BaudRate = 115200;
            _keithley485.Parity = Parity.None;
            _keithley485.DataBits = 8;
            _keithley485.StopBits = StopBits.One;
            _keithley485.Handshake = System.IO.Ports.Handshake.RequestToSend;
            _keithley485.NewLine = "\r\n";
            _keithley485.DtrEnable = true;
            _keithley485.DiscardNull = false;
            _keithley485.ParityReplace = 0;
            _keithley485.ReadTimeout = 100;
            _keithley485.WriteTimeout = 100;

            _keithley485.PortName = portName;

            try { _keithley485.Open(); }
            catch (Exception e)
            {
                Console.WriteLine("ERROR {0}", e);
                return false;
            }
            Console.WriteLine("Keithley485 = {0}", _keithley485.PortName);

            WriteString(ref _keithley485, "R0T1X");
            string s = ReadString(ref _keithley485);

            return (!string.IsNullOrEmpty(s));
        }

        public static bool ConnectToHP34401A(string portName)
        {
            _hp34401a = new SerialPort();
            _hp34401a.PortName = portName;
            _hp34401a.BaudRate = 9600;
            _hp34401a.Parity = Parity.Even;    // Can be changed from the device's front panel
            _hp34401a.DataBits = 7;            // Can be changed from the device's front panel
            _hp34401a.StopBits = StopBits.Two; // Can be changed from the device's front panel
            _hp34401a.Handshake = System.IO.Ports.Handshake.None;
            _hp34401a.DtrEnable = true;
            _hp34401a.RtsEnable = true;
            _hp34401a.ReadTimeout = 100;
            _hp34401a.WriteTimeout = 100;

            try { _hp34401a.Open(); }
            catch (Exception e)
            {
                Console.WriteLine("ERROR {0}", e);
                return false;
            }
            Console.WriteLine("HP34401A = {0}", _hp34401a.PortName);

            WriteString(ref _hp34401a, "*IDN?"); // Should return the name of the device
            string s = ReadString(ref _hp34401a);
            WriteString(ref _hp34401a, "*RST"); // reset
            WriteString(ref _hp34401a, "*CLS"); // clear
            WriteString(ref _hp34401a, "SYSTEM:REMOTE"); // enable serial commands

            // we failed to connect if the box failed to identify itself when asked
            return (!string.IsNullOrEmpty(s));
        }


        // the squidstat has its own I/O thread since ADCDC data comes through asynchronously.
        // the other devices do not have their own thread as they can be treated as synchronous.
        static Thread _squidstatThread;

        // use this flag to tell the squidstat I/O thread when to start and stop
        static volatile bool _continueSquidstatReadThread = true;

        public static bool ConnectToSquidstat(string portName)
        {
            _squidstat = new SerialPort();
            _squidstat.BaudRate = 128000;
            _squidstat.Parity = Parity.None;
            _squidstat.DataBits = 8;
            _squidstat.StopBits = StopBits.One;
            _squidstat.Handshake = System.IO.Ports.Handshake.None;
            _squidstat.Encoding = System.Text.Encoding.GetEncoding(1252);
            _squidstat.ReadTimeout = 100;
            _squidstat.WriteTimeout = 100;

            _squidstat.PortName = portName;

            _squidstat.Open();
            Console.WriteLine("Connected to Squidstat on port {0}", _squidstat.PortName);

            // Read squidstat responses asynchronously in its own thread.
            _squidstatThread = new Thread(SquidstatReadThread);
            _squidstatThread.Start();

            WriteBytes(ref _squidstat, HANDSHAKE());

            WriteBytes(ref _squidstat, SET_MANUAL_MODE());

            //WriteBytes(ref _squidstat, MANUAL_SAMPLING_PARAMS_SET(0x00, 195312, 256));
            // 195312 100ths of us
            // 256 of those samples are being averaged
            // samples/second = 2

            WriteBytes(ref _squidstat, MANUAL_SAMPLING_PARAMS_SET(0x00, 39062, 256));
            // 39062 100ths of us
            // 256 of those samples are being averaged
            // samples/second = 10

            return true;
        }
    }
}
