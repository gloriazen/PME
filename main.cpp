#include <iostream>
#include "librerie.h"

#ifdef _WIN32
    #include "stdafx.h"
#endif

#include <ctime>
#include <opencv2/opencv.hpp>

#include "parameters.h"
#include "dataset.h"

using namespace std;
using namespace cv;


void fun_prova_get_path(){

    char buffer[500];
    getcwd(buffer, sizeof(buffer));
    cout << "Working path : " << std::string(buffer) << endl;
}


void fun_prova_resize_gpu(){

    std::cout << "Hello, World !" << std::endl;
    Mat inputCpu = imread("../../imgs/lena.jpg",CV_LOAD_IMAGE_COLOR);
    cuda::GpuMat input(inputCpu);

    cuda::GpuMat output;
    cuda::resize(input, output, Size(), .25, 0.25, CV_INTER_AREA);
    Mat outputCpu;
    output.download(outputCpu);

    namedWindow("Image out", CV_WINDOW_NORMAL);
    imshow("Image out", outputCpu);
    waitKey();
    std::cout << "Bye bye, World!" << std::endl;
}


void fun_prova_remap_cpu(){

    srand(time(NULL));
    //time_t tstart, tend;

    std::cout << "Hello, World!" << std::endl;
    Mat inputCpu = imread("../../imgs/lena.jpg",CV_LOAD_IMAGE_COLOR);

    Mat xmapCpu, ymapCpu;
    xmapCpu.create(inputCpu.size(), CV_32FC1);
    ymapCpu.create(inputCpu.size(), CV_32FC1);
    for (int i = 0; i < xmapCpu.rows; i++)
        for (int j = 0; j < xmapCpu.cols; j++) {
            xmapCpu.at<float>(j, i) = 1.1*i;
            ymapCpu.at<float>(j, i) = 1.2*j;
        }

    Mat outputCpu;
    remap(inputCpu, outputCpu, xmapCpu, ymapCpu, INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );

    namedWindow("Image in", CV_WINDOW_NORMAL);
    imshow("Image in", inputCpu);
    namedWindow("Image out", CV_WINDOW_NORMAL);
    imshow("Image out", outputCpu);
    waitKey();
    std::cout << "Bye bye, World!" << std::endl;
}

void fun_prova_remap_gpu(){

    srand(time(NULL));
    time_t tstart, tend;

    cout << "Hello, World!" << std::endl;
    Mat inputCpu = imread("../../imgs/lena.jpg",CV_LOAD_IMAGE_COLOR);
    Mat inputCpuGray;
    cvtColor(inputCpu, inputCpuGray, CV_BGR2GRAY);
    cuda::GpuMat input(inputCpuGray);

    Mat xmapCpu, ymapCpu;
    xmapCpu.create(cv::Size(225, 225), CV_32FC1);
    ymapCpu.create(cv::Size(225, 225), CV_32FC1);
    for (int i = 0; i < xmapCpu.rows; i++)
        for (int j = 0; j < xmapCpu.cols; j++) {
            //xmapCpu.at<float>(j, i) = xmapCpu.rows-i + rand()%10;
            xmapCpu.at<float>(j, i) = i+rand()%10;
            ymapCpu.at<float>(j, i) = j+rand()%10;
        }
    cuda::GpuMat xmap;
    cuda::GpuMat ymap;
    xmap.upload(xmapCpu);
    ymap.upload(ymapCpu);

    cuda::GpuMat output;
    tstart = time(0);
    cuda::remap(input, output, xmap, ymap, INTER_CUBIC, BORDER_CONSTANT, Scalar(0,0, 0) );
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
    Mat outputCpu;
    output.download(outputCpu);


    namedWindow("Image in", CV_WINDOW_NORMAL);
    imshow("Image in", inputCpuGray);
    namedWindow("Image out", CV_WINDOW_NORMAL);
    imshow("Image out", outputCpu);
    waitKey();
    std::cout << "Bye bye, World!" << std::endl;
}



int main() {

    cout << " Hello ";
    //fun_prova_resize_gpu();

    //initialize CUDA
    cv::cuda::setDevice(0);

    // Load parameters
    parameters p;

    // Load datasets
    dataset d;

    cout << "Loading data from : " << p.deposit << endl ;
    //fun_prova_remap_gpu();

    for (int pid_n = 34; pid_n <=34; pid_n++){

        stringstream ss;
        ss << pid_n;
        string pid_n_str = ss.str();







    }

    return 0;
}




