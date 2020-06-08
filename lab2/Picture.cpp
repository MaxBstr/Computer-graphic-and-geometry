#include "Picture.h"
#include <fstream>

#include <cmath>
#include <algorithm>

#define AREA Width * Height
#define EPS 1e-5

const short PGM = 5, COLOR_DEPTH = 255;

Picture::Picture() = default;
Picture::~Picture() { Image.clear(); }

void Picture::file_read(const string & f_name)
{
    ifstream fin(f_name, ios::binary);
    if (!fin.is_open())
        throw IO_WARNING();

    char P;
    fin >> P;
    if (P != 'P')
        throw FORMAT_WARNING();
    fin >> Format;

    if (Format != PGM)
        throw FORMAT_WARNING();

    fin >> Width >> Height;
    fin >> ColorDepth;
    if (ColorDepth != COLOR_DEPTH)
        throw FORMAT_WARNING();

    fin.get();

    Image.resize(AREA);
    fin.read(reinterpret_cast<char*>(&Image[0]), AREA);

    if (fin.fail())
        throw IO_WARNING();

    fin.close();
}

void Picture::file_write(const string & f_name)
{
    ofstream fout(f_name, ios::binary);
    if (!fout.is_open())
        throw IO_WARNING();

    fout << "P" << Format << '\n';
    fout << Width << ' ' << Height << '\n';
    fout << ColorDepth << '\n';

    fout.write(reinterpret_cast<char*>(&Image[0]), AREA);
    if (fout.fail())
        throw IO_WARNING();

    fout.close();
}

void Picture::drawPoint(int X, int Y, double Alpha, unsigned char Color, double Gamma) {
    Alpha = max(min(Alpha, 1.0), 0.0);
    if (Y < 0 || Y >= Height || X < 0 || X >= Width)
        return;

    if (Alpha == 0)
        return;

    if (Gamma == 0)
    {
        double LineColorSRGB = Color / 255.0; //коэффициент
        double LineColorLinear = LineColorSRGB <= 0.04045 ? LineColorSRGB / 12.92 : pow((LineColorSRGB + 0.055) / 1.055, 2.4); //конвертация из srgb цвета линии в линейное
        double PicColorSRGB = Image[Width * Y + X] / 255.0; //пиксель в srgb, получаем коэффициент
        double PicColorLinear = PicColorSRGB <= 0.04045 ? PicColorSRGB / 12.92 : pow((PicColorSRGB + 0.055) / 1.055, 2.4); //перевод коэфа в линейный из srgb
        double C_Linear = (1 - Alpha) * PicColorLinear + Alpha * LineColorLinear; //получение нового цвета пикселя, накладка 2х пикселей
        double C_SRGB = C_Linear <= 0.0031308 ? 12.92 * C_Linear : 1.055 * pow(C_Linear, 1 / 2.4) - 0.055; //перевод в srgb
        Image[Width * Y + X] = 255 * C_SRGB; //перевод из коэффа в палитру
    }
    else
    {
        double LineColorGamma = Color / 255.0; //коэффициент
        double LineColorLinear = pow(LineColorGamma, Gamma); //получение линейного цвета
        double PicColorGamma = Image[Width * Y + X] / 255.0; //коэффициент
        double PicColorLinear = pow(PicColorGamma, Gamma); //получение линейного цвета
        double C_Linear = (1 - Alpha) * PicColorLinear + Alpha * LineColorLinear; //наложение
        double C_Gamma = pow(C_Linear, 1.0 / Gamma); //результирующий коэф
        Image[Width * Y + X] = 255 * C_Gamma; //сам цвет
    }
}

