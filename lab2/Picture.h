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

    void line_draw(float, float, float, float, unsigned char, float, float = 0);

private:
    vector<unsigned char> Image;
    short Format;
    int Width, Height, ColorDepth;

    void draw_point(int, int, double, unsigned char);
    void draw_point(int, int, double, unsigned char, float);
};