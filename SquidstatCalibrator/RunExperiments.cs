using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SSC
{
    partial class SquidstatCalibrator
    {
        // these variables aren't really being used anymore
        // as the functionality is captured in Excel.
        // TODO: remove unnecessary variables
        public static int SquidstatVoltRange = 10;
        
        static Dictionary<byte, double> _currentRangesV1 = new Dictionary<byte, double> {
            { 0x00, 100E-3 },
            { 0x01,   5E-3 },
            { 0x02, 200E-6 },
            { 0x03,  10E-6 },
            };
        static Dictionary<byte, double> _currentRangesV2 = new Dictionary<byte, double> {
            { 0x00, 1000 },
            { 0x01,  100 },
            { 0x02,   10 },
            { 0x03,    1 },
            { 0x04,  100E-3 },
            { 0x05,   10E-3 },
            { 0x06,    1E-3 },
            { 0x07,  100E-6 },
            };
        static Dictionary<byte, double> _currentRanges = _currentRangesV2;

        // Commanded setpoints
        static short[] _setpoints =
        {
            -27000, -24300, -21600, -18900,-16200,-13500,-10800,-8100,-5400,-2700,
            0,
            2700,5400,8100,10800,13500,16200,18900,21600,24300,27000
        };

        static short[] _v2CurrentSetpoints =
        {
              -27000,-27000, -24300, -21600, -18900,-16200,-13500,-10800,-8100,-5400,-2700, //redundant first point
            0,
            2700,5400,8100,10800,13500,16200,18900,21600,24300,27000
        };

        static short[] _v1CurrentSetpoints =
        {
            -5000, -5000, -4500, -4000, -3500, -3000, -2500, -2000, -1500, -1000, -500, //redundant first point
            0,
            500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000
        };

        // stores the raw commands and responses from devices.
        // to use the information, you have to clean up the strings and
        // convert the bytes to structs.
        // Working with the raw data makes it easier to see where problems are introduced.
        public struct Exchange
        {
            public IList<byte> CmdToSS; // command to squidstat
            public IList<byte> RspFromSS; // response from squidstat
            public string FromMeter; // measurement from agilent or keithley
        }

        // run agilent in voltmeter mode
        public static void RunVoltmeter()
        {
            //fix current range
            byte[] cmdBytes = new MANUAL_CURRENT_RANGING_MODE_SET(0).Bytes;
            WriteBytes(ref _squidstat, cmdBytes);

            //SkipAdcdc(4);
            SkipAdcdc(5);

            WriteBytes(ref _squidstat, new MANUAL_POT_SETPOINT_SET(_setpoints[0]).Bytes);
            WriteString(ref _hp34401a, "MEASURE:VOLTAGE:DC? DEF,DEF");
            ReadString(ref _hp34401a);

            // for each commanded setpoint
            foreach (short sp in _setpoints)
            {
                Exchange e = new Exchange();

                // command the squidstat
                byte[] cmd = new MANUAL_POT_SETPOINT_SET(sp).Bytes;
                e.CmdToSS = cmd;
                WriteBytes(ref _squidstat, cmd);

                //SkipAdcdc(1);
                SkipAdcdc(4);

                // take a measurement
                WriteString(ref _hp34401a, "MEASURE:VOLTAGE:DC? DEF,DEF");
                e.FromMeter = ReadString(ref _hp34401a);

                //SkipAdcdc(1);
                SkipAdcdc(2);

                e.RspFromSS = ReadAdcdc();

                // log the exchange so that you can print it to a csv file later
                MeterExchanges.Add(e);
            }

            WriteBytes(ref _squidstat, MANUAL_OCP_SET());
        }

        public static void RunAmmeter(byte range)
        {
            SkipAdcdc(4);

            for (int i = 0; i < _setpoints.Length; i++)
            {
                Exchange e = new Exchange();

                byte[] cmd = new MANUAL_GALV_SETPOINT_SET(_setpoints[i], range).Bytes;
                e.CmdToSS = cmd;
                WriteBytes(ref _squidstat, cmd);

                // never skip less than 2 to guarantee consistent data
                SkipAdcdc(3);

                WriteString(ref _hp34401a, "MEASURE:CURRENT:DC? DEF,DEF");
                e.FromMeter = ReadString(ref _hp34401a);

                e.RspFromSS = ReadAdcdc();

                if (i!=0)   //skip first point, since it's repeated
                    MeterExchanges.Add(e);
            }
            WriteBytes(ref _squidstat, MANUAL_OCP_SET());
        }

        public static void RunPicoammeter(byte range)
        {
            SkipAdcdc(4);

            for (int i = 0; i < _setpoints.Length; i++)
            {
                Exchange e = new Exchange();

                byte[] cmd = new MANUAL_GALV_SETPOINT_SET(_setpoints[i], range).Bytes;
                e.CmdToSS = cmd;
                WriteBytes(ref _squidstat, cmd);

                SkipAdcdc(3);

                WriteString(ref _keithley485, "R0T1X");
                e.FromMeter = ReadString(ref _keithley485);

                e.RspFromSS = ReadAdcdc();

                if (i != 0)   //skip first point, since it's repeated
                    MeterExchanges.Add(e);
            }
            WriteBytes(ref _squidstat, MANUAL_OCP_SET());
        }
    }
}
