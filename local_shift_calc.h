#ifndef __LOCAL_SHIFT_CALC__
#define __LOCAL_SHIFT_CALC__

#include "librerie.h"

using namespace std;
using namespace cv;

class local_shift_calc
{	
	Mat img1, img2;
	load_param p;
	struct gradient_criteria { int dec_x, dec_y, NTHR; };
public:
	struct Txy { vector<float> Tx, Ty, Tt; };
	local_shift_calc(Mat img1, Mat img2, load_param p);
	local_shift_calc::Txy get_shift();
	local_shift_calc::gradient_criteria shift_criteria(Mat img);
};

#endif