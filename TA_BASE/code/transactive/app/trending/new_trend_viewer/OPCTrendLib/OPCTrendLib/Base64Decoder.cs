namespace OPCTrendLib
{
    using System;

    public class Base64Decoder
    {
        private int blockCount;
        private int length;
        private int length2;
        private int length3;
        private int paddingCount;
        private char[] source;

        public Base64Decoder(char[] input)
        {
            int num = 0;
            this.source = input;
            this.length = input.Length;
            for (int i = 0; i < 2; i++)
            {
                if (input[(this.length - i) - 1] == '=')
                {
                    num++;
                }
            }
            this.paddingCount = num;
            this.blockCount = this.length / 4;
            this.length2 = this.blockCount * 3;
        }

        private byte CharToByte(char c)
        {
            char[] chArray = new char[] { 
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
                'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
             };
            if (c != '=')
            {
                for (int i = 0; i < 0x40; i++)
                {
                    if (chArray[i] == c)
                    {
                        return (byte) i;
                    }
                }
            }
            return 0;
        }

        public byte[] GetDecoded()
        {
            int num;
            byte[] buffer = new byte[this.length];
            byte[] buffer2 = new byte[this.length2];
            for (num = 0; num < this.length; num++)
            {
                buffer[num] = this.CharToByte(this.source[num]);
            }
            for (num = 0; num < this.blockCount; num++)
            {
                byte num6 = buffer[num * 4];
                byte num7 = buffer[(num * 4) + 1];
                byte num8 = buffer[(num * 4) + 2];
                byte num9 = buffer[(num * 4) + 3];
                byte num2 = (byte) (num6 << 2);
                byte num3 = (byte) ((num7 & 0x30) >> 4);
                num3 = (byte) (num3 + num2);
                num2 = (byte) ((num7 & 15) << 4);
                byte num4 = (byte) ((num8 & 60) >> 2);
                num4 = (byte) (num4 + num2);
                num2 = (byte) ((num8 & 3) << 6);
                byte num5 = num9;
                num5 = (byte) (num5 + num2);
                buffer2[num * 3] = num3;
                buffer2[(num * 3) + 1] = num4;
                buffer2[(num * 3) + 2] = num5;
            }
            this.length3 = this.length2 - this.paddingCount;
            byte[] buffer3 = new byte[this.length3];
            for (num = 0; num < this.length3; num++)
            {
                buffer3[num] = buffer2[num];
            }
            return buffer3;
        }
    }
}
