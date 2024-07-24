#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h> // GUI ��� ����
#include <math.h>

///////////////////
// ���� ���� �����
///////////////////
HWND  hwnd; //  ���� ȭ��(�ܺ�, ������)
HDC hdc; // ���� ȭ��(����, ����)

int oriH, oriW, tarH, tarW;
unsigned char** oriImage = NULL;
unsigned char** tarImage = NULL;

char filename[200];  // ���ϸ�

///////////////////
// �Լ� �����
///////////////////
// ���� �Լ���
void loadImage(); void saveImage(); void displayImage(); void printMenu();
void mallocOriImage(); void mallocTarImage(); void freeOriImage(); void freeTarImage();
// ����ó�� �Լ���
void equalImage();
void addImage();
void reverseImage();
void zoomOut(); void zoomIn(); void zoomIn2(); void rotate(); void rotate2();
void histoStretch(); void endIn(); void histoEqual();
void emboss();  void blurr(); void edge1(); void edge2(); void edge3();

void printMenu() {
	puts("\n GrayScale Image Processing");
	puts("\n 1.����  2.����  9.����");
	puts("\n A.���Ͽ���  B.���/��Ӱ�  C.����  D.���  E.Ȯ��(������)  F.Ȯ��(�����)");
	puts("\n G.ȸ�� H.ȸ��(����) I.������׷� ��Ʈ��Ī J. ����-�� K.��Ȱȭ");
	puts("\n L.������ M.���� N.�������� O.������ P.����+����(�Ӱ谪)");
}
///////////////////
// ���� �ڵ��
///////////////////
void main() {
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);  // ����10��.  ����11(hwnd--> NULL)

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
	puts("�ȳ��� ������~~ ����� ��������...");


}

////////////////////
/// �Լ� ���Ǻ�
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
	printf("���ϸ�-->"); scanf("%s", tmpName);  // cat01, dog05, etc11
	strcat(fullname, tmpName);
	strcat(fullname, ".raw");
	strcpy(filename, fullname);

	//(�߿�!) ���������� ũ�⸦ �ľ�!
	FILE* rfp;
	rfp = fopen(filename, "rb");
	if (!rfp) {
		printf("���ϸ� Ʋ����~ \n");
		return;
	}
	fseek(rfp, 0L, SEEK_END); // ������ ������ �̵�
	long long fsize = ftell(rfp);
	fclose(rfp);
	// ���� ���� ũ��
	oriH = oriW = sqrt(fsize);
	// 2���� �޸� �Ҵ�
	freeOriImage();
	mallocOriImage();

	// ���� --> �޸�
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
	printf("���� ���ϸ�-->"); scanf("%s", tmpName);  // out01, out02 ....
	strcat(fullname, tmpName);
	strcat(fullname, "_512.raw");
	strcpy(filename, fullname);

	FILE* wfp;
	wfp = fopen(filename, "wb");
	// �޸� >> ����
	for (int i = 0; i < tarH; i++) {
		fwrite(tarImage[i], sizeof(unsigned char), tarW, wfp);
	}
	fclose(wfp);
	printf("%s �� �����", filename);
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
///  ����ó�� �Լ���
/// </summary>

void equalImage() {
	freeTarImage();
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	int value;
	printf("���� ��(-255~+255) :");  scanf("%d", &value);
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

void reverseImage() {  // ���� : 0>255, 1>254, 2>253..... 254>1, 255>0
	freeTarImage();
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	int scale;
	printf("��� ����:"); scanf("%d", &scale);
	tarH = (int)(oriH / scale);
	tarW = (int)(oriW / scale);
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	int scale;
	printf("Ȯ�� ����:"); scanf("%d", &scale);
	tarH = (int)(oriH * scale);
	tarW = (int)(oriW * scale);
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�	
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	int scale;
	printf("Ȯ�� ����:"); scanf("%d", &scale);
	tarH = (int)(oriH * scale);
	tarW = (int)(oriW * scale);
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�	
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	int angle;
	printf("ȸ�� ����: "); scanf("%d", &angle);
	// ȸ�� ����
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�

	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	int angle;
	printf("ȸ�� ����: "); scanf("%d", &angle);
	// ȸ�� ����
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	freeTarImage();
	mallocTarImage();

	// ���� :  new = (old - low) / (high - low) * 255.0
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
	// *** ��¥ ����ó�� �˰��� ****
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			tarImage[i][k] = ((double)oriImage[i][k] - low) / (high - low) * 255;
		}
	}
	///////////////////////////////
	displayImage();
}