void Picture::draw_line(double X_First, double Y_First, double X_End, double Y_End, unsigned char Color, double Thickness, double Gamma)
{

    if (Thickness <= 0)
        return;

    StartPoint = { X_First, Y_First };
    EndPoint   = { X_End, Y_End };

    Point Vector = {(EndPoint.y - StartPoint.y) * 0.5 * Thickness / sqrt((EndPoint.y - StartPoint.y) * (EndPoint.y - StartPoint.y) + (StartPoint.x - EndPoint.x) * (StartPoint.x - EndPoint.x))
            , (StartPoint.x - EndPoint.x) * 0.5 * Thickness / sqrt((EndPoint.y - StartPoint.y) * (EndPoint.y - StartPoint.y) + (StartPoint.x - EndPoint.x) * (StartPoint.x - EndPoint.x))};

    Point A = {StartPoint.x + Vector.x, StartPoint.y + Vector.y};
    Point B = {EndPoint.x + Vector.x, EndPoint.y + Vector.y};
    Point C = {EndPoint.x - Vector.x, EndPoint.y - Vector.y};
    Point D = {StartPoint.x - Vector.x, StartPoint.y - Vector.y};

    Line = { A, B, C, D }; // vector Line
    // drawing raster Line
    Point LT{ min(min(A.x, B.x), min(C.x, D.x)), min(min(A.y, B.y), min(C.y, D.y)) };
    Point RB{ max(max(A.x, B.x), max(C.x, D.x)), max(max(A.y, B.y), max(C.y, D.y)) };

    for (int x = (int)LT.x - 3; x <= RB.x + 3; x++)
        for (int y = (int)LT.y - 3; y <= RB.y + 3; y++)
            drawPoint(x, y, Opacity(x, y), Color, Gamma);
}

double Picture::Opacity(double x, double y)
{
    Point A = { x, y } ;
    Point B = { x + 1, y };
    Point C = { x + 1, y + 1 };
    Point D = { x, y + 1 };

    auto CheckPoint = [](Rectangle Rectangle, Point p) -> bool
            {
        auto TriangleArea = [](Point A, Point B, Point C) -> double
                {
            return sqrt((
                                (
                                        sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)) +
                                        sqrt((A.x - C.x) * (A.x - C.x) + (A.y - C.y) * (A.y - C.y)) + // p
                                        sqrt((C.x - B.x) * (C.x - B.x) + (C.y - B.y) * (C.y - B.y))
                                ) / 2
                        ) * (
                                (
                                        -sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)) +
                                        sqrt((A.x - C.x) * (A.x - C.x) + (A.y - C.y) * (A.y - C.y)) + // p - a
                                        sqrt((C.x - B.x) * (C.x - B.x) + (C.y - B.y) * (C.y - B.y))
                                ) / 2
                        ) * (
                                (
                                        sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)) -
                                        sqrt((A.x - C.x) * (A.x - C.x) + (A.y - C.y) * (A.y - C.y)) + // p - b
                                        sqrt((C.x - B.x) * (C.x - B.x) + (C.y - B.y) * (C.y - B.y))
                                ) / 2
                        ) * (
                                (
                                        sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)) +
                                        sqrt((A.x - C.x) * (A.x - C.x) + (A.y - C.y) * (A.y - C.y)) - // p - c
                                        sqrt((C.x - B.x) * (C.x - B.x) + (C.y - B.y) * (C.y - B.y))
                                ) / 2
                        ));
        };
        return fabs
        (
                sqrt((Rectangle.A.x - Rectangle.D.x) * (Rectangle.A.x - Rectangle.D.x) +
                     (Rectangle.A.y - Rectangle.D.y) * (Rectangle.A.y - Rectangle.D.y)) *
                sqrt((Rectangle.A.x - Rectangle.B.x) * (Rectangle.A.x - Rectangle.B.x) +
                     (Rectangle.A.y - Rectangle.B.y) * (Rectangle.A.y - Rectangle.B.y))
                -
                TriangleArea(p, Rectangle.A, Rectangle.B) - TriangleArea(p, Rectangle.B, Rectangle.C) -
                TriangleArea(p, Rectangle.C, Rectangle.D) - TriangleArea(p, Rectangle.D, Rectangle.A)
        ) < EPS;
    };
    if (
            CheckPoint(Line, A) &&
            CheckPoint(Line, B) &&
            CheckPoint(Line, C) &&
            CheckPoint(Line, D)
        )
        return 1;

    double Area = 0;
    for (double i = x; i < x + 1; i += 0.1)
    {
        for (double j = y; j < y + 1; j += 0.1)
            if (CheckPoint(Line, { i, j }))
                Area += 0.01;
    }
    return Area;
}
