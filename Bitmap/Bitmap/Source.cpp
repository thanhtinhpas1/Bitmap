#include "myHeader.h"

// *************************Common function*****************************
void readBMP(const char* path, Bitmap& header, char* &data)
{
	FILE* fp = fopen(path, "rb");
	if (fp != NULL)
	{
		fread(&header, sizeof(header), 1, fp);
		if (header.bfType != 0x4D42)
		{
			printf("Day khong phai file bitmap\n");
			fclose(fp);
			return;
		}
		int padding = header.biWidth % 4;
		int size = header.biWidth * header.biHeight * (header.biBitCount/8) + padding * header.biHeight;
		data = new char[size];
		fread(data, sizeof(char), size, fp);
	}
	else
		printf("Khong doc duoc file\n");
	fclose(fp);
}

Pix** convertDataToPixelArray(Bitmap& header, char* &data)
{
	int i, j;
	int size = header.biWidth * header.biHeight;
	Pix** pixel = new Pix*[header.biHeight];
	for (i = 0; i < header.biHeight; i++)
	{
		pixel[i] = new Pix[header.biWidth];
	}

	int padding = header.biWidth % 4;
	char* buffer = data;

	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			pixel[i][j].B = *buffer;	buffer++;
			pixel[i][j].G = *buffer;	buffer++;
			pixel[i][j].R = *buffer;	buffer++;
		}
		buffer = buffer + padding;
	}
	return pixel;
}

char* convertPixelArrayToData(Bitmap& header, Pix** pixel)
{
	int padding = header.biWidth % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	return data;
}

void writeBMP(const char* path, Bitmap& header, char* &data)
{
	FILE* fp = fopen(path, "wb");
	if (fp != NULL)
	{
		fwrite(&header, sizeof(header), 1, fp);
		int padding = header.biWidth % 4;
		int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
		fwrite(data, sizeof(char), size, fp);
	}
	else
		printf("Khong ghi duoc file\n");
	fclose(fp);
}

// Cập nhật lại Header của ảnh mới
void updateHeader(Bitmap& header, int height, int width)
{
	int headersize = sizeof(header); // 54 bytes
	int padding = (4 - (width * 3) % 4) %4; // số 3 là 3 màu RGB

	header.bfType = 0x4D42;					//
	header.bfSize = (width * height) * 3 + headersize;
	header.bfReserved1 = 0;					//			
	header.bfReserved2 = 0;					//
	header.bfDataOffSet = headersize;		//

	header.biSize = 40;						//
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;					//
	header.biBitCount = header.biBitCount;	//
	header.biCompression = 0;				//
	header.biSizeImage = (width * 3 + padding) * height;
	header.biXPixelsPremeter = 0;			//
	header.biYPixelsPremeter = 0;			//
	header.biColorsUsed = 0;				//
	header.biColorsImportant = 0;			//

	// những dòng có dấu "//" là không cần thiết, ghi vào như thế cho đầy đủ thôi
}
// *********************************************************************

// ================================================ Resize image ===========================================================

// Tăng kích thước file ảnh lên n lần
char* scaleUp(Bitmap& header, Pix** pixel, int n)
{
	// convert pixel array to data
	// EX: 1 2 3 4		n=3		1 1 1  2 2 2  3 3 3 
	//	   5 6 ...	   ---->	1 1 1  2 2 2  3 3 3 ...
	//	   ...					1 1 1  2 2 2  3 3 3
	//							
	//							5 5 5  6 6 6 ...
	
	int new_Height = header.biHeight * n;
	int new_Width = header.biWidth * n;
	int padding = new_Width % 4;
	int size = (new_Height * new_Width) * (header.biBitCount / 8) + (padding * new_Height);
	char* data = new char[size];

	int i, j, k, u, v;
	char* buffer = data;
	for (i = 0; i < header.biHeight; i++)
	{
		u = 0;
		while (u < n)
		{
			for (j = 0; j < header.biWidth; j++)
			{
				v = 0;
				while (v < n)
				{
					*buffer = pixel[i][j].B;	buffer++;
					*buffer = pixel[i][j].G;	buffer++;
					*buffer = pixel[i][j].R;	buffer++;
					v++;
				}
			}
			
			for (k = 0; k < padding; k++)
			{
				*buffer = 0;
				buffer++;
			}
			u++;
		}
	}

	// cập nhật header
	updateHeader(header, new_Height, new_Width);

	return data;
}

