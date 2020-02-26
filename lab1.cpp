#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

struct RGB
{
	unsigned char Red = 0;
	unsigned char Green = 0;
	unsigned char Blue = 0;
};

int size_to_int(vector <char>& Size)
{
	switch (Size.size()) //перевод из char в int
	{
	case 2:
		return (Size[0] - '0') * 10 + (Size[1] - '0');
	case 3:
		return (Size[0] - '0') * 100 + (Size[1] - '0') * 10 + (Size[2] - '0');
	case 4:
		return (Size[0] - '0') * 1000 + (Size[1] - '0') * 100 + (Size[2] - '0') * 10 + (Size[3] - '0');
	}
}

//=========================================================================================================
//Реализация самих функций
void inversion(vector <vector<unsigned char>>& Pixels) //для P5
{
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
			Pixels[i][j] = 255 - Pixels[i][j];
}
void inversion(vector <vector <struct RGB>>& Pixels) //для P6
{
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
		{
			Pixels[i][j].Red = 255 - Pixels[i][j].Red;
			Pixels[i][j].Green = 255 - Pixels[i][j].Green;
			Pixels[i][j].Blue = 255 - Pixels[i][j].Blue;
		}
}

void horizontal_reflextion(vector <vector<unsigned char>>& Pixels) //для P5
{
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size() / 2; ++j)
			swap(Pixels[i][j], Pixels[i][Pixels[i].size() - j - 1]);
}

void horizontal_reflextion(vector <vector<struct RGB>>& Pixels) //для P6
{
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size() / 2; ++j)
		{
			swap(Pixels[i][j].Red, Pixels[i][Pixels[i].size() - j - 1].Red);
			swap(Pixels[i][j].Green, Pixels[i][Pixels[i].size() - j - 1].Green);
			swap(Pixels[i][j].Blue, Pixels[i][Pixels[i].size() - j - 1].Blue);
		}
}

