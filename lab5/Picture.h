#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <exception>
#include <cmath>
typedef unsigned char uchar;
using namespace std;

struct Pixel
{
    uchar First, Second, Third;
};

class Picture
{
private:
    int Height, Width, ColorDepth, type;
    vector <vector<Pixel>> Pixels;
    char P, FormatNum;
    double Offset, Multiplier; //смещение и множитель
    uchar UseOffsetAndMultiplier(uchar Pixel);
    void FROM_RGB_TO_YCbCr601();
    void FROM_YCbCr601_TO_RGB();
    void UseType0();
    void UseType1();
    void UseType2();
    void UseType3();

public:
    Picture();
    ~Picture();
    void GetFile(string& FName);
    void WriteFile(string& FName);
    void SetParams(double Offset, double Multiplier);
    void SetType(int Choice);
    void WriteParamsConsole();
};
