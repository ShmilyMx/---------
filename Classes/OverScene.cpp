#include "OverScene.h"
#include "CommonData.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d::ui;
using namespace CocosDenshion;

Scene *OverScene::createScene(int score) {
	auto layer = OverScene::createWithScore(score);
	auto scene = Scene::create();
	scene->addChild(layer);
	return scene;
}

OverScene *OverScene::createWithScore(int score) {
	auto over = new OverScene();
	if (over && over->initWithScore(score))
	{
		over->autorelease();
		return over;
	}
	CC_SAFE_DELETE(over);
	return over;
}

bool OverScene::initWithScore(int score) {
	do
	{
		CC_BREAK_IF(!Layer::init());

		auto bg = Sprite::create(OVER_SCENE_BACKGROUND);
		bg->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE)/2);
		this->addChild(bg, MAIN_SCENE_BACKGROUND_ZORDER);

		auto lblScore = Label::createWithTTF(StringUtils::format("%d", score), "haibaoti.ttf", 68);
		lblScore->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width / 2, VISIBLE_SIZE.height / 3 * 2));
		this->addChild(lblScore, LABEL_ZORDER);

		auto highScore = UserDefault::getInstance()->getIntegerForKey("HIGH_SCORE", 0);
		if (score > highScore)
		{
			UserDefault::getInstance()->setIntegerForKey("HIGH_SCORE", score);
			UserDefault::getInstance()->flush();

			auto newRecord = Sprite::create("sprite_newRecord.png");
			newRecord->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
			newRecord->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width / 10, VISIBLE_SIZE.height / 14 * 13));
			this->addChild(newRecord, INFORMATION_ZORDER);
		}

		auto btnBack = Button::create(BUTTON_BACK_NORMAL, BUTTON_BACK_PRESSED);
		btnBack->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width/4, VISIBLE_SIZE.height/6));
		this->addChild(btnBack);
		btnBack->addClickEventListener([](Ref *) {
			auto scene = MainScene::createScene();
			Director::getInstance()->replaceScene(scene);
		});
		btnBack->setRotation(-20);

		auto btnRestart = Button::create(BUTTON_RESTART_NORMAL, BUTTON_RESTART_PRESSED);
		btnRestart->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width / 4 * 3, VISIBLE_SIZE.height / 6));
		this->addChild(btnRestart);
		btnRestart->addClickEventListener([](Ref *) {
			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(scene);
		});
		btnRestart->setRotation(20);

		return true;
	} while (0);
	 
	return false;
}

void OverScene::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
	SimpleAudioEngine::getInstance()->playBackgroundMusic(OVER_SCENE_BACKGROUND_MUSIC);
}

void OverScene::onExitTransitionDidStart() {
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	{
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
	Layer::onExitTransitionDidStart();
}