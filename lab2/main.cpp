#include <iostream>
#include "Picture.h"

int main(int argc, char *argv[])
{
    if(argc != 9 && argc != 10)
    {
        cerr << "Too much or few command line arguments!" << "\n";
        return 1;
    }

    const string Input = string(argv[1]);
    const string Output = string(argv[2]);
    double Thickness, X_Start, Y_Start, X_End, Y_End, Gamma = 2.4;
    bool SRGB = true;
    int Brightness;

    try
    {
        Brightness = atoi(argv[3]);
        Thickness = stod(argv[4]);
        X_Start = stod(argv[5]);
        Y_Start = stod(argv[6]);
        X_End = stod(argv[7]);
        Y_End = stod(argv[8]);
    }
    catch (const exception& error)
    {
        cerr << error.what() << "\n";
        return 1;
    }

    if(argc == 10)
    {
        try
        {
            Gamma = stod(argv[9]);
            SRGB = false;
        }
        catch (const exception& error)
        {
            cerr << error.what() << endl;
            return 1;
        }
    }

    PGM* Picture;
    try
    {
        Picture = new PGM(Input);
    }
    catch (const exception& error)
    {
        cerr << error.what() << "\n";
        return 1;
    }

    Picture->DrawLine(NewPoint{ X_Start, Y_Start }, NewPoint{ X_End, Y_End }, Thickness, Brightness, Gamma, SRGB);
    try
    {
        Picture->WriteToFile(Output);
    }
    catch (const exception& error)
    {
        cerr << error.what() << "\n";
        delete Picture;
        return 1;
    }

    delete Picture;
    return 0;
}
