﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;

namespace SSC // Squidstat Calibrator
{
    // The whole application is contained within a single class.
    partial class SquidstatCalibrator
    {
        // This is where we keep track of all the 
        // sent commands, received ADCDC data, and measured voltages/currents.
        static List<Exchange> MeterExchanges;
        
        // Command-Line Arguments
        public enum Args
        {
            SQUIDSTAT_VERSION,   // v1, v2
            SQUIDSTAT_CHANNEL,   // 0, 1, 2, 3
            SQUIDSTAT_PORT_NAME, // COM#
            METER_NAME,          // keithley485, hp34401a, calibrate
            METER_PORT_NAME,     // COM#
            RANGE                // 0, 1, 2, ... 
        };

        const string directory = @"C:\Potentiostat\SquidstatCalibrator\bin\Debug";

        static CAL_DATA cal_rsp;

        // Example Usage:
        // _.exe v2 COM6 hp34401a COM4 current 3
        // _.exe v2 COM6 hp34401a COM4 voltage
        // _.exe v2 COM6 keithley COM4
        // _.exe v2 COM6 calibrate <-- sends calibration data
        static void Main(string[] args)
        {
            Console.WriteLine("Directory: " + directory);

            StreamWriter csv;
            MeterExchanges = new List<Exchange>();

            // Set Squidstat version
            if (args[(int)Args.SQUIDSTAT_VERSION].ToLower() == "v1")
            {
                _squidstatVersion = SquidstatVersions.V1;
            }
            else if (args[(int)Args.SQUIDSTAT_VERSION].ToLower() == "v2")
            {
                _squidstatVersion = SquidstatVersions.V2;
            }
            else
            {
                Console.WriteLine("Invalid Squidstat version: {0}", args[(int)Args.SQUIDSTAT_VERSION]);
            }

            CHANNEL = byte.Parse(args[(int)Args.SQUIDSTAT_CHANNEL]);
            Console.WriteLine("Channel: " + CHANNEL);

            ConnectToSquidstat(args[(int)Args.SQUIDSTAT_PORT_NAME]);

            if (args[(int)Args.METER_NAME].ToLower() == "keithley485")
            {
                byte range = byte.Parse(args[(int)Args.RANGE]);
                if (
                    (_squidstatVersion == SquidstatVersions.V1 && range > 0x04) ||
                    (_squidstatVersion == SquidstatVersions.V2 && range > 0x07))
                {
                    Console.WriteLine("Invalid range: {0} for Squidstat Version: {1}", args[(int)Args.RANGE], args[(int)Args.SQUIDSTAT_VERSION]);
                    return;
                }

                if (!ConnectToKeithley485(args[(int)Args.METER_PORT_NAME]))
                {
                    Console.WriteLine("Failed to connect to Keithley485");
                    return;
                }

                if (_squidstatVersion == SquidstatVersions.V1)
                {
                    _setpoints = _v1CurrentSetpoints;
                }
                else if (_squidstatVersion == SquidstatVersions.V2)
                {
                    _setpoints = _v2CurrentSetpoints;
                }

                RunPicoammeter(range);

                _keithley485.Close();

                // Write CSV file:
                // if it doesn't exist, then create it
                // if it exists, then open it
                // in write mode
                // either way, overwrite it
                // allow other programs to read it
                using (var stream = File.Open(
                    Path.Combine(directory, "r" + args[(int)Args.RANGE] + ".csv"), 
                    FileMode.Create, FileAccess.Write, FileShare.Read))
                {
                    // StreamWriter lets you call .WriteLine(...) on the object
                    csv = new StreamWriter(stream);

                    // AutoFlush makes sure that there isn't anything left in the buffer to write 
                    // in the event that it crashes
                    csv.AutoFlush = true;

                    // Column 1: Current measured from Agilent or Keithley
                    // Column 2: "current" field of ADCDC data struct

                    string headers = string.Join(",", "Measured Current", "current");
                    csv.WriteLine(headers);

                    // write each command / response / measurement triplet to the csv file
                    foreach (Exchange e in MeterExchanges)
                    {
                        // ugliness to convert from IList to byte array
                        byte[] from_ss = new byte[e.RspFromSS.Count];
                        e.RspFromSS.CopyTo(from_ss, 0);

                        // create an ADCDC_DATA response object from the raw bytes received from the squidstat
                        ADCDC_DATA adcdc = new ADCDC_DATA(from_ss);

                        // Measurement from Keithley:
                        // remove trailing newline characters and leading status characters
                        // Note that if there's an error, the leading characters will not be removed,
                        // making it very obvious in the Excel file that something is wrong.
                        string from_meter = e.FromMeter.TrimEnd('\r', '\n').TrimStart('N','D','C','A');

                        string line_to_write = string.Join(",", from_meter, adcdc.Current);
                        csv.WriteLine(line_to_write);
                    }
                }

            }
            else if (args[(int)Args.METER_NAME].ToLower() == "hp34401a")
            {
                ConnectToHP34401A(args[(int)Args.METER_PORT_NAME]);

                if (args[(int)Args.RANGE].ToLower() == "voltage")
                {
                    RunVoltmeter();

                    _hp34401a.Close();

                    using (var stream = File.Open(Path.Combine(directory, "v.csv"), FileMode.Create, FileAccess.Write, FileShare.Read))
                    {
                        csv = new StreamWriter(stream);
                        csv.AutoFlush = true;

                        string headers = string.Join(",", "Measured Voltage", "ref", "ewe", "ece");
                        csv.WriteLine(headers);
                        
                        foreach (Exchange e in MeterExchanges)
                        {
                            byte[] from_ss = new byte[e.RspFromSS.Count];
                            e.RspFromSS.CopyTo(from_ss, 0);

                            ADCDC_DATA adcdc = new ADCDC_DATA(from_ss);

                            string from_meter = e.FromMeter.TrimEnd('\r', '\n');

                            string line_to_write = string.Join(",", from_meter, adcdc.Ref, adcdc.Ewe, adcdc.Ece);
                            csv.WriteLine(line_to_write);
                        }
                    }
                }
                else
                {
                    // validate the commanded range
                    byte range = byte.Parse(args[(int)Args.RANGE]);
                    if (
                        (_squidstatVersion == SquidstatVersions.V1 && range > 0x04) ||
                        (_squidstatVersion == SquidstatVersions.V2 && range > 0x07))
                    {
                        Console.WriteLine("Invalid range: {0} for Squidstat Version: {1}", args[(int)Args.RANGE], args[(int)Args.SQUIDSTAT_VERSION]);
                        return;
                    }

                    if (_squidstatVersion == SquidstatVersions.V1)
                    {
                        _setpoints = _v1CurrentSetpoints;
                    }
                    else if (_squidstatVersion == SquidstatVersions.V2)
                    {
                        _setpoints = _v2CurrentSetpoints;
                    }

                        RunAmmeter(range);

                    _hp34401a.Close();

                    using (var stream = File.Open(
                        Path.Combine(directory, "r" + args[(int)Args.RANGE] + ".csv"),
                        FileMode.Create, FileAccess.Write, FileShare.Read))
                    {
                        csv = new StreamWriter(stream);
                        csv.AutoFlush = true;

                        string headers = string.Join(",", "Measured Current", "current");
                        csv.WriteLine(headers);

                        foreach (Exchange e in MeterExchanges)
                        {
                            byte[] from_ss = new byte[e.RspFromSS.Count];
                            e.RspFromSS.CopyTo(from_ss, 0);

                            ADCDC_DATA adcdc = new ADCDC_DATA(from_ss);

                            string from_meter = e.FromMeter.TrimEnd('\r', '\n').TrimStart('N', 'D', 'C', 'A');

                            string line_to_write = string.Join(",", from_meter, adcdc.Current);
                            csv.WriteLine(line_to_write);
                        }
                    }
                }
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "calibrate")
            {
                var cal = new CAL_DATA();
                cal.FromCsv("cal.csv");
                byte[] cmd = cal.Bytes;
                WriteBytes(ref _squidstat, cmd);
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "quality")
            {
                // ask for cal struct
                var cmd = SEND_CAL_DATA();
                WriteBytes(ref _squidstat, cmd);

                // wait for response
                while (cal_rsp == null) { }

                using (var stream = File.Open(Path.Combine(directory, "cal_rsp.csv"), FileMode.Create, FileAccess.Write, FileShare.Read))
                {
                    csv = new StreamWriter(stream);
                    csv.AutoFlush = true;

                    cal_rsp.ToCsv(csv);
                }
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "save_hw_data")
            {
                var cmd = new SAVE_HW_DATA();

                using (var stream = File.Open(AppDomain.CurrentDomain.BaseDirectory + "\\save_hw_data.csv", FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    StreamReader reader = new StreamReader(stream);
                    string s_hardwareModel = reader.ReadLine();
                    string s_serialNumber = reader.ReadLine();
                    s_serialNumber += "\n";

                    List<string> text = new List<string>();
                    while (!reader.EndOfStream)
                    {
                        text.Add(reader.ReadLine());
                    }

                    string s_generalText = string.Join("\n", text);

                    cmd.hardwareModel = (HardwareModel_t)byte.Parse(s_hardwareModel);
                    cmd.serialNumber = s_serialNumber;
                    cmd.text = s_generalText;
                }

                Thread.Sleep(500);
                WriteBytes(ref _squidstat, cmd.Bytes);
                Thread.Sleep(500);
                WriteBytes(ref _squidstat, SEND_HW_DATA());
                Thread.Sleep(500);
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "send_hw_data")
            {
                WriteBytes(ref _squidstat, SEND_HW_DATA());
                Thread.Sleep(500);
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "send_channel_name")
            {
                WriteBytes(ref _squidstat, SEND_CHANNEL_NAME());
                Thread.Sleep(500);
            }
            else if (args[(int)Args.METER_NAME].ToLower() == "save_channel_name")
            {
                var cmd = new SAVE_CHANNEL_NAME();

                using (var stream = File.Open(AppDomain.CurrentDomain.BaseDirectory + "\\save_channel_name.csv", FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    StreamReader reader = new StreamReader(stream);
                    cmd.name = reader.ReadLine();
                }

                Thread.Sleep(500);
                WriteBytes(ref _squidstat, cmd.Bytes);
                Thread.Sleep(500);
                WriteBytes(ref _squidstat, SEND_CHANNEL_NAME());
                Thread.Sleep(500);
            }
            else
            {
                Console.WriteLine("Invalid meter: {0}", args[(int)Args.METER_NAME]);
            }

            // Stop reading from squidstat

            // flag to tell the read thread to stop reading
            _continueSquidstatReadThread = false;

            // wait for the read thread to stop, then kill it
            _squidstatThread.Join();

            _squidstat.Close();

            // leave the command line open in case you want to inspect it
            Console.WriteLine("Press any key to exit.");
            ConsoleKeyInfo key = Console.ReadKey();
            
        }
    }
}
