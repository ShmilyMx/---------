#pragma once
#include "cocos2d.h"

USING_NS_CC;

class MainScene : public Layer {
public:
	static Scene *createScene();
	CREATE_FUNC(MainScene);
	virtual bool init() override;

	virtual void onEnterTransitionDidFinish() override;
	virtual void onExitTransitionDidStart() override;
};