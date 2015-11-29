#include "MainScene.h"
#include "CommonData.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d::ui;
using namespace CocosDenshion;

Scene *MainScene::createScene() {
	auto layer = MainScene::create();
	auto scene = Scene::create();
	scene->addChild(layer);
	return scene;
}

bool MainScene::init() {
	do
	{
		CC_BREAK_IF(!Layer::init());

		auto bg = Sprite::create(MAIN_SCENE_BACKGROUND);
		bg->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE)/2);
		this->addChild(bg, MAIN_SCENE_BACKGROUND_ZORDER);

		auto btnStart = Button::create(BUTTON_START_NORMAL, BUTTON_START_PRESSED);
		btnStart->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width/2, VISIBLE_SIZE.height/5));
		this->addChild(btnStart);
		btnStart->addClickEventListener([](Ref *) {
			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(scene);
		});

		auto btnExit = Button::create(BUTTON_EXIT_NORMAL, BUTTON_EXIT_PRESSED);
		btnExit->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width/6 * 5, VISIBLE_SIZE.height/12));
		this->addChild(btnExit);
		btnExit->addClickEventListener([](Ref *) {
			Director::getInstance()->end();
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
			exit(0);
#endif
		});

		return true;
	} while (0);
	 
	return false;
}

void MainScene::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
	SimpleAudioEngine::getInstance()->playBackgroundMusic(MAIN_SCENE_BACKGROUND_MUSIC, true);
}

void MainScene::onExitTransitionDidStart() {
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	Layer::onExitTransitionDidStart();
}