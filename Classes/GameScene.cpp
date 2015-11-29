#include "GameScene.h"
#include "OverScene.h"
#include "MainScene.h"
#include "CommonData.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d::ui;
using namespace CocosDenshion;

GameScene::GameScene() {
	auto size = sizeof(Icon *) * MATRIX_COLUMN * MATRIX_ROW;
	m_matrix = (Icon **)malloc(size);
	memset(m_matrix, 0, size);

	m_positonOfLeftBottom = Vec2::ZERO;
	m_icon = nullptr;
	m_iconWidth = 0;
	m_state = Nothing;
	m_spaceCount = 0;
	m_totalScore = 0;
}

GameScene::~GameScene() {
	CC_SAFE_FREE(m_matrix);
	CC_SAFE_DELETE(m_icon);
}

Scene *GameScene::createScene() {
	auto layer = GameScene::create();
	auto scene = Scene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init() {
	do
	{
		CC_BREAK_IF(!Layer::init());

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("icon.plist");

		auto icon = Icon::create();
		m_iconWidth = icon->getContentSize().width;

		auto bg = Sprite::create(GAME_SCENE_BACKGROUND);
		bg->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE) / 2);
		this->addChild(bg, MAIN_SCENE_BACKGROUND_ZORDER);

		//初始化矩阵
		initMatrix();

		//添加触摸事件处理
		auto listener = EventListenerTouchOneByOne::create();

		listener->onTouchBegan = [&](Touch *touch, Event *) {
			//循环检测没有结束就不能响应触摸
			if (m_state != Nothing) return false;
			auto touchPos = touch->getLocation();
			for (auto i = 0; i < MATRIX_ROW * MATRIX_COLUMN; i++)
			{
				if (m_matrix[i] && m_matrix[i]->getBoundingBox().containsPoint(touchPos))
				{
					m_icon = m_matrix[i];
				}
			}
			return m_icon != nullptr;
		};
		listener->onTouchEnded = [&](Touch *touch, Event *) {
			auto endPos = touch->getLocation();
			auto vec = endPos - m_icon->getPosition();
			if (fabs(vec.x) < m_iconWidth / 2 + SPACE_WIDTH && fabs(vec.y) < m_iconWidth / 2 + SPACE_WIDTH) {
				m_icon = nullptr;
				return;
			}

			//停止提示动画
			for (auto icon : m_tips)
			{
				icon->stopActionByTag(FOREVER_ACTION_TAG);
				icon->setScale(1);
				unschedule(schedule_selector(GameScene::tip));
			}
			m_tips.clear();


			if (fabs(vec.x) > fabs(vec.y))
			{
				if (vec.x>0)
				{
					right();
				}
				else 
				{
					left();
				}
			}
			else
			{
				if (vec.y>0)
				{
					up();
				}
				else
				{
					down();
				}
			}
		};

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		//添加分值显示标签
		auto lblScore = Label::createWithTTF("0", "haibaoti.ttf", 48);
		lblScore->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		lblScore->setPosition(VISIBLE_ORIGIN + Vec2(20, VISIBLE_SIZE.height - 42));
		this->addChild(lblScore, LABEL_ZORDER, LABEL_SCORE_TAG);

		auto btnBack = Button::create(BUTTON_BACK_NORMAL, BUTTON_BACK_PRESSED);
		btnBack->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		btnBack->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE.width - 20, VISIBLE_SIZE.height - 20));
		this->addChild(btnBack);
		btnBack->addClickEventListener([](Ref *) {
			auto scene = MainScene::createScene();
			Director::getInstance()->replaceScene(scene);
		});


		return true;
	} while (0);

	return false;
}

