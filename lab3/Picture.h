#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <exception>
#include <algorithm>
#include <cmath>
#include <cstring>

using namespace std;
typedef unsigned char uchar;

class IO_WARNING : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error while trying to read_file file or write_file to file";
    }
};

class FORMAT_WARNING : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Invalid Format of PGM file";
    }
};

class EXECUTE_WARNING : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Failed to execute the command";
    }
};

class OUT_OF_RANGE_WARNING : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Index is out of range";
    }
};

enum AlgoType {
    NONE = 0,
    ORDERED = 1,
    RANDOM = 2,
    FLOYD_STEINBERG = 3,
    JJN = 4,
    SIERRA = 5,
    ATKINSON = 6,
    HALFTONE = 7
};

class Image
{
public:
    Image();
    ~Image();
    explicit Image(string fileName);

    void read_file(string fileName);
    void read_file(ifstream& inputFile);

    void write_file(const string& fileName);
    void write_file(ofstream& outputFile);

    void UseGradient();

    void Dithering(AlgoType Number);

    double Gamma;
    uchar BitRate;

private:
    short Format;
    int Width, Height;
    int ColorDepth;
    vector<uchar> PGM;

    uchar& GetPixel(int h, int w);
    double DecodeGamma(double Value);
    double EncodeGamma(double Value);
    double PaletteColorCount(uchar PX);

    void NoDithering();
    void OrderedDithering();
    void RandomDithering();
    void FloydSteinbergDithering();
    void JJNDithering();
    void SierraDithering();
    void AtkinsonDithering();
    void HalftoneDithering();
};
