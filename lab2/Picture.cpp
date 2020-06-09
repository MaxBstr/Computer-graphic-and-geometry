#include "Picture.h"
#include <cmath>
#include <algorithm>
#include <fstream>


double sqr(double a) { return a * a; }

double GetDistance(NewPoint p1, NewPoint p2) {
    return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));
}
double TriangleSquare(NewPoint A, NewPoint B, NewPoint C)
{
    double a = GetDistance(A, B);
    double b = GetDistance(B, C);
    double c = GetDistance(C, A);
    double p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

bool PointInRectangle(NewPoint A, NewPoint B, NewPoint C, NewPoint D, NewPoint P)
{
    const double RectHeight = GetDistance(A, B);
    const double RectWidth = GetDistance(A, C);
    const double RectSquare = RectHeight * RectWidth;
    const double PossibleSize = TriangleSquare(P, A, B) + TriangleSquare(P, A, C) + TriangleSquare(P, C, D) + TriangleSquare(P, D, B);
    return fabs(RectSquare - PossibleSize) < 1e-5;
}

double SquareIntersection(NewPoint A, NewPoint B, NewPoint C, NewPoint D, double x, double y)
{
    if (
        PointInRectangle(A, B, C, D, NewPoint{x, y}) &&
        PointInRectangle(A, B, C, D, NewPoint{x + 1.0, y * 1.0}) &&
        PointInRectangle(A, B, C, D, NewPoint{x * 1.0, y + 1.0}) &&
        PointInRectangle(A, B, C, D, NewPoint{x + 1.0, y + 1.0})
        )
        return 1.0;

    int Ins = 0;
    for(double i = x + 0.1; i + 0.1 <= x + 1; i += 0.1)
        for(double j = y + 0.1; j + 0.1 <= y + 1; j += 0.1)
            if(PointInRectangle(A, B, C, D, NewPoint{i, j}))
                Ins++;

    return Ins * 1.0 / 100;
}


NewPoint::NewPoint(double x, double y)
{
    this->x = x;
    this->y = y;
}


PGM::PGM(string FName)
{
    ifstream InputFile(FName, ios::binary);
    if(!InputFile.is_open())
        throw runtime_error("Failed to open file");

    char Header[2];
    InputFile >> Header[0] >> Header[1];
    if(Header[0] != 'P' || Header[1] != '5')
        throw runtime_error("Excepted not P5 format");

    InputFile >> this->Width >> this->Height >> this->ColorDepth;
    this->Picture.assign(this->Height, vector<unsigned char>(this->Width));

    char PX;
    InputFile.read(&PX, 1);

    for (int i = 0; i < this->Height; i++)
    {
        for (int j = 0; j < this->Width; j++)
        {
            InputFile.read(&PX, sizeof(unsigned char));
            this->Picture[i][j] = PX;
        }
    }
    InputFile.close();
}


void PGM::WriteToFile(string FName)
{
    ofstream OutputFile(FName, ios::binary);
    if(!OutputFile.is_open()) {
        throw runtime_error("Output file did not open");
    }

    OutputFile << "P5\n" << Width << ' ' << Height << '\n' << ColorDepth << '\n';

    for(int i = 0; i < Height; i ++)
        for(int j = 0; j < Width; j ++)
            OutputFile << (unsigned char)(Picture[i][j]);

    OutputFile.flush();
    OutputFile.close();
}


void PGM::MakePlot(int x, int y, double Brightness, int Color, double Gamma, bool SRGB)
{
    if (x < 0 || x >= this->Width || y < 0 || y >= this->Height || Brightness < 0)
        return;

    double PX = (double)(this->Picture[y][x]) / this->ColorDepth;
    double ColorCorrect = Color * 1.0 / 255;

    if (SRGB)
    {
        PX = (PX < 0.04045 ? PX / 12.92 : pow((PX + 0.055) / 1.055, Gamma));
        ColorCorrect = (ColorCorrect < 0.04045 ? ColorCorrect / 12.92 : pow((ColorCorrect + 0.055) / 1.055, Gamma));
    }
    else
    {
        PX = pow(PX, Gamma);
        ColorCorrect = pow(ColorCorrect, Gamma);
    }

    PX *= (1.0 - Brightness);
    PX += Brightness * ColorCorrect;

    if(SRGB)
        PX = (PX <= 0.0031308 ? PX * 12.92 : pow(PX, 1.0 / Gamma) * 1.055 - 0.055);
    else
        PX = pow(PX, 1.0 / Gamma);

    if(PX >= 0.9999) PX = 1.0;
    this->Picture[y][x] = (unsigned char)(this->ColorDepth * PX);
}


void PGM::DrawLine(NewPoint StartPoint, NewPoint EndPoint, double Thickness, int Color, double Gamma, bool SRGB)
{
    if(StartPoint.x == EndPoint.x)
    {
        StartPoint.x += 0.5;
        EndPoint.x += 0.5;
    }
    if(StartPoint.y == EndPoint.y)
    {
        StartPoint.y += 0.5;
        EndPoint.y += 0.5;
    }

    const NewPoint LineVector = {EndPoint.x - StartPoint.x, EndPoint.y - StartPoint.y};
    NewPoint ThicknessVector = {1.0, 0.0};

    if(LineVector.x != 0)
        ThicknessVector = {-LineVector.y / LineVector.x, 1.0};

    const double ThicknessCoefficient = sqrt((Thickness * Thickness / 4) / (ThicknessVector.x * ThicknessVector.x + ThicknessVector.y * ThicknessVector.y));

    NewPoint A = {StartPoint.x + ThicknessCoefficient * ThicknessVector.x, StartPoint.y + ThicknessCoefficient * ThicknessVector.y};
    NewPoint B = {StartPoint.x - ThicknessCoefficient * ThicknessVector.x, StartPoint.y - ThicknessCoefficient * ThicknessVector.y};
    NewPoint C = {EndPoint.x + ThicknessCoefficient * ThicknessVector.x, EndPoint.y + ThicknessCoefficient * ThicknessVector.y};
    NewPoint D = {EndPoint.x - ThicknessCoefficient * ThicknessVector.x, EndPoint.y - ThicknessCoefficient * ThicknessVector.y};

    for(int i = max(0, int(min(min(A.x, B.x), min(C.x, D.x))) - 3); i < min(Width, int(max(max(A.x, B.x), max(C.x, D.x))) + 3); i++)
    {
        for (int j = max(0, int(min(min(A.y, B.y), min(C.y, D.y))) - 3); j < min(Height, int(max(max(A.y, B.y), max(C.y, D.y))) + 3); j++)
            MakePlot(i, j, SquareIntersection(A, B, C, D, i, j), Color, Gamma, SRGB);
    }
}
