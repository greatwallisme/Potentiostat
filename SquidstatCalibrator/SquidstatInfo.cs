namespace SSC
{
    partial class SquidstatCalibrator
    {
        public enum SquidstatVersions { V1, V2 }

        public static SquidstatVersions _squidstatVersion = SquidstatVersions.V1;

        const byte FRAME1 = 0xEE;
        const byte FRAME2 = 0xFF;
        public static byte CHANNEL;

        const int iCMDRSP = 2; // index of command/response byte
        const int iCHAN_NUM = 3; //index of channel sending/receiving packet

        const int H = 6; // number of header bytes
    }
}
