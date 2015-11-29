#pragma once

#define VISIBLE_ORIGIN Director::getInstance()->getVisibleOrigin()
#define VISIBLE_SIZE Director::getInstance()->getVisibleSize()

#define MATRIX_COLUMN 9
#define MATRIX_ROW 8
#define	SPACE_WIDTH 2
#define ICON_TYPE_COUNT 7
#define	TIP_INTERVAL 5

#define MAIN_SCENE_BACKGROUND "scene_sta.png"
#define OVER_SCENE_BACKGROUND "scene_end.png"
#define BUTTON_START_NORMAL "btn_start01.png"
#define BUTTON_START_PRESSED "btn_start02.png"
#define BUTTON_EXIT_NORMAL "btn_exit01.png"
#define BUTTON_EXIT_PRESSED "btn_exit02.png"
#define BUTTON_BACK_NORMAL "btn_back01.png"
#define BUTTON_BACK_PRESSED "btn_back02.png"
#define BUTTON_RESTART_NORMAL "btn_restart01.png"
#define BUTTON_RESTART_PRESSED "btn_restart02.png"

#define MAIN_SCENE_BACKGROUND_MUSIC "music_mainScene.mp3"
#define OVER_SCENE_BACKGROUND_MUSIC "music_gameOver.mp3"
#define GAME_BACKGROUND_MUSIC "music_bg.mp3"

#define GAME_SCENE_BACKGROUND "scene_bg.png"

#define MAIN_SCENE_BACKGROUND_ZORDER -1
#define CIRCLE_ZORDER 5
#define EFFECT_ZORDER 6
#define LABEL_ZORDER 90
#define INFORMATION_ZORDER 99

#define CIRCLE_TAG 10
#define FOREVER_ACTION_TAG 20
#define LABEL_SCORE_TAG 30