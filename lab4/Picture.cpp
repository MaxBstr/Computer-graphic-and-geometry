#include "Picture.h"

vector<string> GetFiles(string Pattern)
{
    int LastDot = -1;
    for(int i = 0; i < Pattern.size(); i++)
        if(Pattern[i] == '.')
            LastDot = i;

    if(LastDot == -1)
        throw runtime_error("WARNING! Pattern has no dots, so can`t match name.ext");

    string FP = Pattern.substr(0, LastDot), SP = Pattern.substr(LastDot);
    return { FP + "_" + "1" + SP, FP + "_" + "2" + SP, FP + "_" + "3" + SP };
}

PIC::PIC(int Count, string FName)
{
    if(Count == 1)
    {
        ifstream fin(FName, ios::binary);
        if(!fin.is_open())
            throw runtime_error("WARNING! Failed to open file");

        char Header[2];
        fin >> Header[0] >> Header[1];
        if(Header[0] != 'P' || Header[1] != '6')
            throw runtime_error("WARNING! Excepted not P6 format");

        fin >> Width >> Height >> ColorDepth;
        if(ColorDepth != 255)
            throw runtime_error("WARNING! Only 255 color depth is supported");

        Picture.assign(Height, vector<SomePixel>(Width));
        char PX;
        fin.read(&PX, 1);

        for (int i = 0; i < Height; i++)
            for (int j = 0; j < Width; j ++)
            {
                SomePixel CurrentPX;
                fin.read(&PX, sizeof(unsigned char));

                CurrentPX.First = PX;
                fin.read(&PX, sizeof(unsigned char));

                CurrentPX.Second = PX;
                fin.read(&PX, sizeof(unsigned char));

                CurrentPX.Third = PX;
                Picture[i][j] = CurrentPX;
            }
        fin.close();
    }
    else
    {
        vector<string> Files = GetFiles(FName);
        if(Files.size() != 3)
            throw runtime_error("WARNING! Got error during name generation");

        for (int k = 0; k < 3; k++)
        {
            ifstream fin(Files[k], ios::binary);
            if(!fin.is_open())
                throw runtime_error("WARNING! Failed to open file " + Files[k]);

            char Header[2];
            fin >> Header[0] >> Header[1];
            if(Header[0] != 'P' || Header[1] != '5')
                throw runtime_error("WARNING! Excepted not P5 format");

            fin >> Width >> Height >> ColorDepth;

            if(ColorDepth != 255)
                throw runtime_error("WARNING! Only 255 color depth is supported");

            if(Picture.size() == 0)
                Picture.assign(Height, vector<SomePixel>(Width));

            char PX;
            fin.read(&PX, 1);

            for (int i = 0; i < Height; i++)
            {
                for (int j = 0; j < Width; j++)
                {
                    fin.read(&PX, sizeof(unsigned char));

                    if (k == 0)
                        Picture[i][j].First = PX;

                    if (k == 1)
                        Picture[i][j].Second = PX;

                    if (k == 2)
                        Picture[i][j].Third = PX;
                }
            }
            fin.close();
        }
    }
}

void PIC::WriteToFile(int Count, string FName)
{
    if(Count == 1)
    {
        ofstream OutputFile(FName, ios::binary);
        if(!OutputFile.is_open())
            throw runtime_error("WARNING! Cannot open output file");

        OutputFile << "P6\n" << Width << ' ' << Height << '\n' << ColorDepth << '\n';

        for(int i = 0; i < Height; i++)
        {
            for (int j = 0; j < Width; j++)
            {
                OutputFile << Picture[i][j].First;
                OutputFile << Picture[i][j].Second;
                OutputFile << Picture[i][j].Third;
            }
        }
        OutputFile.flush();
        OutputFile.close();
    }
    else
    {
        vector<string> Files = GetFiles(FName);
        if(Files.size() != 3)
            throw runtime_error("WARNING! Error during name generation");

        for(int k = 0; k < 3; k++)
        {
            ofstream OutputFile(Files[k], ios::binary);
            if(!OutputFile.is_open())
                throw runtime_error("WARNING! Cannot open output file " + Files[k]);

            OutputFile << "P5\n" << Width << ' ' << Height << '\n' << ColorDepth << '\n';

            for (int i = 0; i < Height; i++)
                for (int j = 0; j < Width; j++)
                {
                    if(k == 0)
                        OutputFile << Picture[i][j].First;
                    if(k == 1)
                        OutputFile << Picture[i][j].Second;
                    if(k == 2)
                        OutputFile << Picture[i][j].Third;
                }

            OutputFile.flush();
            OutputFile.close();
        }
    }
}