//初始化矩阵
void GameScene::initMatrix() {
	//获得屏幕中点
	auto VISIBLE_CENTER = VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE) / 2;
	//sp->setPosition(VISIBLE_CENTER + Vec2(0, 100));
	//this->addChild(sp);
	//1.假设最左侧的偏移就是中点
	m_positonOfLeftBottom.x = VISIBLE_CENTER.x;
	//2.如果列数十偶数
	if (MATRIX_COLUMN % 2 == 0)
	{
		//2.1.算出左侧离中点最近的精灵锚点位置
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) / 2;
		//2.1.算出左侧离中点最远的精灵锚点位置（剩下的左侧精灵宽度和间距距离左侧离中点最近的精灵锚点的距离）
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_COLUMN / 2 - 1);
	}
	else
	{
		//2.2.算出最左侧精灵的锚点（总宽度除二个精灵宽度和间距距离）
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_COLUMN / 2);
	}

	m_positonOfLeftBottom.y = VISIBLE_CENTER.y + m_iconWidth + SPACE_WIDTH;
	if (MATRIX_ROW % 2 == 0)
	{
		//2.1.算出左侧离中点最近的精灵锚点位置
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) / 2;
		//2.1.算出左侧离中点最远的精灵锚点位置（剩下的左侧精灵宽度和间距距离左侧离中点最近的精灵锚点的距离）
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_ROW / 2 - 1);
	}
	else
	{
		//2.2.算出最左侧精灵的锚点（总宽度除二个精灵宽度和间距距离）
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_ROW / 2);
	}

	do
	{
		for (auto row = 0; row < MATRIX_ROW; row++)
		{
			//3.遍历列，依次生成一行的精灵
			for (auto col = 0; col < MATRIX_COLUMN; col++)
			{
				std::vector<int> extendsIndex;
				//从第三个开始检查
				if (col > 1)
				{
					//取出前两个
					auto pre = m_matrix[row * MATRIX_COLUMN + col - 1];
					auto prepre = m_matrix[row * MATRIX_COLUMN + col - 2];
					if (pre->getImageIndex() == prepre->getImageIndex())
					{
						//如果前两个相同，则这个就不能再出现这个图标下标了
						extendsIndex.push_back(pre->getImageIndex());
					}
				}
				if (row > 1)
				{
					//取出前两个
					auto pre = m_matrix[(row - 1) * MATRIX_COLUMN + col];
					auto prepre = m_matrix[(row - 2) * MATRIX_COLUMN + col];
					if (pre->getImageIndex() == prepre->getImageIndex())
					{
						extendsIndex.push_back(pre->getImageIndex());
					}
				}

				auto icon = Icon::create(row, col, extendsIndex);

				m_matrix[row * MATRIX_COLUMN + col] = icon;
			}
		}
		checkTip();
	} while (m_tips.size() == 0);

	//执行动画
	for (auto row = 0; row < MATRIX_ROW; row++)
	{
		for (auto col = 0; col < MATRIX_COLUMN; col++)
		{
			auto icon = m_matrix[row*MATRIX_COLUMN + col];

			//计算当前精灵所在位置，先放置到屏幕之外便于实现进入动画
			auto pos = convertToPosition(row, col) + Vec2(0, VISIBLE_SIZE.height);
			icon->setPosition(pos);
			this->addChild(icon);

			changeState(Down);
			ActionInterval *action = MoveBy::create(1 + row*0.1f + col*0.02f, Vec2(0, -VISIBLE_SIZE.height));
			//==========注意1
			if (row == MATRIX_ROW-1 && col == MATRIX_COLUMN - 1)
			{
				action = Sequence::create(action, CallFunc::create([&]() {
					changeState(Check);
				}), nullptr);
			}
			icon->runAction(action);
		}
	}
}

void GameScene::checkMatrix() {
	bool needEliminate = false;
	for (auto row = 0; row < MATRIX_ROW; row++)
	{
		for (auto col = 0; col < MATRIX_COLUMN; col++)
		{
			auto icon = m_matrix[row * MATRIX_COLUMN + col];
			if (icon == nullptr) continue;
			//最右侧的图标不要向右检查
			if (col < MATRIX_COLUMN - 1)
			{
				bool res = rightCheck(icon);
				needEliminate = needEliminate ? true : res;
			}
			//最顶行的图标不要向上检查
			if (row < MATRIX_ROW - 1)
			{
				bool res = upCheck(icon);
				needEliminate = needEliminate ? true : res;
			}
		}
	}
	changeState(needEliminate ? Eliminate : Nothing);
}

