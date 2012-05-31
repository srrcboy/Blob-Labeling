#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "Labeling.h"

#ifdef __BENCH__ // �x���`�}�[�N������ꍇ�� __BENCH__����ԏ��define���Ă�������
#include <time.h>
void benchmark(const cv::Mat& img, Labeling& labeler);
#endif

int main(int argc, char **argv){
	// �ΏۂƂȂ�摜�iCV_8UC1�̓�l�摜�ł���K�v������܂��j
	cv::Mat img=cv::imread("test.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	// �������ʁicv::Mat_<int>�ŕԂ��܂��j
	cv::Mat label;
	// ���ʕ\���p
	cv::Mat resultimg;

	int num; // �̈搔�i�w�i���P�̗̈�i0�Ԗځj�Ƃ��Ċ܂ށj

	Labeling labeler;

	// ���x�����O����
	num=labeler(img,LABELING_CONNECT_8);

#ifdef __BENCH__ // �x���`�}�[�N������ꍇ�� __BENCH__��define���Ă�������
	benchmark(img, labeler);
#endif

	// ���x�����O���ʂ��擾
	label=labeler.getLabel();
	std::cout << "number of regions: " << num << std::endl;

	// 1���炪���̗̈�
	for(int i=1;i<num;i++){
		std::cout << "size of region " << i;
		std::cout << ": " << labeler.getRegionSize(i) << std::endl;
	}

	// �K���ɉ�������
	label*=(double)256/num;
	label.convertTo(resultimg,CV_8UC1);

	// �\��
	cv::imshow("test", img);
	cv::imshow("label", resultimg);
	cv::waitKey(0);
	return 0;
}

#ifdef __BENCH__
void benchmark(const cv::Mat& img, Labeling& labeler){
	const int repeat=1000;
	int num;
	clock_t start,end;
	start = clock();

	for(int i=0;i<repeat;i++) num=labeler(img,LABELING_CONNECT_8);
	end = clock();
	std::cout << "average:" << (double)(end-start)/CLOCKS_PER_SEC / repeat << std::endl;
}
#endif
