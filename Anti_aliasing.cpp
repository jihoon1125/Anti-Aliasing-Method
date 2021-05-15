#pragma warning(disable : 4996)

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <Windows.h>
#include<cstdio>
using namespace std;


#define PIXEL_NUM1  1920*1080 //고정 픽셀 수
#define PIXEL_NUM2  832*480
#define M_ICH 3//채널 수

vector<vector<UCHAR>> m_ui8Up(3, vector<UCHAR>(PIXEL_NUM1));//upsampled image
vector<vector<UCHAR>> m_ui8Down(3, vector<UCHAR>(PIXEL_NUM1 / 4));//downsampled image
vector<vector<UCHAR>> m_ui8Comp(3, vector<UCHAR>(PIXEL_NUM1));//파일로부터 읽어온 데이터 8-bit버전
vector<vector<FLOAT>> m_ui8Anti(3, vector<FLOAT>(PIXEL_NUM1));//읽어온 데이터 8-bit anti aliasing 버전

int m1_size[3] = { PIXEL_NUM1  , PIXEL_NUM1 / 4 , PIXEL_NUM1 / 4 };//각 채널별 픽셀 수
int m2_size[3] = { PIXEL_NUM2 , PIXEL_NUM2 / 4 , PIXEL_NUM2 / 4 };//각 채널별 픽셀 수



void readOneFrame(FILE* file, int resolution)//하나의 프레임을 읽어오는 함수
{
	int* m_size;
	if (resolution == 0)
		m_size = m1_size;
	else
		m_size = m2_size;
	
	for (int ch = 0; ch < M_ICH; ch++)
	{
		fread(&m_ui8Comp[ch][0], sizeof(UCHAR), m_size[ch], file);
	}
}


/* anti aliasing 작업 수행*/
void anti_aliasing(int resolution)
{	
	float gaussian_filter[5][5] = { 1.0 / 273, 4.0 / 273, 7.0 / 273, 4.0 / 273, 1.0 / 273,
									4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273,
									7.0 / 273, 26.0 / 273, 41.0 / 273, 26.0 / 273, 7.0 / 273,
									4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273,
									1.0 / 273, 4.0 / 273, 7.0 / 273, 4.0 / 273, 1.0 / 273 };
	int width[3], height[3];
	
	
	if (resolution == 0)
	{
		width[0] = 1920; height[0] = 1080;
		width[1] = 960; height[1] = 540;
		width[2] = 960; height[2] = 540;		
	}
	else
	{
		width[0] = 832; height[0] = 480;
		width[1] = 416; height[1] = 240;
		width[2] = 416; height[2] = 240;
		}


	for (int ch = 0; ch < 3; ch++)
	{
		for (int h = 0; h < height[ch]; h++)
		{
			for (int w = 0; w < width[ch]; w++)
			{
				m_ui8Anti[ch][width[ch] * h + w] = m_ui8Comp[ch][width[ch] * h + w] * gaussian_filter[2][2];
				if (h > 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 2) + w] * gaussian_filter[0][2];
				if (h > 0) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 1) + w] * gaussian_filter[1][2];
				if (h < height[ch] - 2) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 2) + w] * gaussian_filter[4][2];
				if (h < height[ch] - 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 1) + w] * gaussian_filter[3][2];

				if (w > 1)
				{
					if( h > 1)	m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 2) + w - 2] * gaussian_filter[0][0];
					if( h > 0)  m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 1) + w - 2] * gaussian_filter[1][0];
					if( h < height[ch] - 2) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 2) + w - 2] * gaussian_filter[4][0];
					if( h < height[ch] - 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 1) + w - 2] * gaussian_filter[3][0];
					
					m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * h + w - 2] * gaussian_filter[2][0];					
				}

				if (w > 0)
				{
					if( h > 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 2) + w - 1] * gaussian_filter[0][1];
					if( h > 0) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 1) + w - 1] * gaussian_filter[1][1];
					if( h < height[ch] - 2) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 2) + w - 1] * gaussian_filter[4][1];
					if( h < height[ch] - 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 1) + w - 1] * gaussian_filter[3][1];
					
					m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * h + w - 1] * gaussian_filter[2][1];						
				}

				if (w < width[ch] - 2)
				{
					if( h > 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 2) + w + 2] * gaussian_filter[0][4];
					if( h > 0) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 1) + w + 2] * gaussian_filter[1][4];
					if( h < height[ch] - 2) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 2) + w + 2] * gaussian_filter[4][4];
					if( h < height[ch] - 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 1) + w + 2] * gaussian_filter[3][4];

					m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * h + w + 2] * gaussian_filter[2][4];				
				}

				if (w < width[ch] - 1)
				{
					if( h > 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 2) + w + 1] * gaussian_filter[0][3];
					if( h > 0) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h - 1) + w + 1] * gaussian_filter[1][3];
					if( h < height[ch] - 2) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 2) + w + 1] * gaussian_filter[4][3];
					if( h < height[ch] - 1) m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * (h + 1) + w + 1] * gaussian_filter[3][3];

					m_ui8Anti[ch][width[ch] * h + w] += m_ui8Comp[ch][width[ch] * h + w + 1] * gaussian_filter[2][3];				
				}			
				

			}
		}

	}


}

