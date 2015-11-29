#pragma once
#include "cocos2d.h"
#include "Icon.h"

USING_NS_CC;

typedef enum {
	Nothing,	//什么都没做
	Down,		//下落
	Check,		//正在检查
	Eliminate,	//有爆炸
	CreateAndDown,	//创建并下落
	Cross,		//交换
	Over		//游戏结束
} IconState;

class GameScene : public Layer {
public:
	GameScene();
	~GameScene();

	static Scene *createScene();
	CREATE_FUNC(GameScene);
	virtual bool init() override;

	virtual void onEnterTransitionDidFinish() override;
	virtual void onExitTransitionDidStart() override;
private:
	//操作矩阵指针
	Icon ** m_matrix;
	//矩阵的左下角坐标
	Vec2 m_positonOfLeftBottom;
	//被点中的图标
	Icon *m_icon;
	//图标宽度
	float m_iconWidth;
	//图标状态
	IconState m_state;
	//空白格数量
	int m_spaceCount;
	//存放可以构成三消的图标
	Vector<Icon *> m_tips;
	//分值统计
	int m_totalScore;

	//初始化矩阵
	void initMatrix();
	//检查是否满足消除条件
	void checkMatrix();
	//向右统计可消除图标
	bool rightCheck(Icon *icon);
	//向上统计可消除图标
	bool upCheck(Icon *icon);
	//水平方向统计可消除图标
	bool horizontalCheck(Icon *icon);
	//垂直方向统计可消除图标
	bool verticalCheck(Icon *icon);
	//从矩阵中移除可以移除图标
	void eliminateMatrix();
	//创建新的图标并下落
	void createAndDownIcon();
	//通过图标的行列编号得到图标的坐标
	Vec2 convertToPosition(int, int);
	void up();
	void down();
	void left();
	void right();
	//改变状态
	void changeState(const IconState&);
	//检测是否有三消的可能
	void checkTip();
	//提示用户可能的三消
	void tip(float);
};