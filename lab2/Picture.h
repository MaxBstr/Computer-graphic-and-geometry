#pragma once

#include <string>
#include <vector>

#include "Warnings.h"

class Picture
{
public:
    Picture();
    ~Picture();

    void file_read(const string&);
    void file_write(const string&);
    
    void draw_line(double, double, double, double, unsigned char, double, double);

private:
    vector<unsigned char> Image;
    short Format;
    int Width, Height, ColorDepth;

    struct Point 
    {
        double x;
        double y;
        Point& operator=(const Point& other) = default;
    };
    struct Rectangle
    {
        Point A, B, C, D;
    };

    struct Point StartPoint, EndPoint;
    struct Rectangle Line;

    void drawPoint(int, int, double, unsigned char, double);

    double Opacity(double x, double y);
};
