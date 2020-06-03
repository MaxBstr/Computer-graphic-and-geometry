#pragma once
#include <exception>
using namespace std;

struct IO_WARNING : public exception
{
    const char* what() const noexcept override;
};

struct FORMAT_WARNING : public exception
{
    const char* what() const noexcept override;
};
