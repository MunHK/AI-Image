#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h> // GUI 기능 지원
#include <math.h>

///////////////////
// 전역 변수 선언부
///////////////////
HWND  hwnd; //  윈도 화면(외부, 물리적)
HDC hdc; // 윈도 화면(내부, 논리적)

int oriH, oriW, tarH, tarW;
unsigned char** oriImage = NULL;
unsigned char** tarImage = NULL;

char filename[200];  // 파일명

///////////////////
// 함수 선언부
///////////////////
// 공통 함수부
void loadImage(); void saveImage(); void displayImage(); void printMenu();
void mallocOriImage(); void mallocTarImage(); void freeOriImage(); void freeTarImage();
// 영상처리 함수부
void equalImage();
void addImage();
void reverseImage();
void zoomOut(); void zoomIn(); void zoomIn2(); void rotate(); void rotate2();
void histoStretch(); void endIn(); void histoEqual();
void emboss();  void blurr(); void edge1(); void edge2(); void edge3();

void printMenu() {
	puts("\n GrayScale Image Processing");
	puts("\n 1.열기  2.저장  9.종료");
	puts("\n A.동일영상  B.밝게/어둡게  C.반전  D.축소  E.확대(포워딩)  F.확대(백워딩)");
	puts("\n G.회전 H.회전(수정) I.히스토그램 스트래칭 J. 엔드-인 K.평활화");
	puts("\n L.엠보싱 M.블러링 N.수직에지 O.수평에지 P.수평+수직(임계값)");
}
///////////////////
// 메인 코드부
///////////////////
void main() {
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);  // 윈도10용.  윈도11(hwnd--> NULL)

	char key = 0;
	while (key != '9') {
		printMenu();
		key = _getch();
		system("cls");
		switch (key) {
		case '1': loadImage(); break;
		case '2': saveImage(); break;
		case 'A':
		case 'a': equalImage(); break;
		case 'B':
		case 'b': addImage(); break;
		case 'C':
		case 'c': reverseImage(); break;
		case 'D':
		case 'd': zoomOut(); break;
		case 'E':
		case 'e': zoomIn(); break;
		case 'F':
		case 'f': zoomIn2(); break;
		case 'G':
		case 'g': rotate(); break;
		case 'H':
		case 'h': rotate2(); break;
		case 'I':
		case 'i': histoStretch(); break;
		case 'J':
		case 'j': endIn(); break;
		case 'K':
		case 'k': histoEqual(); break;
		case 'L':
		case 'l': emboss(); break;
		case 'M':
		case 'm': blurr(); break;
		case 'N':
		case 'n': edge1(); break;
		case 'O':
		case 'o': edge2(); break;
		case 'P':
		case 'p': edge3(); break;
		case '9': freeOriImage(); freeTarImage(); break;
		}
	}
	puts("안녕히 가세요~~ 도비는 자유에요...");


}

////////////////////
/// 함수 정의부
////////////////
void mallocOriImage() {
	oriImage = (unsigned char**)malloc(sizeof(unsigned char*) * oriH);
	for (int i = 0; i < oriH; i++)
		oriImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * oriW);
}
void mallocTarImage() {
	tarImage = (unsigned char**)malloc(sizeof(unsigned char*) * tarH);
	for (int i = 0; i < tarH; i++)
		tarImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * tarW);
}
void freeOriImage() {
	if (oriImage == NULL)
		return;
	for (int i = 0; i < oriH; i++)
		free(oriImage[i]);
	free(oriImage);
	oriImage = NULL;
}
void freeTarImage() {
	if (tarImage == NULL)
		return;
	for (int i = 0; i < tarH; i++)
		free(tarImage[i]);
	free(tarImage);
	tarImage = NULL;
}

