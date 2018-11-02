//   _____________________________________________________________________________________________________________________________________________________
//  |  _________________________________________________________________________________________________________________________________________________  |
//  | | Multimedia Information Systems                                                                                                                  | |
//  | | Homework#2 - Yuv files                                                                                                                          | |
//  | | Alim Furkan CANBULAT - 05140000555                                                                                                              | |
//  | | YUV_PSNR_cal: Programa komut sat�r�ndan girilen iki tane .yuv uzant�l� resim veya video dosyalr�n� Y->U->V s�ras�nda kodlanm�� �ekilde ��zerek  | |
//  | | iki dosyan�n her pixelindeki Y, U ve V de�erlerini kendi aralar�nda kar��la�t�r�p Mean Square Error hesaplayarak PSNR de�erlerini ayr� ayr� he- | |
//  | | saplar.																																		    | |
//  | | komut sat�r�nda program �u �ekilde �al��t�r�l�r;																								| |
//  | | .\YUV_PSNR_cal.exe original.yuv distorted.yuv witdh height format(i.e. 4:2:0)																    | |
//  | |_________________________________________________________________________________________________________________________________________________| |
//  |_____________________________________________________________________________________________________________________________________________________|
//
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <math.h>
#include <iomanip>

using namespace std;

static int frame_num;
// Yuv_Decoder s�n�f� bir yuv dosyas�n�n sahip olabilece�i �o�u �zelli�i bar�nd�rmas� i�in tasarlanm��t�r.
// S�n�f�n i�indeki nesleler ve �zellikleri
// file -> .yuv dosyalar�n� binary olarak a�mak ve okumak i�in olu�turulmu� FILE pointerd�r
// buffer -> .yuv dosyas�ndan okunan de�erleri her seferinde dosya a�mamak ve dosyadaki verileri tutmak i�in olu�turulmu� bir char array dir
// frame_number -> .yuv dosyas� bir video dosyas� ise i�inde ka� frame oldu�unu tutan bir nesnedir
// total_coeff -> .yuv dosyas�n�n format�na g�re Y, U ve V birimlerinin toplam�n� tutan bir nesnedir
// one_frame_size -> videodaki 1 frame'in ��z�n�rl�l���n� tutan nesnedir
// witdh, height -> .yuv dosyas�n�n ��z�n�rl���n� tutar
// size -> .yuv dosyas�n�n ka� byte oldu�unu tutar
// filename -> .yuv dosyas�n�n ismini tutar
// chroma_subsample -> chroma format�n� tutar
// coeff_y, coeff_u, coeff_v -> Y, U ve V birim katsay�lar�n� tutar
// func Yuv_Decoder -> Yuv_Decoder s�n�f�n� yap�c�(constructor) fonksiyonudur. Dosyan�n ad�, ��z�n�rl���,chroma format�n� ve frame boyutunu hesaplar 
// func Detect_Size -> .yuv dosyas�n� binary olarak okur. Dosyan�n boyutunu bulur ve dosyan�n i�indeki bilgileri buffer a atar.
// func Detect_Frame_number -> .yuv dosyas�n�n chroma format�na g�re i�erisinde ka� frame oldu�unu hesaplar
class Yuv_Decoder
{
public:
	FILE* file;
	uint8_t *buffer;
	double frame_number;
	double total_coeff;
	unsigned int one_frame_size;
	int witdh;
	int height;
	long int size;
	string filename;
	string chroma_subsample;
	double coeff_y, coeff_u, coeff_v;
public:
	Yuv_Decoder(string file_name, int witdh_, int height_, string chroma_subsample_);
	int Detect_Frame_number();
	long int Detect_Size();
};

Yuv_Decoder::Yuv_Decoder(string file_name, int witdh_, int height_, string chroma_subsample_) {
	filename = file_name;
	witdh = witdh_;
	height = height_;
	chroma_subsample = chroma_subsample_;
	Detect_Frame_number();
}

long int Yuv_Decoder::Detect_Size() {
	const char* X = (char*)filename.c_str();
	fopen_s(&file, X, "rb");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (uint8_t *)malloc(size + 1);
	fread(buffer, size, 1, file);
	fclose(file);
	return size;
}

