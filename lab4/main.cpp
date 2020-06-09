#include <iostream>
#include <string>
#include <vector>
#include "Picture.h"

using namespace std;

vector<string> ColorSpaces = {"RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY"};

int main(int argc, char *argv[])
{
    string StartColorSpace, EndColorSpace, FileInput, FileOutput;
    int InputCount, OutCount;
    bool IsWarning = false;

    for(int i = 1; i < argc; i ++)
    {
        if(string(argv[i]) == "-f" && i < argc - 1)
            StartColorSpace = string(argv[i + 1]);

        if(string(argv[i]) == "-t" && i < argc - 1)
            EndColorSpace = string(argv[i + 1]);

        if(string(argv[i]) == "-i" && i < argc - 2)
        {
            InputCount = atoi(argv[i + 1]);
            FileInput = string(argv[i + 2]);
        }

        if(string(argv[i]) == "-o" && i < argc - 2)
        {
            OutCount = atoi(argv[i + 1]);
            FileOutput = string(argv[i + 2]);
        }
    }

    IsWarning |= argc != 11;
    IsWarning |= StartColorSpace == "" || EndColorSpace == "" || FileInput == "" || FileOutput == "";
    IsWarning |= (InputCount != 1 && InputCount != 3);
    IsWarning |= (OutCount != 1 && OutCount != 3);

    bool InSpaceDefined = false, OutSpaceDefined = false;

    for(int i = 0; i < ColorSpaces.size(); i++)
    {
        if(ColorSpaces[i] == StartColorSpace)
            InSpaceDefined = true;
        if(ColorSpaces[i] == EndColorSpace)
            OutSpaceDefined = true;
    }
    IsWarning |= (!InSpaceDefined) | (!OutSpaceDefined);

    if(IsWarning)
    {
        cerr << "WARNING! Command line arguments are invalid" << endl;
        return 1;
    }

    PIC* IMG;
    try
    {
        IMG = new PIC(InputCount, FileInput);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << '\n';
        delete(IMG);
        return 1;
    }

    IMG->ChangeSpace(StartColorSpace, EndColorSpace);

    try
    {
        IMG->WriteToFile(OutCount, FileOutput);
    }
    catch(const exception& e)
    {
        cerr << e.what() << '\n';
        delete(IMG);
        return 1;
    }

    delete(IMG);
    return 0;
}
