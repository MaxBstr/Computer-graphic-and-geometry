#include "Picture.h"

const double MatrixOrdered[8][8] =
        {
        {1.0 / 64.0, 49.0 / 64.0, 13.0 / 64.0, 61.0 / 64.0, 4.0 / 64.0, 52.0 / 64.0, 16.0 / 64.0, 64.0 / 64.0},
        {33.0 / 64.0, 17.0 / 64.0, 45.0 / 64.0, 29.0 / 64.0, 36.0 / 64.0, 20.0 / 64.0, 48.0 / 64.0, 32.0 / 64.0},
        {9.0 / 64.0, 57.0 / 64.0, 5.0 / 64.0, 53.0 / 64.0, 12.0 / 64.0, 60.0 / 64.0, 8.0 / 64.0, 56.0 / 64.0},
        {41.0 / 64.0, 25.0 / 64.0, 37.0 / 64.0, 21.0 / 64.0, 44.0 / 64.0, 28.0 / 64.0, 40.0 / 64.0, 24.0 / 64.0},
        {3.0 / 64.0, 51.0 / 64.0, 15.0 / 64.0, 63.0 / 64.0, 2.0 / 64.0, 50.0 / 64.0, 14.0 / 64.0, 62.0 / 64.0},
        {35.0 / 64.0, 19.0 / 64.0, 47.0 / 64.0, 31.0 / 64.0, 34.0 / 64.0, 18.0 / 64.0, 46.0 / 64.0, 30.0 / 64.0},
        {11.0 / 64.0, 59.0 / 64.0, 7.0 / 64.0, 55.0 / 64.0, 10.0 / 64.0, 58.0 / 64.0, 6.0 / 64.0, 54.0 / 64.0},
        {43.0 / 64.0, 27.0 / 64.0, 39.0 / 64.0, 23.0 / 64.0, 42.0 / 64.0, 26.0 / 64.0, 38.0 / 64.0, 22.0 / 64.0}
        };

const double MatrixJJN[3][5] =
        {
        {0, 0, 0, 7.0 / 48.0, 5.0 / 48.0},
        {3.0 / 48.0, 5.0 / 48.0, 7.0 / 48.0, 5.0 / 48.0, 3.0 / 48.0},
        {1.0 / 48.0, 3.0 / 48.0, 5.0 / 48.0, 3.0 / 48.0, 1.0 / 48.0}
        };

const double Sierra3Matrix[3][5] =
        {
        {0, 0, 0, 5.0 / 32.0, 3.0 / 32.0},
        {2.0 / 32.0, 4.0/ 32.0, 5.0 / 32.0, 4.0 / 32.0, 2.0 / 32.0},
        {0, 2.0 / 32.0, 3.0 / 32.0, 2.0 / 32.0, 0}
        };

const int AtkinsonMatrix[3][5] =
        {
        {0, 0, 0, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}
        };

const double MatrixHalftone[4][4] =
        {
        {7 / 17.0, 13 / 17.0, 11 / 17.0, 4 / 17.0},
        {12 / 17.0, 16 / 17.0, 14 / 17.0, 8 / 17.0},
        {10 / 17.0, 15 / 17.0, 6 / 17.0, 2 / 17.0},
        {5 / 17.0, 9 / 17.0, 3 / 17.0, 1 / 17.0}
        };

Image::Image() = default;
Image::Image(string filename) { read_file(filename); }
Image::~Image() { PGM.clear(); }

void Image::read_file(string fileName)
{
    ifstream inputFile(fileName, ios::binary);
    if (!inputFile.is_open())
        throw IO_WARNING();
    read_file(inputFile);
    inputFile.close();
}

void Image::read_file(ifstream& inputFile)
{
    char P;
    inputFile >> P;
    if (P != 'P')
        throw FORMAT_WARNING();

    inputFile >> Format;
    if (Format != 5)
        throw FORMAT_WARNING();

    inputFile >> Width >> Height;
    inputFile >> ColorDepth;
    if (ColorDepth != 255)
        throw FORMAT_WARNING();

    inputFile.get();

    PGM.resize(Width * Height);
    inputFile.read((char*) &PGM[0], Width * Height);
    if (inputFile.fail())
        throw IO_WARNING();
}

void Image::write_file(const string& fileName)
{
    ofstream outputFile(fileName, ios::binary);
    if (!outputFile.is_open())
        throw IO_WARNING();
    write_file(outputFile);
    outputFile.close();
}

void Image::write_file(ofstream& outputFile)
{
    outputFile << "P" << Format << '\n';
    outputFile << Width << ' ' << Height << '\n';
    outputFile << ColorDepth << '\n';
    outputFile.write((char*) &PGM[0], Width * Height);
}

void Image::Dithering(AlgoType Number)
{
    switch (Number)
    {
        case NONE:
            NoDithering();
            break;
        case ORDERED:
            OrderedDithering();
            break;
        case RANDOM:
            RandomDithering();
            break;
        case FLOYD_STEINBERG:
            FloydSteinbergDithering();
            break;
        case JJN:
            JJNDithering();
            break;
        case SIERRA:
            SierraDithering();
            break;
        case ATKINSON:
            AtkinsonDithering();
            break;
        case HALFTONE:
            HalftoneDithering();
            break;
        default:
            throw EXECUTE_WARNING();
    }
}

uchar& Image::GetPixel(int h, int w)
{
    if (h < 0 || w < 0 || h >= Height || w >= Width)
        throw OUT_OF_RANGE_WARNING();
    return PGM[Width * h + w];
}