void PIC::ChangeSpace(string StartColorSpace, string EndColorSpace)
{
    // convert RGB
    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {

            if (StartColorSpace == "HSV")
                Picture[i][j] = FROM_HSV_TO_RGB(Picture[i][j]);

            if (StartColorSpace == "HSL")
                Picture[i][j] = FROM_HSL_TO_RGB(Picture[i][j]);

            if (StartColorSpace == "CMY")
                Picture[i][j] = FROM_CMY_TO_RGB(Picture[i][j]);

            if (StartColorSpace == "YCoCg")
                Picture[i][j] = FROM_YCoCg_TO_RGB(Picture[i][j]);

            if (StartColorSpace == "YCbCr.601")
                Picture[i][j] = FROM_YCbCr601_TO_RGB(Picture[i][j]);

            if (StartColorSpace == "YCbCr.709")
                Picture[i][j] = FROM_YCbCr709_TO_RGB(Picture[i][j]);
        }
    }

    // convert StartColorSpace rgb EndColorSpace needed
    for(int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            if (EndColorSpace == "HSL")
                Picture[i][j] = FROM_RGB_TO_HSL(Picture[i][j]);
            if (EndColorSpace == "HSV")
                Picture[i][j] = FROM_RGB_TO_HSV(Picture[i][j]);
            if (EndColorSpace == "CMY")
                Picture[i][j] = FROM_RGB_TO_CMY(Picture[i][j]);
            if (EndColorSpace == "YCoCg")
                Picture[i][j] = FROM_RGB_TO_YCoCg(Picture[i][j]);
            if (EndColorSpace == "YCbCr.601")
                Picture[i][j] = FROM_RGB_TO_YCbCr601(Picture[i][j]);
            if (EndColorSpace == "YCbCr.709")
                Picture[i][j] = FROM_RGB_TO_YCbCr709(Picture[i][j]);
        }
    }
}

SomePixel FROM_HSL_TO_RGB(SomePixel PX)
{
    double Hue = PX.First / 255.0;
    double Saturation = PX.Second / 255.0;
    double Lightness = PX.Third / 255.0;
    double K, M;

    if(Lightness < 0.5)
        K = Lightness * (Saturation + 1.0);
    else
        K = Lightness + Saturation - (Lightness * Saturation);

    M = Lightness * 2 - K;
    double TR = Hue + 1.0 / 3.0, TG = Hue, TB = Hue - 1.0 / 3.0;

    if(TR < 0) TR += 1.0;
    if(TR > 1.0) TR -= 1.0;
    if(TG < 0) TG += 1.0;
    if(TG > 1.0) TG -= 1.0;
    if(TB < 0) TB += 1.0;
    if(TB > 1.0) TB -= 1.0;

    double R, G, B;

    if(TR < 1.0 / 6.0)
        R = M + ((K - M) * 6.0 * TR);

    else if(TR >= 1.0 / 6.0 && TR < 0.5)
        R = K;

    else if(TR >= 0.5 && TR < 2.0 / 3.0)
        R = M + ((K - M) * (2.0 / 3.0 - TR) * 6.0);

    else
        R = M;

    if(TG < 1.0 / 6.0)
        G = M + ((K - M) * 6.0 * TG);

    else if(TG >= 1.0 / 6.0 && TG < 0.5)
        G = K;

    else if(TG >= 0.5 && TG < 2.0 / 3.0)
        G = M + ((K - M) * (2.0 / 3.0 - TG) * 6.0);

    else
        G = M;

    if(TB < 1.0 / 6.0)
        B = M + ((K - M) * 6.0 * TB);

    else if(TB >= 1.0 / 6.0 && TB < 0.5)
        B = K;

    else if(TB >= 0.5 && TB < 2.0 / 3.0)
        B = M + ((K - M) * (2.0 / 3.0 - TB) * 6.0);

    else
        B = M;

    return SomePixel
    {
            (unsigned char)round(R * 255),
            (unsigned char)round(G * 255),
            (unsigned char)round(B * 255)
    };
}

SomePixel FROM_RGB_TO_HSL(SomePixel PX)
{
    double R = PX.First * 1.0 / 255;
    double G = PX.Second * 1.0 / 255;
    double B = PX.Third * 1.0 / 255;

    double MX = max(R, max(G, B));
    double MN = min(R, min(G, B));
    double Hue, Saturation, Lightness = (MX + MN) / 2;
    Saturation = (MX - MN) / (1 - abs(1 - (MX + MN)));

    if(MX == MN)
        Hue = 0;

    else if(R == MX && G >= B)
        Hue = (G - B) / (MX - MN) * 60;

    else if(R == MX && G < B)
        Hue = (G - B) / (MX - MN) * 60 + 360;

    else if(MX == G)
        Hue = (B - R) / (MX - MN) * 60 + 120;

    else if(MX == B)
        Hue = (R - G) / (MX - MN) * 60 + 240;

    return SomePixel
    {
            (unsigned char)round(Hue * 255 / 360),
            (unsigned char)round(Saturation * 255),
            (unsigned char)round(Lightness * 255)
    };
}