// Giảm kích thước của file ảnh    ---   Requirement not achieved: Can not compress pixels
char* scaleDown(Bitmap& header, Pix** pixel, int n)
{
	// convert pixel array to data
	int new_Height = header.biHeight / n;
	int new_Width = header.biWidth / n;

	int padding = header.biWidth % 4;
	// chỉnh lại padding
	padding = new_Width % 4;
	int size = new_Height * new_Width * (header.biBitCount / 8) + (padding * new_Height);
	
	char* data = new char[size];
	char* buffer = data;
	Pix temp;

	// giới hạn dòng: chiều cao - số dư của chiều cao - n
	// giới hạn cột: chiều rộng - số dư của chiều rộng - n
	// dùng dấu "<="
	// VD: hình 378 x 421. Có 421 cột, khi giảm 2 lần sẽ còn 240, tức là phải lấy 240 cột xen kẽ đó
	// nhưng nếu cho j chạy từ 0 -> 421 với j = j + n thì ta đã lấy cột 0 -> 420, tức là 241 cột
	// để lấy đúng 240 cột thì j chỉ chạy từ 0 -> 418
	int lim_h = header.biHeight - (header.biHeight % n) - n;
	int lim_w = header.biWidth - (header.biWidth % n) - n;
	int i, j, k;
	for (i = 0; i <= lim_h; i = i + n)
	{
		for (j = 0; j <= lim_w; j = j + n)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}

	// cập nhật header
	updateHeader(header, new_Height, new_Width);

	return data;
}
// =========================================================================================================================


// ================================================ Crop image ===========================================================

// Cắt một phần ảnh và CPATD
// CPATD: convert pixel array to data
char* cropBMP_CPATD(Bitmap& header, Pix** pixel, int startrow, int endrow, int startcol, int endcol)
{
	int height = endrow - (startrow-1);
	int width = endcol - (startcol-1);
	int padding = width % 4;
	int size = height * width * (header.biBitCount / 8) + (padding * height);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = startrow-1; i < endrow; i++)
	{
		for (j = startcol-1; j < endcol; j++)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	// cập nhật lại chiều cao và chiều rộng mới của ảnh
	updateHeader(header, height, width);
	return data;
}
// ========================================================================================================================


// ====================================== Turn image to black and white ===================================================

// Chuyển các pixel màu trong ảnh về dạng trắng đen
void toBlackAndWhite(Bitmap header, Pix** pixels)
{
	int i, j, value;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			value = (pixels[i][j].R + pixels[i][j].G + pixels[i][j].B) / 3;
			pixels[i][j].R = pixels[i][j].G = pixels[i][j].B = value;
		}
	}
}
// ========================================================================================================================


// ======================================= Change a color in the image ====================================================

// Hàm chọn lấy màu của pixel để đổi
Pix getPixelColor(Bitmap header, Pix** pixel)
{
	int i, j;
	printf("Nhap toa do pixel de lay mau ban muon doi:\n");
	do {
		printf("Nhap hoanh do (0 -> %d): ", header.biWidth);	scanf_s("%d", &j);
		printf("Nhap tung do (0 -> %d): ", header.biHeight);	scanf_s("%d", &i);
		if (i < 0 || i > header.biHeight || j < 0 || j > header.biWidth)
			printf("\nThong so ban nhap khong hop le! Xin vui long nhap lai\n");
	} while (i < 0 || i > header.biHeight || j < 0 || j > header.biWidth);
	
	return pixel[i][j];
}

// Hàm lấy màu mới thay màu cũ
Pix getDestColor(Bitmap header, Pix** pixel)
{
	int i, j;
	printf("Nhap toa do pixel de lay mau moi:\n");
	do {
		printf("Nhap hoanh do (0 -> %d): ", header.biWidth);	scanf_s("%d", &j);
		printf("Nhap tung do (0 -> %d): ", header.biHeight);	scanf_s("%d", &i);
		if (i < 0 || i > header.biHeight || j < 0 || j > header.biWidth)
			printf("\nThong so ban nhap khong hop le! Xin vui long nhap lai\n");
	} while (i < 0 || i > header.biHeight || j < 0 || j > header.biWidth);

	return pixel[i][j];
}