bool GameScene::rightCheck(Icon *icon) {
	//统计连续相同图片的icon数量
	auto cnt = 1;
	//准备比较的图标下标
	auto compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() + 1;
	//循环要比较的图标
	while (compareIconIndex < (icon->getRow() + 1) * MATRIX_COLUMN)
	{
		//如果要比较的图标存在且图片相同则计数加一
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			cnt++;
		}
		else {
			break;
		}
		
		//比较下一个
		compareIconIndex++;
	}

	//满足三消
	if (cnt >= 3)
	{
		//cnt后面会变化，这里先记录下来
		auto count = cnt;
		while (cnt > 0)
		{
			auto index = icon->getRow() * MATRIX_COLUMN + icon->getCol() + cnt - 1;
			//如果是四消或五消，且是倒数第二个需要标记的
			if (count > 3 && cnt == count-2)
			{
				//本身只是一个普通图标，就标注为水平四消图标
				if (m_matrix[index]->getType() == NormalIcon)
				{
					m_matrix[index]->setType(HorizontalIcon);
					cnt--;
					continue;
				}
			}
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;
		}
		return true;
	}
	return false;
}

bool GameScene::upCheck(Icon *icon) {
	auto cnt = 1;
	auto compareIconIndex = (icon->getRow() + 1) * MATRIX_COLUMN + icon->getCol();
	//最上的下标应该小于当前图标所在列的最上一个的后面一个位置
	while (compareIconIndex <= (MATRIX_ROW - 1) * MATRIX_COLUMN + icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			cnt++;
		}
		else {
			break;
		}

		//比较下一个
		compareIconIndex += MATRIX_COLUMN;
	}

	if (cnt >= 3)
	{
		auto count = cnt;
		while (cnt > 0)
		{
			auto index = (icon->getRow() + cnt - 1) * MATRIX_COLUMN + icon->getCol();
			if (count > 3 && cnt == count-2)
			{
				if (m_matrix[index]->getType() == NormalIcon)
				{
					m_matrix[index]->setType(VerticalIcon);
					cnt--;
					continue;
				}
			}
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;
		}
		return true;
	}
	return false;
}

bool GameScene::horizontalCheck(Icon *icon) {
	//统计连续相同图片的icon数量
	auto leftCnt = 0;
	//准备比较的左侧图标下标
	auto compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() - 1;
	//循环要比较的图标
	while (compareIconIndex >= icon->getRow() * MATRIX_COLUMN)
	{
		//如果要比较的图标存在且图片相同则计数加一
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			leftCnt++;
		}
		else {
			break;
		}

		//比较下一个
		compareIconIndex--;
	}

	int rightCnt = 1;
	//准备比较的右侧图标下标
	compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() + 1;
	//循环要比较的图标
	while (compareIconIndex < (icon->getRow() + 1) * MATRIX_COLUMN)
	{
		//如果要比较的图标存在且图片相同则计数加一
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			rightCnt++;
		}
		else {
			break;
		}

		//比较下一个
		compareIconIndex++;
	}

	auto cnt = leftCnt + rightCnt;
	if (cnt >= 3)
	{
		//得到最左侧需要消除的图标下标
		auto index = icon->getRow() * MATRIX_COLUMN + icon->getCol() - leftCnt;
		while (cnt > 0)
		{
			//左右的总和判断是否满足四消，当cnt只剩rightCnt个图标，说明刚好遍历被检测图标自己
			if (leftCnt + rightCnt > 3 && cnt == rightCnt)
			{
				if (icon->getType() == NormalIcon)
				{
					icon->setType(HorizontalIcon);
					//当前图标算是检测过了，所以计数变量和下标要同步
					cnt--;
					index++;
					continue;
				}
			}
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;	//标记一个减少一个
			index++;	//下标向右移动
		}
		return true;
	}
	return false;
}