void vertical_reflextion(vector <vector <unsigned char>>& Pixels) //для P5
{
	for (int i = 0; i < Pixels.size() / 2; ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
			swap(Pixels[i][j], Pixels[Pixels.size() - i - 1][j]);
}

void vertical_reflextion(vector <vector <struct RGB>>& Pixels) //для P6
{
	for (int i = 0; i < Pixels.size() / 2; ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
		{
			swap(Pixels[i][j].Red, Pixels[Pixels.size() - i - 1][j].Red);
			swap(Pixels[i][j].Green, Pixels[Pixels.size() - i - 1][j].Green);
			swap(Pixels[i][j].Blue, Pixels[Pixels.size() - i - 1][j].Blue);
		}
}

void clock_and_counter_wise_ninety(vector <vector<unsigned char>>& Pixels, int type) //для P5
{
	vector <vector <unsigned char>> PixelsCopy(Pixels.size(), vector <unsigned char>(Pixels[0].size()));
	//создание копии массива пикселей
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
			PixelsCopy[i][j] = Pixels[i][j]; //непосредственно копирование
	Pixels.clear(); //чистка старого массива
	Pixels.assign(PixelsCopy[0].size(), vector<unsigned char>(PixelsCopy.size()));
	//смена ширины и высоты исходного массива местами


	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j)
		{
			if (type == 1)
				Pixels[i][j] = PixelsCopy[Pixels[i].size() - j - 1][i]; //смена пикселей по часовой
			if (type == 2)
			{
				Pixels[i][j] = PixelsCopy[j][Pixels.size() - i - 1]; //смена пикселей против часовой
			}
		}

	PixelsCopy.clear(); //чистка копии
}

void clock_and_counter_wise_ninety(vector <vector<struct RGB>>& Pixels, int type) //для P6
{
	vector <vector <struct RGB>> PixelsCopy(Pixels.size(), vector <struct RGB>(Pixels[0].size()));
	//создание копии массива пикселей
	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j) //непосредственно копирование
		{
			PixelsCopy[i][j].Red = Pixels[i][j].Red;
			PixelsCopy[i][j].Green = Pixels[i][j].Green;
			PixelsCopy[i][j].Blue = Pixels[i][j].Blue;
		}
	Pixels.clear(); //чистка старого массива
	Pixels.assign(PixelsCopy[0].size(), vector<struct RGB>(PixelsCopy.size()));
	//смена ширины и высоты исходного массива местами

	for (int i = 0; i < Pixels.size(); ++i)
		for (int j = 0; j < Pixels[i].size(); ++j) //смена пикселей
		{
			if (type == 1)
			{
				Pixels[i][j].Red = PixelsCopy[Pixels[i].size() - j - 1][i].Red;
				Pixels[i][j].Green = PixelsCopy[Pixels[i].size() - j - 1][i].Green;
				Pixels[i][j].Blue = PixelsCopy[Pixels[i].size() - j - 1][i].Blue;
			}
			if (type == 2)
			{
				Pixels[i][j].Red = PixelsCopy[j][Pixels.size() - i - 1].Red;
				Pixels[i][j].Green = PixelsCopy[j][Pixels.size() - i - 1].Green;
				Pixels[i][j].Blue = PixelsCopy[j][Pixels.size() - i - 1].Blue;
			}
		}

	PixelsCopy.clear(); //чистка копии
}
//НАЧАЛО ПРОГРАММЫ
int main(int argc, char* argv[])
{
	//Часть 1. Считка формата файла, ширины и высоты, глубины цвета. Преобразование данных из char в int
	setlocale(LC_ALL, "rus");
	FILE* Source = fopen(argv[1], "rb");
	if (Source == NULL)
	{
		cout << "Warning! File didn`t open";
		return 1;
	}

	if (argc < 4)
	{
		cout << "Not enough parametrs!";
		fclose(Source);
		return 1;
	}
	else if (argc > 4)
	{
		cout << "Too much parametrs!";
		fclose(Source);
		return 1;
	}
	if (argv[3][0] < '0' || argv[3][0] > '4') //если пришла неизвестная команда
	{
		cout << "Unknown command!";
		fclose(Source);
		return 1;
	}

	char Format[2]; //данные под формат файла (хедер)
	char buff;
	for (int i = 0; i < 2; ++i) //считка формата pnm или pgm файла. For exp. P5, P6 and etc.
		Format[i] = fgetc(Source);
	if (Format[0] != 'P' || (Format[1] != '5' && Format[1] != '6')) //проверка на корректность формата
	{
		cout << "Warning! Unknown format!";
		fclose(Source);
		return 1;
	}

	buff = fgetc(Source); //символ переноса строки

	vector <char> Width(1);
	Width[0] = fgetc(Source); //считка 1го символа
	while (true) //считка ширины в char
	{
		buff = fgetc(Source);
		if (buff == ' ') break;
		Width.push_back(buff);
	}

	int width = size_to_int(Width);

	vector <char> Height(1);
	Height[0] = fgetc(Source); //считка 1го символа
	while (true) //считка высоты в char
	{
		buff = fgetc(Source);
		if (buff == '\n') break;
		Height.push_back(buff);
	}
	int height = size_to_int(Height);

	char ColorDepth[3];
	for (int i = 0; i < 3; ++i)
		ColorDepth[i] = fgetc(Source); //считка значения глубины цвета в char
	if (((ColorDepth[0] - '0') * 100 + (ColorDepth[1] - '0') * 10 + (ColorDepth[2] - '0')) != 255)
		//перевод значения глубины цвета из char в int и сравнение с 255
	{
		cout << "Warning! Incorrect ColorDepth. Must be equal 255";
		fclose(Source);
		return 1;
	}
	buff = fgetc(Source); //считка символа переноса строки

	//проверка на битость файла
	long BeginPointer = ftell(Source); //запоминаем текущее положения указателя в файле
	if (Format[1] == '5') //если файл в градации серого
	{
		fseek(Source, 0, SEEK_END); //переходим в конец файла
		long SizePointer = ftell(Source); //считываем позицию последнего байта
		if ((SizePointer - BeginPointer) != height * width) //делаем проверку на битость файла
		{
			cout << "Warning! Broken file!";
			fclose(Source);
			return 1;
		}
		fseek(Source, BeginPointer, SEEK_SET); //возвращаем указатель на прежнее место
	}
	else if (Format[1] == '6') //если файл цветной
	{
		fseek(Source, 0, SEEK_END); //переходим в конец файла
		long SizePointer = ftell(Source); //считываем позицию последнего байта
		if ((SizePointer - BeginPointer) != (height * width * 3)) //проверяем на битость файла,
		{                                                       //учитывая тот факт, что каждый пиксель = 3 байта
			cout << "Warning! Broken file!";
			fclose(Source);
			return 1;
		}
		fseek(Source, BeginPointer, SEEK_SET); //возвращаем указатель на прежнее место
	}
	//проверка на битость файла

	//Часть 2. Создание выходного файла. Запись хедера в файл
	FILE* Output = fopen(argv[2], "wb");
	if (Output == NULL)
	{
		cout << "Warning! Output file didn`t open!";
		fclose(Source);
		return 1;
	}
	fputc(Format[0], Output); //запись данных хедера
	fputc(Format[1], Output);
	fputc(buff, Output); //запись символа переноса строки
	if (((argv[3][0] - '0') == 2) || ((argv[3][0] - '0') == 1) || ((argv[3][0] - '0') == 0)) //если пришла команда
		                                                             //на отражение или инверсию
	{
		for (int i = 0; i < Width.size(); ++i)
			fputc(Width[i], Output); //запись значения ширины в файл
		fputc(' ', Output); //запись пробела между шириной и высотой
		Width.clear(); //чистка памяти

		for (int i = 0; i < Height.size(); ++i)
			fputc(Height[i], Output); //запись значения высоты в файл
		fputc(buff, Output); //запись символа переноса строки
		Height.clear(); //чистка памяти
	}
	else if (((argv[3][0] - '0') == 3) || ((argv[3][0] - '0') == 4)) //если пришла команда на поворот файла,
		                                                             //меняем местами ширину и высоту
	{
		for (int i = 0; i < Height.size(); ++i)
			fputc(Height[i], Output); //запись значения ширины в файл
		fputc(' ', Output); //запись пробела между шириной и высотой
		Height.clear(); //чистка памяти

		for (int i = 0; i < Width.size(); ++i)
			fputc(Width[i], Output); //запись значения высоты в файл
		fputc('\n', Output); //запись символа переноса строки
		Width.clear(); //чистка памяти
	}

	for (int i = 0; i < 3; ++i)
		fputc(ColorDepth[i], Output); //запись значения глубины цвета
	fputc(buff, Output); //запись символа переноса строки

	//Часть 3. Работа с матрицей пикселей. Считка и преобразование char в int.
	//Реализация функций. Запись матрицы пикселей в выходной файл

	char symbol = 0; //буфер для пикселей
	if (Format[1] == '5') //если файл в градации серого
	{
		vector <vector <unsigned char>> Pixels(height, vector<unsigned char>(width));
		for (int i = 0; i < height; ++i)
			for (int j = 0; j < width; ++j)
			{
				Pixels[i][j] = 0;
				symbol = fgetc(Source);
				Pixels[i][j] = symbol;
			}
		fclose(Source); //после считки данных, файл больше не нужен, закрываем
	    int CommandNumber = argv[3][0] - '0';
		switch (CommandNumber) //проверяем, какая команда пришла
		{
		case 0:
			inversion(Pixels); //инверсия
			break;
		case 1:
			horizontal_reflextion(Pixels); //горизонтальное отражение
			break;
		case 2:
			vertical_reflextion(Pixels); //вертикальное отражение
			break;
		case 3:
			clock_and_counter_wise_ninety(Pixels, 1); //по часовой
			break;
		case 4:
			clock_and_counter_wise_ninety(Pixels, 2); //против часовой
			break;
		default:
			cout << "Unknown command!"; //если пришла неизвестная команда
			fclose(Output);
			Pixels.clear(); //очистка памяти
			return 1;
		}

		for (int i = 0; i < Pixels.size(); ++i)
			for (int j = 0; j < Pixels[i].size(); ++j)
				fputc(Pixels[i][j], Output); //запись матрицы пикселей в выходной файл

		Pixels.clear(); //чистка памяти
		fclose(Output); //закрытие выходного файла
		return 0; //выход из программы
	}

	else if (Format[1] == '6') //если цветное
	{
		vector <vector <struct RGB>> Pixels(height, vector<struct RGB>(width));

		for (int i = 0; i < height; ++i)
			for (int j = 0; j < width; ++j)
			{
				symbol = fgetc(Source); //считка 1 байта пикселя
				Pixels[i][j].Red = symbol;

				symbol = fgetc(Source); //считка 2 байта пикселя
				Pixels[i][j].Green = symbol;

				symbol = fgetc(Source); //считка 3 байта пикселя
				Pixels[i][j].Blue = symbol;
			}

		fclose(Source); //после считки данных, файл больше не нужен, закрываем
		int CommandNumber = argv[3][0] - '0';
		switch (CommandNumber) //проверяем, какая команда пришла
		{
		case 0:
			inversion(Pixels); //инверсия
			break;
		case 1:
			horizontal_reflextion(Pixels); //горизонтальное отражение
			break;
		case 2:
			vertical_reflextion(Pixels); //вертикальное отражение
			break;
		case 3:
			clock_and_counter_wise_ninety(Pixels, 1); //по часовой
			break;
		case 4:
			clock_and_counter_wise_ninety(Pixels, 2); //против часовой
			break;
		default:
			cout << "Unknown command!"; //если пришла неизвестная команда
			fclose(Output);
			Pixels.clear(); //очистка памяти
			return 1;
		}

		for (int i = 0; i < Pixels.size(); ++i)
			for (int j = 0; j < Pixels[i].size(); ++j)
				//запись матрицы пикселей в выходной файл
			{
				fputc(Pixels[i][j].Red, Output);
				fputc(Pixels[i][j].Green, Output);
				fputc(Pixels[i][j].Blue, Output);
			}

		Pixels.clear(); //чистка памяти
		fclose(Output); //закрытие выходного файла
		return 0; //выход из программы
	}
}
