#ifndef __LIBRERIE__
#define __LIBRERIE__

////////////////////////////////////opencv
#include<opencv2/opencv.hpp>
//#include <opencv/cv.h>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/core/utility.hpp>
//#include "opencv2/core/affine.hpp"

////////////////////////////////////cuda
//
//#include <opencv2/core/cuda_types.hpp>
//#include <opencv2/core/cuda.inl.hpp>
//#include <opencv2/core/cuda.hpp>
//#include <opencv2/core/cuda_stream_accessor.hpp>
//#include <opencv2/cudalegacy/NCVHaarObjectDetection.hpp>
//#include <opencv2/cudalegacy/NCV.hpp>
//#include <opencv2/cudalegacy/NPP_staging.hpp>
//#include <opencv2/core/ocl.hpp>
//#include <opencv2/core/ocl_genbase.hpp>
//#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudawarping.hpp>
//#include <opencv2/cudaimgproc.hpp>
//#include <opencv2/cudafilters.hpp>
//////////////////////////////////////


#include <ctime>
#include <cstdio>

#include <iostream>
#include <fstream>

#include <string>
#include <iterator>
#include <algorithm>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "load_param.h"
#include "CSVReader.h"
#include "load_pid.h"
#include "crop_img_glob.h"
#include "local_shift_calc.h"
#include "spline.h"

#endif