void endIn() {
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	freeTarImage();
	mallocTarImage();

	// ���� :  new = (old - low) / (high - low) * 255.0
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
	// *** ��¥ ����ó�� �˰��� ****
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�	
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// 1�ܰ� : �󵵼� ������׷� ����
	int hist[256] = { 0, };
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			hist[oriImage[i][k]]++;
	// 2�ܰ� : ���� ������׷� ����
	int sumHist[256] = { 0, };
	sumHist[0] = hist[0];
	for (int i = 1; i < 256; i++)
		sumHist[i] = sumHist[i - 1] + hist[i];
	// 3�ܰ� : ����ȭ�� ������׷� ����
	// ���� : normalHist = sumHist * (1/(oriH*oriW)) * 255.0;
	double normalHist[256] = { 0.0, };
	for (int i = 0; i < 256; i++)
		normalHist[i] = sumHist[i] * (1.0 / (oriH * oriW)) * 255.0;

	// 4�ܰ� : �������� ����ȭ�� ������ ġȯ
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// const int MSIZE = 3; // ����ũ ũ��
	// �߿�! ����ũ �غ�
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  1.0, 0.0,  0.0 },
		{  0.0, 0.0,  0.0 },
		{  0.0, 0.0, -1.0 } };
	// �ӽ� ����� �޸� �غ�
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// �ӽ� ���� �̹����� 127 ä��� (��հ��� �� ������...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// ���� --> �ӽ� ����
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// ȸ�� ���� --> ����ũ�� �ܾ�鼭 ����ϱ�
	double S = 0; // ������ ���� ����ũ ���� �հ�
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // ���� �ʱ�ȭ
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(��ó��). ����ũ �հ谡 0�̸� 127������ ����.
	for (int i = 0; i < tarH; i++)
		for (int k = 0; k < tarW; k++)
			tmpTarImage[i][k] += 127.0;
	// �ӽ� Ÿ�� --> Ÿ��
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// const int MSIZE = 3; // ����ũ ũ��
	// �߿�! ����ũ �غ�
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 },
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 },
		{  1.0 / 9, 1.0 / 9, 1.0 / 9 } };
	// �ӽ� ����� �޸� �غ�
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// �ӽ� ���� �̹����� 127 ä��� (��հ��� �� ������...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// ���� --> �ӽ� ����
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// ȸ�� ���� --> ����ũ�� �ܾ�鼭 ����ϱ�
	double S = 0; // ������ ���� ����ũ ���� �հ�
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // ���� �ʱ�ȭ
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(��ó��). ����ũ �հ谡 0�̸� 127������ ����.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// �ӽ� Ÿ�� --> Ÿ��
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// const int MSIZE = 3; // ����ũ ũ��
	// �߿�! ����ũ �غ�
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, 0.0,  0.0 },
		{ -1.0, 1.0,  0.0 },
		{  0.0, 0.0,  0.0 } };
	// �ӽ� ����� �޸� �غ�
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// �ӽ� ���� �̹����� 127 ä��� (��հ��� �� ������...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// ���� --> �ӽ� ����
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// ȸ�� ���� --> ����ũ�� �ܾ�鼭 ����ϱ�
	double S = 0; // ������ ���� ����ũ ���� �հ�
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // ���� �ʱ�ȭ
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(��ó��). ����ũ �հ谡 0�̸� 127������ ����.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// �ӽ� Ÿ�� --> Ÿ��
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// const int MSIZE = 3; // ����ũ ũ��
	// �߿�! ����ũ �غ�
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, -1.0,  0.0 },
		{  0.0,  1.0,  0.0 },
		{  0.0,  0.0,  0.0 } };
	// �ӽ� ����� �޸� �غ�
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// �ӽ� ���� �̹����� 127 ä��� (��հ��� �� ������...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// ���� --> �ӽ� ����
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// ȸ�� ���� --> ����ũ�� �ܾ�鼭 ����ϱ�
	double S = 0; // ������ ���� ����ũ ���� �հ�
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // ���� �ʱ�ȭ
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(��ó��). ����ũ �հ谡 0�̸� 127������ ����.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// �ӽ� Ÿ�� --> Ÿ��
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
	// (�߿�!) ��� ������ ũ�⸦ ���� ---> �˰��� ����.
	tarH = oriH;
	tarW = oriW;
	// ��� ���� �޸� �Ҵ�
	// tarImage[tarH][tarW] ũ��� �Ҵ�
	mallocTarImage();

	// *** ��¥ ����ó�� �˰��� ****
	// const int MSIZE = 3; // ����ũ ũ��
	// �߿�! ����ũ �غ�
#define MSIZE 3   
	double mask[MSIZE][MSIZE] = {
		{  0.0, -1.0,  0.0 },
		{ -1.0,  2.0,  0.0 },
		{  0.0,  0.0,  0.0 } };
	// �ӽ� ����� �޸� �غ�
	double** tmpOriImage, ** tmpTarImage;
	tmpOriImage = malloc_double(oriH + 2, oriW + 2);
	tmpTarImage = malloc_double(tarH, tarW);
	// �ӽ� ���� �̹����� 127 ä��� (��հ��� �� ������...)
	for (int i = 0; i < oriH + 2; i++)
		for (int k = 0; k < oriW + 2; k++)
			tmpOriImage[i][k] = 127.0;
	// ���� --> �ӽ� ����
	for (int i = 0; i < oriH; i++)
		for (int k = 0; k < oriW; k++)
			tmpOriImage[i + 1][k + 1] = oriImage[i][k];
	// ȸ�� ���� --> ����ũ�� �ܾ�鼭 ����ϱ�
	double S = 0; // ������ ���� ����ũ ���� �հ�
	for (int i = 0; i < oriH; i++) {
		for (int k = 0; k < oriW; k++) {
			S = 0.0; // ���� �ʱ�ȭ
			for (int m = 0; m < MSIZE; m++)
				for (int n = 0; n < MSIZE; n++)
					S += mask[m][n] * tmpOriImage[i + m][k + n];
			tmpTarImage[i][k] = S;
		}
	}
	// Post Processing(��ó��). ����ũ �հ谡 0�̸� 127������ ����.
	//for (int i = 0; i < tarH; i++)
	//	for (int k = 0; k < tarW; k++)
	//		tmpTarImage[i][k] += 127.0;
	// �ӽ� Ÿ�� --> Ÿ��

	int value;
	printf("�Ӱ谪 :"); scanf("%d", &value);
	for (int i = 0; i < tarH; i++) {
		for (int k = 0; k < tarW; k++) {
			double v = tmpTarImage[i][k];
			if (v < 0.0)
				v = 0.0;
			if (v > 255.0)
				v = 255.0;

			// �Ӱ谪 30
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