double Image::DecodeGamma(double Value)
{
    double Correction = this->Gamma == 0 ? Value <= 0.04045 ? Value / 12.92 : pow((Value + 0.055) / 1.055, 2.4) : pow(Value, this->Gamma);
    return Correction;
}

double Image::EncodeGamma(double Value)
{
    double Correction = this->Gamma == 0 ? Value <= 0.0031308 ? 12.92 * Value : 1.055 * pow(Value, 1 / 2.4) - 0.055 : pow(Value, 1.0 / this->Gamma);
    return Correction;
}

void Image::UseGradient()
{
    for (int i = 0; i < Height; ++i)
        for (int j = 0; j < Width; ++j)
            GetPixel(i, j) = EncodeGamma((double) j / (Width - 1.0)) * 255;
}

double Image::PaletteColorCount(uchar PX)
{
    int temp = this->BitRate;
    uchar Result = PX;
    while (temp < 8)
    {
        Result = Result >> this->BitRate;
        Result += (PX >> (8 - this->BitRate)) << (8 - this->BitRate);
        temp += this->BitRate;
    }
    return Result;
}

void Image::NoDithering()
{
    for (int i = 0; i < Height; ++i)
    {
        for (int j = 0; j < Width; ++j)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = min(max(Value, 0.0), 1.0);
            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));
            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);
        }
    }
}


void Image::OrderedDithering()
{

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + (MatrixOrdered[i % 8][j % 8] - 0.5) / this->BitRate;
            Value = min(max(Value, 0.0), 1.0);
            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));
            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);
        }
    }
}

void Image::RandomDithering()
{
    random_device RD;
    mt19937 gen(RD());
    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            double Noise = (double)gen() / UINT32_MAX + 1e-7;
            Value = Value + (Noise - 0.5) / this->BitRate;
            Value = min(max(Value, 0.0), 1.0);
            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));
            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);
        }
    }
}

void Image::FloydSteinbergDithering()
{
    vector<double> Errors(Height * Width, 0);

    auto GetError = [&](int h, int w) -> double&
    {
        return Errors[h * Width + w];
    };

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + GetError(i, j) / 255.0;
            Value = min(max(Value, 0.0), 1.0);

            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));

            double Error = GetPixel(i, j) + GetError(i, j) - NewPaletteColor;

            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);

            if (j + 1 < Width)
                GetError(i, j + 1) += Error * 7.0 / 16.0;

            if (i + 1 < Height && j + 1 < Width)
                GetError(i + 1, j + 1) += Error * 1.0 / 16.0;

            if (i + 1 < Height)
                GetError(i + 1, j) += Error * 5.0 / 16.0;

            if (i + 1 < Height && j - 1 >= 0)
                GetError(i + 1, j - 1) += Error * 3.0 / 16.0;
        }
    }
}

void Image::JJNDithering()
{

    vector<double> Errors(Height * Width, 0);

    auto GetError = [&](int h, int w) -> double&
    {
        return Errors[h * Width + w];
    };

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + GetError(i, j) / 255.0;
            Value = min(max(Value, 0.0), 1.0);

            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));

            double Error = GetPixel(i, j) + GetError(i, j) - NewPaletteColor;

            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);

            for (int ie = 0; ie < 3; ie++)
            {
                for (int je = 0; je < 5; je++)
                {
                    if (i + ie >= Height || j + (je - 2) >= Width || j + (je - 2) < 0)
                        continue;
                    if (ie == 0 && je < 3)
                        continue;

                    GetError(i + ie, j + (je - 2)) += Error * MatrixJJN[ie][je];
                }
            }
        }
    }
}

void Image::SierraDithering()
{

    vector<double> Errors(Height * Width, 0);

    auto GetError = [&](int h, int w) -> double&
    {
        return Errors[h * Width + w];
    };

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + GetError(i, j) / 255.0;
            Value = min(max(Value, 0.0), 1.0);

            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));

            double Error = GetPixel(i, j) + GetError(i, j) - NewPaletteColor;

            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);

            for (int ie = 0; ie < 3; ie++)
            {
                for (int je = 0; je < 5; je++)
                {
                    if (i + ie >= Height || j + (je - 2) >= Width || j + (je - 2) < 0)
                        continue;
                    if (ie == 0 && je <= 2)
                        continue;

                    GetError(i + ie, j + (je - 2)) += Error * Sierra3Matrix[ie][je];
                }
            }
        }
    }
}

void Image::AtkinsonDithering()
{

    vector<double> Errors(Height * Width, 0);

    auto GetError = [&](int h, int w) -> double&
    {
        return Errors[h * Width + w];
    };

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + GetError(i, j) / 255.0;
            Value = min(max(Value, 0.0), 1.0);

            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));

            double Error = GetPixel(i, j) + GetError(i, j) - NewPaletteColor;

            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);

            for (int ie = 0; ie < 3; ie++)
            {
                for (int je = 0; je < 5; je++)
                {
                    if (i + ie >= Height || j + (je - 2) >= Width || j + (je - 2) < 0)
                        continue;
                    if (ie == 0 && je <= 2)
                        continue;

                    GetError(i + ie, j + (je - 2)) += Error * AtkinsonMatrix[ie][je] / 8.0;
                }
            }
        }
    }
}

void Image::HalftoneDithering()
{

    for (int i = 0; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            double Value = DecodeGamma(GetPixel(i, j) / 255.0);
            Value = Value + (MatrixHalftone[i % 4][j % 4] - 0.5) / this->BitRate;
            Value = min(max(Value, 0.0), 1.0);
            double NewPaletteColor = PaletteColorCount((uchar) (Value * 255));
            GetPixel(i, j) = (uchar)(EncodeGamma(NewPaletteColor / 255) * 255);
        }
    }
}