void loadImage() {
	char fullname[200] = "C:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	printf("파일명-->"); scanf("%s", tmpName);  // cat01, dog05, etc11
	strcat(fullname, tmpName);
	strcat(fullname, ".raw");
	strcpy(filename, fullname);

	//(중요!) 원본영상의 크기를 파악!
	FILE* rfp;
	rfp = fopen(filename, "rb");
	if (!rfp) {
		printf("파일명 틀렸음~ \n");
		return;
	}
	fseek(rfp, 0L, SEEK_END); // 파일의 끝으로 이동
	long long fsize = ftell(rfp);
	fclose(rfp);
	// 원본 영상 크기
	oriH = oriW = sqrt(fsize);
	// 2차원 메모리 할당
	freeOriImage();
	mallocOriImage();

	// 파일 --> 메모리
	rfp = fopen(filename, "rb");
	for (int i = 0; i < oriH; i++)
		fread(oriImage[i], sizeof(unsigned char), oriW, rfp);
	fclose(rfp);
	// printf("%d ", oriImage[50][50]);
	equalImage();

}
void saveImage() {
	char fullname[200] = "C:/RAW/Pet_RAW(squre)/Pet_RAW(512x512)/";
	char tmpName[50];
	printf("저장 파일명-->"); scanf("%s", tmpName);  // out01, out02 ....
	strcat(fullname, tmpName);
	strcat(fullname, "_512.raw");
	strcpy(filename, fullname);

	FILE* wfp;
	wfp = fopen(filename, "wb");
	// 메모리 >> 파일
	for (int i = 0; i < tarH; i++) {
		fwrite(tarImage[i], sizeof(unsigned char), tarW, wfp);
	}
	fclose(wfp);
	printf("%s 로 저장됨", filename);
}
void displayImage() {
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			int px = tarImage[i][k];
			SetPixel(hdc, k + 50, i + 350, RGB(px, px, px));
		}
	}
}

/// <summary>
///  영상처리 함수부
/// </summary>

void equalImage() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i][k] = oriImage[i][k];
		}
	}
	///////////////////////////////
	displayImage();
}



void addImage() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	int value;
	printf("더할 값(-255~+255) :");  scanf("%d", &value);
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			int pixel = oriImage[i][k] + value;
			if (pixel > 255)
				pixel = 255;
			if (pixel < 0)
				pixel = 0;
			tarImage[i][k] = (unsigned char)pixel;
		}
	}
	///////////////////////////////
	displayImage();
}

void reverseImage() {  // 반전 : 0>255, 1>254, 2>253..... 254>1, 255>0
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i][k] = 255 - oriImage[i][k];
		}
	}
	///////////////////////////////
	displayImage();
}

void zoomOut() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	int scale;
	printf("축소 배율:"); scanf("%d", &scale);
	tarH = (int)(oriH / scale);
	tarW = (int)(oriW / scale);
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i / scale][k / scale] = oriImage[i][k];
		}
	}
	///////////////////////////////
	displayImage();
}
void zoomIn() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	int scale;
	printf("확대 배율:"); scanf("%d", &scale);
	tarH = (int)(oriH * scale);
	tarW = (int)(oriW * scale);
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당	
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i * scale][k * scale] = oriImage[i][k];
		}
	}
	///////////////////////////////
	displayImage();
}
void zoomIn2() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	int scale;
	printf("확대 배율:"); scanf("%d", &scale);
	tarH = (int)(oriH * scale);
	tarW = (int)(oriW * scale);
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당	
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			tarImage[i][k] = oriImage[i / scale][k / scale];
		}
	}
	///////////////////////////////
	displayImage();
}

void rotate() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	int angle;
	printf("회전 각도: "); scanf("%d", &angle);
	// 회전 수식
	// tarX = cos * oriX - sin * oriY
	// tarY = sin * oriX + cos * oriY
	double radian = angle * 3.141592 / 180.0;		// radian = degree * 3.141592 / 180.0;

	int tarX, tarY, oriX, oriY;
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			oriX = i;
			oriY = k;

			tarX = cos(radian) * oriX - sin(radian) * oriY;
			tarY = sin(radian) * oriX + cos(radian) * oriY;

			if ((0 <= tarX && tarX < tarH) && (0 <= tarY && tarY < tarW))
				tarImage[tarX][tarY] = oriImage[oriX][oriY];
		}
	}
	///////////////////////////////
	displayImage();
}
void rotate2() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당

	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	int angle;
	printf("회전 각도: "); scanf("%d", &angle);
	// 회전 수식
	// oriX = cos * (tarX - Cx) + sin * (tarY - Cy) + Cx
	// oriY = -sin * (tarX - Cx) + cos * (tarY - Cy) + Cy
	int Cx = oriH / 2;
	int Cy = oriW / 2;

	double radian = -angle * 3.141592 / 180.0;		// radian = degree * 3.141592 / 180.0;

	int tarX, tarY, oriX, oriY;
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			tarX = i;
			tarY = k;

			oriX = cos(radian) * (tarX - Cx) + sin(radian) * (tarY - Cy) + Cx;
			oriY = -sin(radian) * (tarX - Cx) + cos(radian) * (tarY - Cy) + Cy;

			if ((0 <= oriX && oriX < oriH) && (0 <= oriY && oriY < oriW))
				tarImage[tarX][tarY] = oriImage[oriX][oriY];
		}
	}
	///////////////////////////////
	displayImage();
}

