#include "librerie.h"

using namespace std;
using namespace cv;


int main(){

	//initialize CUDA
	cv::cuda::setDevice(0);

	// Load parameters and pid images
	load_param p;
	int pid_from, pid_to; 
	pid_from =	 34;	//21 - 29 - 37 - 47
	//pid_to =   60;	//28 - 36 - 46 - 60
	pid_to = pid_from;

	for (int pid_n = pid_from; pid_n <= pid_to; pid_n++) {
		ostringstream str1;
		str1 << pid_n;
		string pid_ = str1.str();

		// Read the images to be aligned
		cout << "Loading pid " << pid_ << " : " << p.pairs << " ... " << endl;
		load_pid::imgs im_pair = load_pid(p.pairs, pid_).get_imgs();
		cout << "Loading " << im_pair.fix << " ; " << im_pair.mov << " ... " << endl;
		Mat im1 = imread(im_pair.fix);
		Mat im2 = imread(im_pair.mov);

		cout << "[Debug] im1 " <<  im_pair.fix << " size: (" << im1.rows << "," << im1.cols << ")" << endl;
		cout << "[Debug] im2 " <<  im_pair.mov << " size: (" << im2.rows << "," << im2.cols << ")" << endl;


		// Convert images to gray scale;
		Mat im1_gray, im2_gray;
		if (im1.channels() == 3) { cvtColor(im1, im1_gray, CV_BGR2GRAY); }
		else { im1_gray = im1; }
		if (im2.channels() == 3) { cvtColor(im2, im2_gray, CV_BGR2GRAY); }
		else { im2_gray = im2; }
		
		// Control images sizes
		Size sz1 = im1_gray.size(), sz2 = im2_gray.size();
		int row_g, col_g;
		if (sz1.width < sz2.width) { row_g = sz1.width; }
		else { row_g = sz2.width; }
		if (sz1.height < sz2.height) { col_g = sz1.height; }
		else { col_g = sz2.height; }
		Rect c1 = Rect(0, 0, row_g, col_g);
		im1_gray = im1_gray(c1);
		im2_gray = im2_gray(c1);

		cout << "[Debug] im1_gray size: (" << im1_gray.rows << "," << im1_gray.cols << ")" << endl;
		cout << "[Debug] im2_gray size: (" << im2_gray.rows << "," << im2_gray.cols << ")" << endl;

		// crop imgs after global match, avoiding black side bands
		Mat img1_g = crop_img_glob(im1_gray, pid_).crop(); 
		Mat img2_g = crop_img_glob(im2_gray, pid_).crop();

		// get local shift points 	
		std::clock_t start; // start clock
		double shift_time;
		start = std::clock();
		cout << endl << endl << "Pid:" << pid_n << "	Window size: " << p.sizeH << endl << "Finding local shift: " << endl;
		//namedWindow("Image 2", CV_WINDOW_NORMAL); imshow("Image 2", img2_g); cv::waitKey();
		local_shift_calc::Txy Txy_ = local_shift_calc(img1_g, img2_g, p).get_shift();
		shift_time = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		std::cout << "Time: " << shift_time << endl;

		int ROW_N = 0;
		cv::Size sz = img2_g.size(); 
		int bound_row = sz.height - (p.stepH * 2);
		for (int i = 0; i < bound_row ; i = i + p.stepH) { ROW_N++; }

		//open Txy txt
		ofstream Txy_file;
		string Txy_file_string = p.deposit + "opencv_vs_matlab\\Txy" + pid_ + ".txt";
		Txy_file.open(Txy_file_string);
		for (int ii = 0; ii < ROW_N; ii++) {
			Txy_file << Txy_.Tx.at(ii) << " " << Txy_.Ty.at(ii) << " " << Txy_.Tt.at(ii) << endl;
		}
		Txy_file.close();

		//open time txt
		ofstream Txy_file_time; string Txy_file_time_string = p.deposit + "opencv_vs_matlab\\Txy" + pid_ + "_time.txt";
		Txy_file.open(Txy_file_time_string);
		Txy_file << "SizeH:	" << p.sizeH << endl << endl << "Calculate shift:	" << shift_time << endl;

		//end of local shift calc

		
		//interpolation//

		//spacing sample
		std::vector<double> X, Y; // X->stepH:stepH:ROW for Y values 
		for (int i = 0; i < ROW_N; i++) { //eventualmente qui considerare limitazioni con std, lim_y ecc
			if (!isnan(Txy_.Ty.at(i))) {
				X.push_back((i + 1) * p.stepH);
				Y.push_back(Txy_.Ty.at(i));
			}
			else {
				X.push_back((i + 1) * p.stepH);
				Y.push_back(0);
			}
		} 

		//interpolation function
		tk::spline s;	s.set_points(X, Y);   
		
		//get and save X-Y pixel shift
		start = std::clock();
		cout << endl << "Interpolo i valori lungo Y:" << endl;
		Mat interp_x_vec = Mat(cv::Size(1, img1_g.size().width), CV_32FC1);
		Mat interp_y_vec = Mat(cv::Size(1, img1_g.size().height), CV_32FC1);	
		for (int i = 0; i < img1_g.size().height; i++) {  		
			interp_y_vec.at<float>(i) = s(i)+i;
		}
		for (int i = 0; i < img1_g.size().width; i++) {
			interp_x_vec.at<float>(i) = i;
		}
		shift_time = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		std::cout << "Time: " << shift_time << endl;
		Txy_file  << "Get Interp values:	" << shift_time << endl;

		//generate Xmap and Ymap in order to remap the moving image
		start = std::clock();
		cout << endl << "Genero Xmap e Ymap:" << endl;
		Mat Xmap = Mat(img2_g.size(), CV_32FC1);
		Mat Ymap = Mat(img2_g.size(), CV_32FC1);
		Xmap = repeat(interp_x_vec, 1, img1_g.size().height); transpose(Xmap, Xmap);
		Ymap = repeat(interp_y_vec, 1, img1_g.size().width);
		shift_time = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		std::cout << "Time: " << shift_time << endl;
		Txy_file << "generate Xmap and Ymap:	" << shift_time << endl;

		//remap
		start = std::clock();
		cout << endl << "Remap:" << endl;
		cuda::GpuMat xmap_gpu, ymap_gpu, img_2_REMAP_gpu, img1_g_GPU, img_absdiff, img2_g_GPU;
		Mat img_2_REMAP_cpu, absdiff_cpu;

		//load images on GPU
		xmap_gpu.upload(Xmap);
		ymap_gpu.upload(Ymap);
		img1_g_GPU.upload(img1_g);
		img2_g_GPU.upload(img2_g);

		//GPU remap
		cuda::remap(img2_g_GPU, img_2_REMAP_gpu, xmap_gpu, ymap_gpu, INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
		cuda::absdiff(img_2_REMAP_gpu, img1_g_GPU, img_absdiff);

		//download images from GPU to CPU
		img_2_REMAP_gpu.download(img_2_REMAP_cpu);
		img_absdiff.download(absdiff_cpu);
		shift_time = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		std::cout << "Time: " << shift_time << endl;
		Txy_file << "Remap image:		" << shift_time << endl;

		//save images
		cout << endl << "Salvo immagini:" << endl;
		string imgout1 = p.deposit + "opencv_vs_matlab\\Txy_" + pid_ + "_interp_img.png";	imwrite(imgout1, img_2_REMAP_cpu);
		string imgout2 = p.deposit + "opencv_vs_matlab\\Txy_" + pid_ + "_absdiff.png";	imwrite(imgout2, absdiff_cpu);
		string imgout3 = p.deposit + "opencv_vs_matlab\\Txy_" + pid_ + "img2_g.png";	imwrite(imgout3, img1_g);

		std::cout << "Bye bye, World!" << std::endl;
	}
	return 0;
}


