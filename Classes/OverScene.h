#pragma once
#include "cocos2d.h"

USING_NS_CC;

class OverScene : public Layer {
public:
	static Scene *createScene(int);
	static OverScene *createWithScore(int);
	bool initWithScore(int);

	virtual void onEnterTransitionDidFinish() override;
	virtual void onExitTransitionDidStart() override;
};