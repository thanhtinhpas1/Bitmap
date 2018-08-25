#include "myHeader.h"

// Lưu ý: chương trình chỉ áp dụng với ảnh .bmp 24 bit màu //

#define BMP_in "logo.bmp"
#define BMP_out "logo_2.bmp"

int main(int argc, char *argv[])
{
	Bitmap header;
	int i;
	char* data = NULL;

	readBMP(BMP_in, header, data);
	Pix** pixel = convertDataToPixelArray(header, data);

	delete []data;
	data = NULL; // để đảm bảo an toàn

	printf("---------Available options---------\n");
	printf("01: Chinh sua kich thuoc anh\n02: Crop anh\n03: Chuyen anh thanh anh trang den\n");
	printf("04: Chuyen mot mau trong anh thanh mau khac\n05: Chinh sua do sang, do tuong phan\n");
	printf("06: Xoay anh 90 do, 180 do, 270 do\n");
	
	int z;
	printf("Nhap thao tac ban muon: ");
	scanf_s("%d", &z);

	switch(z)
	{
	case 1:
		{
			printf("\n->>Ban da chon resize anh\n");
			int k, n;
			printf("01: Thu nho anh\n02: Phong to anh\nNhap lua chon cua ban: ");
			scanf_s("%d", &k);
			switch (k)
			{
			case 1:
				{
					printf("\n->>->>Ban da chon thu nho anh\n");
					printf("Nhap so lan thu nho: ");
					scanf_s("%d", &n);
					data = scaleDown(header, pixel, n);
					writeBMP(BMP_out,header,data);
					printf("THU NHO HOAN TAT!\n");
					break;
				}
			case 2:
				{
					printf("\n->>->>Ban da chon phong to anh\n");
					printf("Nhap so lan phong to: ");
					scanf_s("%d", &n);
					data = scaleUp(header, pixel, n);
					writeBMP(BMP_out,header,data);
					printf("PHONG TO HOAN TAT!\n");
					break;
				}
			}
			break;
		}
	case 2:
		{
			printf("\n->>Ban da chon crop anh\n");
			int startrow, endrow, startcol, endcol;
			do {
				printf("Nhap thong tin ve chieu cao (1 -> %d)\n", header.biHeight);
				printf("Nhap vi tri bat dau: ");
				scanf_s("%d", &startrow);
				printf("Nhap vi tri ket thuc: ");
				scanf_s("%d", &endrow);
				printf("Nhap thong tin ve chieu rong (1 -> %d)\n", header.biWidth);
				printf("Nhap vi tri bat dau: ");
				scanf_s("%d", &startcol);
				printf("Nhap vi tri ket thuc: ");
				scanf_s("%d", &endcol);
				if (startcol < 1 || startcol > header.biWidth || endcol < 1 || endcol > header.biWidth ||
					 startrow < 1 || startrow > header.biHeight|| endrow < 1 || endrow > header.biHeight)
				{
					printf("Thong so khong hop le! Xin ban vui long nhap lai\n\n");
				}
			} while (startcol < 1 || startcol > header.biWidth || endcol < 1 || endcol > header.biWidth ||
					 startrow < 1 || startrow > header.biHeight|| endrow < 1 || endrow > header.biHeight);
			data = cropBMP_CPATD(header,pixel,startrow,endrow,startcol,endcol);
			writeBMP(BMP_out, header, data);
			printf("CAT ANH HOAN TAT!\n"); break;
		}
	case 3:
		{
			printf("\n->>Ban da chon chuyen anh thanh anh trang den\n");
			toBlackAndWhite(header,pixel);
			data = convertPixelArrayToData(header,pixel);
			//renderImage(header,data);
			writeBMP(BMP_out,header,data);
			printf("CHUYEN DOI HOAN TAT\n");
			break;
		}
	case 4:
		{
			printf("\n->>Ban da chon doi mau anh\n");
			int k, n;
			printf("01: Chon mot mau tu mot pixel khac trong anh de doi\n");
			printf("02: Chon mot mau tu bang mau co ban (khuyen khich)\nNhap lua chon cua ban: ");
			scanf_s("%d", &k);
			switch(k)
			{
			case 1:
				{
					printf("\n->>->>Ban da chon cach dung mot mau tu mot pixel khac trong anh de doi\n");
					Pix src = getPixelColor(header, pixel);
					Pix dest = getDestColor(header, pixel);
					data = changeOneColorToAnother(header,pixel,src,dest);
					writeBMP(BMP_out,header,data);
					break;
				}
			case 2:
				{
					printf("\n->>->>Ban da chon cach dung mot mau trong bang mau tao san de doi\n");
					Pix* colorTab = new Pix[20];
					colorTable(colorTab);
					int n;
					Pix src = getPixelColor(header, pixel);
					printf("--Cac mau hien co san--\n");
					printf("01: black\t02: gray-50%\t03: dark-red\t 04: red\t05: orange\n");
					printf("06: yellow\t07: green\t08: turquoise\t09: blue/indigo\t10: purple\n");
					printf("11: white\t12: gray-25%/light-gray\t13: brown\t14: mistyRose/pink\t15: gold\n");
					printf("16: light-yellow\t17: lime\t18: paleTurquoise\t19: steelBlue/blue-gray\t20: lavender\n");
					printf("Nhap mau ma ban muon: ");
					scanf_s("%d", &n);
					data = changeOneColorToAnother(header,pixel,src,colorTab[n-1]);
					writeBMP(BMP_out,header,data);
					break;
				}
			}
		break;
		}
	case 5:
		{
			int brightness, contrast, k;
			printf("\n->>Ban da chon chinh do sang va do tuong phan cua anh\n");
			printf("01: Dieu chinh do sang\n02: Dieu chinh do tuong phan\nNhap lua chon cua ban: ");
			scanf_s("%d", &k);
			switch(k)
			{
			case 1:
				{
					printf("\n->>->>Ban da chon dieu chinh do sang\n");
					printf("Nhap muc do anh sang (-255 -> 255): ");
					scanf_s("%d", &brightness);
					data = adjustBrightnessCPATD(header,pixel,brightness);
					writeBMP(BMP_out,header,data);
					printf("DIEU CHINH ANH SANG HOAN TAT!\n");
					break;
				}
			case 2:
				{
					printf("\n->>->>Ban da chon dieu chinh do tuong phan\n");
					printf("Nhap muc do tuong phan (-255 -> 255)\nLuu y: Gia tri am se cho ra anh toan mau xam): ");
					scanf_s("%d", &contrast);
					data = adjustContrastCPATD(header,pixel,contrast);
					writeBMP(BMP_out,header,data);
					printf("DIEU CHINH DO TUONG PHAN HOAN TAT!\n");
					break;
				}
			}
			break;
		}
	case 6:
		{
			int k;
			printf("\n->>Ban da chon xoay anh\n");
			printf("01: 90 do theo chieu kim dong ho\n02: 180 do (dao nguoc anh)\n");
			printf("03: 270 do theo chieu kim dong ho\n04: Lat anh theo cot\n05: Lat anh theo dong\n");
			printf("Nhap lua chon cua ban: ");
			scanf_s("%d", &k);
			switch(k)
			{
			case 1:
				{
					printf("\n->>->>Ban da chon xoay anh 90 do tu trai sang phai hoac 270 do chieu nguoc lai\n");
					data = rotate90DegreeClockwise_CPATD(header,pixel);
					writeBMP(BMP_out,header,data); 
					printf("XOAY ANH HOAN TAT\n"); break;
				}
			case 2:
				{
					printf("\n->>->>Ban da chon xoay anh 180 do\n");
					data = rotateImageUpSideDown_CPATD(header,pixel);
					writeBMP(BMP_out,header,data); 
					printf("XOAY ANH HOAN TAT\n"); break;
				}
			case 3:
				{
					printf("\n->>->>Ban da chon xoay anh 90 do tu phai sang trai hoac 270 do chieu nguoc lai\n");
					data = rotate90DegreeCounterClockwise_CPATD(header,pixel);
					writeBMP(BMP_out,header,data); 
					printf("XOAY ANH HOAN TAT\n"); break;
				}
			case 4:
				{
					printf("\n->>->>Ban da chon lat anh theo cot (vertically) [Easter egg 1/2]\n");
					data = flipImageVert(header,pixel);
					writeBMP(BMP_out,header,data);
					printf("LAT ANH HOAN TAT\n"); break;
				}
			case 5:
				{
					printf("\n->>->>Ban da chon lat anh theo dong (horizontally) [Easter egg 2/2]\n");
					data = flipImageHor(header,pixel);
					writeBMP(BMP_out,header,data);
					printf("LAT ANH HOAN TAT\n"); break;
				}
			}
			break;
		}
	}

	delete []data;
	data = NULL;
	delete []pixel;
	pixel = NULL;

	_getch();
	return 0;
}