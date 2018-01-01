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
            RESET_TO_BOOTLOADER,
            SET_COMP_RANGE,
            SEND_CHANNEL_NAME,
            SAVE_CHANNEL_NAME
        };

        static byte[] HANDSHAKE()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.HANDSHAKE, CHANNEL, 0x00, 0x00 };
        }

        static byte[] SEND_CAL_DATA()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.SEND_CAL_DATA, CHANNEL, 0x00, 0x00 };
        }

        static byte[] SET_MANUAL_MODE()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.SET_MANUAL_MODE, CHANNEL, 0x00, 0x00 };
        }

        static byte[] MANUAL_OCP_SET()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.MANUAL_OCP_SET, CHANNEL, 0x00, 0x00};
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

        static byte[] SEND_HW_DATA()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.SEND_HW_DATA, CHANNEL, 0x00, 0x00 };
        }

        static byte[] SEND_CHANNEL_NAME()
        {
            return new byte[] { FRAME1, FRAME2, (byte)Cmd.SEND_CHANNEL_NAME, CHANNEL, 0x00, 0x00 };
        }

        enum HardwareModel_t
        {
            PRIME = 0,
            EDGE,
            PICO,
            SOLO,
            PLUS,
            PLUS_2_0,
            SOLO_2_0,
            PRIME_2_0
        };

        class SAVE_HW_DATA
        {
            public HardwareModel_t hardwareModel;
            public string serialNumber;
            public string text;

            public SAVE_HW_DATA()
            {

            }

            public byte[] Bytes
            {
                get
                {
                    byte b_hardwareModel = (byte)hardwareModel;
                    byte[] b_serialNumber = System.Text.Encoding.ASCII.GetBytes(serialNumber);
                    byte[] b_text = System.Text.Encoding.ASCII.GetBytes(text);

                    List<byte> bytes = new List<byte>();
                    bytes.Add(b_hardwareModel);
                    bytes.AddRange(b_serialNumber);
                    bytes.AddRange(b_text);

                    if (bytes.Count < 256)
                    {
                        int size = 256 - bytes.Count;
                        byte[] padding = new byte[size];
                        bytes.AddRange(padding);
                    }

                    if (bytes.Count > 256)
                    {
                        Console.WriteLine("ERR  Text exceeds 256 characters.");
                        byte[] rval = new byte[0];
                        return rval;
                    }

                    var cmd_ = new byte[] { FRAME1, FRAME2, (byte)Cmd.SAVE_HW_DATA, CHANNEL };
                    List<byte> cmd = cmd_.ToList();

                    ushort numDataBytes = 256;
                    cmd.AddRange(BitConverter.GetBytes(numDataBytes));


                    cmd.AddRange(bytes);


                    return cmd.ToArray();
                }
            }
            
        }

        class SAVE_CHANNEL_NAME
        {
            public string name;

            public SAVE_CHANNEL_NAME()
            {

            }

            public byte[] Bytes
            {
                get
                {
                    List<byte> bytes = new List<byte>();
                    bytes.AddRange(System.Text.Encoding.ASCII.GetBytes(name));

                    if (bytes.Count < 32)
                    {
                        byte[] padding = new byte[32 - bytes.Count];
                        bytes.AddRange(padding);
                    }

                    if (bytes.Count > 32)
                    {
                        Console.WriteLine("ERR  Text exceeds 32 characters.");
                        byte[] rval = new byte[0];
                        return rval;
                    }

                    var cmd_ = new byte[] { FRAME1, FRAME2, (byte)Cmd.SAVE_CHANNEL_NAME, CHANNEL };
                    List<byte> cmd = cmd_.ToList();

                    ushort numDataBytes = 32;
                    cmd.AddRange(BitConverter.GetBytes(numDataBytes));

                    cmd.AddRange(bytes);

                    return cmd.ToArray();
                }
            }

        }

        class CAL_DATA
        {
            public float m_DACac;    //DACac mVAC-to-bin slope
            public float m_DACircomp; //DACdc V-to-bin slope for iR compensation. Units = bits/V
            public float m_DACwebias;   //DACdc V-to-bin slope for biasing WE gain stages. Units = bits/V
            public float m_DACibias;    //DACdc V-to-bin slope for biasing I gain stages. Units = bits/V

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

            /* Phase angle calibration */
            public float[] stagePhaseDelay;  // delay of each gain stage relative to a gain of 1
                                            // [0] = Vgain2 stage delay (nanoseconds)
                                            // [1] = Vgain5 stage delay (nanoseconds)
                                            // [2] = Vgain10 stage delay (nanoseconds)
                                            // [3] = Vgain10alt1 stage delay (nanoseconds)
                                            // [4] = Vgain10alt2 stage delay (nanoseconds)
                                            // [5] = Igain2 stage delay (nanoseconds)
                                            // [6] = Igain5 stage delay (nanoseconds)
                                            // [7] = Igain10 stage delay (nanoseconds)
                                            // [8] = Igain10alt1 stage delay (nanoseconds)
                                            // [9] = Igain10alt2 stage delay (nanoseconds)
            public float[] stageDCGain;      // DC gain of each gain stage
                                            // [0] = Vgain2 gain
                                            // [1] = Vgain5 gain
                                            // [2] = Vgain10 gain
                                            // etc...
            public float[] stageHFGain_A;    // Stage's gain above 50kHz, quadratic term [=] 1/Hz^2
                                                 // [0] = Vgain2's quadratic term
                                                 // [1] = Vgain5's quadratic term
                                                 // [2] = Vgain10's quadratic term
                                                 // etc...
            public float[] stageHFGain_B;    // Stage's gain above 50kHz, linear term [=] 1/Hz
                                                 // [0] = Vgain2's linear term
                                                 // [1] = Vgain5's linear term
                                                 // [2] = Vgain10's linear term
                                                 // etc...
            public float[] stageHFGain_C;    // Stage's gain above 50kHz, scalar term
                                                 // [0] = Vgain2's scalar term
                                                 // [1] = Vgain5's scalar term
                                                 // [2] = Vgain10's scalar term
                                                 // etc...

            public CAL_DATA()
            {
                int N = _currentRanges.Count;
                m_iP = new float[N];
                m_iN = new float[N];
                b_i = new float[N];
                m_DACdcP_I = new float[N];
                m_DACdcN_I = new float[N];
                b_DACdc_I = new float[N];

                stagePhaseDelay = new float[10];
                stageDCGain = new float[10];
                stageHFGain_A = new float[10];
                stageHFGain_B = new float[10];
                stageHFGain_C = new float[10];
            }

            public CAL_DATA(byte[] b)
            {
                int N = _currentRanges.Count;
                m_iP = new float[N];
                m_iN = new float[N];
                b_i = new float[N];
                m_DACdcP_I = new float[N];
                m_DACdcN_I = new float[N];
                b_DACdc_I = new float[N];

                stagePhaseDelay = new float[10];
                stageDCGain = new float[10];
                stageHFGain_A = new float[10];
                stageHFGain_B = new float[10];
                stageHFGain_C = new float[10];

                int index = H;
                m_DACac = BitConverter.ToSingle(b, index);      index += sizeof(Single);
                m_DACircomp = BitConverter.ToSingle(b, index);  index += sizeof(Single);
                m_DACwebias = BitConverter.ToSingle(b, index);  index += sizeof(Single);
                m_DACibias = BitConverter.ToSingle(b, index);   index += sizeof(Single);

                m_DACdcP_V = BitConverter.ToSingle(b, index);   index += sizeof(Single);
                m_DACdcN_V = BitConverter.ToSingle(b, index);   index += sizeof(Single);
                b_DACdc_V = BitConverter.ToSingle(b, index);    index += sizeof(Single);

                m_refP = BitConverter.ToSingle(b, index);     index += sizeof(Single);
                m_refN = BitConverter.ToSingle(b, index);     index += sizeof(Single);
                b_ref = BitConverter.ToSingle(b, index);      index += sizeof(Single);

                m_eweP = BitConverter.ToSingle(b, index);     index += sizeof(Single);
                m_eweN = BitConverter.ToSingle(b, index);     index += sizeof(Single);
                b_ewe = BitConverter.ToSingle(b, index);      index += sizeof(Single);

                m_eceP = BitConverter.ToSingle(b, index);    index += sizeof(Single);
                m_eceN = BitConverter.ToSingle(b, index);    index += sizeof(Single);
                b_ece = BitConverter.ToSingle(b, index);     index += sizeof(Single);

                for (int i = 0; i < 8; i++)
                {
                    m_iP[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 8; i++)
                {
                    m_iN[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 8; i++)
                {
                    b_i[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 8; i++)
                {
                    m_DACdcP_I[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 8; i++)
                {
                    m_DACdcN_I[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 8; i++)
                {
                    b_DACdc_I[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }

                for (int i = 0; i < 10; i++)
                {
                    stagePhaseDelay[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }
                for (int i = 0; i < 10; i++)
                {
                    stageDCGain[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }
                for (int i = 0; i < 10; i++)
                {
                    stageHFGain_A[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }
                for (int i = 0; i < 10; i++)
                {
                    stageHFGain_B[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }
                for (int i = 0; i < 10; i++)
                {
                    stageHFGain_C[i] = BitConverter.ToSingle(b, index);
                    index += sizeof(Single);
                }
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
                    ushort numDataBytes = (16 + 6 * 8 + 5 * 10) * 4; // 16 floats, 6 arrays of 8 floats, 5 arrays of 10 floats, floats are 4 bytes
                    b.AddRange(BitConverter.GetBytes(numDataBytes));

                    // payload bytes

                    b.AddRange(BitConverter.GetBytes(m_DACac));
                    b.AddRange(BitConverter.GetBytes(m_DACircomp));
                    b.AddRange(BitConverter.GetBytes(m_DACwebias));
                    b.AddRange(BitConverter.GetBytes(m_DACibias));

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

                    Array.ForEach(stagePhaseDelay, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(stageDCGain, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(stageHFGain_A, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(stageHFGain_B, x => b.AddRange(BitConverter.GetBytes(x)));
                    Array.ForEach(stageHFGain_C, x => b.AddRange(BitConverter.GetBytes(x)));

                    return b.ToArray();
                }
            }

            public void ToCsv(StreamWriter s)
            {
                s.WriteLine(m_DACac);
                s.WriteLine(m_DACircomp);
                s.WriteLine(m_DACwebias);
                s.WriteLine(m_DACibias);
                s.WriteLine(m_DACdcP_V);
                s.WriteLine(m_DACdcN_V);
                s.WriteLine(b_DACdc_V);
                s.WriteLine(m_refP);
                s.WriteLine(m_refN);
                s.WriteLine(b_ref);
                s.WriteLine(m_eweP);
                s.WriteLine(m_eweN);
                s.WriteLine(b_ewe);
                s.WriteLine(m_eceP);
                s.WriteLine(m_eceN);
                s.WriteLine(b_ece);

                for (int i = 0; i < 8; i++) { s.WriteLine(m_iP[i]); }
                for (int i = 0; i < 8; i++) { s.WriteLine(m_iN[i]); }
                for (int i = 0; i < 8; i++) { s.WriteLine(b_i[i]); }

                for (int i = 0; i < 8; i++) { s.WriteLine(m_DACdcP_I[i]); }
                for (int i = 0; i < 8; i++) { s.WriteLine(m_DACdcN_I[i]); }
                for (int i = 0; i < 8; i++) { s.WriteLine(b_DACdc_I[i]); }

                for (int i = 0; i < 10; i++) { s.WriteLine(stagePhaseDelay[i]); }
                for (int i = 0; i < 10; i++) { s.WriteLine(stageDCGain[i]); }
                for (int i = 0; i < 10; i++) { s.WriteLine(stageHFGain_A[i]); }
                for (int i = 0; i < 10; i++) { s.WriteLine(stageHFGain_B[i]); }
                for (int i = 0; i < 10; i++) { s.WriteLine(stageHFGain_C[i]); }
            }

            public void FromCsv(string fileName)
            {
                using (StreamReader reader = new StreamReader(fileName))
                {
                    m_DACac = getNumFromCsv(reader);
                    m_DACircomp = getNumFromCsv(reader);
                    m_DACwebias = getNumFromCsv(reader);
                    m_DACibias = getNumFromCsv(reader);

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

                    for (int i = 0; i < 10; i++) { stagePhaseDelay[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 10; i++) { stageDCGain[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 10; i++) { stageHFGain_A[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 10; i++) { stageHFGain_B[i] = getNumFromCsv(reader); }
                    for (int i = 0; i < 10; i++) { stageHFGain_C[i] = getNumFromCsv(reader); }
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
                    "m_DACac=[{0}] m_DACircomp=[{}] m_DACwebias=[{}]    m_DACibias=[{}] m_DACdcP_V=[{}] m_DACdcN_V=[{}] b_DACdc_V=[{}] m_refP=[{}] m_refN=[{}] b_ref=[{}] m_eweP=[{}] m_eweN=[{}] b_ewe=[{}] m_eceP=[{}] m_eceN=[{}] b_ece=[{}] m_iP=[{}] m_iN=[{}] b_i=[{}] m_DACdcP_I=[{}] m_DACdcN_I=[{}] b_DACdc_I=[{}] stagePhaseDelay=[{}] stageDCGain=[{}] stageHFGain_A=[{}] stageHFGain_B=[{}] stageHFGain_C=[{}]",
                     m_DACac,      m_DACircomp,     m_DACwebias,        m_DACibias,     m_DACdcP_V,     m_DACdcN_V,     b_DACdc_V,     m_refP,     m_refN,     b_ref,     m_eweP,     m_eweN,     b_ewe,     m_eceP,     m_eceN,     b_ece, string.Join(",", m_iP), string.Join(",", m_iN), string.Join(",", b_i), string.Join(",", m_DACdcP_I), string.Join(",", m_DACdcN_I), string.Join(",", b_DACdc_I), string.Join(",", stagePhaseDelay), string.Join(",", stageDCGain), string.Join(",", stageHFGain_A), string.Join(",", stageHFGain_B), string.Join(",", stageHFGain_C)
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
