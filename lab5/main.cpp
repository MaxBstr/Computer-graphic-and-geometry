#include <iostream>
#include "Picture.h"
//lab.exe InputName, OutputName, type, offset, multiplier
int main(int argc, char *argv[])
{
    if (argc != 6 && argc != 4)
    {
        cerr << "Invalid command line args!";
        return 1;
    }

    string InputName = argv[1];
    string OutPutName = argv[2];
    int Choice = atoi(argv[3]);
    double Offset = atof(argv[4]);
    double Multiplier = atof(argv[5]);

    Picture* IMG = new Picture;
    try
    {
        IMG->GetFile(InputName);
    }
    catch (const exception& WARNING)
    {
        cerr << WARNING.what() << endl;
        delete IMG;
        return 1;
    }
    //0 - применить указанные значения <смещение> и <множитель> в пространстве RGB к каждому каналу;
    //1 - применить указанные значения <смещение> и <множитель> в пространстве YCbCr.601 к каналу Y;
    //2 - автояркость в пространстве RGB: <смещение> и <множитель> вычисляются на основе минимального и максимального значений пикселей;
    //3 - аналогично 2 в пространстве YCbCr.601;
    //4 - автояркость в пространстве RGB: <смещение> и <множитель> вычисляются на основе минимального и максимального значений пикселей, после игнорирования 0.39% самых светлых и тёмных пикселей;
    //5 - аналогично 4 в пространстве YCbCr.601.

        switch (Choice)
        {
            case 0:
            {
                IMG->SetParams(Offset, Multiplier);
                IMG->SetType(0);
                break;
            }
            case 1:
            {
                IMG->SetParams(Offset, Multiplier);
                IMG->SetType(1);
                break;
            }
            case 2:
            {
                IMG->SetType(2);
                IMG->WriteParamsConsole();
                break;
            }
            case 3:
            {
                IMG->SetType(3);
                IMG->WriteParamsConsole();
                break;
            }
            default:
            {
                cerr << "Unknown type! Can be in [0; 5]" << endl;
                delete IMG;
                return 1;
            }
        }

    try
    {
        IMG->WriteFile(OutPutName);
    }
    catch (const exception& WARNING)
    {
        cerr << WARNING.what() << endl;
        delete IMG;
        return 1;
    }

    return 0;
}
