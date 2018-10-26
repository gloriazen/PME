#ifndef __LOAD_PARAM__
#define __LOAD_PARAM__

#include "librerie.h"

using namespace std;
using namespace cv;

typedef struct load_param {

	//set window size and step for local shift
	int sizeH = 200; //multicam = 200 singlecam = 100
	int stepH = (sizeH / 2); //set windows width and windows step
	
	//set threshold for b/w + limit for x and y shift
	int bw_thr = 70;
	int lim_x = 0; // 0 for no shift on X-Axis
	double lim_y = (0.85*sizeH);

	//set paths
//	string deposit = "C:\\Users\\morom\\Desktop\\";
//	string path_gen = deposit + "dataset\\";
//	string pid_path = path_gen + "affine_matrix\\matrix_";
//	string pairs = path_gen + "stampa_estratto\\pairs.csv";

	string deposit = "/home/gzen/WORK/projects/CLEVER/PME1.0/";
	string path_gen = deposit + "Archive/";
//	string path_pid = path_gen + "stampa_risultati/affine_matrix/matrix_";
	string path_original = path_gen + "stampa_estratto_stable1/";
	string path_affine = path_gen + "stampa_risultati/affine_matrix/";
	string pairs = path_gen + "stampa_estratto_stable1/pairs.csv";



	
}p;


#endif