// Bảng mã màu (20 màu)
void colorTable(Pix* table)
{
	// 01: black
	table[0].R = table[0].G = table[0].B = 0;
	// 02: gray-50%
	table[1].R = table[1].G = table[1].B = 190;
	// 03: dark-red
	table[2].R = 139;
	table[2].G = table[2].B = 0;
	// 04: red
	table[3].R = 255;
	table[3].G = table[3].B = 0;
	// 05: orange
	table[4].R = 255;
	table[4].G = 165;
	table[4].B = 0;
	// 06: yellow
	table[5].R = table[5].G = 255;
	table[5].B = 0;
	// 07: green
	table[6].R = 0;
	table[6].G = 255;
	table[6].B = 0;
	// 08: turquoise
	table[7].R = 64;
	table[7].G = 224;
	table[7].B = 208;
	// 09: blue/indigo
	table[8].R = table[8].G = 0;
	table[8].B = 255;
	// 10: purple
	table[9].R = 128;
	table[9].G = 0;
	table[9].B = 128;
	// 11: white
	table[10].B = table[10].G = table[10].R = 255;
	// 12: gray-25%/light-gray
	table[11].R = table[11].G = table[11].B = 211;
	// 13: brown
	table[12].R = 139;
	table[12].G = 69;
	table[12].B = 19;
	// 14: mistyRose/pink
	table[13].R = 255;
	table[13].G = 228;
	table[13].B = 225;
	// 15: gold
	table[14].R = 255;
	table[14].G = 215;
	table[14].B = 0;
	// 16: light-yellow
	table[15].R = 255;
	table[15].G = 255;
	table[15].B = 224;
	// 17: lime
	table[16].R = 50;
	table[16].G = 205;
	table[16].B = 50;
	// 18: paleTurquoise
	table[17].R = 175;
	table[17].G = 238;
	table[17].B = 238;
	// 19: steelBlue/blue-gray
	table[18].R = 70;
	table[18].G = 130;
	table[18].B = 180;
	// 20: lavender
	table[19].R = 230;
	table[19].G = 230;
	table[19].B = 250;
}

// Đổi một màu của 1 pixel đã cho trước thành một màu của 1 pixel khác cho trước
char* changeOneColorToAnother(Bitmap header, Pix** pixel, Pix srcColor, Pix destColor)
{
	int padding = header.biWidth % 4;
	int size = header.biWidth * header.biHeight * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			if (pixel[i][j].B == srcColor.B && 
				pixel[i][j].G == srcColor.G &&
				pixel[i][j].R == srcColor.R)
			{
				*(buffer++) = destColor.B;
				*(buffer++) = destColor.G;
				*(buffer++) = destColor.R;
			}
			else
			{
				*(buffer++) = pixel[i][j].B;
				*(buffer++) = pixel[i][j].G;
				*(buffer++) = pixel[i][j].R;
			}
		}

		for (k = 0; k < padding; k++)
		{
			*(buffer++) = 0;
		}
	}

	return data;
}

// ========================================================================================================================


// ====================================== Adjust brightness and contrast ==================================================

// để chắc chắn các giá trị màu mới nằm trong phạm vi cho phép (phải từ 0 -> 255)
// version integer number, dùng cho độ sáng
size_t truncate(size_t value)
{
	// size_t <=> unsigned int
    if(value < 0) 
		value = 0;
    if(value > 255) 
		value = 255;
    return value;
}

// version floating point number, dùng cho độ tương phản
float trunCate(float value)
{
	// size_t <=> unsigned int
    if(value < 0) 
		value = 0;
    if(value > 255) 
		value = 255;
    return value;
}

// điều chỉnh ánh sáng
// CPATD: convert pixel array to data
char* adjustBrightnessCPATD(Bitmap header, Pix** pixel, int brightness)
{
	int padding = header.biWidth % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			*buffer = truncate(pixel[i][j].B + brightness);	buffer++;
			*buffer = truncate(pixel[i][j].G + brightness);	buffer++;
			*buffer = truncate(pixel[i][j].R + brightness);	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	
	// Do kích cỡ không có thay đổi gì, thay đổi màu sắc không ảnh hưởng tới cấu trúc bitmap nên không cần update

	return data;
}

// điều chỉnh độ tương phản
// CPATD: convert pixel array to data
char* adjustContrastCPATD(Bitmap header, Pix** pixel, int contrast)
{
	int padding = header.biWidth % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
	//				259 x (C + 255)
	//	factor: F = ---------------				<C: contrast>
	//				255 x (259 - C)
	float factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

	int i, j, k;
	char* data = new char[size];
	char* buffer = data;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			// xét màu xanh (B)
			// new_B = F x (B - 128) + 128
			// rồi dùng hàm truncate để xác định lại giá trị
			*buffer = trunCate(factor * (pixel[i][j].B - 128) + 128);	buffer++;
			*buffer = trunCate(factor * (pixel[i][j].G - 128) + 128);	buffer++;
			*buffer = trunCate(factor * (pixel[i][j].R - 128) + 128);	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	return data;
}
// ========================================================================================================================


