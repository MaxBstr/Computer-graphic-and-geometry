#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <exception>
#define PI 3.14159265358979323846
using namespace std;

typedef unsigned char uchar;
class Picture
{
public:
    void GetFile(string&);
    void WriteFile(string&);
    void SetType(int);
    Picture(double, double, int, int);
    ~Picture();
    void MakeNewPicture();
private:
    int Width, Height, ColorDepth, NewWidth, NewHeight;
    vector <vector <uchar>> PixelsOld;
    vector <vector <uchar>> PixelsNew;
    uchar P, FormatNum;
    double BSpline, CSpline;
    void ClosestNeighbour();
    double Interpolate(double, double, double, double, double, double);
    void Bilinear();
    double LanczoshFilter(double);
    void Lanczosh();
    double BCFilterK(double);
    void BCSplines();
};
