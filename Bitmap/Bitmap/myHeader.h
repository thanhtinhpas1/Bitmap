#include <stdio.h>
#include <conio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <malloc.h>

#pragma pack(push, 1)
struct Pix
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Bitmap
{
	// bitmap header
	short	bfType;
	long	bfSize;
	short	bfReserved1;
	short	bfReserved2;
	long	bfDataOffSet;

	// bitmap information
	long	biSize;
	long	biWidth;
	long	biHeight;
	short	biPlanes;
	short	biBitCount;
	long	biCompression;
	long	biSizeImage;
	long	biXPixelsPremeter;
	long	biYPixelsPremeter;
	long	biColorsUsed;
	long	biColorsImportant;
};
#pragma pack(pop)

// ************************ Common Function *************************
// đọc file bmp và ghi dữ liệu vào mảng 1 chiều (xem như hai chiều)
void readBMP(const char* path, Bitmap& header, char* &data);
Pix** convertDataToPixelArray(Bitmap& header, char* &data);

// ghi dữ liệu từ mảng vào file bmp									
char* convertPixelArrayToData(Bitmap& header, Pix** pixel);
void writeBMP(const char* path, Bitmap& header, char* &data);

// Cập nhất lại thông tin file Header trước khi ghi ra thành BMP
void updateHeader(Bitmap& header, int height, int width);
// *******************************************************************


// ================================================ Resize image ===========================================================

// Tăng kích thước file ảnh lên n lần
char* scaleUp(Bitmap& header, Pix** pixel, int n);

// Giảm kích thước của file ảnh
char* scaleDown(Bitmap& header, Pix** pixel, int n);
// ========================================================================================================================


// ================================================ Crop image ===========================================================

// Cắt một phần ảnh và CPATD
// CPATD: convert pixel array to data
char* cropBMP_CPATD(Bitmap& header, Pix** pixel, int startrow, int endrow, int startcol, int endcol);
// ========================================================================================================================


// ====================================== Turn image to black and white ===================================================

// Chuyển các pixel màu trong ảnh về dạng trắng đen
void toBlackAndWhite(Bitmap header, Pix** pixels);
// ========================================================================================================================

// ======================================= Change a color in the image ====================================================

// Hàm lấy màu của pixel đã chọn
Pix getPixelColor(Bitmap header, Pix** pixel);

// Hàm lấy màu mới thay màu cũ
Pix getDestColor(Bitmap header, Pix** pixel);

// Bảng mã màu (20 màu)
void colorTable(Pix* table);

// Đổi một màu của 1 pixel đã cho trước thành một màu của 1 pixel khác cho trước
char* changeOneColorToAnother(Bitmap header, Pix** pixel, Pix srcColor, Pix destColor);
// ========================================================================================================================


// ====================================== Adjust brightness and contrast ==================================================

// CPATD: convert pixel array to data

// để chắc chắn các giá trị màu mới nằm trong phạm vi cho phép (phải từ 0 -> 255)
// version integer number, dùng cho độ sáng
size_t truncate(size_t value);

// version floating point number, dùng cho độ tương phản
// Lí do: với số thực, ta sẽ tính được F (factor) với các giá trị thập phân tạo nên sự thay đổi tinh vi hơn
float trunCate(float value);

// điều chỉnh ánh sáng
char* adjustBrightnessCPATD(Bitmap header, Pix** pixel, int brightness);

// điều chỉnh độ tương phản
char* adjustContrastCPATD(Bitmap header, Pix** pixel, int contrast);
//========================================================================================================================


// ================================================ Rotate image ===========================================================

// CPATD: convert pixel array to data
// Lưu ý: vì các byte pixel lưu ngược lại nên thuật toán tưởng như sẽ quay sang phải thì nó sẽ quay sang trái và ngược lại

// đảo file từ trái sang phải 1 góc 90 độ <=> quay từ phải sang trái 1 góc 270 độ
char* rotate90DegreeClockwise_CPATD(Bitmap& header, Pix** pixel);

// đảo file từ phải sang trái 1 góc 90 độ <=> quay từ trái sang phải 1 góc 270 độ
char* rotate90DegreeCounterClockwise_CPATD(Bitmap& header, Pix** pixel);

// đảo file 180 do
char* rotateImageUpSideDown_CPATD(Bitmap header, Pix** pixel);

// Bonus: Flip image vertically
char* flipImageVert(Bitmap& header, Pix** pixel);

// Bonus: File image Horizontally
char* flipImageHor(Bitmap& header, Pix** pixel);
//========================================================================================================================