bool GameScene::verticalCheck(Icon *icon) {
	//1.1.向上比较
	auto upCnt = 1;
	auto compareIconIndex = (icon->getRow() + 1) * MATRIX_COLUMN + icon->getCol();
	//最上的下标应该小于当前图标所在列的最上一个的后面一个位置
	while (compareIconIndex <= (MATRIX_ROW - 1) * MATRIX_COLUMN + icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			upCnt++;
		}
		else {
			break;
		}

		//比较下一个
		compareIconIndex += MATRIX_COLUMN;
	}

	//1.2.向下比较
	auto downCnt = 0;
	compareIconIndex = (icon->getRow() - 1) * MATRIX_COLUMN + icon->getCol();
	//最上的下标应该小于当前图标所在列的最上一个的后面一个位置
	while (compareIconIndex >= icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			downCnt++;
		}
		else {
			break;
		}

		//比较下一个
		compareIconIndex -= MATRIX_COLUMN;
	}

	//2.检查并标记
	auto cnt = upCnt + downCnt;
	if (cnt >= 3)
	{
		//先找到最底部一个要消除的图标
		auto index = (icon->getRow() - downCnt) * MATRIX_COLUMN + icon->getCol();
		while (cnt > 0)
		{
			if (upCnt + downCnt > 3 && cnt == upCnt)
			{
				if (icon->getType() == NormalIcon)
				{
					icon->setType(VerticalIcon);
					cnt--;
					index += MATRIX_COLUMN;
					continue;
				}
			}
			//标记需要移除的图标
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;
			//==========注意：加整行
			index += MATRIX_COLUMN;
		}
		return true;
	}
	return false;
}

