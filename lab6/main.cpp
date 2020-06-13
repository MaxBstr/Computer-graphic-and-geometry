#include "Picture.h"

int main(int argc, char* argv[])
{
    if (argc < 9)
    {
        cerr << "Not enough params!";
        return 1;
    }

    string InputName = argv[1];
    string OutPutName = argv[2];
    int NewWidth = atoi(argv[3]);
    int NewHeight = atoi(argv[4]);

    if (NewWidth < 0 || NewHeight < 0)
    {
        cerr << "New sizes are negative! Must be positive integer.";
        return 1;
    }

    double DX = atof(argv[5]); //не учитываем
    double DY = atof(argv[6]); //не учитываем
    double Gamma = atof(argv[7]); //не учитываем гамму
    int TypeAlgo = atoi(argv[8]);

    double B = 0, C = 0.5;
    if (argc == 9)
        B = atof(argv[9]);
    if (argc == 10)
        C = atof(argv[10]);

    Picture* IMG = new Picture(B, C, NewWidth, NewHeight);

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

    IMG->MakeNewPicture();

    try
    {
        IMG->SetType(TypeAlgo);
    }
    catch (const exception& WARNING)
    {
            cerr << WARNING.what() << endl;
            delete IMG;
            return 1;
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
