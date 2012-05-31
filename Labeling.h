/*!
 * @file Labeling.h
 * @author Yasutomo Kawanishi
 * @date Last Change:2011/May/30.
 * */

#ifndef __LABELING_H_
#define __LABELING_H_
#include <cv.h>
#include <vector>

#define LABELING_CONNECT_8 0
#define LABELING_CONNECT_4 1

/**
 * @class Labeling
 * @note 0���w�i
 * �\�[�g����ꍇ�͖ʐς̑傫������1,2,3�c
 * �\�[�g���Ȃ��ꍇ�͌���������1,2,3�c
 * �̃��x�����U����B
 * */
class Labeling{
public:
	Labeling();
	~Labeling();
	Labeling(const Labeling& other);
	Labeling& operator=(const Labeling& other);

	unsigned int operator()(const cv::Mat& src, int connect=LABELING_CONNECT_8); // �̈惉�x�����O���s��
	cv::Mat_<unsigned int> getLabel()const; // ���x���摜���擾�i�w�i��0,����ȊO�̓��x������f�l�Ƃ��Ď���CV_32UC1�摜�j
	unsigned int getRegionSize(unsigned int i)const; // �w�肵���̈�̑傫����Ԃ�

private:
	inline unsigned int _checkNeighbor();
	inline unsigned int _compaction();
	inline unsigned int _compaction(unsigned int i);
	inline void _compaction2();
	inline void _sort(unsigned int num);

	unsigned int neighbor[4];
	std::vector<unsigned int> labeltable;
	std::vector<unsigned int> regionsize;
	cv::Mat label;
	bool isLabeled;

	struct _PairSort{
	public:
		bool operator()(const std::pair<unsigned int,unsigned int>& x,const std::pair<unsigned int,unsigned int>& y)const
			{return x.second>y.second;}
	};
};

#endif