void GameScene::eliminateMatrix() {
	//用于控制音效播放类型
	bool isOne = true;
	//1.先完成特殊的图标的功能
	for (auto row = 0; row < MATRIX_ROW; row++)
	{
		for (auto col = 0; col < MATRIX_COLUMN; col++)
		{
			auto icon = m_matrix[row * MATRIX_COLUMN + col];
			if (icon == nullptr || !icon->getCanRemove()) continue;

			switch (icon->getType())
			{
			case HorizontalIcon:
			{
				for (auto sbilingCol = 0; sbilingCol < MATRIX_COLUMN; sbilingCol++)
				{
					auto sbiling = m_matrix[row * MATRIX_COLUMN + sbilingCol];
					if (sbiling->getCanMark())
					{
						sbiling->setCanRemove(true);
					}
				}
				auto leftEffect = Sprite::create("colorHLeft.png");
				auto rightEffect = Sprite::create("colorHRight.png");
				auto x = VISIBLE_ORIGIN.x + VISIBLE_SIZE.width / 2;
				auto y = m_positonOfLeftBottom.y + (m_iconWidth + SPACE_WIDTH) * row;
				leftEffect->setPosition(x, y);
				rightEffect->setPosition(x, y);
				this->addChild(leftEffect, EFFECT_ZORDER);
				this->addChild(rightEffect, EFFECT_ZORDER);

				leftEffect->setScale(3, 0.5f);
				rightEffect->setScale(3, 0.5f);
				leftEffect->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(-VISIBLE_SIZE.width, 0)), RemoveSelf::create(), nullptr));
				rightEffect->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(VISIBLE_SIZE.width, 0)), RemoveSelf::create(), nullptr));

				isOne = false;
				break;
			}
			case VerticalIcon:
			{
				for (auto sbilingRow = 0; sbilingRow < MATRIX_ROW; sbilingRow++)
				{
					auto sbiling = m_matrix[sbilingRow * MATRIX_COLUMN + col];
					if (sbiling->getCanMark())
					{
						sbiling->setCanRemove(true);
					}
				}

				//整列消除附加效果
				auto upEffect = Sprite::create("colorVUp.png");
				auto downEffect = Sprite::create("colorVDown.png");
				auto x = m_positonOfLeftBottom.x + (m_iconWidth + SPACE_WIDTH) * col;
				auto y = VISIBLE_ORIGIN.y + VISIBLE_SIZE.height / 2;
				upEffect->setPosition(x, y);
				downEffect->setPosition(x, y);
				this->addChild(upEffect, EFFECT_ZORDER);
				this->addChild(downEffect, EFFECT_ZORDER);

				upEffect->setScale(0.5f, 3);
				downEffect->setScale(0.5f, 3);
				upEffect->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(0, VISIBLE_SIZE.height)), RemoveSelf::create(), nullptr));
				downEffect->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(0, -VISIBLE_SIZE.height)), RemoveSelf::create(), nullptr));

				isOne = false;
				break;
			}
			default:
				break;
			}
		}
	}

	//2.消除被标记的图标
	//统计空格数量
	m_spaceCount = 0;
	for (auto i = 0; i < MATRIX_ROW * MATRIX_COLUMN; i++)
	{
		if (m_matrix[i] == nullptr) continue;
		m_matrix[i]->setCanMark(true);

		if (m_matrix[i]->getCanRemove())
		{
			m_totalScore += m_matrix[i]->getScore();
			m_matrix[i]->runAction(Sequence::create(ScaleTo::create(0.4f, 0), RemoveSelf::create(), nullptr));

			auto circle = Sprite::create("circle.png");
			circle->setPosition(m_matrix[i]->getPosition());
			circle->setScale(0.1f);
			this->addChild(circle, CIRCLE_ZORDER, CIRCLE_TAG);
			circle->runAction(Sequence::create(ScaleTo::create(0.4f, 1), RemoveSelf::create(), CallFuncN::create([&](Node *node) {
				//==========注意2
				if (this->getChildByTag(CIRCLE_TAG) == nullptr)
				{
					changeState(CreateAndDown);
				}
			}), nullptr));

			m_matrix[i] = nullptr;
			m_spaceCount++;
		}
	}

	//更新分值
	auto lblScore = dynamic_cast<Label *>(getChildByTag(LABEL_SCORE_TAG));
	lblScore->setString(StringUtils::format("%d", m_totalScore));

	if (isOne)
	{
		SimpleAudioEngine::getInstance()->playEffect("music_explode.wav");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("music_explodeOne.wav");
	}
}

void GameScene::createAndDownIcon() {
	//移位
	for (auto col = 0; col < MATRIX_COLUMN; col++)
	{
		auto spaceCnt = 0;
		for (auto row = 0; row < MATRIX_ROW; row++)
		{
			auto icon = m_matrix[row * MATRIX_COLUMN + col];
			if (icon == nullptr)
			{
				spaceCnt++;
			}
			else if(spaceCnt > 0) {
				//移动矩阵
				auto newRow = icon->getRow() - spaceCnt;
				m_matrix[newRow * MATRIX_COLUMN + icon->getCol()] = icon;
				m_matrix[icon->getRow() * MATRIX_COLUMN + icon->getCol()] = nullptr;
				//更新行号
				icon->setRow(newRow);

				//移动画面
				auto oldPos = icon->getPosition();
				auto newPos = convertToPosition(newRow, col);
				auto time = (oldPos - newPos).y / VISIBLE_SIZE.height;
				icon->runAction(MoveTo::create(time, newPos));
			}
		}
		
		//创建新图标（补缺）
		while (spaceCnt > 0) {
			auto row = MATRIX_ROW - spaceCnt;
			auto icon = Icon::create(row, col);
			icon->setPosition(convertToPosition(row, col) + Vec2(0, VISIBLE_SIZE.height));
			this->addChild(icon);
			auto index = row * MATRIX_COLUMN + col;
			m_matrix[index] = icon;

			//==========注意4
			auto action = Sequence::create(MoveBy::create(1, Vec2(0, -VISIBLE_SIZE.height)), CallFunc::create([&]() {
				m_spaceCount--;
				if (m_spaceCount == 0)
				{
					changeState(Check);
				}
			}), nullptr);
			icon->runAction(action);

			spaceCnt--;
		}
	}
}

