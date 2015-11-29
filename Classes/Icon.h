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

	//默认0行0列的单个图标
	static Icon *create();
	static Icon *create(int, int);
	//基于行和列创建单个图标
	static Icon *create(int, int, const std::vector<int>&);
	virtual bool initWithRowAndCol(int, int, const std::vector<int>&);

	//记录图标所在的行
	CC_SYNTHESIZE(int, m_row, Row);
	//图标所在的列
	CC_SYNTHESIZE(int, m_col, Col);
	//图标所使用的图片下标
	CC_SYNTHESIZE(int, m_imageIndex, ImageIndex);
	//图标是否可以被移除
	CC_SYNTHESIZE(bool, m_canRemove, CanRemove);
	//图标类型
	CC_SYNTHESIZE_READONLY(IconType, m_type, Type);
	void setType(const IconType &);
	//是否可以被标注
	CC_SYNTHESIZE(bool, m_canMark, CanMark);
	//分值
	CC_SYNTHESIZE_READONLY(int, m_score, Score);
};