void histoStretch() {
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	freeTarImage();
	mallocTarImage();

	// 수식 :  new = (old - low) / (high - low) * 255.0
	int low, high;
	low = high = oriImage[0][0];
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			if (oriImage[i][k] < low)
				low = oriImage[i][k];
			if (oriImage[i][k] > high)
				high = oriImage[i][k];
		}
	}
	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i][k] = ((double)oriImage[i][k] - low) / (high - low) * 255;
		}
	}
	///////////////////////////////
	displayImage();
}


void endIn() {
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	freeTarImage();
	mallocTarImage();

	// 수식 :  new = (old - low) / (high - low) * 255.0
	int low, high;
	low = high = oriImage[0][0];
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			if (oriImage[i][k] < low)
				low = oriImage[i][k];
			if (oriImage[i][k] > high)
				high = oriImage[i][k];
		}
	}
	low = low + 50;
	high = high - 50;
	// *** 진짜 영상처리 알고리즘 ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			double newVal = ((double)oriImage[i][k] - low) / (high - low) * 255;
			if (newVal < 0.0)
				newVal = 0;
			if (newVal > 255.0)
				newVal = 255;
			tarImage[i][k] = newVal;
		}
	}
	///////////////////////////////
	displayImage();
}

void histoEqual() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당	
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// 1단계 : 빈도수 히스토그램 생성
	int hist[256] = { 0, };
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			hist[oriImage[i][k]]++;
	// 2단계 : 누적 히스토그램 생성
	int sumHist[256] = { 0, };
	sumHist[0] = hist[0];
	for (int i = 1; i < 256; i++)
		sumHist[i] = sumHist[i - 1] + hist[i];
	// 3단계 : 정규화된 히스토그램 생성
	// 수식 : normalHist = sumHist * (1/(oriH*oriW)) * 255.0;
	double normalHist[256] = { 0.0, };
	for (int i = 0; i < 256; i++)
		normalHist[i] = sumHist[i] * (1.0 / (oriH * oriW)) * 255.0;

	// 4단계 : 원래값을 정규화된 값으로 치환
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i][k] = (unsigned char)normalHist[oriImage[i][k]];
		}
	}
	///////////////////////////////
	displayImage();
}

double** malloc_double(int h, int w) {
	double** memory;
	memory = (double**)malloc(sizeof(double*) * h);
	for (int i = 0; i < h; i++)
		memory[i] = (double*)malloc(sizeof(double) * w);
	return memory;
}
void free_double(double** memory, int h) {
	if (memory == NULL)
		return;
	for (int i = 0; i < h; i++)
		free(memory[i]);
	free(memory);
	memory = NULL;
}

void emboss() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// const int MSIZE = 3; // 마스크 크기
	// 중요! 마스크 준비
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  1.0, 0.0,  0.0 },
		{  0.0, 0.0,  0.0 },
		{  0.0, 0.0, -1.0 } };
	// 임시 입출력 메모리 준비
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// 임시 원본 이미지에 127 채우기 (평균값이 더 나을듯...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// 원본 --> 임시 원본
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// 회선 연산 --> 마스크로 긁어가면서 계산하기
	double S = 0; // 각점에 대한 마스크 연산 합계
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // 누적 초기화
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(후처리). 마스크 합계가 0이면 127정도를 더함.
	for (int i = 0; i < tarH; i++)
		for (int k = 0; k < tarW; k++)
			tmpTarImage[i][k] += 127.0;
	// 임시 타겟 --> 타겟
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;
			tarImage[i][k] = v;
		}
	}

	free_double(tmpOriImage, oriH + 2);
	free_double(tmpTarImage, tarH);
	///////////////////////////////
	displayImage();
}


