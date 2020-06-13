#include "Picture.h"

Picture::Picture(double B, double C, int NewWidth, int NewHeight)
{
    this->BSpline = B;
    this->CSpline = C;
    this->NewWidth = NewWidth;
    this->NewHeight = NewHeight;
}

Picture::~Picture() { PixelsOld.clear(); }

void Picture::GetFile(string& FName)
{
    ifstream Input(FName, ios::binary);
    if (!Input.is_open())
        throw runtime_error("Filed to open input file!");

    Input >> this->P >> this->FormatNum;
    if(P != 'P' || FormatNum != '5')
        throw runtime_error("Unknown format! Only for P5 format files.");

    Input >> this->Width >> this->Height;
    if (this->Width < 0 || this->Height < 0)
        throw runtime_error("Broken file! Negative sizes.");

    PixelsOld.assign(this->Height, vector<uchar>(this->Width));

    Input >> this->ColorDepth;
    if(this->ColorDepth != 255)
        throw runtime_error("Invalid ColorDepth! Must be equal 255.");

            char space; //лишний пиксель '\n'
            Input.read(&space, 1);
            for (int i = 0; i < this->Height; ++i)
                for (int j = 0; j < this->Width; ++j)
                {
                    char CurPx;
                    Input.read(&CurPx, sizeof(char));
                    PixelsOld[i][j] = CurPx;
                }

    Input.close();
}

void Picture::WriteFile(string &FName)
{
    ofstream Output(FName, ios::binary);
    if (!Output.is_open())
        throw runtime_error("Failed to open output file!");

    Output << this->P << this->FormatNum << '\n';
    Output << this->NewWidth << ' ' << this->NewHeight << '\n';
    Output << this->ColorDepth << '\n';


    for (int i = 0; i < this->NewHeight; ++i)
        for (int j = 0; j < this->NewWidth; ++j)
             Output << PixelsNew[i][j];

    Output.flush();
    Output.close();
}

void Picture::SetType(int TypeAlgo)
{
    //0 - ближайший сосед
    //1 - билинейная интерполяция
    //2 - Lanczos3
    //3 - BC - сплайны
    switch(TypeAlgo)
    {
        case 0:
        {
            ClosestNeighbour();
            break;
        }
        case 1:
        {
            Bilinear();
            break;
        }
        case 2:
        {
            Lanczos3();
            break;
        }
        case 3:
        {
            BCSplines();
            break;
        }
        default:
        {
            throw runtime_error("Unknown type! Must be integer in [0; 3].");
            break;
        }
    }
}

void Picture::MakeNewPicture()
{
    PixelsNew.assign(this->NewHeight, vector <uchar>(this->NewWidth));
}
void Picture::ClosestNeighbour()
{
    double CoefHeight = (double)this->Height / (double)this->NewHeight;
    double CoefWidth = (double)this->Width / (double)this->NewWidth;

    for (int i = 0; i < this->NewHeight; ++i)
        for (int j = 0; j < this->NewWidth; ++j)
            this->PixelsNew[i][j] = this->PixelsOld[floor(CoefHeight * i)][floor(CoefWidth * j)];
}
double Picture::Interpolate(double SquareWidth, double SquareHeight, double A, double B, double C, double D)
{
    A *= ((1.0 - SquareWidth) * (1.0 - SquareHeight));
    B *= (SquareWidth*(1.0 - SquareHeight));
    C *= (SquareHeight * (1.0 - SquareWidth));
    D *= (SquareHeight * SquareWidth);

    double ResultPX = A + B + C + D;
    if (ResultPX > 255)
        ResultPX = 255;
    if (ResultPX < 0)
        ResultPX = 0;

    return ResultPX;
}
void Picture::Bilinear()
{
    for (int i = 0; i < this->NewWidth; ++i) //i = cur_x; j = cur_y
        for (int j = 0; j < this->NewHeight; ++j) //бегаем по новым координатам
        {
            double CoefHeight = (double)this->Height / (double)this->NewHeight;
            double CoefWidth = (double)this->Width / (double)this->NewWidth;

            int PxOldX = floor(i * CoefWidth); //для каждой точки в новой системе вычисляем в старой координату X
            int PxOldY = floor(j * CoefHeight); //для каждой точки в новой системе вычисляем в старой координату Y

            double DifferenceX = i * CoefWidth - PxOldX; // Вычисляем разницу между целыми координатами
            double DifferenceY = j * CoefHeight - PxOldY; //в старой системе и вычисленной из новой

            if ((PxOldY < this->NewHeight) && (PxOldX < this->NewWidth)
                && (PxOldX >= 0) && (PxOldY >= 0))
            {
                //конкретная точка
                double A = this->PixelsOld[PxOldY][PxOldX];

                //точка правее A                                        // A.......B
                double B = PixelsOld[PxOldY][PxOldX]; //0               // .........
                if (PxOldX + 1 < this->Width)                           // .........
                    B = this->PixelsOld[PxOldY][PxOldX + 1];            // C.......D

                //точка ниже A
                double C = PixelsOld[PxOldY][PxOldX];//0
                if (PxOldY + 1 < this->Height)
                    C = this->PixelsOld[PxOldY + 1][PxOldX];

                //точка правее и ниже A
                double D = PixelsOld[PxOldY][PxOldX];
                if(PxOldY + 1 < this->Height && PxOldX + 1 < this->Width)
                    D = this->PixelsOld[PxOldY + 1][PxOldX + 1];

                //нашли значения пикселей в старой картинке, на их основе
                //применяем интерполяцию, чтобы получить новый пиксель
                //пикселю новой картинки присваем значение интерполяции координат из старой картинки
                this->PixelsNew[j][i] = Interpolate(DifferenceX, DifferenceY, A, B, C, D);
            }
        }
}