SomePixel FROM_HSV_TO_RGB(SomePixel PX)
{
    double Hue = PX.First / 255.0 * 360.0;
    double Saturation = PX.Second / 255.0;
    double Value = PX.Third / 255.0; //brightness

    double C = Value * Saturation;
    double HH = Hue / 60;
    double X = C * (1 - abs(((int) HH) % 2 + (HH - (int)HH) - 1));
    double M = Value - C;
    double R, G, B;

    if(Hue >= 0 && Hue <= 60)
        R = C, G = X, B = 0;

    else if(Hue >= 60 && Hue <= 120)
        R = X, G = C, B = 0;

    else if(Hue >= 120 && Hue <= 180)
        R = 0, G = C, B = X;

    else if(Hue >= 180 && Hue <= 240)
        R = 0, G = X, B = C;

    else if(Hue >= 240 && Hue <= 300)
        R = X, G = 0, B = C;

    else
        R = C, G = 0, B = X;

    int RR = (int)(round((R + M) * 255));
    int GG = (int)(round((G + M) * 255));
    int BB = (int)(round((B + M) * 255));

    if(RR < 0) RR = 0;
    if(RR > 255) RR = 255;
    if(BB < 0) BB = 0;
    if(BB > 255) BB = 255;
    if(GG < 0) GG = 0;
    if(GG > 255) GG = 255;
    return SomePixel
    {
        (unsigned char)RR,
        (unsigned char)GG,
        (unsigned char)BB
    };
}

SomePixel FROM_RGB_TO_HSV(SomePixel PX)
{
    double R = PX.First * 1.0 / 255;
    double G = PX.Second * 1.0 / 255;
    double B = PX.Third * 1.0 / 255;

    double MX = max(R, max(G, B));
    double MN = min(R, min(G, B));
    double H, S, V = MX, C = MX - MN;

    if(C == 0)
        H = 0;

    else if(V == R)
        H = (G - B) / C;

    else if(V == G)
        H = 2 + (B - R) / C;

    else
        H = 4 + (R - G) / C;

    H *= 60.0;
    if(H < 0)
        H += 360;

    S = (V == 0 ? 0 : C / V);

    return SomePixel
    {
            (unsigned char)round(H / 360.0 * 255.0),
            (unsigned char)round(S * 255.0),
            (unsigned char)round(V * 255.0)
    };
}

SomePixel FROM_CMY_TO_RGB(SomePixel PX)
{
    return SomePixel
    {(unsigned char)(255 - PX.First),
     (unsigned char)(255 - PX.Second),
     (unsigned char)(255 - PX.Third)
    };
}

SomePixel FROM_RGB_TO_CMY(SomePixel PX)
{
    return SomePixel
    {(unsigned char)(255 - PX.First),
     (unsigned char)(255 - PX.Second),
     (unsigned char)(255 - PX.Third)
    };
}

SomePixel FROM_RGB_TO_YCoCg(SomePixel PX)
{
    double R = PX.First * 1.0 / 255;
    double G = PX.Second * 1.0 / 255;
    double B = PX.Third * 1.0 / 255;
    double Y = R / 4 + G / 2 + B / 4; //brightness
    double ChromaOrange = R / 2 - B / 2 + 0.5;
    double ChromaGreen = -R / 4 + G / 2 - B / 4 + 0.5;

    if(Y > 1.0) Y = 1.0;
    if(Y < 0) Y = 0;
    if(ChromaOrange > 1.0) ChromaOrange = 1.0;
    if(ChromaOrange < 0) ChromaOrange = 0;
    if(ChromaGreen > 1.0) ChromaGreen = 1.0;
    if(ChromaGreen < 0) ChromaGreen = 0;

    return SomePixel
    {
            (unsigned char)(Y * 255),
            (unsigned char)(ChromaOrange * 255),
            (unsigned char)(ChromaGreen * 255)
    };
}

