using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

// since 
// (1) C# doesn't do struct packing nicely, and
// (2) C# is strictly typed (whereas c++ is weakly typed),
// the code for converting between bytes and structs is a bit large ...

namespace SSC
{
    partial class SquidstatCalibrator
    {
        enum Cmd : byte
        {
            HANDSHAKE = 65,
            REPORT_STATUS,
            SEND_CAL_DATA,
            SAVE_CAL_DATA,
            SEND_HW_DATA,
            SAVE_HW_DATA,
            SEND_NUM_CHANNELS,
            SET_OPMODE,
            MANUAL_DC_SAMPLE,
            BEGIN_NEW_EXP_DOWNLOAD,
            END_NEW_EXP_DOWNLOAD,
            APPEND_EXP_NODE,
            RUN_EXPERIMENT,
            STOP_EXPERIMENT,
            PAUSE_EXPERIMENT,
            RESUME_EXPERIMENT,
            INIT_DEFAULT_SAMPLING,
            SET_MANUAL_MODE,
            MANUAL_SAMPLING_PARAMS_SET,
            MANUAL_GALV_SETPOINT_SET,
            MANUAL_POT_SETPOINT_SET,
            MANUAL_OCP_SET,
            MANUAL_CURRENT_RANGING_MODE_SET,
            RESET_TO_BOOTLOADER
        };