Vec2 GameScene::convertToPosition(int row, int col) {
	Vec2 pos;
	//以最左侧精灵坐标为参照，计算当前精灵所在位置
	pos.x = m_positonOfLeftBottom.x + (m_iconWidth + SPACE_WIDTH) * col;
	pos.y = m_positonOfLeftBottom.y + (m_iconWidth + SPACE_WIDTH) * row;
	return pos;
}

void GameScene::up() {
	//1.如果是顶行则无需交换
	if (m_icon->getRow() == MATRIX_ROW - 1) return;

	//获得希望交换的图标
	auto otherIndex = (m_icon->getRow() + 1) * MATRIX_COLUMN + m_icon->getCol();
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;

	//=========注意：2.交换双方在矩阵中的位置
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex - MATRIX_COLUMN] = other;
	//交换双方的行号
	m_icon->setRow(m_icon->getRow() + 1);
	other->setRow(other->getRow() - 1);

	//3.检查移动之后是否满足三消
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	//4.根据检测结果进行相应的处理
	if (isEliminate)
	{
		//4.1.有消除，说明操作是对的，只需执行交换位置动画再执行消除
		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(MoveTo::create(0.5f, otherPos));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Eliminate);
		}), nullptr));
	}
	else
	{
		//4.2.如果没有消除，说明操作是无效
		//4.2.1还原双方在矩阵中的位置
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex - MATRIX_COLUMN] = m_icon;
		m_icon->setRow(m_icon->getRow() - 1);
		other->setRow(other->getRow() + 1);

		//4.2.2执行交换动画，以提醒用户
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(Sequence::create(MoveTo::create(0.5f, otherPos), MoveTo::create(0.5f, iconPos), nullptr));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), MoveTo::create(0.5f, otherPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Nothing);
		}), nullptr));

		SimpleAudioEngine::getInstance()->playEffect("music_fail.mp3");
	}
}

void GameScene::down() {
	//如果是顶行则无需交换
	if (m_icon->getRow() == 0) return;
	//获得希望交换的图标
	auto otherIndex = (m_icon->getRow() - 1) * MATRIX_COLUMN + m_icon->getCol();
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========注意：交换双方在矩阵中的位置
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex + MATRIX_COLUMN] = other;
	//交换双方的行号
	m_icon->setRow(m_icon->getRow() - 1);
	other->setRow(other->getRow() + 1);

	//检查移动之后是否满足三消
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(MoveTo::create(0.5f, otherPos));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Eliminate);
		}), nullptr));
	}
	else
	{
		//交换双方在矩阵中的位置
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex + MATRIX_COLUMN] = m_icon;
		m_icon->setRow(m_icon->getRow() + 1);
		other->setRow(other->getRow() - 1);

		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(Sequence::create(MoveTo::create(0.5f, otherPos), MoveTo::create(0.5f, iconPos), nullptr));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), MoveTo::create(0.5f, otherPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Nothing);
		}), nullptr));

		SimpleAudioEngine::getInstance()->playEffect("music_fail.mp3");
	}
}

void GameScene::left() {
	//如果是顶行则无需交换
	if (m_icon->getCol() == 0) return;
	//获得希望交换的图标
	auto otherIndex = m_icon->getRow() * MATRIX_COLUMN + m_icon->getCol() - 1;
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========注意：交换双方在矩阵中的位置
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex + 1] = other;
	//交换双方的行号
	m_icon->setCol(m_icon->getCol() - 1);
	other->setCol(other->getCol() + 1);

	//检查移动之后是否满足三消
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(MoveTo::create(0.5f, otherPos));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Eliminate);
		}), nullptr));
	}
	else
	{
		//交换双方在矩阵中的位置
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex + 1] = m_icon;
		m_icon->setCol(m_icon->getCol() + 1);
		other->setCol(other->getCol() - 1);

		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(Sequence::create(MoveTo::create(0.5f, otherPos), MoveTo::create(0.5f, iconPos), nullptr));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), MoveTo::create(0.5f, otherPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Nothing);
		}), nullptr));

		SimpleAudioEngine::getInstance()->playEffect("music_fail.mp3");
	}
}

