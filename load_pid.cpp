#include "load_pid.h"

using namespace std;
using namespace cv;

load_pid::load_pid(string file_, string pid_) : file(file_), pid(pid_) {}

load_pid::imgs load_pid::get_imgs(){

	load_param p;
	// Creating an object of CSVWriter
	CSVReader reader(p.pairs);
	// Get the data from CSV File
	vector<vector<string> > dataList = reader.getData();
	// Print the content of row by row on screen
	int i = 0, ii = 0; 
	for (vector<string> vec : dataList) {
		for (string data : vec) {
			cout << data << ",";
		}
		cout << std::endl;
		if (dataList[i][1] == pid) {
			ii = i;
		}
		i++;
	}

	string mov_ = dataList[ii][3], fix_ = dataList[ii][1];
	stringstream f, m;
	f << p.path_affine << "affine_" << fix_ << ".png";
	m << p.path_original << mov_;
	string fix = f.str(), mov = m.str();
	imgs img; img.fix = fix; img.mov = mov;
	return img;
};



