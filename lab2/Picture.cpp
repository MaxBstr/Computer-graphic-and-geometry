#include "Picture.h"
#include <fstream>

#include <cmath>
//#include <functional>
#include <algorithm>
#define AREA Width * Height

using NewPoint = pair<float, float>;
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

void Picture::draw_point(int x, int y, double alpha, unsigned char Color, float GammaCorrection)
{
    alpha = max(min(alpha, 1.0), 0.0);
    if (y < 0 || y >= Height || x < 0 || x >= Width)
        return;

    double LineColorGamma = Color / 255.0; //коэффициент
    double LineColorLinear = pow(LineColorGamma, GammaCorrection); //получение линейного цвета
    double PicColorGamma = Image[Width * y + x] / 255.0; //коэффициент
    double PicColorLinear = pow(PicColorGamma, GammaCorrection); //получение линейного цвета
    double C_Linear = (1 - alpha) * LineColorLinear + alpha * PicColorLinear; //наложение
    double C_Gamma = pow(C_Linear, 1.0 / GammaCorrection); //результирующий коэф
    Image[Width * y + x] = 255 * C_Gamma; //сам цвет
}

void Picture::draw_point(int x, int y, double alpha, unsigned char Color)
{
    alpha = max(min(alpha, 1.0), 0.0);
    if (y < 0 || y >= Height || x < 0 || x >= Width)
        return;

    double  LineColorSRGB = Color / 255.0,
        LineColorLinear = LineColorSRGB <= 0.04045 ? LineColorSRGB / 12.92 : pow((LineColorSRGB + 0.055) / 1.055, 2.4), //конвертация из srgb цвета линии в линейное
        ImgColorSRGB = Image[Width * y + x] / 255.0, //пиксель в srgb, получаем коэффициент
        ImgColorLinear = ImgColorSRGB <= 0.04045 ? ImgColorSRGB / 12.92 : pow((ImgColorSRGB + 0.055) / 1.055, 2.4), //перевод коэфа в линейный из srgb
        C_Linear = (1 - alpha) * LineColorLinear + alpha * ImgColorLinear, //получение нового цвета пикселя, накладка 2х пикселей
        C_sRGB = C_Linear <= 0.0031308 ? 12.92 * C_Linear : 1.055 * pow(C_Linear, 1.0 / 2.4) - 0.055; //перевод в srgb
        Image[Width * y + x] = 255 * C_sRGB; //перевод из коэффа в палитру
}

void Picture::line_draw(float x_first, float y_first, float x_end, float y_end, unsigned char Brightness, float Thickness, float GammaCorrection)
{
    if (Thickness <= 0)
        return;

    bool is_steep = abs(y_end - y_first) > abs(x_end - x_first);

    auto IntPart = [](double x) -> int
    {
        return static_cast<int>(x);
    };

    auto get_distance = [](NewPoint a, NewPoint b) -> double
    {
        return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
    };

    auto plot = [&](int x, int y, double Intensity) -> void
    {
        GammaCorrection == 0 ?
            is_steep ?
            draw_point(y, x, 1.0 - Intensity, Brightness) :
            draw_point(x, y, 1.0 - Intensity, Brightness)
            :
            is_steep ?
            draw_point(y, x, 1.0 - Intensity, Brightness, GammaCorrection) :
            draw_point(x, y, 1.0 - Intensity, Brightness, GammaCorrection);
    };

    if (is_steep)
    {
        swap(x_first, y_first);
        swap(x_end, y_end);
    }
    if (x_first > x_end)
    {
        swap(x_first, x_end);
        swap(y_first, y_end);
    }

    double dx = x_end - x_first, dy = y_end - y_first;
    double Grad = dy / dx;

    double y = y_first + Grad * (round(x_first) - x_first);

    for (int x = (int)round(x_first); x <= (int)round(x_end); ++x)
    {
        for (int plotY = IntPart(y - (Thickness - 1) / 2); plotY <= IntPart(y - (Thickness - 1) / 2 + Thickness); ++plotY)
            plot(x, plotY, min(1.0, (Thickness + 1.0) / 2.0 - fabs(y - plotY)));
        y += Grad;
    }

    NewPoint plotStart = make_pair(round(x_first), round(y_first));

    for (int plotX = int(round(x_first) - Thickness / 2); plotX < (int)round(x_first); ++plotX)
    {
        y = y_first + Grad * ((float)plotX - x_first);

        for (int plotY = int(y - (Thickness - 1) / 2.0); plotY <= int(y - (Thickness - 1) / 2.0 + Thickness); ++plotY)
            plot(plotX, plotY, min(1.0, (Thickness + 0.5) / 2.0 -get_distance({ (float)plotX, (float)plotY }, plotStart)));
    }

    NewPoint plotEnd = { round(x_end), round(y_end) };

    for (int plotX = (int)round(x_end) + 1; plotX <= int(round(x_end) + Thickness / 2); plotX++)
    {
        y = y_first + Grad * ((float)plotX - x_first);
        for (int plotY = int(y - (Thickness - 1) / 2.0); plotY <= int(y - (Thickness - 1) / 2.0 + Thickness); ++plotY)
            plot(plotX, plotY, min(1.0, (Thickness + 0.5) / 2.0 - get_distance({ (float)plotX, (float)plotY }, plotEnd)));   
    }
}