void Downsampling(bool aliasing, int resolution)
{
	int width[3], height[3];

	if (resolution == 0)
	{
		width[0] = 1920; height[0] = 1080;
		width[1] = 960; height[1] = 540;
		width[2] = 960; height[2] = 540;
		
	}
	else
	{
		width[0] = 832; height[0] = 480;
		width[1] = 416; height[1] = 240;
		width[2] = 416; height[2] = 240;		
	}

	for (int ch = 0; ch < 3; ch++)
	{
		for (int h = 0; h < height[ch] / 2; h++)
		{
			for (int w = 0; w < width[ch] / 2; w++)
			{
				if(aliasing == true)
					m_ui8Down[ch][h * width[ch] / 2 + w] = (int)(m_ui8Anti[ch][(h * 2 + 1) * width[ch] + (w * 2 + 1)] + 0.5);
				else
					m_ui8Down[ch][h * width[ch] / 2 + w] = (int)(m_ui8Comp[ch][(h * 2 + 1) * width[ch] + (w * 2 + 1)] + 0.5);
			}
		}
	}

}

void Upsampling(int resolution)
{
	FLOAT * H[3], * V[3], * D[3];
	int width[3], height[3];
	
	float filter_coefficient[6] = { 11.0 / 256, -43.0 / 256, 160.0 / 256, 160.0 / 256, -43.0 / 256, 11.0 / 256 };
	if (resolution == 0)
	{
		H[0] = new FLOAT[960 * 540];	V[0] = new FLOAT[960 * 540];	D[0] = new FLOAT[960 * 540];
		H[1] = new FLOAT[480 * 270];	V[1] = new FLOAT[480 * 270];	D[1] = new FLOAT[480 * 270];
		H[2] = new FLOAT[480 * 270];	V[2] = new FLOAT[480 * 270];	D[2] = new FLOAT[480 * 270];	
		width[0] = 960; height[0] = 540;
		width[1] = 480; height[1] = 270;
		width[2] = 480; height[2] = 270;		
	}
	else
	{
		H[0] = new FLOAT[416 * 240];	V[0] = new FLOAT[416 * 240];	D[0] = new FLOAT[416 * 240];
		H[1] = new FLOAT[208 * 120];	V[1] = new FLOAT[208 * 120];	D[1] = new FLOAT[208 * 120];
		H[2] = new FLOAT[208 * 120];	V[2] = new FLOAT[208 * 120];	D[2] = new FLOAT[208 * 120];
		width[0] = 416; height[0] = 240;
		width[1] = 208; height[1] = 120;
		width[2] = 208; height[2] = 120;
	}

	/*  H값 저장하기 */
	for (int ch = 0; ch < 3; ch++)
	{
		for (int h = 0; h < height[ch] ; h++)
		{
			for (int w = 0; w < width[ch]; w++)
			{
				H[ch][h * width[ch] + w] = m_ui8Down[ch][h * width[ch] + w] * filter_coefficient[3];
				if(w>0)		H[ch][h * width[ch] + w] += m_ui8Down[ch][h * width[ch] + w - 1] * filter_coefficient[2];
				if(w>1)		H[ch][h * width[ch] + w] += m_ui8Down[ch][h * width[ch] + w - 2] * filter_coefficient[1];
				if(w>2)		H[ch][h * width[ch] + w] += m_ui8Down[ch][h * width[ch] + w - 3] * filter_coefficient[0];
				if(w < width[ch] - 1)	 H[ch][h * width[ch] + w] += m_ui8Down[ch][h * width[ch] + w + 1] * filter_coefficient[4];
				if(w < width[ch] - 2)	H[ch][h * width[ch] + w] += m_ui8Down[ch][h * width[ch] + w + 2] * filter_coefficient[5];				
			}
		}
	}

	/* V값 저장하기 */
	for (int ch = 0; ch < 3; ch++)
	{
		for (int w = 0; w < width[ch]; w++)
		{
			for (int h = 0; h < height[ch]; h++)
			{
				V[ch][h * width[ch] + w] = m_ui8Down[ch][h * width[ch] + w] * filter_coefficient[3];
				if (h > 0)		V[ch][h * width[ch] + w] += m_ui8Down[ch][(h - 1) * width[ch] + w] * filter_coefficient[2];
				if (h > 1)		V[ch][h * width[ch] + w] += m_ui8Down[ch][(h - 2) * width[ch] + w] * filter_coefficient[1];
				if (h > 2)		V[ch][h * width[ch] + w] += m_ui8Down[ch][(h - 3) * width[ch] + w] * filter_coefficient[0];
				if (h < height[ch] - 1)	 V[ch][h * width[ch] + w] += m_ui8Down[ch][(h + 1) * width[ch] + w] * filter_coefficient[4];
				if (h < height[ch] - 2)	 V[ch][h * width[ch] + w] += m_ui8Down[ch][(h + 2) * width[ch] + w] * filter_coefficient[5];
			}
		}
	}

	/* D값 저장하기 */
	for (int ch = 0; ch < 3; ch++)
	{
		for (int h = 0; h < height[ch]; h++)
		{
			for (int w = 0; w < width[ch]; w++)
			{
				D[ch][h * width[ch] + w] = V[ch][h * width[ch] + w] * filter_coefficient[3];
				if (w > 0)		D[ch][h * width[ch] + w] += V[ch][h * width[ch] + w - 1] * filter_coefficient[2];
				if (w > 1)		D[ch][h * width[ch] + w] += V[ch][h * width[ch] + w - 2] * filter_coefficient[1];
				if (w > 2)		D[ch][h * width[ch] + w] += V[ch][h * width[ch] + w - 3] * filter_coefficient[0];
				if (w < width[ch] - 1)	 D[ch][h * width[ch] + w] += V[ch][h * width[ch] + w + 1] * filter_coefficient[4];
				if (w < width[ch] - 2)	D[ch][h * width[ch] + w] += V[ch][h * width[ch] + w + 2] * filter_coefficient[5];;
			}
		}
	}

	/* upsampling 결과 저장 */
	for (int ch = 0; ch < 3; ch++)
	{
		for (int h = 0; h < height[ch]; h++)
		{
			for (int w = 0; w < width[ch]; w++)
			{
				m_ui8Up[ch][2 * h * (width[ch]*2) + (2 * w)] = (INT)(D[ch][h * width[ch] + w] + 0.5);
				m_ui8Up[ch][2 * h * (width[ch]*2) + (2 * w) + 1] = (INT)(V[ch][h * width[ch] + w] + 0.5);
				m_ui8Up[ch][(2 * h + 1) * (width[ch]*2) + (2 * w)] = (INT)(H[ch][h * width[ch] + w] + 0.5);
				m_ui8Up[ch][(2 * h + 1) * (width[ch]*2) + (2 * w) + 1] = m_ui8Down[ch][h * width[ch] + w];
			}
		}
	}
}

