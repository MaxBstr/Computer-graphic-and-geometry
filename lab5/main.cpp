#include <iostream>
#include "Picture.h"
//lab.exe InputName, OutputName, type, offset, multiplier
int main(int argc, char *argv[])
{
    if (argc < 4) //минимум 4 параметра: lab5.exe input output choice
    {
        cerr << "Invalid command line args!";
        return 1;
    }
    double Multiplier = 0;
    int Offset = 0;

    string InputName = argv[1];
    string OutPutName = argv[2];
    int Choice = atoi(argv[3]);
    if (Choice > 3)
    {
        cerr << "This programm doesn`t support types > 3!" << endl;
        return 1;
    }

    if (Choice <= 1) //если преобразование 0 или 1, обязательно должны быть offset, multiplier
    {
        if (argc < 6)
        {
            cerr << "You chose 0 or 1 type, but didn`t set offset and multiplier!" << endl;
            return 1;
        }
        Offset = atof(argv[4]);
        Multiplier = atof(argv[5]);

        if (Offset < -255 || Offset > 255)
        {
            cerr << "Invalid offset! Must be in [-255; 255]";
            return 1;
        }
        if (Multiplier < 1.0 / 255.0 || Multiplier > 255)
        {
            cerr << "Invalid multiplier! Must be in [1/255; 255]";
            return 1;
        }
    }

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