        static byte[] HANDSHAKE()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.HANDSHAKE, CHANNEL, 0x00, 0x00 };
        }

        static byte[] SET_MANUAL_MODE()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.SET_MANUAL_MODE, CHANNEL, 0x00, 0x00 };
        }

        static byte[] MANUAL_SAMPLING_PARAMS_SET(byte TimerDiv, uint TimerPeriod, ushort ADCBufSize) //TODO
        {
            ushort NDB = 7; // number of data bytes
            byte[] ndb = BitConverter.GetBytes(NDB);
            byte timerDiv = TimerDiv;
            byte[] timerPeriod = BitConverter.GetBytes(TimerPeriod);
            byte[] adcBufSize = BitConverter.GetBytes(ADCBufSize);

            return new byte[] { FRAME1, FRAME2, (byte)Cmd.MANUAL_SAMPLING_PARAMS_SET, CHANNEL, ndb[0], ndb[1],
                timerDiv,
                timerPeriod[0], timerPeriod[1], timerPeriod[2], timerPeriod[3],
                adcBufSize[0], adcBufSize[1] };
        }

        static string MANUAL_SAMPLING_PARAMS_SET(byte[] b)
        {
            byte TimerDiv = b[H + 0];
            uint TimerPeriod = BitConverter.ToUInt32(new ArraySegment<byte>(b, H + 1, 4).ToArray(), 0);
            ushort ADCBufSize = BitConverter.ToUInt16(new ArraySegment<byte>(b, H + 5, 2).ToArray(), 0); ;

            return string.Format("TimerDiv=[{0}] TimerPeriod=[{1}] ADCBufSize=[{2}]",
                                  TimerDiv, TimerPeriod, ADCBufSize);
        }

        public class MANUAL_GALV_SETPOINT_SET
        {
            public short Setpoint { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 0, 2).ToArray(), 0); } }
            public byte Range { get { return Bytes[H + 2]; } }
            
            public MANUAL_GALV_SETPOINT_SET(short setpoint, byte range)
            {
                byte[] setpoint_ = BitConverter.GetBytes(setpoint);
                Bytes = new byte[] { FRAME1, FRAME2, (byte)Cmd.MANUAL_GALV_SETPOINT_SET, CHANNEL, 0x03, 0x00,
                setpoint_[0], setpoint_[1], range };
            }

            public byte[] Bytes { get; private set; }
            public MANUAL_GALV_SETPOINT_SET(byte[] b) { Bytes = b; }

            public override string ToString()
            {
                return string.Format("Setpoint=[{0}] Range=[{1}]",
                                      Setpoint,      Range);
            }

            public string ToCsv()
            {
                return string.Join(",", new string[] { Range.ToString(), Setpoint.ToString() });
            }
        }

        public class MANUAL_CURRENT_RANGING_MODE_SET
        {
            public MANUAL_CURRENT_RANGING_MODE_SET(byte currentRange)
            {
                Bytes = new byte[] { FRAME1, FRAME2, (byte)Cmd.MANUAL_CURRENT_RANGING_MODE_SET, CHANNEL, 0x01, 0x00, currentRange };
            }
            public byte[] Bytes { get; private set; }
        }

        public class MANUAL_POT_SETPOINT_SET
        {
            public short Setpoint { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 0, 2).ToArray(), 0); } }

            public MANUAL_POT_SETPOINT_SET(short setpoint)
            {
                byte[] setpoint_ = BitConverter.GetBytes(setpoint);
                Bytes = new byte[] { FRAME1, FRAME2, (byte)Cmd.MANUAL_POT_SETPOINT_SET, CHANNEL, 0x02, 0x00,
                setpoint_[0], setpoint_[1] };
            }

            public byte[] Bytes { get; private set; }
            public MANUAL_POT_SETPOINT_SET(byte[] b) { Bytes = b; }

            public override string ToString()
            {
                return string.Format("Setpoint=[{0}]",
                                      Setpoint);
            }

            public string ToCsv()
            {
                return string.Join(",", new string[] { "", Setpoint.ToString() });
            }
        }

        class SEND_CAL_DATA
        {
            public float m_DACac;    //DACac mVAC-to-bin slope

            public float m_DACdcP_V;   //DACdc V-to-bin slope, x > 0. Units = bits/V
            public float m_DACdcN_V;   //DACdc V-to-bin slope, x < 0. Units = bits/V
            public float b_DACdc_V;    //DACdc V-to-bin intercept.	   Units = bits

            public float m_refP;     //Ref bin-to-V slope, x > 0.     Units = V/bit
            public float m_refN;     //Ref bin-to-V slope, x < 0.	   Units = V/bit
            public float b_ref;      //Ref bin-to-V intercept.		   Units = V

            public float m_eweP;     //Ewe bin-to-V slope, x > 0.     Units = V/bit	
            public float m_eweN;     //Ewe bin-to-V slope, x < 0.     Units = V/bit
            public float b_ewe;      //Ewe bin-to-V intercept.		   Units = V

            public float m_eceP;     //Ece bin-to-V slope, x > 0.     Units = V/bit
            public float m_eceN;     //Ece bin-to-V slope, x < 0.     Units = V/bit
            public float b_ece;      //Ece bin-to-V intercept.		   Units = V

            public float[] m_iP;    //Current bin-to-mA slope, x > 0. Units = mA/bit
            public float[] m_iN;    //Current bin-to-mA slope, x < 0.	Units = mA/bit
            public float[] b_i;     //Current bin-to-mA intercept.	Units = mA

            public float[] m_DACdcP_I; //DACdc current mA-to-bin slope, x > 0.	Units = bits/mA
            public float[] m_DACdcN_I; //DACdc current mA-to-bin slope, x < 0.	Units = bits/mA
            public float[] b_DACdc_I;  //DACdc current mA-to-bin intercept		Units = bits

            public SEND_CAL_DATA()
            {
                int N = _currentRanges.Count;
                m_iP = new float[N];
                m_iN = new float[N];
                b_i = new float[N];
                m_DACdcP_I = new float[N];
                m_DACdcN_I = new float[N];
                b_DACdc_I = new float[N];
            }

            public byte[] Bytes
            {
                get
                {
                    List<byte> b = new List<byte>();

                    // header bytes
                    b.Add(FRAME1);
                    b.Add(FRAME2);
                    b.Add(((byte)Cmd.SAVE_CAL_DATA));
                    b.Add(CHANNEL);
                    ushort numDataBytes = (13 + 6 * 8) * 4; // 13 floats, 6 arrays of 8 floats, floats are 4 bytes
                    b.AddRange(BitConverter.GetBytes(numDataBytes));

                    // payload bytes

                    b.AddRange(BitConverter.GetBytes(m_DACac));

                    b.AddRange(BitConverter.GetBytes(m_DACdcP_V));
                    b.AddRange(BitConverter.GetBytes(m_DACdcN_V));
                    b.AddRange(BitConverter.GetBytes(b_DACdc_V));

                    b.AddRange(BitConverter.GetBytes(m_refP));
                    b.AddRange(BitConverter.GetBytes(m_refN));
                    b.AddRange(BitConverter.GetBytes(b_ref));

                    b.AddRange(BitConverter.GetBytes(m_eweP));
                    b.AddRange(BitConverter.GetBytes(m_eweN));
                    b.AddRange(BitConverter.GetBytes(b_ewe));

                    b.AddRange(BitConverter.GetBytes(m_eceP));
                    b.AddRange(BitConverter.GetBytes(m_eceN));
                    b.AddRange(BitConverter.GetBytes(b_ece));

                    Array.ForEach(m_iP, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(m_iN, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(b_i, x => b.AddRange(BitConverter.GetBytes(x)));

                    Array.ForEach(m_DACdcP_I, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(m_DACdcN_I, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(b_DACdc_I, x => b.AddRange(BitConverter.GetBytes(x)));

                    return b.ToArray();
                }
            }

            public void FromCsv(string fileName)
            {
                using (StreamReader reader = new StreamReader(fileName))
                {
                    m_DACac = getNumFromCsv(reader);

                    m_DACdcP_V = getNumFromCsv(reader);
                    m_DACdcN_V = getNumFromCsv(reader);
                    b_DACdc_V = getNumFromCsv(reader);

                    m_refP = getNumFromCsv(reader);
                    m_refN = getNumFromCsv(reader);
                    b_ref = getNumFromCsv(reader);

                    m_eweP = getNumFromCsv(reader);
                    m_eweN = getNumFromCsv(reader);
                    b_ewe = getNumFromCsv(reader);

                    m_eceP = getNumFromCsv(reader);
                    m_eceN = getNumFromCsv(reader);
                    b_ece = getNumFromCsv(reader);
                    
                    for (int i = 0; i < 8; i++) { m_iP[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 8; i++) { m_iN[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 8; i++) { b_i[i] = getNumFromCsv(reader); }

                    for (int i = 0; i < 8; i++) { m_DACdcP_I[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 8; i++) { m_DACdcN_I[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 8; i++) { b_DACdc_I[i] = getNumFromCsv(reader); }
                }
            }

            private float getNumFromCsv(StreamReader reader)
            {
                string line = reader.ReadLine();
                float num = 0;
                float.TryParse(line, out num);
                byte[] b = BitConverter.GetBytes(num);
                double number = 0;
                double.TryParse(line, out number);
                return num;
            }

            public override string ToString()
            {
                return string.Format(
                    "m_DACac=[{0}] m_DACdcP_V=[{}] m_DACdcN_V=[{}] b_DACdc_V=[{}] m_refP=[{}] m_refN=[{}] b_ref=[{}] m_eweP=[{}] m_eweN=[{}] b_ewe=[{}] m_eceP=[{}] m_eceN=[{}] b_ece=[{}] m_iP=[{}] m_iN=[{}] b_i=[{}] m_DACdcP_I=[{}] m_DACdcN_I=[{}] b_DACdc_I=[{}]",
                     m_DACac,      m_DACdcP_V,     m_DACdcN_V,     b_DACdc_V,     m_refP,     m_refN,     b_ref,     m_eweP,     m_eweN,     b_ewe,     m_eceP,     m_eceN,     b_ece, string.Join(",", m_iP), string.Join(",", m_iN), string.Join(",", b_i), string.Join(",", m_DACdcP_I), string.Join(",", m_DACdcN_I), string.Join(",", b_DACdc_I)
                    );
            }
        };

        static string CmdStr(byte[] bytes)
        {
            byte b = bytes[iCMDRSP];
            Cmd c = (Cmd)b;
            string m = string.Empty;

            if (c == Cmd.MANUAL_GALV_SETPOINT_SET)
                m = new MANUAL_GALV_SETPOINT_SET(bytes).ToString();
            else if (c == Cmd.MANUAL_POT_SETPOINT_SET)
                m = new MANUAL_POT_SETPOINT_SET(bytes).ToString();
            else if (c == Cmd.MANUAL_SAMPLING_PARAMS_SET)
                m = MANUAL_SAMPLING_PARAMS_SET(bytes);

            return string.Format("CMD {0} ({1}) {2}", c.ToString(), b.ToString(), m);
        }
    }
}
