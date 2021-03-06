#include "Picture.h"

Picture::Picture()
{
    this->Width = 0;
    this->Height = 0;
    this->FormatNum = 0;
    this->ColorDepth = 0;
    this->Multiplier = 0;
    this->Offset = 0;
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
    if (this->Width < 0 || this->Height < 0)
        throw("Broken file! Negative sizes");

    Pixels.assign(this->Height, vector<Pixel>(this->Width));

    Input >> this->ColorDepth;
    if(this->ColorDepth != 255)
        throw("Invalid ColorDepth! Must be equal 255");

    switch(FormatNum)
    {
        case '5':
        {
            char space; //лишний пиксель '\n'
            Input.read(&space, 1);
            for (int i = 0; i < this->Height; ++i)
            {
                for (int j = 0; j < this->Width; ++j)
                {
                    char CurPx;
                    Input.read(&CurPx, sizeof(char));
                    Pixels[i][j].First = CurPx;
                    Pixels[i][j].Second = CurPx;
                    Pixels[i][j].Third = CurPx;
                }
            }
            break;
        }
        case '6':
        {
            char space; //лишний пиксель '\n'
            Input.read(&space, 1);
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
    cout << this->Offset << " " << this->Multiplier << endl;
}

void Picture::SetParams(int Offset, double Multiplier)
{
    this->Offset = Offset;
    this->Multiplier = Multiplier;
}

void Picture::SetType(int Choice)
{
    switch(Choice)
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
        default:
            break;
    }
}

uchar Picture::UseOffsetAndMultiplier(uchar Pixel)
{
    int NewPx = (int)(((double)Pixel - this->Offset) * this->Multiplier);

    if (NewPx > 255) NewPx = 255;
    if (NewPx < 0) NewPx = 0;

    return (uchar)NewPx;
}

void Picture::UseType0() //0 - применить указанные значения <смещение> и <множитель> в пространстве RGB к каждому каналу;
{
    for (int i = 0; i < this->Height; ++i)
        for (int j = 0; j < this->Width; ++j)
        {
            this->Pixels[i][j].First = UseOffsetAndMultiplier(this->Pixels[i][j].First);
            this->Pixels[i][j].Second = UseOffsetAndMultiplier(this->Pixels[i][j].Second);
            this->Pixels[i][j].Third = UseOffsetAndMultiplier(this->Pixels[i][j].Third);
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
            double R = this->Pixels[i][j].First / 255.0;
            double G = this->Pixels[i][j].Second / 255.0;
            double B = this->Pixels[i][j].Third / 255.0;

            //Y must be in [0; 1]
            double Y = CoefR * R + CoefG * G + CoefB * B;
            //Cb Cr must be in [-0.5; 0.5]
            double Cb = (B - Y) / (2.0 * (1 - CoefB));
            double Cr = (R - Y) / (2.0 * (1 - CoefR));

            //true colors
            int ResultY = (int)round(Y * 255);
            int ResultCb = (int)round ((Cb + 0.5) * 255);
            int ResultCr = (int)round((Cr + 0.5) * 255);

            if (ResultY > 255)
                ResultY = 255;
            if (ResultY < 0)
                ResultY = 0;

            if (ResultCb > 255)
                ResultCb = 255;
            if (ResultCb < 0)
                ResultCb = 0;

            if (ResultCr > 255)
                ResultCr = 255;
            if (ResultCr < 0)
                ResultCr = 0;

            this->Pixels[i][j].First = (uchar)ResultY;
            this->Pixels[i][j].Second = (uchar)ResultCb;
            this->Pixels[i][j].Third = (uchar)ResultCr;
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
            double Y = this->Pixels[i][j].First / 255.0;
            double Cb = this->Pixels[i][j].Second / 255.0 - 0.5;
            double Cr = this->Pixels[i][j].Third / 255.0 - 0.5;

            double R = Y + Cr * (2.0 - 2.0 * CoefR);
            double G = Y - Cb * CoefB * (2.0 - 2.0 * CoefB) / CoefG
                    - Cr * CoefR * (2.0 - 2.0 * CoefR) / CoefG;
            double B = Y + Cb * (2.0 - 2.0 * CoefB);

            int ResultR = (int)round(R * 255.0);
            int ResultG = (int)round(G * 255.0);
            int ResultB = (int)round(B * 255.0);

            if (ResultR > 255)
                ResultR = 255;
            if (ResultR < 0)
                ResultR = 0;

            if (ResultG > 255)
                ResultG = 255;
            if (ResultG < 0)
                ResultG = 0;

            if (ResultB > 255)
                ResultB = 255;
            if (ResultB < 0)
                ResultB= 0;

            this->Pixels[i][j].First = (uchar)ResultR;
            this->Pixels[i][j].Second = (uchar)ResultG;
            this->Pixels[i][j].Third = (uchar)ResultB;
        }
}
void Picture::UseType1() //1 - применить указанные значения <смещение> и <множитель> в пространстве YCbCr.601 к каналу Y;
{
    FROM_RGB_TO_YCbCr601();
    //если цветное только для Y канала, если черно-белое, только для 1 канала
        for (int i = 0; i < this->Height; ++i)
            for (int j = 0; j < this->Width; ++j)
                this->Pixels[i][j].First = UseOffsetAndMultiplier(this->Pixels[i][j].First);

    FROM_YCbCr601_TO_RGB();
}
void Picture::UseType2()//2 - автояркость в пространстве RGB: <смещение> и <множитель> вычисляются на основе минимального и максимального значений пикселей;
{
    uchar MIN = 255;
    uchar MAX = 0;

    if (FormatNum == '6') //если цветное, ищем смещение и множители по всем каналам
    {
        for (int i = 0; i < this->Height; ++i)
        {
            for (int j = 0; j < this->Width; ++j)
            {
                //ищем смещение и множитель среди всех каналов
                if (this->Pixels[i][j].First > MAX)
                    MAX = this->Pixels[i][j].First;
                if (this->Pixels[i][j].First < MIN)
                    MIN = this->Pixels[i][j].First;

                if (this->Pixels[i][j].Second > MAX)
                    MAX = this->Pixels[i][j].Second;
                if (this->Pixels[i][j].Second < MIN)
                    MIN = this->Pixels[i][j].Second;

                if (this->Pixels[i][j].Third > MAX)
                    MAX = this->Pixels[i][j].Third;
                if (this->Pixels[i][j].Third < MIN)
                    MIN = this->Pixels[i][j].Third;
            }
        }
    }
    else //если черно-белое, только по 1 каналу
    {
        for (int i = 0; i < this->Height; ++i)
        {
            for (int j = 0; j < this->Width; ++j)
            {
                //ищем смещение и множитель среди 1 канала
                if (this->Pixels[i][j].First > MAX)
                    MAX = this->Pixels[i][j].First;
                if (this->Pixels[i][j].First < MIN)
                    MIN = this->Pixels[i][j].First;
            }
        }
    }
    //нашли минимум и максимум
    //Offset = MIN;
    //Multiplier = 255.0 / (max-min);
    this->Offset = (int) MIN;
    this->Multiplier = 255.0 / ((int)MAX - (int)MIN); //преобразовываем в int MAX & MIN, что не страшно

    for (int i = 0; i < this->Height; ++i)
    {
        for (int j = 0; j < this->Width; ++j)
        {
            this->Pixels[i][j].First = UseOffsetAndMultiplier(this->Pixels[i][j].First);
            this->Pixels[i][j].Second = UseOffsetAndMultiplier(this->Pixels[i][j].Second);
            this->Pixels[i][j].Third = UseOffsetAndMultiplier(this->Pixels[i][j].Third);
        }
    }
}
void Picture::UseType3() //3 - аналогично 2 в пространстве YCbCr.601;
{
    FROM_RGB_TO_YCbCr601();
    uchar MIN = 255;
    uchar MAX = 0;

        for (int i = 0; i < this->Height; ++i)
            for (int j = 0; j < this->Width; ++j)
            {
                //ищем смещение и множитель среди Y канала
                if (this->Pixels[i][j].First > MAX)
                    MAX = this->Pixels[i][j].First;
                if (this->Pixels[i][j].First < MIN)
                    MIN = this->Pixels[i][j].First;

            }
        //нашли минимум и максимум
        //Offset = MIN;
        //Multiplier = 255.0 / (max-min);
        this->Offset = MIN;
        this->Multiplier = 255.0 / ((int)MAX - (int)MIN);;

        //если цветное только для Y канала, если черно-белое, только для 1 канала
        for (int i = 0; i < this->Height; ++i)
            for (int j = 0; j < this->Width; ++j)
                this->Pixels[i][j].First = UseOffsetAndMultiplier(this->Pixels[i][j].First);

    FROM_YCbCr601_TO_RGB();
}
