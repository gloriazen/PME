#include "crop_img_glob.h"

using namespace std;
using namespace cv;

crop_img_glob::crop_img_glob(Mat img_, string pid_) : img(img_), pid(pid_) { }


cv::Mat crop_img_glob::crop() {
	Mat img_crop_, img_crop;
	load_param p;
	string sx;
	stringstream s; s << p.path_affine << "matrix_" << pid << ".txt";
	sx = s.str();
	ifstream tform_(sx);
	int x, y; double tform[3][3] = { 0 };

	if (!tform_) { cout << "Cannot open file.\n"; }
	for (y = 0; y < 3; y++) {
		for (x = 0; x < 3; x++) {
			tform_ >> tform[y][x];
		}
	}	tform_.close();
	for (y = 0; y < 3; y++) {
		for (x = 0; x < 3; x++) {
			cout << tform[x][y] << " ";
		} cout << "\n";
	}

	int Width, Height, cropX, cropY; 
	cv::Size sz = img.size();
	int col = sz.width;
	int row = sz.height;
	//sin crop
	int sincrop_y = sin(abs(tform[0][1]))*row;
	int	sincrop_x = sin(abs(tform[0][1]))*col;
	
	//sin crop + X-Y shift
	cropX = sincrop_x + abs(tform[0][2]);
	cropY = sincrop_y + abs(tform[1][2]);  

	int c = 2;
	Height = row - c * cropY;
	Width  = col - c * cropX;

	Rect croppedImage = Rect(cropX, cropY, Width, Height);
	cout << "Cropping img (" << img.rows << "," << img.cols << ")";
	cout << " --> to (" << cropX+Width << "," << cropY+Height << ")" << endl;

	img_crop_ = img(croppedImage);
	img_crop_.copyTo(img_crop);

	return img_crop;
}