void PRINT_PSNR(int resolution, bool antialiased)//PSNR 출력함수
{
	int MAX = 1023;
    double MSE;
	int* m_size;
	if (resolution ==0)
	{
		m_size = m1_size;
	}
	else {
		m_size = m2_size;
	}

	for (int ch = 0; ch < M_ICH; ch++)
	{
		MSE = 0;
		for (int i = 0; i < m_size[ch]; i++)//각 채널별로 MSE구하고 PSNR 출력
		{
			if(antialiased == 0)
				MSE += (m_ui8Comp[ch][i] - m_ui8Up[ch][i]) * (m_ui8Comp[ch][i] - m_ui8Up[ch][i]);
			else
				MSE += (m_ui8Anti[ch][i] - m_ui8Up[ch][i]) * (m_ui8Anti[ch][i] - m_ui8Up[ch][i]);
		}
		MSE /= m_size[ch];

		if (ch == 0)
			cout << "Y의 PSNR : ";
		else if (ch == 1)
			cout << "Cb의 PSNR : ";
		else
			cout << "Cr의 PSNR : ";

		cout << 10 * log10(MAX * MAX / (double)MSE) << endl;

	}
	cout << endl;
}
int main(void)
{
	FILE* fp_InputImg;
	FILE* fp_outputImg;
	
	/* 이거 각각 결과폴더 생성해주셔야 합니다 ㅠㅠ */
	const char* input_name[13] = { "./input/ParkScene_1920x1080_yuv420_8bit_frame0.yuv",
								  "./input/ParkScene_1920x1080_yuv420_8bit_frame200.yuv",
								  "./input/Kimono1_1920x1080_yuv420_8bit_frame0.yuv",
								  "./input/Cactus_1920x1080_yuv420_8bit_frame200.yuv",
								  "./input/BQTerrace_1920x1080_yuv420_8bit_frame0.yuv" ,
								  "./input/PartyScene_832x480_yuv420_8bit_frame0.yuv",
							      "./input/PartyScene_832x480_yuv420_8bit_frame120.yuv",
								  "./input/RaceHorsesC_832x480_yuv420_8bit_frame0.yuv",
								  "./input/RaceHorsesC_832x480_yuv420_8bit_frame120.yuv",
								  "./input/BQMall_832x480_yuv420_8bit_frame0.yuv",
								  "./input/BQMall_832x480_yuv420_8bit_frame120.yuv",
								  "./input/BasketballDrill_832x480_yuv420_8bit_frame0.yuv",
								  "./input/BasketballDrill_832x480_yuv420_8bit_frame360.yuv"};

	const char* output_anti_name[13] = { "./up_anti/ParkScene_1920x1080_yuv420_8bit_frame0.yuv",
								  "./up_anti/ParkScene_1920x1080_yuv420_8bit_frame200.yuv",
								  "./up_anti/Kimono1_1920x1080_yuv420_8bit_frame0.yuv",
								  "./up_anti/Cactus_1920x1080_yuv420_8bit_frame200.yuv",
								  "./up_anti/BQTerrace_1920x1080_yuv420_8bit_frame0.yuv", 	
								  "./up_anti/PartyScene_832x480_yuv420_8bit_frame0.yuv",
								  "./up_anti/PartyScene_832x480_yuv420_8bit_frame120.yuv",
								  "./up_anti/RaceHorsesC_832x480_yuv420_8bit_frame0.yuv",
								  "./up_anti/RaceHorsesC_832x480_yuv420_8bit_frame120.yuv",
								  "./up_anti/BQMall_832x480_yuv420_8bit_frame0.yuv",
								  "./up_anti/BQMall_832x480_yuv420_8bit_frame120.yuv",
								  "./up_anti/BasketballDrill_832x480_yuv420_8bit_frame0.yuv",
								  "./up_anti/BasketballDrill_832x480_yuv420_8bit_frame360.yuv"};	

	const char* output_noanti_name[13] = { "./up_nonanti/ParkScene_1920x1080_yuv420_8bit_frame0.yuv",
								  "./up_nonanti/ParkScene_1920x1080_yuv420_8bit_frame200.yuv",
								  "./up_nonanti/Kimono1_1920x1080_yuv420_8bit_frame0.yuv",
								  "./up_nonanti/Cactus_1920x1080_yuv420_8bit_frame200.yuv",
								  "./up_nonanti/BQTerrace_1920x1080_yuv420_8bit_frame0.yuv",
									"./up_nonanti/PartyScene_832x480_yuv420_8bit_frame0.yuv",
									"./up_nonanti/PartyScene_832x480_yuv420_8bit_frame120.yuv",
									 "./up_nonanti/RaceHorsesC_832x480_yuv420_8bit_frame0.yuv",
									 "./up_nonanti/RaceHorsesC_832x480_yuv420_8bit_frame120.yuv",
									 "./up_nonanti/BQMall_832x480_yuv420_8bit_frame0.yuv",
									 "./up_nonanti/BQMall_832x480_yuv420_8bit_frame120.yuv",
									 "./up_nonanti/BasketballDrill_832x480_yuv420_8bit_frame0.yuv",
									 "./up_nonanti/BasketballDrill_832x480_yuv420_8bit_frame360.yuv"};

	const char* down_anti_name[13] = { "./down_anti/ParkScene_1920x1080_yuv420_8bit_frame0.yuv",
								  "./down_anti/ParkScene_1920x1080_yuv420_8bit_frame200.yuv",
								  "./down_anti/Kimono1_1920x1080_yuv420_8bit_frame0.yuv",
								  "./down_anti/Cactus_1920x1080_yuv420_8bit_frame200.yuv",
								  "./down_anti/BQTerrace_1920x1080_yuv420_8bit_frame0.yuv",
								  "./down_anti/PartyScene_832x480_yuv420_8bit_frame0.yuv",
								  "./down_anti/PartyScene_832x480_yuv420_8bit_frame120.yuv",
								  "./down_anti/RaceHorsesC_832x480_yuv420_8bit_frame0.yuv",
								  "./down_anti/RaceHorsesC_832x480_yuv420_8bit_frame120.yuv",
								  "./down_anti/BQMall_832x480_yuv420_8bit_frame0.yuv",
								  "./down_anti/BQMall_832x480_yuv420_8bit_frame120.yuv",
								  "./down_anti/BasketballDrill_832x480_yuv420_8bit_frame0.yuv",
								  "./down_anti/BasketballDrill_832x480_yuv420_8bit_frame360.yuv"	};

	const char* down_noanti_name[13] = { "./down_nonanti/ParkScene_1920x1080_yuv420_8bit_frame0.yuv",
								  "./down_nonanti/ParkScene_1920x1080_yuv420_8bit_frame200.yuv",
								  "./down_nonanti/Kimono1_1920x1080_yuv420_8bit_frame0.yuv",
								  "./down_nonanti/Cactus_1920x1080_yuv420_8bit_frame200.yuv",
								  "./down_nonanti/BQTerrace_1920x1080_yuv420_8bit_frame0.yuv" ,
								 "./down_nonanti/PartyScene_832x480_yuv420_8bit_frame0.yuv",
								 "./down_nonanti/PartyScene_832x480_yuv420_8bit_frame120.yuv",
								 "./down_nonanti/RaceHorsesC_832x480_yuv420_8bit_frame0.yuv",
								 "./down_nonanti/RaceHorsesC_832x480_yuv420_8bit_frame120.yuv",
								 "./down_nonanti/BQMall_832x480_yuv420_8bit_frame0.yuv",
								 "./down_nonanti/BQMall_832x480_yuv420_8bit_frame120.yuv",
								 "./down_nonanti/BasketballDrill_832x480_yuv420_8bit_frame0.yuv",
								 "./down_nonanti/BasketballDrill_832x480_yuv420_8bit_frame360.yuv"};

	for (int i = 0; i < 13; i++) {
		fp_InputImg = fopen(input_name[i], "rb");
		int* m_size;
		int resolution = 0;
		if (!fp_InputImg) {
			printf("Can not open file.");
		}

		if (i > 4) resolution = 1;

		readOneFrame(fp_InputImg, resolution);//8-bit 이미지 데이터 저장

		if (resolution == 0)
		{
			m_size = m1_size;
		}
		else {
			m_size = m2_size;
		}

		fclose(fp_InputImg);

		anti_aliasing(resolution);

		Downsampling(true, resolution);//anti-aliasing한 값으로 다운 샘플링
		fp_outputImg = fopen(down_anti_name[i], "wb"); // antialising한 데이터로 다운샘플링한 결과 저장하기
		for (int ch = 0; ch < M_ICH; ch++) {
			fwrite(&m_ui8Down[ch][0], sizeof(UCHAR), m_size[ch] / 4, fp_outputImg);
		}
		fclose(fp_outputImg);


		Upsampling(resolution); // upsampling		

		fp_outputImg = fopen(output_anti_name[i], "wb"); // antialising 한거 upsampling 한 거 저장하기
		for (int ch = 0; ch < M_ICH; ch++) {
			fwrite(&m_ui8Up[ch][0], sizeof(UCHAR), m_size[ch], fp_outputImg);
		}
		fclose(fp_outputImg);			

		cout << "<Antialiasing 버전>" << endl;
		cout << output_anti_name[i] << "의 PSNR 출력" << endl;
		PRINT_PSNR(resolution, 1);//PSNR 출력	

		Downsampling(false,resolution);//anti-aliasing 하지 않은 값으로 다운 샘플링
		fp_outputImg = fopen(down_noanti_name[i], "wb"); // antialising 안한 데이터로 다운샘플링한 결과 저장하기
		for (int ch = 0; ch < M_ICH; ch++) {
			fwrite(&m_ui8Down[ch][0], sizeof(UCHAR), m_size[ch] / 4, fp_outputImg);
		}
		fclose(fp_outputImg);


		Upsampling(resolution); // upsampling	
		fp_outputImg = fopen(output_noanti_name[i], "wb"); // antialiasing 안한거로 upsampling 한 거 저장하기
		for (int ch = 0; ch < M_ICH; ch++) {
			fwrite(&m_ui8Up[ch][0], sizeof(UCHAR), m_size[ch], fp_outputImg);
		}
		fclose(fp_outputImg);

		cout << "<Not antialiasing 버전>" << endl;
		cout << output_noanti_name[i] << "의 PSNR 출력" << endl;
		PRINT_PSNR(resolution, 0);//PSNR 출력	

		
	}
	
	return 0;
}