#pragma once
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <cmath>
using namespace std;

struct SomePixel
    {
        unsigned char
        First,
        Second,
        Third;
    };

class PIC
{

public:
    PIC(int, string);
    void WriteToFile(int Count, string FName);
    void ChangeSpace(string StartColorSpace, string EndColorSpace);
private:
    int Width, Height, ColorDepth;
    vector<vector<SomePixel>> Picture;
};

SomePixel FROM_CMY_TO_RGB(SomePixel PX);

SomePixel FROM_RGB_TO_CMY(SomePixel PX);

SomePixel FROM_HSL_TO_RGB(SomePixel PX);

SomePixel FROM_RGB_TO_HSL(SomePixel PX);

SomePixel FROM_HSV_TO_RGB(SomePixel PX);

SomePixel FROM_RGB_TO_HSV(SomePixel PX);

SomePixel FROM_YCgCo_TO_RGB(SomePixel PX);

SomePixel FROM_RGB_TO_YCgCo(SomePixel PX);

SomePixel FROM_RGB_TO_YCbCr601(SomePixel PX);

SomePixel FROM_YCbCr601_TO_RGB(SomePixel PX);

SomePixel FROM_RGB_TO_YCbCr709(SomePixel PX);

SomePixel FROM_YCbCr709_TO_RGB(SomePixel PX);
