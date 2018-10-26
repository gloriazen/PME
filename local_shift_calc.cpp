#include "local_shift_calc.h"

using namespace std;
using namespace cv;

local_shift_calc::local_shift_calc(Mat img1, Mat img2, load_param p) : img1(img1) , img2(img2), p(p)  { }

local_shift_calc::gradient_criteria local_shift_calc::shift_criteria(Mat img) {

	gradient_criteria criteria;
	Mat grad_x, grad_y;
	int delta = 0, scale = 1, ddepth = CV_8U;

	// Gradient X
	cv::Sobel(img, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	// Gradient Y
	cv::Sobel(img, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	double s_x = cv::sum(grad_x)[0];
	double s_y = cv::sum(grad_y)[0];

	//cout << "SOBEL	" << s_x << "	" << s_y << "	";

	if (s_y < 100 && s_x < 100) {		//soglia spannometrica
		criteria.dec_x = 0;
		criteria.dec_y = 0;
		criteria.NTHR = 0;
		return criteria;
	}
	else {
		criteria.dec_x = 1;
		criteria.dec_y = 1;
		criteria.NTHR = 1;
		return criteria;
	}
}

local_shift_calc::Txy local_shift_calc::get_shift(){
	

	load_param p;
	Txy Txy_;
	vector<float> Tx, Ty, Tt;
	Mat img1_crop, img2_crop, img1_crop_, img2_crop_, img1_crop_borders;
	cv::Size yEnd_sz,sz = img2.size();	int row = sz.width-1, col = sz.height-1;
	vector<int> yEnd;
	for (int i = 0; i < col-(p.stepH*2); i = i + p.stepH ) { yEnd.push_back(i);	} 
	int ROW_N = yEnd.size() - 1; 
	vector<int> dec_x, dec_y; 
	int left = p.lim_x; int right = left;

	//Binary image
	Mat img1_b(img1.size(), img1.type());
	Mat img2_b(img2.size(), img2.type());
	//Apply thresholding
	cuda::threshold(img1, img1_b, p.bw_thr, 255, THRESH_BINARY);
	cuda::threshold(img2, img2_b, p.bw_thr, 255, THRESH_BINARY);


	for (int i = 0; i <= ROW_N; i++) {

		Rect croppedImage2 = Rect(0, yEnd[i], row, p.sizeH);
		img2_crop_ = img2_b(croppedImage2);
		img2_crop_.copyTo(img2_crop);
				
		local_shift_calc::gradient_criteria criteria2 = shift_criteria(img2_crop);
		
		//namedWindow("Image 1", CV_WINDOW_NORMAL); imshow("Image 1", img1_crop); cv::waitKey();

		if ((criteria2.dec_x > 0 || criteria2.dec_y > 0) && i>1 && i<ROW_N) {
			
			Rect croppedImage1_ = Rect(0, yEnd[i-1], row, p.sizeH*2); //img1_crop with size_y > img2_crop		
			img1_crop_ = img1_b(croppedImage1_);
			img1_crop_.copyTo(img1_crop);			
			copyMakeBorder(img1_crop, img1_crop_borders, 0, 0, left, right, BORDER_CONSTANT, 0); //add black borders only on left and right sides

			//cuda match template		
			cv::cuda::GpuMat result;
			cv::cuda::GpuMat image_d(img1_crop_borders); 
			cv::cuda::GpuMat templ_d(img2_crop); 
			
			cv::Ptr<cv::cuda::TemplateMatching> alg = cv::cuda::createTemplateMatching(img2_crop.type(), CV_TM_CCOEFF_NORMED);  //CV_TM_CCOEFF_NORMED // CV_TM_CCORR		
			alg->match(image_d, templ_d, result);

			//get X-Y shift 
			cv::cuda::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1);
			double max_value;
			cv::Point location;
			cv::cuda::minMaxLoc(result, 0, &max_value, 0, &location);

			int y_ = (location.y - p.stepH)*(-1); //delete constant Y shift and invert movement
			Ty.push_back(y_);
			int x_ = (location.x - left); //delete constant X shift
			Tx.push_back(x_); 

			if ((criteria2.dec_y) > 0){ //0.6*criteria2.NTHR) {
				Tt.push_back(2);
			}
			else { Tt.push_back(0); }

			//cout << "SHIFT:	";
			//cout << "	X: " << x_ << "	" << "	Y: " <<	y_ << "	" << "	CRITERIA:	" 	<< Tt.at(i) << "	Iter:" << i << "/" << ROW_N << "\n";

		}
		else {
			//cout << "NO SHIFT:	";
			//cout << "X: NAN	"  << "	" << "Y: NAN	" << "	" << "CRITERIA:	" << "0" << "	Iter:" << i << "/" << ROW_N << "\n";

			Tx.push_back(NAN);
			Ty.push_back(NAN);
			Tt.push_back(0);
		}
		Txy_.Tx = Tx;
		Txy_.Ty = Ty;
		Txy_.Tt = Tt;
	}

	return Txy_;
}
