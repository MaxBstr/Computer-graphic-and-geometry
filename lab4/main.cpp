#include <iostream>
#include <string>
#include <vector>
#include "Picture.h"

using namespace std;

vector<string> ColorSpaces = {"RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY"};

int main(int argc, char *argv[])
{
    string StartColorSpace, EndColorSpace, FileInput, FileOutput;
    int INC, OUTC;
    bool WF = false;

    for(int i = 1; i < argc; i ++)
    {
        if(string(argv[i]) == "-f" && i < argc - 1)
            StartColorSpace = string(argv[i + 1]);

        if(string(argv[i]) == "-t" && i < argc - 1)
            EndColorSpace = string(argv[i + 1]);

        if(string(argv[i]) == "-i" && i < argc - 2)
        {
            INC = atoi(argv[i + 1]);
            FileInput = string(argv[i + 2]);
        }

        if(string(argv[i]) == "-o" && i < argc - 2)
        {
            OUTC = atoi(argv[i + 1]);
            FileOutput = string(argv[i + 2]);
        }
    }

    WF |= argc != 11;
    WF |= StartColorSpace == "" || EndColorSpace == "" || FileInput == "" || FileOutput == "";
    WF |= (INC != 1 && INC != 3);
    WF |= (OUTC != 1 && OUTC != 3);
    bool INF = false, OUTF = false;

    for(int i = 0; i < ColorSpaces.size(); i++)
    {
        if(ColorSpaces[i] == StartColorSpace)
            INF = true;
        if(ColorSpaces[i] == EndColorSpace)
            OUTF = true;
    }
    WF |= (!INF) | (!OUTF);

    if(WF)
    {
        cerr << "WARNING! Command line arguments are invalid" << endl;
        return 1;
    }

    PIC* IMG;
    try
    {
        IMG = new PIC(INC, FileInput);
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
        IMG->WriteToFile(OUTC, FileOutput);
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
