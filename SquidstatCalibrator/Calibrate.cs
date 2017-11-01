using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SSC
{
    partial class SquidstatCalibrator
    {
        public class ADCDC_Decimal
        {
            public double Ref;
            public double Ewe;
            public double Current;
            public double Ece;

            public ADCDC_Decimal(ADCDC_DATA d)
            {
                Ref = d.Ref / short.MaxValue * SquidstatVoltRange;
                Ewe = d.Ewe / short.MaxValue * SquidstatVoltRange;
                Current = d.CurrentRange / short.MaxValue * _currentRanges[d.CurrentRange];
                Ece = d.Ece / short.MaxValue * SquidstatVoltRange;
            }
        }
    }
}