void blurr() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// const int MSIZE = 3; // 마스크 크기
	// 중요! 마스크 준비
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 },
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 },
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 } };
	// 임시 입출력 메모리 준비
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// 임시 원본 이미지에 127 채우기 (평균값이 더 나을듯...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// 원본 --> 임시 원본
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// 회선 연산 --> 마스크로 긁어가면서 계산하기
	double S = 0; // 각점에 대한 마스크 연산 합계
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // 누적 초기화
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(후처리). 마스크 합계가 0이면 127정도를 더함.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// 임시 타겟 --> 타겟
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;
			tarImage[i][k] = v;
		}
	}

	free_double(tmpOriImage, oriH + 2);
	free_double(tmpTarImage, tarH);
	///////////////////////////////
	displayImage();
}

void edge1() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// const int MSIZE = 3; // 마스크 크기
	// 중요! 마스크 준비
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, 0.0,  0.0 },
		{ -1.0, 1.0,  0.0 },
		{  0.0, 0.0,  0.0 } };
	// 임시 입출력 메모리 준비
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// 임시 원본 이미지에 127 채우기 (평균값이 더 나을듯...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// 원본 --> 임시 원본
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// 회선 연산 --> 마스크로 긁어가면서 계산하기
	double S = 0; // 각점에 대한 마스크 연산 합계
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // 누적 초기화
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(후처리). 마스크 합계가 0이면 127정도를 더함.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// 임시 타겟 --> 타겟
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;
			tarImage[i][k] = v;
		}
	}

	free_double(tmpOriImage, oriH + 2);
	free_double(tmpTarImage, tarH);
	///////////////////////////////
	displayImage();
}

void edge2() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// const int MSIZE = 3; // 마스크 크기
	// 중요! 마스크 준비
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, -1.0,  0.0 },
		{  0.0,  1.0,  0.0 },
		{  0.0,  0.0,  0.0 } };
	// 임시 입출력 메모리 준비
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// 임시 원본 이미지에 127 채우기 (평균값이 더 나을듯...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// 원본 --> 임시 원본
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// 회선 연산 --> 마스크로 긁어가면서 계산하기
	double S = 0; // 각점에 대한 마스크 연산 합계
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // 누적 초기화
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(후처리). 마스크 합계가 0이면 127정도를 더함.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// 임시 타겟 --> 타겟
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;
			tarImage[i][k] = v;
		}
	}

	free_double(tmpOriImage, oriH + 2);
	free_double(tmpTarImage, tarH);
	///////////////////////////////
	displayImage();
}
void edge3() {
	freeTarImage();
	// (중요!) 결과 영상의 크기를 결정 ---> 알고리즘에 의존.
	tarH = oriH;
	tarW = oriW;
	// 결과 영상 메모리 할당
	// tarImage[tarH][tarW] 크기로 할당
	mallocTarImage();

	// *** 진짜 영상처리 알고리즘 ****
	// const int MSIZE = 3; // 마스크 크기
	// 중요! 마스크 준비
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, -1.0,  0.0 },
		{ -1.0,  2.0,  0.0 },
		{  0.0,  0.0,  0.0 } };
	// 임시 입출력 메모리 준비
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// 임시 원본 이미지에 127 채우기 (평균값이 더 나을듯...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// 원본 --> 임시 원본
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// 회선 연산 --> 마스크로 긁어가면서 계산하기
	double S = 0; // 각점에 대한 마스크 연산 합계
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // 누적 초기화
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(후처리). 마스크 합계가 0이면 127정도를 더함.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// 임시 타겟 --> 타겟

	int value;
	printf("임계값 :"); scanf("%d", &value);
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;

			// 임계값 30
			if (v < value)
				v = 0;
			else
				v = 255;

			tarImage[i][k] = v;
		}
	}

	free_double(tmpOriImage, oriH + 2);
	free_double(tmpTarImage, tarH);
	///////////////////////////////
	displayImage();
}