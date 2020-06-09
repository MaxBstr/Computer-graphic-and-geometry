#pragma once
#include <vector>
#include <string>
using namespace std;

class NewPoint
{
public:
    double x, y;
    NewPoint(double, double);
};

class PGM
{
public:
    PGM(string);
    void WriteToFile(string FName);
    void DrawLine(NewPoint, NewPoint, double, int, double, bool);
private:
    int Width;
    int Height;
    int ColorDepth;
    vector<vector<unsigned char>> Picture;
    void MakePlot(int, int, double, int, double, bool);
};