void GameScene::right() {
	//如果是顶行则无需交换
	if (m_icon->getCol() == MATRIX_COLUMN - 1) return;
	//获得希望交换的图标
	auto otherIndex = m_icon->getRow() * MATRIX_COLUMN + m_icon->getCol() + 1;
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========注意：交换双方在矩阵中的位置
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex - 1] = other;
	//交换双方的行号
	m_icon->setCol(m_icon->getCol() + 1);
	other->setCol(other->getCol() - 1);

	//检查移动之后是否满足三消
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(MoveTo::create(0.5f, otherPos));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Eliminate);
		}), nullptr));
	}
	else
	{
		//交换双方在矩阵中的位置
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex - 1] = m_icon;
		m_icon->setCol(m_icon->getCol() - 1);
		other->setCol(other->getCol() + 1);

		//通过动画交换双方在层中的位置
		changeState(Cross);
		auto iconPos = m_icon->getPosition();
		auto otherPos = other->getPosition();
		m_icon->runAction(Sequence::create(MoveTo::create(0.5f, otherPos), MoveTo::create(0.5f, iconPos), nullptr));
		other->runAction(Sequence::create(MoveTo::create(0.5f, iconPos), MoveTo::create(0.5f, otherPos), CallFunc::create([&]() {
			m_icon = nullptr;
			changeState(Nothing);
		}), nullptr));

		SimpleAudioEngine::getInstance()->playEffect("music_fail.mp3");
	}
}

void GameScene::changeState(const IconState &state) {
	//如果新状态与之前相同则不作处理
	if (m_state == state) return;
	m_state = state;
	switch (state)
	{
	case Check:
		checkMatrix();
		break;
	case Eliminate:
		eliminateMatrix();
		break;
	case CreateAndDown:
		createAndDownIcon();
		break;
	case Nothing:
		checkTip();
		if (m_tips.empty())
		{
			changeState(Over);
		}
		else {
			scheduleOnce(schedule_selector(GameScene::tip), TIP_INTERVAL);
		}
		break;
	case Over:
	{
		auto spOver = Sprite::create("pic_gameover.png");
		spOver->setPosition(VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE)/2 );
		spOver->setScale(0.2f);
		this->addChild(spOver, INFORMATION_ZORDER);
		spOver->runAction(Sequence::create(Spawn::create(RotateBy::create(1, 345), ScaleTo::create(1, 2), nullptr),
			CallFunc::create([&]() {
			auto scene = OverScene::createScene(m_totalScore);
			Director::getInstance()->replaceScene(scene);
		}), nullptr));
	}
		break;
	default:
		break;
	}
}

