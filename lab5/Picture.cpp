#include "Picture.h"

Picture::Picture()
{
    this->Width = 0;
    this->Height = 0;
    this->FormatNum = 0;
    this->ColorDepth = 0;
    this->Multiplier = 0;
    this->Offset = 0;
    this->type = -1;
    this->P = 0;
}

Picture::~Picture()
{
    Pixels.clear();
}

void Picture::GetFile(string& FName)
{
    ifstream Input(FName, ios::binary);
    if (!Input.is_open())
        throw("Filed to open input file!");

    Input >> this->P >> this->FormatNum;
    if(P != 'P' || (FormatNum != '5' && FormatNum != '6'))
        throw("Unknown format! Only for P5 or P6 format files");

    Input >> this->Width >> this->Height;
    Pixels.assign(this->Height, vector<Pixel>(this->Height));

    Input >> this->ColorDepth;
    if(this->ColorDepth != 255)
        throw("Invalid ColorDepth! Must be equal 255");

    switch(FormatNum)
    {
        case '5':
        {
            char buf = Input.readsome(&buf, 1);
            for (int i = 0; i < this->Height; ++i)
            {
                for (int j = 0; j < this->Width; ++j)
                {
                    char buf;
                    Input.read(&buf, sizeof(char));
                    Pixels[i][j].First = buf;
                    Pixels[i][j].Second = buf;
                    Pixels[i][j].Third = buf;
                }
            }
            break;
        }
        case '6':
        {
            char buf = Input.readsome(&buf, 1);
            for (int i = 0; i < this->Height; ++i)
            {
                for (int j = 0; j < this->Width; ++j)
                {
                    char f, s, t;
                    Input.read(&f, sizeof(char));
                    Input.read(&s, sizeof(char));
                    Input.read(&t, sizeof(char));

                    Pixels[i][j].First = f;
                    Pixels[i][j].Second = s;
                    Pixels[i][j].Third = t;
                }
            }
            break;
        }
    }
    Input.close();
}

void Picture::WriteFile(string &FName)
{
    ofstream Output(FName, ios::binary);
    if (!Output.is_open())
        throw("Failed to open output file");

    Output << this->P << this->FormatNum << '\n';
    Output << this->Width << ' ' << this->Height << '\n';
    Output << this->ColorDepth << '\n';

    switch(FormatNum)
    {
        case '5':
        {
            for (int i = 0; i < this->Height; ++i)
                for (int j = 0; j < this->Width; ++j)
                    Output << Pixels[i][j].First;
             break;
        }
        case '6':
         {
            for (int i = 0; i < this->Height; ++i)
            {
                for (int j = 0; j < this->Width; ++j)
                {
                    Output << Pixels[i][j].First;
                    Output << Pixels[i][j].Second;
                    Output << Pixels[i][j].Third;
                }
            }
            break;
        }
    }
    Output.flush();
    Output.close();
}

void Picture::WriteParamsConsole()
{
    cout << "Offset: " << this->Offset << endl;
    cout << "Multiplier: " << this->Multiplier << endl;
}

void Picture::SetParams(double Offset, double Multiplier)
{
    this->Offset = Offset;
    this->Multiplier = Multiplier;
}

void Picture::SetType(int Choice)
{
    this->type = Choice;
    switch(this->type)
    {
        case 0:
        {
            UseType0();
            break;
        }
        case 1:
        {
            UseType1();
            break;
        }
        case 2:
        {
            UseType2();
            break;
        }
        case 3:
        {
            UseType3();
            break;
        }
        case 4:
        {

            break;
        }
        case 5:
        {

            break;
        }
    }
}

uchar Picture::UseOffsetAndMultiplier(uchar Pixel)
{
    uchar buf = (Pixel - this->Offset) * this->Multiplier;
    if(buf > 255)
        buf = 255;
    if (buf < 0)
        buf = 0;
    return buf;
}

