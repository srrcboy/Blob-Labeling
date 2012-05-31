/*!
 * @file Labeling.h
 * @author Yasutomo Kawanishi
 * @date Last Change:01-Jun-2012.
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

	unsigned short operator()(const cv::Mat& src, int connect=LABELING_CONNECT_8); // �̈惉�x�����O���s��
	cv::Mat getLabel()const; // ���x���摜���擾�i�w�i��0,����ȊO�̓��x������f�l�Ƃ��Ď���CV_32UC1�摜�j
	unsigned short getRegionSize(unsigned short i)const; // �w�肵���̈�̑傫����Ԃ�

private:
	inline unsigned short _checkNeighbor();
	inline unsigned short _compaction();
	inline unsigned short _compaction(unsigned short i);
	inline void _compaction2();
	inline void _sort(unsigned short num);

	unsigned short neighbor[4];
	std::vector<unsigned short> labeltable;
	std::vector<unsigned short> regionsize;
	cv::Mat label;
	bool isLabeled;

	struct _PairSort{
	public:
		bool operator()(const std::pair<unsigned short,unsigned short>& x,const std::pair<unsigned short,unsigned short>& y)const
			{return x.second>y.second;}
	};
};

#endif