void GameScene::checkTip() {
	m_tips.clear();
	for (auto row = 0; row < MATRIX_ROW; row++)
	{
		for (auto col = 0; col < MATRIX_COLUMN; col++)
		{
			//当前图标
			auto currentIcon = m_matrix[row * MATRIX_COLUMN + col];
			m_tips.pushBack(currentIcon);
			//比较右侧图标
			if (col + 1 < MATRIX_COLUMN)
			{
				auto rightIcon = m_matrix[row * MATRIX_COLUMN + (col + 1)];
				if (currentIcon->getImageIndex() == rightIcon->getImageIndex())
				{
					m_tips.pushBack(rightIcon);
					//左侧
					if (col - 2 >= 0)
					{
						auto thirdIcon = m_matrix[row * MATRIX_COLUMN + (col - 2)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (col - 1 >= 0)
					{
						//左上
						if (row + 1 < MATRIX_ROW)
						{
							auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//左下
						if (row - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}

					//右侧
					if (col + 3 < MATRIX_COLUMN)
					{
						auto thirdIcon = m_matrix[row * MATRIX_COLUMN + (col + 3)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (col + 2 < MATRIX_COLUMN)
					{
						//右上
						if (row + 1 < MATRIX_ROW)
						{
							auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col + 2)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//右下
						if (row - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col + 2)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}
				}

				//走到这里表示上面推导都不成立，还原集合为一个
				if (m_tips.size() == 2)
				{
					m_tips.popBack();
				}
			}

			//比较上侧的图标
			if (row + 1 < MATRIX_ROW)
			{
				auto upIcon = m_matrix[(row + 1) * MATRIX_COLUMN + col];
				if (currentIcon->getImageIndex() == upIcon->getImageIndex()) {
					m_tips.pushBack(upIcon);
					//下侧
					if (row - 2 >= 0)
					{
						auto thirdIcon = m_matrix[(row - 2) * MATRIX_COLUMN + col];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (row - 1 >= 0)
					{
						//右下
						if (col + 1 < MATRIX_COLUMN)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col + 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//左下
						if (col - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}

					//上侧
					if (row + 3 < MATRIX_ROW)
					{
						auto thirdIcon = m_matrix[(row + 3) * MATRIX_COLUMN + col];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (row + 2 < MATRIX_ROW)
					{
						//右上
						if (col + 1 < MATRIX_COLUMN)
						{
							auto thirdIcon = m_matrix[(row + 2) * MATRIX_COLUMN + (col + 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//左上
						if (col - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row + 2) * MATRIX_COLUMN + (col -1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}
				}

				if (m_tips.size() == 2)
				{
					m_tips.popBack();
				}
			}

			//比较水平菱形
			if (col + 2 < MATRIX_COLUMN)
			{
				auto rightIcon = m_matrix[row * MATRIX_COLUMN + (col + 2)];
				if (currentIcon->getImageIndex() == rightIcon->getImageIndex()) {
					m_tips.pushBack(rightIcon);
					if (row - 1 >= 0)
					{
						auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col + 1)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (row + 1 < MATRIX_ROW)
					{
						auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col + 1)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
				}

				if (m_tips.size() == 2)
				{
					m_tips.popBack();
				}

			}

			//比较垂直菱形
			if (row + 2 < MATRIX_ROW)
			{
				auto upIcon = m_matrix[(row + 2) * MATRIX_COLUMN + col];
				if (currentIcon->getImageIndex() == upIcon->getImageIndex()) {
					m_tips.pushBack(upIcon);
					if (col - 1 >= 0)
					{
						auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col - 1)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
					if (col + 1 < MATRIX_COLUMN)
					{
						auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col + 1)];
						if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
							m_tips.pushBack(thirdIcon);
							return;
						}
					}
				}
			}

			//走到这里说明十六种三消可能都不成立
			m_tips.clear();
		}
	}
}

void GameScene::tip(float) {
	if (m_tips.size() == 0) return;
	for (auto icon : m_tips)
	{
		auto scale1 = ScaleTo::create(0.2f, 0.8f);
		auto scale2 = ScaleTo::create(0.2f, 1.3f);
		auto scale3 = ScaleTo::create(0.2f, 1);
		auto repeat = Repeat::create(Sequence::create(scale1, scale2, scale3, nullptr), 2);
		auto forever = RepeatForever::create(Sequence::create(repeat, DelayTime::create(1), nullptr));
		forever->setTag(FOREVER_ACTION_TAG);
		icon->runAction(forever);
	}
}

void GameScene::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
	SimpleAudioEngine::getInstance()->playBackgroundMusic(GAME_BACKGROUND_MUSIC, true);
}

void GameScene::onExitTransitionDidStart() {
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	{
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
	Layer::onExitTransitionDidStart();
}
