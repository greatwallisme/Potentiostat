using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;

namespace SSC
{
    partial class SquidstatCalibrator
    {
        // use this to send any command to the squidstat
        public static bool WriteBytes(ref SerialPort sp, byte[] b)
        {
            try
            {
                sp.Write(b, 0, b.Length);
                Console.WriteLine("CMD  {0}", CmdStr(b));
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine("ERR  Failed CMD {0} {1}", CmdStr(b), e);
            }
            return false;
        }
        
        static volatile bool save_next_adcdc = false;

        static volatile int num_adcdc = 0;
        static volatile List<byte> adcdc;

        public static void SkipAdcdc(int numPacketsToSkip)
        {
            int n = num_adcdc;

            while (num_adcdc <= n + numPacketsToSkip) { } // wait for some packets to go by
        }

        public static List<byte> ReadAdcdc()
        {
            save_next_adcdc = true;

            while (save_next_adcdc) { }

            List<byte> msg = new List<byte>(adcdc);

            return msg;
        }

        // convoluted state machine.
        // has special logic for skipping and saving adcdc data.
        public static void SquidstatReadThread()
        {
            List<byte> msg = new List<byte>();

            int numBytesRead = 0;
            byte[] numDataBytes = new byte[] { 0, 0 }; // number of payload data bytes
            ushort numBytesToRead = 6;

            while (_continueSquidstatReadThread)
            {
                try
                {
                    byte currentByte = (byte)_squidstat.ReadByte(); // throws an exception on failure
                    numBytesRead++; // successfully read one byte
                    msg.Add(currentByte);

                    if (numBytesRead == 5) // first byte of number of data bytes uint16_6 in header
                        numDataBytes[0] = currentByte;
                    else if (numBytesRead == 6) // second byte of number of data bytes uint16_6 in header
                    {
                        numDataBytes[1] = currentByte;
                        numBytesToRead = (ushort)(numBytesToRead + BitConverter.ToUInt16(numDataBytes, 0));
                    } // done reading header

                    if (numBytesRead == numBytesToRead) // we've read all of the bytes in the packet
                    {
                        if (msg[iCMDRSP] == (byte)Rsp.ADCDC_DATA && msg[iCHAN_NUM] == CHANNEL)
                        {
                            num_adcdc++;

                            if (save_next_adcdc)
                            {
                                adcdc = new List<byte>(msg);
                                save_next_adcdc = false;
                                Console.WriteLine("SAVE RSP {0}", RspStr(msg.ToArray()));
                            }
                            else
                            {
                                Console.WriteLine("SKIP ADCdc");
                            }
                        }
                        else if (msg[iCMDRSP] == (byte)Rsp.DEBUG_LOG_MSG)
                        {
                            Console.WriteLine("RSP  {0}", RspStr(msg.ToArray()));
                        }
                        else if (msg[iCMDRSP] == (byte)Rsp.CAL_DATA)
                        {
                            cal_rsp = new CAL_DATA(msg.ToArray());
                            Console.WriteLine("RSP  {0}", cal_rsp.ToString());
                        }
                        else if (msg[iCMDRSP] == (byte)Rsp.HW_DATA)
                        {
                            string hw_data = Encoding.ASCII.GetString(msg.ToArray());
                            Console.WriteLine("RSP  {0}", hw_data);
                        }

                        // Throw out the packet, resetting everything.
                        msg = new List<byte>();
                        numBytesRead = 0;
                        numBytesToRead = 6;
                    }
                }
                catch (Exception) { }
            }
        }
    }
}