SomePixel FROM_YCoCg_TO_RGB(SomePixel PX)
{
    double Y = PX.First * 1.0 / 255;
    double ChromaOrange = PX.Second * 1.0 / 255 - 0.5;
    double ChromaGreen = PX.Third * 1.0 / 255 - 0.5;
    double R = Y + ChromaOrange - ChromaGreen;
    double G = Y + ChromaGreen;
    double B = Y - ChromaOrange - ChromaGreen;

    if(R > 1.0) R = 1.0;
    if(R < 0) R = 0;
    if(G > 1.0) G = 1.0;
    if(G < 0) G = 0;
    if(B > 1.0) B = 1.0;
    if(B < 0) B = 0;

    return SomePixel
    {
            (unsigned char)(R * 255),
            (unsigned char)(G * 255),
            (unsigned char)(B * 255)
    };
}

SomePixel FROM_RGB_TO_YCbCr601(SomePixel PX) {
    double R = PX.First / 255.0;
    double G = PX.Second / 255.0;
    double B = PX.Third / 255.0;
    double KB = 0.299;
    double KR = 0.587;
    double KG = 0.114;

    double Y = KR * R + KG * G + KB * B;
    double CB = (B - Y) / (2.0 * (1.0 - KB));
    double CR = (R - Y) / (2.0 * (1.0 - KR));
    int RR = (int) round(Y * 255), GG = (int) round((CB + 0.5) * 255), BB = (int) round((CR + 0.5) * 255);

    if (RR < 0) RR = 0;
    if (RR > 255) RR = 255;
    if (BB < 0) BB = 0;
    if (BB > 255) BB = 255;
    if (GG < 0) GG = 0;
    if (GG > 255) GG = 255;

    return SomePixel
            {
                    (unsigned char) RR,
                    (unsigned char) GG,
                    (unsigned char) BB
            };
}

SomePixel FROM_YCbCr601_TO_RGB(SomePixel PX)
{
    double Y = PX.First / 255.0;
    double CB = PX.Second / 255.0 - 0.5;
    double CR = PX.Third / 255.0 - 0.5;
    double KB = 0.299;
    double KR = 0.587;
    double KG = 0.114;

    double R = Y + (2.0 - 2.0 * KR) * CR;
    double G = Y - KB * (2.0 - 2.0 * KB) * CB / KG - KR * (2 - 2.0 * KR) * CR / KG;
    double B = Y + (2.0 - 2.0 * KB) * CB;
    int RR = (int)round(R * 255), GG = (int)round(G * 255), BB = (int)round(B * 255);

    if(RR > 255) RR = 255;
    if(RR < 0) RR = 0;
    if(GG > 255) GG = 255;
    if(GG < 0) GG = 0;
    if(BB > 255) BB = 255;
    if(BB < 0) BB = 0;

    return SomePixel
    {
        (unsigned char)RR,
        (unsigned char)GG,
        (unsigned char)BB
    };
}

SomePixel FROM_RGB_TO_YCbCr709(SomePixel PX)
{
    double R = PX.First / 255.0;
    double G = PX.Second / 255.0;
    double B = PX.Third / 255.0;
    double KB = 0.0722;
    double KR = 0.2126;
    double KG = 0.7152;

    double Y = KR * R + KG * G + KB * B;
    double CB = (B - Y) / (2 * (1 - KB));
    double CR = (R - Y) / (2 * (1 - KR));
    int RR = round(Y * 255), GG = round((CB + 0.5) * 255), BB = round((CR + 0.5) * 255);

    if(RR < 0) RR = 0;
    if(RR > 255) RR = 255;
    if(BB < 0) BB = 0;
    if(BB > 255) BB = 255;
    if(GG < 0) GG = 0;
    if(GG > 255) GG = 255;

    return SomePixel
    {
        (unsigned char)RR,
        (unsigned char)GG,
        (unsigned char)BB
    };
}

SomePixel FROM_YCbCr709_TO_RGB(SomePixel PX)
{
    double Y = PX.First / 255.0;
    double CB = PX.Second / 255.0 - 0.5;
    double CR = PX.Third / 255.0 - 0.5;
    double KB = 0.0722;
    double KR = 0.2126;
    double KG = 0.7152;
    double R = Y + (2 - 2 * KR) * CR;
    double G = Y - KB * (2 - 2 * KB) * CB / KG - KR * (2 - 2 * KR) * CR / KG;
    double B = Y + (2 - 2 * KB) * CB;

    int RR = round(R * 255), GG = round(G * 255), BB = round(B * 255);
    if(RR > 255) RR = 255;
    if(RR < 0) RR = 0;
    if(GG > 255) GG = 255;
    if(GG < 0) GG = 0;
    if(BB > 255) BB = 255;
    if(BB < 0) BB = 0;

    return SomePixel
    {
        (unsigned char)RR,
        (unsigned char)GG,
        (unsigned char)BB
    };
}
