#include "Labeling.h"
#include <limits>

Labeling::Labeling():isLabeled(false){
}

Labeling::~Labeling(){
}

Labeling::Labeling(const Labeling& other)
	:labeltable(other.labeltable),regionsize(other.regionsize),label(other.label.clone()),isLabeled(other.isLabeled){
}

Labeling& Labeling::operator=(const Labeling& other){
	if(this==&other) return *this;
	this->labeltable=other.labeltable;
	this->regionsize=other.regionsize;
	this->label=other.label.clone();
	this->isLabeled=other.isLabeled;
	return *this;
}

unsigned int Labeling::operator()(const cv::Mat& img, int connect){
	labeltable.resize(0);
	labeltable.push_back(0);

	label=cv::Mat_<unsigned int>::zeros(img.rows,img.cols);
	unsigned int currentlabel=0;

	const unsigned char *line=img.ptr<unsigned char>(0);
	unsigned int *line_label=label.ptr<unsigned int>(0), *line_label_prev=NULL;

	// ���Ȃ��Ƃ�2x2�ȏ�̉摜�łȂ��Ƃ���
	if(img.rows < 2 || img.cols < 2) throw "Image size is invalid.";

	for(int y=0; y<img.rows; y++, line_label_prev=line_label){
		/// �s���Ƃ̏���
		line=img.ptr<unsigned char>(y);
		line_label=label.ptr<unsigned int>(y);

		for(int x=0; x<img.cols; x++){
			if(line[x]!=0){
				neighbor[0]= y>0 && x>0 && connect==LABELING_CONNECT_8 ? line_label_prev[x-1] : 0;
				neighbor[1]= y>0                   ? line_label_prev[x]   : 0;
				neighbor[2]= y>0 && x < img.cols-1 && connect==LABELING_CONNECT_8 ? line_label_prev[x+1] : 0;
				neighbor[3]= x>0                   ? line_label[x-1]      : 0;

				line_label[x]=_checkNeighbor();
				if(line_label[x]==0){
					/// ����ɗ̈悪�Ȃ��ꍇ
					if(currentlabel==std::numeric_limits<unsigned int>::max()){
						throw "Label number overflow.";
					}
					currentlabel++;
					line_label[x]=currentlabel;
					labeltable.push_back(currentlabel);
				}
			}
		}
	}

	// ���x�����𐔂��āC���x���̍Ĕz�u���s��
	int num=_compaction();

	// �̈�̑傫�����Ƀ\�[�g���āC���x���̍Ċ��蓖�Ă��s��
	_sort(num);

	// �̈敪���摜�Ƀ��x���Ċ����Ă𔽉f
	for(int y=0;y<label.rows;y++){
		unsigned int *label_line=label.ptr<unsigned int>(y);
		for(int x=0;x<label.cols;x++){
			label_line[x]=labeltable[label_line[x]];
		}
	}

	isLabeled=true;
	return num;
}

cv::Mat_<unsigned int> Labeling::getLabel()const{
	if(!isLabeled) throw "call operator() first";
	return label;
}

unsigned int Labeling::getRegionSize(unsigned int i)const{
	if(!isLabeled) throw "call operator() first";
	return regionsize[i];
}

unsigned int Labeling::_compaction(){
	for(unsigned int i=0;i<labeltable.size();i++){
		labeltable[i]=_compaction(i);
	}
	_compaction2();
	return *(std::max_element(labeltable.begin(),labeltable.end()))+1;
}

unsigned int Labeling::_compaction(unsigned int i){
	if(i==labeltable[i]) return i;
	
	labeltable[i]=_compaction(labeltable[i]);
	return labeltable[i];
}

unsigned int Labeling::_checkNeighbor(){
	unsigned int val=std::numeric_limits<unsigned int>::max();

	for(unsigned int i=0;i<4;i++){
		if(neighbor[i]==0)continue;
		if(val >= labeltable[neighbor[i]]) val=labeltable[neighbor[i]];
	}

	// �̈悪��������ꍇ�e�[�u��������������
	if(val!=std::numeric_limits<unsigned int>::max()){
		for(unsigned int i=0;i<4;i++){
			if(neighbor[i]==0)continue;
			labeltable[neighbor[i]]=val;
		}
		return val;
	}
	return 0;
}

void Labeling::_compaction2(){
	std::map<unsigned int,unsigned int> usedlabel;
	usedlabel[0]=0;
	std::map<unsigned int,unsigned int>::iterator prev;
	unsigned int i=0;

	for(std::vector<unsigned int>::iterator it=labeltable.begin();it!=labeltable.end();it++){
		prev=usedlabel.find(*it);
		if(prev==usedlabel.end()){
			i++;
			usedlabel[*it]=i;
			*it=i;
		}else{
			*it=prev->second;
		}
	}
}

void Labeling::_sort(unsigned int num){
	std::vector<std::pair<unsigned int,unsigned int> > regiontemp;

	regionsize.resize(num);

	// ��f�l�̃J�E���g
	for(int y=0;y<label.rows;y++){
		const unsigned int *label_line=label.ptr<unsigned int>(y);
		for(int x=0;x<label.cols;x++){
			// �w�i�łȂ���΃J�E���g
			if(label_line[x]!=0) regionsize[labeltable[label_line[x]]]++;
		}
	}

	// �\�[�g�̂���map����vector��
	unsigned int i=0;
	for(std::vector<unsigned int>::iterator it=regionsize.begin(); it!=regionsize.end(); it++){
		regiontemp.push_back(std::make_pair(i,*it));
		i++;
	}

	// 2�ڈȍ~��傫�����Ƀ\�[�g
	std::vector<std::pair<unsigned int,unsigned int> >::iterator it=regiontemp.begin();it++;
	std::sort(it, regiontemp.end(), _PairSort());

	// ���x���̍Ċ��蓖�ă}�b�v�����
	i=0;
	regionsize.clear();
	std::map<unsigned int,unsigned int> remap; // <���蓖�đO,���蓖�Č�>
	for(std::vector<std::pair<unsigned int,unsigned int> >::iterator it2=regiontemp.begin(); it2!=regiontemp.end(); it2++){
		remap[it2->first]=i;
		regionsize.push_back(it2->second);
		i++;
	}

	// ���x���̍Ċ��蓖��
	for(std::vector<unsigned int>::iterator lt=labeltable.begin(); lt!=labeltable.end(); lt++){
		*lt=remap[*lt];
	}
}