// ================================================ Rotate image ===========================================================

// đảo file từ trái sang phải 1 góc 90 độ <=> quay từ phải sang trái 1 góc 270 độ - sai
char* rotate90DegreeClockwise_CPATD(Bitmap& header, Pix** pixel)
{
	// pixel nó cần ngược lại với cái ma trận mình nghĩ
	// chạy cũng thứ tự trái sang phải nhưng lại từ dưới lên
	// 8  9  10 11						0  4  8
	// 4  5  6  7		Và ta cần		1  5  9
	// 0  1  2  3						2  6  10
	//									3  7  11
	int padding = header.biHeight % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biWidth);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = header.biWidth - 1; i > -1; i--)
	{
		for (j = header.biHeight - 1; j > -1; j--)
		{
			*(buffer++) = pixel[header.biHeight - 1 - j][i].B;
			*(buffer++) = pixel[header.biHeight - 1 - j][i].G;
			*(buffer++) = pixel[header.biHeight - 1 - j][i].R;
		}
		for (k = 0; k < padding; k++)
		{
			*(buffer++) = 0;
		}
	}
	
	// chiều cao thành chiều rộng và ngược lại
	updateHeader(header, header.biWidth, header.biHeight);

	return data;
}

// đảo file từ phải sang trái 1 góc 90 độ <=> quay từ trái sang phải 1 góc 270 độ
char* rotate90DegreeCounterClockwise_CPATD(Bitmap& header, Pix** pixel)
{
	// pixel nó cần ngược lại với cái ma trận mình nghĩ
	// chạy cũng thứ tự trái sang phải nhưng lại từ dưới lên
	// và theo chiều ngược chiều kim đồng hồ thì là ngược lại của hàm theo chiều kim đồng hồ
	// có thể viết theo dưới đây hoặc theo hàm trên và đổi các cột với nhau
	// 8  9  10 11						8  4  0
	// 4  5  6  7		Và ta cần		9  5  1
	// 0  1  2  3						10 6  2
	//									11 7  3
	int padding = header.biHeight % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biWidth);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = 0; i < header.biWidth; i++)
	{
		for (j = 0; j < header.biHeight; j++)
		{
			*buffer = pixel[header.biHeight - 1 - j][i].B;	buffer++;
			*buffer = pixel[header.biHeight - 1 - j][i].G;	buffer++;
			*buffer = pixel[header.biHeight - 1 - j][i].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	
	// chiều cao thành chiều rộng và ngược lại
	updateHeader(header, header.biWidth, header.biHeight);

	return data;
}

// đảo file 180 do
char* rotateImageUpSideDown_CPATD(Bitmap header, Pix** pixel)
{
	// đảo 180 d0 <=> đảo ngược ảnh
	// => Lấy ảnh ngược từ dưới lên, ngược cả về dòng lẫn cột

	// Pixel nó lưu theo kiểu:	8  9  10 11					3  2  1  0
	//							4  5  6  7		Ta cần		7  6  5  4
	//							0  1  2  3					11 10 9  8

	int padding = header.biWidth % 4;
	int size = header.biWidth * header.biHeight * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = header.biHeight-1; i > -1; i--)
	{
		for (j = header.biWidth-1; j > -1; j--)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}

	// không thay đổi chiều cao, độ rộng nên không cần update header
	return data;
}

// Bonus: File image Vertically
char* flipImageVert(Bitmap& header, Pix** pixel)
{
	int padding = header.biWidth % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = header.biHeight -1; i > -1 ; i--)
	{
		for (j = 0; j < header.biWidth; j++)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}

	return data;
}

// Bonus: Flip image Horizontally
char* flipImageHor(Bitmap& header, Pix** pixel)
{
	int padding = header.biWidth % 4;
	int size = header.biHeight * header.biWidth * (header.biBitCount / 8) + (padding * header.biHeight);
	char* data = new char[size];
	char* buffer = data;

	int i, j, k;
	for (i = 0; i < header.biHeight; i++)
	{
		for (j = header.biWidth-1; j > -1; j--)
		{
			*buffer = pixel[i][j].B;	buffer++;
			*buffer = pixel[i][j].G;	buffer++;
			*buffer = pixel[i][j].R;	buffer++;
		}
		for (k = 0; k < padding; k++)
		{
			*buffer = 0;
			buffer++;
		}
	}
	// chiều cao, chiều rộng ko thay đổi nên ko cần update

	return data;
}
// ========================================================================================================================
