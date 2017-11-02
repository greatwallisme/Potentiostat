using System;
using System.Linq;
using System.Text;

// since 
// (1) C# doesn't do struct packing nicely, and
// (2) C# is strictly typed (whereas c++ is weakly typed),
// the code for converting between bytes and structs is a bit large ...

namespace SSC
{
    partial class SquidstatCalibrator
    {
        enum Rsp : byte
        {
            CHANNEL_OK,
            ECE_OVERVOLTAGE_WARNING,
            EWE_OVERVOLTAGE_WARNING,
            REF_OVERVOLTAGE_WARNING,
            OVERCURRENT_WARNING,
            OVERCURRENT_ERROR,
            UNDERCURRENT_WARNING,
            HANDSHAKE_RESPONSE = 65,
            ADCDC_DATA,
            ADCAC_DATA,
            CAL_DATA,
            HW_DATA,
            NUM_CHANNELS_RESP,
            ERROR_MSG,
            STATUS_DATA,
            DATA_RECEIVED_OK,
            EXPERIMENT_NODE_BEGINNING,
            EXPERIMENT_NODE_COMPLETE,
            EXPERIMENT_COMPLETE,
            EXPERIMENT_PAUSED,
            EXPERIMENT_RESUMED,
            DEBUG_LOG_MSG
        };

        public class ADCDC_DATA
        {
            public short Ref     { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 0, 2).ToArray(), 0); } }
            public short Ewe     { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 2, 2).ToArray(), 0); } }
            public short Current { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 4, 2).ToArray(), 0); } }
            public short Ece     { get { return BitConverter.ToInt16(new ArraySegment<byte>(Bytes, H + 6, 2).ToArray(), 0); } }

            public byte CurrentRange  { get { return Bytes[H + 8]; } }

            public float WEgain { get { return BitConverter.ToSingle(new ArraySegment<byte>(Bytes, H + 12, 4).ToArray(),0); } }
            public float Igain { get { return BitConverter.ToSingle(new ArraySegment<byte>(Bytes, H + 16, 4).ToArray(), 0); } }
            // pad 3 bytes
            public ulong Timestamp    { get { return BitConverter.ToUInt64(new ArraySegment<byte>(Bytes, H + 20, 8).ToArray(), 0); } }

            public byte[] Bytes { get; private set; }
            public ADCDC_DATA(byte[] b) { Bytes = b; }

            public override string ToString()
            {
                return string.Format(
                "ref=[{0}] ewe=[{1}] current=[{2}] ece=[{3}] currentRange=[{4}] timestamp=[{5}]",
                 Ref, Ewe, Current, Ece, CurrentRange, Timestamp/Math.Pow(10,8));
            }
        }

        static string DEBUG_LOG_MSG(byte[] b)
        {
            return string.Format("message=[{0}]", Encoding.ASCII.GetString(
                b.Reverse(). // reverse it because "Take" works from the front, and we want the LAST numDataBytes bytes
                Take(b.Length - H). // take the data bytes
                Reverse(). // put it back in the correct order
                ToArray()));
        }

        static string RspStr(byte[] bytes)
        {
            byte b = bytes[2];
            Rsp r = (Rsp)b;
            string m = string.Empty;

            if (r == Rsp.ADCDC_DATA)
                m = new ADCDC_DATA(bytes).ToString();
            else if (r == Rsp.DEBUG_LOG_MSG)
                m = DEBUG_LOG_MSG(bytes);

            return string.Format("RSP {0} ({1}) {2}", r.ToString(), b.ToString(), m);
        }
    }
}
