#include "Picture.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 7)
    {
        cerr << "Incorrect count of arguments" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    bool grad;
    if (argv[3][0] == '1')
        grad = true;
    else if (argv[3][0] == '0')
        grad = false;
    else
        {
        cerr << "Incorrect type of argument 3: grad must by 1 (true) or 0 (false)" << endl;
        return 1;
        }

    if (!isdigit(argv[4][0]) || strlen(argv[4]) != 1)
    {
        cerr << "Incorrect type of argument 4: dithering algorithm must be integer from 0 to 7" << endl;
        return 1;
    }

    AlgoType algo_choice = (AlgoType) (argv[4][0] - 48);
    if (algo_choice > 7) {
        cerr << "Incorrect type of argument 4: dithering algorithm must be integer from 0 to 7" << endl;
        return 1;
    }

    uchar BitRate;
    if (!isdigit(argv[5][0]) || strlen(argv[5]) != 1)
    {
        cerr << "Incorrect type of argument 5: dithering algorithm must be integer from 0 to 7" << endl;
        return 1;
    }
    BitRate = argv[5][0] - 48;
    if (BitRate > 8 || BitRate == 0)
    {
        cerr << "Bit rate must be from 1 to 8";
        return 1;
    }

    Image picture;
    picture.Gamma = stof(argv[6]);
    picture.BitRate = BitRate;

    try
    {
        picture.read_file(inputFile);
    }
    catch (IO_WARNING& ex)
    {
        cerr << "Error trying to read_file file " << inputFile << endl;
        return 1;
    }
    catch (FORMAT_WARNING& ex)
    {
        cerr << "Error: unsupported Format" << endl;
        return 1;
    }

    if (grad)
        picture.UseGradient();

    try
    {
        picture.Dithering(algo_choice);
    }
    catch (EXECUTE_WARNING& ex)
    {
        cerr << "Error trying to dither the picture" << endl;
        return 1;
    }

    try
    {
        picture.write_file(outputFile);
    }
    catch (IO_WARNING& ex)
    {
        cerr << "Error trying to write_file PGM to file " << outputFile << endl;
        return 1;
    }

    return 0;
}
