#include "Picture.h"

Picture::Picture(double B, double C, int NewWidth, int NewHeight)
{
    this->BSpline = B;
    this->CSpline = C;
    this->NewWidth = NewWidth;
    this->NewHeight = NewHeight;
}

Picture::~Picture()
{
    PixelsOld.clear();
    PixelsNew.clear();
}

void Picture::GetFile(string& FName)
{
    ifstream Input(FName, ios::binary);
    if (!Input.is_open())
        throw runtime_error("Failed to open input file!");

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
    //2 - Ланцош
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
            Lanczosh();
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
                else if (PxOldY + 1 < this->Height)
                    D = this->PixelsOld[PxOldY + 1][PxOldX];
                else if (PxOldX + 1 < this->Width)
                    D = this->PixelsOld[PxOldY][PxOldX + 1];

                //нашли значения пикселей в старой картинке, на их основе
                //применяем интерполяцию, чтобы получить новый пиксель
                //пикселю новой картинки присваем значение интерполяции координат из старой картинки
                this->PixelsNew[j][i] = Interpolate(DifferenceX, DifferenceY, A, B, C, D);
        }
}

double Picture::LanczoshFilter(double Value) //фильтр Ланцоша (L)
{
    if (Value == 0)
        return 1.0;
    if (Value >= -3 && Value < 3)
    {
        double Result = (3 * sin(PI * Value) * sin(PI * Value / 3)) / (PI * PI * Value * Value);
        return Result;
    }
    else
        return 0.0;
}

void Picture::Lanczosh()
{
    for (int i = 0; i < this->NewWidth; ++i) //i = cur_x
    {
        for (int j = 0; j < this->NewHeight; ++j) //j = cur_y
        {
            int a = 3;
            double Result = 0;
            double CoefHeight = (double)this->Height / (double)this->NewHeight;
            double CoefWidth = (double)this->Width / (double)this->NewWidth;

            //оконная функция
            //формулы для k и z взяты с вики, сумма от целая часть(х) - а + 1 до целая часть(х) + a
            for (int k = j * CoefHeight - a; k < j * CoefHeight + a; k += 1.0)
            {
                for (int z = i * CoefWidth - a; z < i * CoefWidth + a; z += 1.0)
                {
                    //окно не должно выходить за картинку
                    int dy = k, dx = z;
                    //проверка границ
                    if (dx >= this->Width)
                        dx = this->Width - 1;
                    if (dy >= this->Height)
                        dy = this->Height - 1;
                    if (dx < 0)
                        dx = 0;
                    if (dy < 0)
                        dy = 0;

                    double Lx = LanczoshFilter(i * CoefWidth - z); //двумерное ядро Ланцоша
                    double Ly = LanczoshFilter(j * CoefHeight - k); //двумерное ядро Ланцоша
                    Result += this->PixelsOld[dy][dx] * Lx * Ly; //добавляем в сумму отфильтрованное значение
                }
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;

            this->PixelsNew[j][i] = (uchar)Result;
        }
    }
}

double Picture::BCFilterK(double Value) //формула фильтра с немецкой вики
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
    double CoefHeight = (double)this->Height / (double)this->NewHeight;
    double CoefWidth = (double)this->Width / (double)this->NewWidth;
    vector <vector <uchar>> Buffer(this->NewHeight, vector<uchar>(this->NewWidth));
    //ходим  по картинке в новых координатах по ширине!
    //перемещаемся так, чтобы попадать только в точки, существующие в старых коо-ах
    //растягиваем картинку по горизонтали, по вертикали оставляем пробелы
    for (double i = 0; i < this->NewHeight; i += 1 / CoefHeight)
    {
        for (int j = 0; j < this->NewWidth; j++)
        {
            double Result = 0;
            for (int k = -5 / CoefWidth; k < this->Width + 5 / CoefWidth; ++k)
            {
                int dk = k;
                if (dk < 0)
                    dk = 0;
                if (dk >= this->Width)
                    dk = this->Width - 1; //для правой границы убирает черную линию
                //высчитываем фильтр для точки
                double Kx = BCFilterK(j * CoefWidth - k);
                if (Kx == 0)
                    continue;
                //суммируем отфильтрованные значения
                Result += this->PixelsOld[(int)round(i * CoefHeight)][dk] * Kx;
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;
            //записываем в картинку сглаженную строку
            Buffer[(int)floor(i)][j] = (uchar)Result;
        }
    }

    //ходим по новым координатам
    for (int i = 0; i < this->NewWidth; ++i)
    {
        for (int j = 0; j < this->NewHeight; ++j)
        {
            double Result = 0;
            // Здесь проходим по столбцу, задействуя только строки,
            // записанные на прошлом проходе
            for (double k = -5 / CoefHeight; k < this->NewHeight + 5 / CoefHeight; k += 1 / CoefHeight)
            {
                double dk = k;
                if (dk < 0)
                    dk = 0;
                if (dk >= this->NewHeight)
                    dk = this->NewHeight - 1 / CoefHeight;
                // Высчитываем фильтр для конкретной точки,
                // не забыв, что нужно дать значние для старых координат
                double Ky = BCFilterK((j - k) * CoefHeight);
                if (Ky == 0)
                    continue;
                // Суммируем фильтрованные значения
                Result += Buffer[(int)((floor)(dk))][i] * Ky; //floor
            }
            if (Result > 255)
                Result = 255;
            if (Result < 0)
                Result = 0;
            //запись готового пикселя в картинку
            this->PixelsNew[j][i] = (uchar)Result;
        }
    }
    Buffer.clear();
}
