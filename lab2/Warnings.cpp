#include "Warnings.h"

const char* IO_WARNING::what() const noexcept
{
    return "Error while trying to read file or write to file!";
}

const char* FORMAT_WARNING::what() const noexcept
{
    return "Unsupported PNM file format (only PGM is accepted).";
}