void Picture::UseType0() //0 - применить указанные значения <смещение> и <множитель> в пространстве RGB к каждому каналу;
{
    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
        {
            Pixels[i][j].First = UseOffsetAndMultiplier(Pixels[i][j].First);
            Pixels[i][j].Second = UseOffsetAndMultiplier(Pixels[i][j].Second);
            Pixels[i][j].Third = UseOffsetAndMultiplier(Pixels[i][j].Third);
        }
}
void Picture::FROM_RGB_TO_YCbCr601()
{
    double CoefR = 0.299;
    double CoefG = 0.587;
    double CoefB = 0.114;

    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
        {
            //R G B must be in [0; 1]
            double R = Pixels[i][j].First / 255.0;
            double G = Pixels[i][j].Second / 255.0;
            double B = Pixels[i][j].Third / 255.0;

            //Y must be in [0; 1]
            double Y = CoefR * R + CoefG * G + CoefB * B;
            //Cb Cr must be in [-0.5; 0.5]
            double Cb = (B - Y) / (2.0 * (1 - CoefB));
            double Cr = (R - Y) / (2.0 * (1 - CoefR));

            //true colors
            Y = round(Y * 255);
            Cb = round ((Cb + 0.5) * 255);
            Cr = round((Cr + 0.5) * 255);

            if (Y > 255)
                Y = 255;
            else if (Y < 255)
                Y = 0;

            if (Cb > 255)
                Cb = 255;
            else if (Cb < 255)
                Cb = 0;

            if (Cr > 255)
                Cr = 255;
            else if (Cr < 255)
                Cr = 0;

            Pixels[i][j].First = Y;
            Pixels[i][j].Second = Cb;
            Pixels[i][j].Third = Cr;
        }
}
void Picture::FROM_YCbCr601_TO_RGB()
{
    double CoefR = 0.299;
    double CoefG = 0.587;
    double CoefB = 0.114;

    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
        {
            double Y = Pixels[i][j].First / 255.0 - 0.5;
            double Cb = Pixels[i][j].Second / 255.0 - 0.5;
            double Cr = Pixels[i][j].Third / 255.0 - 0.5;

            double R = Y + Cr * (2.0 - 2.0 * CoefR);
            double G = Y - (Cb * CoefB * (2.0 - 2.0 * CoefB)) / CoefG
                    - (Cr * CoefR * (2.0 - 2.0 * CoefR)) / CoefG;
            double B = Y + Cb * (2.0 - 2.0 * CoefB);

            R = round(R * 255.0);
            G = round(G * 255.0);
            B = round(B * 255.0);

            if (R > 255)
                R = 255;
            else if (R < 0)
                R = 0;

            if (B > 255)
                B = 255;
            else if (B < 0)
                B= 0;

            if (G > 255)
                G = 255;
            else if (G < 0)
                G = 0;

            Pixels[i][j].First = R;
            Pixels[i][j].Second = G;
            Pixels[i][j].Third = B;
        }
}
void Picture::UseType1() //1 - применить указанные значения <смещение> и <множитель> в пространстве YCbCr.601 к каналу Y;
{
    FROM_RGB_TO_YCbCr601();
    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
            Pixels[i][j].First = UseOffsetAndMultiplier(Pixels[i][j].First);
    FROM_YCbCr601_TO_RGB();
}

void Picture::UseType2() //2 - автояркость в пространстве RGB: <смещение> и <множитель> вычисляются на основе минимального и максимального значений пикселей;
{
    auto NewOffsetMIN = (uchar)0;
    auto NewMultiplierMAX = (uchar)256;

    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
        {
            //ищем смещение и множитель
            if (Pixels[i][j].First > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].First;
            if (Pixels[i][j].Second < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].First;

            if (Pixels[i][j].Second > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].Second;
            if (Pixels[i][j].Second < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].Second;

            if (Pixels[i][j].Third > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].Third;
            if (Pixels[i][j].Third < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].Third;

            //Offset = MIN;
            //Multiplier = 255.0 / (max-min);
            NewMultiplierMAX = 255.0 / (NewMultiplierMAX - NewOffsetMIN);
            this->Offset = NewOffsetMIN;
            this->Multiplier = NewMultiplierMAX;

            Pixels[i][j].First = UseOffsetAndMultiplier(Pixels[i][j].First);
            Pixels[i][j].Second = UseOffsetAndMultiplier(Pixels[i][j].Second);
            Pixels[i][j].Third = UseOffsetAndMultiplier(Pixels[i][j].Third);
        }
}

void Picture::UseType3() //3 - аналогично 2 в пространстве YCbCr.601;
{
    FROM_RGB_TO_YCbCr601();
    auto NewOffsetMIN = (uchar)0;
    auto NewMultiplierMAX = (uchar)256;

    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j) {
            //ищем смещение и множитель
            if (Pixels[i][j].First > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].First;
            if (Pixels[i][j].Second < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].First;

            if (Pixels[i][j].Second > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].Second;
            if (Pixels[i][j].Second < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].Second;

            if (Pixels[i][j].Third > NewMultiplierMAX)
                NewMultiplierMAX = Pixels[i][j].Third;
            if (Pixels[i][j].Third < NewOffsetMIN)
                NewOffsetMIN = Pixels[i][j].Third;

            //Offset = MIN;
            //Multiplier = 255.0 / (max-min);
            NewMultiplierMAX = 255.0 / (NewMultiplierMAX - NewOffsetMIN);
            this->Offset = NewOffsetMIN;
            this->Multiplier = NewMultiplierMAX;

            Pixels[i][j].First = UseOffsetAndMultiplier(Pixels[i][j].First);
            Pixels[i][j].Second = UseOffsetAndMultiplier(Pixels[i][j].Second);
            Pixels[i][j].Third = UseOffsetAndMultiplier(Pixels[i][j].Third);
        }
}
