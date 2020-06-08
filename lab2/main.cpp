#include <iostream>
#include "Picture.h"

int main(int argc, char** argv)
{
    if (argc != 9 && argc != 10)
    {
        cerr << "Not enough or too much arguments!" << endl;
        return 0;
    }

    string input_file = argv[1], output_file = argv[2];
    unsigned char Brightness = stoi(argv[3]);
    float Thickness = stof(argv[4]);

    float x_first = stof(argv[5]), y_begin = stof(argv[6]);
    float x_end = stof(argv[7]), y_end = stof(argv[8]);

    float GammaCorrection;
    if (argc == 10)
        GammaCorrection = stof(argv[9]);

    Picture PGM;

    try
    {
        PGM.file_read(input_file);
    }

    catch (IO_WARNING&)
    {
        cerr << "Error while trying to read file " << input_file << '.' << endl;
        return 1;
    }

    catch (FORMAT_WARNING& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    try
    {
        if (argc == 10)
            PGM.draw_line(x_first, y_begin, x_end, y_end, Brightness, Thickness, GammaCorrection);
        else
            PGM.draw_line(x_first, y_begin, x_end, y_end, Brightness, Thickness, 0);
    }

    catch (exception& e)
    {
        cerr << "Failed to draw a Line: " << e.what() << endl;
        return 1;
    }


    try
    {
        PGM.file_write(output_file);
    }

    catch (IO_WARNING&)
    {
        cerr << "Error while trying to write data to file " << output_file << '.' << endl;
        return 1;
    }

    return 0;
}