double Picture::LanczoshFilter(double Value) //фильтр Ланцоша (L)
{
    double Result;
    if (Value == 0)
        return 1;
    if (Value >= -3 && Value < 3)
    {
        Result = (3 * sin(M_PI * Value) * sin(M_PI * Value / 3)) / (M_PI * M_PI * Value * Value);
        return Result;
    }
    else
        return 0.0;
}

void Picture::Lanczos3()
{
    for (int x = 0; x < this->NewWidth; ++x) //x = cur_x
    {
        for (int y = 0; y < this->NewHeight; ++y) //y = cur_y
        {
            int a = 3;
            double Result = 0;
            double CoefHeight = (double)this->Height / (double)this->NewHeight;
            double CoefWidth = (double)this->Width / (double)this->NewWidth;

            //оконная функция
            for (int i = y * CoefHeight - a + 1; i < y * CoefHeight + a; i += 1.0)
            {
                for (int j = x * CoefWidth - a + 1; j < x * CoefWidth + a; j += 1.0)
                {
                    //окно не должно выходить за картинку
                    int dy = i, dx = j;
                    /*if (dx >= this->Width || dx < 0
                        || dy >= this->Height || dy < 0)
                        continue;*/
                    if (dx >= this->Width)
                        dx = this->Width - 1;
                    if (dy >= this->Height)
                        dy = this->Height - 1;
                    if (dx < 0)
                        dx = 0;
                    if (dy < 0)
                        dy = 0;

                    double Lx = LanczoshFilter(x * CoefWidth - j); //двумерное ядро Ланцоша
                    double Ly = LanczoshFilter(y * CoefHeight - i); //двумерное ядро Ланцоша
                    Result += this->PixelsOld[dy][dx] * Lx * Ly;
                }
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;

            this->PixelsNew[y][x] = (uchar)Result;
        }
    }
}

double Picture::BCFilterK(double Value)
{
    double Result = 0;
    if (abs(Value) < 1) //без 1/6
        Result = (12 - 9 * this->BSpline - 6 * this->CSpline) * pow(abs(Value), 3)
                + (-18 + 12 * this->BSpline + 6 * this->CSpline) * pow(abs(Value), 2)
                + (6 - 2 * this->BSpline);

    if (abs(Value) >= 1 && abs(Value) < 2) //без 1/6
        Result = (-this->BSpline - 6 * this->CSpline) * pow(abs(Value), 3)
                + (6 * this->BSpline + 30 * this->CSpline) * pow(abs(Value), 2)
                + (-12 * this->BSpline - 48 * this->CSpline) * abs(Value)
                + (8 * this->BSpline + 24 * this->CSpline);
    //если условия не сработали, возвращаем 0
    Result *= (1.0 / 6.0); //с 1/6
    return Result;
}

void Picture::BCSplines()
{
    double CoefHeight = this->Height / this->NewHeight;
    double CoefWidth = this->Width / this->NewWidth;
    vector <vector <uchar>> Buffer(this->NewHeight, vector<uchar>(this->NewWidth));
    //сначала польностью по ширине, затем по высоте
    for (double y = 0; y < this->NewHeight; y += 1 / CoefHeight)
    {
        for (int x = 0; x < this->NewWidth; x++)
        {
            double Result = 0;
            for (int i = 0; i < this->Width; ++i)
            {
                double Kx = BCFilterK(x * CoefWidth - i);
                if (Kx == 0)
                    continue;
                Result += this->PixelsOld[(int)round(y * CoefHeight)][i] * Kx;
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;

            Buffer[(int)floor(y)][x] = (uchar)Result;
        }
    }

    for (int x = 0; x < this->NewWidth; ++x)
    {
        for (int y = 0; y < this->NewHeight; ++y)
        {
            double Result = 0;
            for (double i = 0; i < this->NewHeight; i += 1 / CoefHeight)
            {
                double Ky = BCFilterK((y - i) * CoefHeight);
                if (Ky == 0)
                    continue;
                Result += Buffer[(int)((floor)(i))][x] * Ky; //floor
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;
            this->PixelsNew[y][x] = (uchar)Result;
        }
    }
    Buffer.clear();
}