int Yuv_Decoder::Detect_Frame_number() {
	long int size_file = Detect_Size();
	coeff_y = ((double)chroma_subsample[0] - 48) / 4;
	if (((double)chroma_subsample[2] - 48) == 2 && ((double)chroma_subsample[4] - 48) == 0)
	{
		coeff_u = ((double)chroma_subsample[2] - 48) / 8;
		coeff_v = ((double)chroma_subsample[2] - 48) / 8;
	}
	else if (((double)chroma_subsample[2] - 48) == 2 && ((double)chroma_subsample[4] - 48) == 2) {
		coeff_u = ((double)chroma_subsample[2] - 48) / 4;
		coeff_v = ((double)chroma_subsample[4] - 48) / 4;
	}
	else {
		coeff_u = ((double)chroma_subsample[2] - 48) / 4;
		coeff_v = ((double)chroma_subsample[4] - 48) / 4;
	}
	total_coeff = (double)(coeff_y + coeff_u + coeff_v);
	frame_number = size_file / (witdh*height*((double)total_coeff));
	one_frame_size = (int)(size_file / frame_number);
	return (int)frame_number;
}

int psnr(Yuv_Decoder* x, Yuv_Decoder* y) {
	if (x->one_frame_size != y->one_frame_size) {
		cout << "Dosyalar e�it dosyalar de�il farkl� ��z�n�rl�kteler!" << endl;
		return -1;
	}
	if (x->frame_number < y->frame_number) {
		frame_num = (int)x->frame_number;
	}
	else
	{
		frame_num = (int)y->frame_number;
	}
	int diff = 0;
	double MSE = 0;
	double PSNR = 0;
	double average_PSNR_Y = 0;
	double average_PSNR_U = 0;
	double average_PSNR_V = 0;
	for (int i = 0; i < frame_num; i++)
	{
		int c = x->one_frame_size*i;
		for (int j = c; j < x->one_frame_size*x->coeff_y / x->total_coeff + c; j++) {
			diff = (int)x->buffer[j] - (int)y->buffer[j];
			MSE += pow(diff, 2);
		}
		MSE = MSE / (double)(x->one_frame_size*x->coeff_y / x->total_coeff);
		PSNR = 10 * log10(255 * 255 / MSE);
		average_PSNR_Y += PSNR;
		cout << "PSNR-Y of " << i + 1 << ".frame = " <<setw(6) <<setprecision(5) << PSNR << " dB" << endl;
		for (int j = x->one_frame_size*x->coeff_y / x->total_coeff + c; j < (x->one_frame_size*x->coeff_y / x->total_coeff) + (x->one_frame_size*x->coeff_u / x->total_coeff) + c; j++) {
			diff = (int)x->buffer[j] - (int)y->buffer[j];
			MSE += pow(diff, 2);
		}
		MSE = MSE / (double)(x->one_frame_size*x->coeff_u / x->total_coeff);
		PSNR = 10 * log10(255 * 255 / MSE);
		average_PSNR_U += PSNR;
		cout << "PSNR-U of " << i + 1 << ".frame = " << setw(6) << setprecision(5) << PSNR << " dB" << endl;
		for (int j = x->one_frame_size*x->coeff_y / x->total_coeff + x->one_frame_size*x->coeff_u / x->total_coeff + c; j < (x->one_frame_size*x->coeff_y / x->total_coeff) + (x->one_frame_size*x->coeff_u / x->total_coeff) + (x->one_frame_size*x->coeff_v / x->total_coeff) + c; j++) {
			diff = (int)x->buffer[j] - (int)y->buffer[j];
			MSE += pow(diff, 2);
		}
		MSE = MSE / (double)(x->one_frame_size*x->coeff_v / x->total_coeff);
		PSNR = 10 * log10(255 * 255 / MSE);
		average_PSNR_V += PSNR;
		cout << "PSNR-V of " << i + 1 << ".frame = " << setw(6) << setprecision(5) << PSNR << " dB" << endl << endl;
	}
	cout << "average PSNR-Y = " << setw(6) << setprecision(5) << average_PSNR_Y /frame_num << " dB" << endl;
	cout << "average PSNR-U = " << setw(6) << setprecision(5) << average_PSNR_U /frame_num<< " dB" << endl;
	cout << "average PSNR-V = " << setw(6) << setprecision(5) << average_PSNR_V /frame_num << " dB" << endl;
	return 1;
}

int main(int argc, char** argv)
{
	if (argc != 6) {
	cout << "Yetersiz veri girdiniz!" << endl;
	return -1;
	}
	time_t current_time;
	time_t previous_time;
	time(&previous_time);
	Yuv_Decoder  ori_yuv_file(argv[1], stoi(argv[3]), stoi(argv[4]), argv[5]);
	Yuv_Decoder  dis_yuv_file(argv[2], stoi(argv[3]), stoi(argv[4]), argv[5]);
	psnr(&ori_yuv_file, &dis_yuv_file);
	time(&current_time);
	cout << endl <<"Calisma zamani "<<current_time - previous_time << "(sec) surdu!"<<endl << endl;
	return 0;
}
