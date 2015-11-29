#pragma once
#include "cocos2d.h"
#include "Icon.h"

USING_NS_CC;

typedef enum {
	Nothing,	//ʲô��û��
	Down,		//����
	Check,		//���ڼ��
	Eliminate,	//�б�ը
	CreateAndDown,	//����������
	Cross,		//����
	Over		//��Ϸ����
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
	//��������ָ��
	Icon ** m_matrix;
	//��������½�����
	Vec2 m_positonOfLeftBottom;
	//�����е�ͼ��
	Icon *m_icon;
	//ͼ����
	float m_iconWidth;
	//ͼ��״̬
	IconState m_state;
	//�հ׸�����
	int m_spaceCount;
	//��ſ��Թ���������ͼ��
	Vector<Icon *> m_tips;
	//��ֵͳ��
	int m_totalScore;

	//��ʼ������
	void initMatrix();
	//����Ƿ�������������
	void checkMatrix();
	//����ͳ�ƿ�����ͼ��
	bool rightCheck(Icon *icon);
	//����ͳ�ƿ�����ͼ��
	bool upCheck(Icon *icon);
	//ˮƽ����ͳ�ƿ�����ͼ��
	bool horizontalCheck(Icon *icon);
	//��ֱ����ͳ�ƿ�����ͼ��
	bool verticalCheck(Icon *icon);
	//�Ӿ������Ƴ������Ƴ�ͼ��
	void eliminateMatrix();
	//�����µ�ͼ�겢����
	void createAndDownIcon();
	//ͨ��ͼ������б�ŵõ�ͼ�������
	Vec2 convertToPosition(int, int);
	void up();
	void down();
	void left();
	void right();
	//�ı�״̬
	void changeState(const IconState&);
	//����Ƿ��������Ŀ���
	void checkTip();
	//��ʾ�û����ܵ�����
	void tip(float);
};