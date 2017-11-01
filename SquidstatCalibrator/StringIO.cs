using System;
using System.IO.Ports;
using System.Threading;

// the agilent and the keithley communicate synchronously with strings

namespace SSC
{
    partial class SquidstatCalibrator
    {
        static bool WriteString(ref SerialPort serialPort, string msg)
        {
            int elapsedTime = 0;
            const int DELAY = 5;

            while (elapsedTime < serialPort.ReadTimeout)
            {
                elapsedTime += DELAY;
                try
                {
                    serialPort.WriteLine(msg);
                    Console.WriteLine("CMD  {0}", msg);
                    return true;
                }
                catch (Exception) { }
                Thread.Sleep(DELAY);
            }

            Console.WriteLine("ERR  Timeout Cmd {0}", msg);
            return false;
        }

        static string ReadString(ref SerialPort serialPort)
        {
            string readString = string.Empty;
            int elapsedTime = 0;
            const int DELAY = 5;

            while (elapsedTime < serialPort.ReadTimeout)
            {
                elapsedTime += DELAY;
                try
                {
                    readString = serialPort.ReadLine();
                    Console.WriteLine("RSP  {0}", readString);
                    return readString;
                }
                catch (Exception) { }
                Thread.Sleep(DELAY);
            }

            Console.WriteLine("ERR  Timeout Rsp {0}", serialPort.PortName);
            return readString;
        }
    }
}
