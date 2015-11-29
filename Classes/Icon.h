#pragma once
#include "cocos2d.h"

USING_NS_CC;

typedef enum {
	NormalIcon,
	HorizontalIcon,
	VerticalIcon
} IconType;

class Icon : public Sprite {
public:
	Icon();

	//Ĭ��0��0�еĵ���ͼ��
	static Icon *create();
	static Icon *create(int, int);
	//�����к��д�������ͼ��
	static Icon *create(int, int, const std::vector<int>&);
	virtual bool initWithRowAndCol(int, int, const std::vector<int>&);

	//��¼ͼ�����ڵ���
	CC_SYNTHESIZE(int, m_row, Row);
	//ͼ�����ڵ���
	CC_SYNTHESIZE(int, m_col, Col);
	//ͼ����ʹ�õ�ͼƬ�±�
	CC_SYNTHESIZE(int, m_imageIndex, ImageIndex);
	//ͼ���Ƿ���Ա��Ƴ�
	CC_SYNTHESIZE(bool, m_canRemove, CanRemove);
	//ͼ������
	CC_SYNTHESIZE_READONLY(IconType, m_type, Type);
	void setType(const IconType &);
	//�Ƿ���Ա���ע
	CC_SYNTHESIZE(bool, m_canMark, CanMark);
	//��ֵ
	CC_SYNTHESIZE_READONLY(int, m_score, Score);
};