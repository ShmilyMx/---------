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

		//��ʼ������
		initMatrix();

		//��Ӵ����¼�����
		auto listener = EventListenerTouchOneByOne::create();

		listener->onTouchBegan = [&](Touch *touch, Event *) {
			//ѭ�����û�н����Ͳ�����Ӧ����
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

			//ֹͣ��ʾ����
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

		//��ӷ�ֵ��ʾ��ǩ
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

//��ʼ������
void GameScene::initMatrix() {
	//�����Ļ�е�
	auto VISIBLE_CENTER = VISIBLE_ORIGIN + Vec2(VISIBLE_SIZE) / 2;
	//sp->setPosition(VISIBLE_CENTER + Vec2(0, 100));
	//this->addChild(sp);
	//1.����������ƫ�ƾ����е�
	m_positonOfLeftBottom.x = VISIBLE_CENTER.x;
	//2.�������ʮż��
	if (MATRIX_COLUMN % 2 == 0)
	{
		//2.1.���������е�����ľ���ê��λ��
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) / 2;
		//2.1.���������е���Զ�ľ���ê��λ�ã�ʣ�µ���ྫ���Ⱥͼ�����������е�����ľ���ê��ľ��룩
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_COLUMN / 2 - 1);
	}
	else
	{
		//2.2.�������ྫ���ê�㣨�ܿ�ȳ����������Ⱥͼ����룩
		m_positonOfLeftBottom.x -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_COLUMN / 2);
	}

	m_positonOfLeftBottom.y = VISIBLE_CENTER.y + m_iconWidth + SPACE_WIDTH;
	if (MATRIX_ROW % 2 == 0)
	{
		//2.1.���������е�����ľ���ê��λ��
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) / 2;
		//2.1.���������е���Զ�ľ���ê��λ�ã�ʣ�µ���ྫ���Ⱥͼ�����������е�����ľ���ê��ľ��룩
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_ROW / 2 - 1);
	}
	else
	{
		//2.2.�������ྫ���ê�㣨�ܿ�ȳ����������Ⱥͼ����룩
		m_positonOfLeftBottom.y -= (m_iconWidth + SPACE_WIDTH) * (MATRIX_ROW / 2);
	}

	do
	{
		for (auto row = 0; row < MATRIX_ROW; row++)
		{
			//3.�����У���������һ�еľ���
			for (auto col = 0; col < MATRIX_COLUMN; col++)
			{
				std::vector<int> extendsIndex;
				//�ӵ�������ʼ���
				if (col > 1)
				{
					//ȡ��ǰ����
					auto pre = m_matrix[row * MATRIX_COLUMN + col - 1];
					auto prepre = m_matrix[row * MATRIX_COLUMN + col - 2];
					if (pre->getImageIndex() == prepre->getImageIndex())
					{
						//���ǰ������ͬ��������Ͳ����ٳ������ͼ���±���
						extendsIndex.push_back(pre->getImageIndex());
					}
				}
				if (row > 1)
				{
					//ȡ��ǰ����
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

	//ִ�ж���
	for (auto row = 0; row < MATRIX_ROW; row++)
	{
		for (auto col = 0; col < MATRIX_COLUMN; col++)
		{
			auto icon = m_matrix[row*MATRIX_COLUMN + col];

			//���㵱ǰ��������λ�ã��ȷ��õ���Ļ֮�����ʵ�ֽ��붯��
			auto pos = convertToPosition(row, col) + Vec2(0, VISIBLE_SIZE.height);
			icon->setPosition(pos);
			this->addChild(icon);

			changeState(Down);
			ActionInterval *action = MoveBy::create(1 + row*0.1f + col*0.02f, Vec2(0, -VISIBLE_SIZE.height));
			//==========ע��1
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
			//���Ҳ��ͼ�겻Ҫ���Ҽ��
			if (col < MATRIX_COLUMN - 1)
			{
				bool res = rightCheck(icon);
				needEliminate = needEliminate ? true : res;
			}
			//��е�ͼ�겻Ҫ���ϼ��
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
	//ͳ��������ͬͼƬ��icon����
	auto cnt = 1;
	//׼���Ƚϵ�ͼ���±�
	auto compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() + 1;
	//ѭ��Ҫ�Ƚϵ�ͼ��
	while (compareIconIndex < (icon->getRow() + 1) * MATRIX_COLUMN)
	{
		//���Ҫ�Ƚϵ�ͼ�������ͼƬ��ͬ�������һ
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			cnt++;
		}
		else {
			break;
		}
		
		//�Ƚ���һ��
		compareIconIndex++;
	}

	//��������
	if (cnt >= 3)
	{
		//cnt�����仯�������ȼ�¼����
		auto count = cnt;
		while (cnt > 0)
		{
			auto index = icon->getRow() * MATRIX_COLUMN + icon->getCol() + cnt - 1;
			//��������������������ǵ����ڶ�����Ҫ��ǵ�
			if (count > 3 && cnt == count-2)
			{
				//����ֻ��һ����ͨͼ�꣬�ͱ�עΪˮƽ����ͼ��
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
	//���ϵ��±�Ӧ��С�ڵ�ǰͼ�������е�����һ���ĺ���һ��λ��
	while (compareIconIndex <= (MATRIX_ROW - 1) * MATRIX_COLUMN + icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			cnt++;
		}
		else {
			break;
		}

		//�Ƚ���һ��
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
	//ͳ��������ͬͼƬ��icon����
	auto leftCnt = 0;
	//׼���Ƚϵ����ͼ���±�
	auto compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() - 1;
	//ѭ��Ҫ�Ƚϵ�ͼ��
	while (compareIconIndex >= icon->getRow() * MATRIX_COLUMN)
	{
		//���Ҫ�Ƚϵ�ͼ�������ͼƬ��ͬ�������һ
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			leftCnt++;
		}
		else {
			break;
		}

		//�Ƚ���һ��
		compareIconIndex--;
	}

	int rightCnt = 1;
	//׼���Ƚϵ��Ҳ�ͼ���±�
	compareIconIndex = icon->getRow() * MATRIX_COLUMN + icon->getCol() + 1;
	//ѭ��Ҫ�Ƚϵ�ͼ��
	while (compareIconIndex < (icon->getRow() + 1) * MATRIX_COLUMN)
	{
		//���Ҫ�Ƚϵ�ͼ�������ͼƬ��ͬ�������һ
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			rightCnt++;
		}
		else {
			break;
		}

		//�Ƚ���һ��
		compareIconIndex++;
	}

	auto cnt = leftCnt + rightCnt;
	if (cnt >= 3)
	{
		//�õ��������Ҫ������ͼ���±�
		auto index = icon->getRow() * MATRIX_COLUMN + icon->getCol() - leftCnt;
		while (cnt > 0)
		{
			//���ҵ��ܺ��ж��Ƿ�������������cntֻʣrightCnt��ͼ�꣬˵���պñ��������ͼ���Լ�
			if (leftCnt + rightCnt > 3 && cnt == rightCnt)
			{
				if (icon->getType() == NormalIcon)
				{
					icon->setType(HorizontalIcon);
					//��ǰͼ�����Ǽ����ˣ����Լ����������±�Ҫͬ��
					cnt--;
					index++;
					continue;
				}
			}
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;	//���һ������һ��
			index++;	//�±������ƶ�
		}
		return true;
	}
	return false;
}

bool GameScene::verticalCheck(Icon *icon) {
	//1.1.���ϱȽ�
	auto upCnt = 1;
	auto compareIconIndex = (icon->getRow() + 1) * MATRIX_COLUMN + icon->getCol();
	//���ϵ��±�Ӧ��С�ڵ�ǰͼ�������е�����һ���ĺ���һ��λ��
	while (compareIconIndex <= (MATRIX_ROW - 1) * MATRIX_COLUMN + icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			upCnt++;
		}
		else {
			break;
		}

		//�Ƚ���һ��
		compareIconIndex += MATRIX_COLUMN;
	}

	//1.2.���±Ƚ�
	auto downCnt = 0;
	compareIconIndex = (icon->getRow() - 1) * MATRIX_COLUMN + icon->getCol();
	//���ϵ��±�Ӧ��С�ڵ�ǰͼ�������е�����һ���ĺ���һ��λ��
	while (compareIconIndex >= icon->getCol())
	{
		if (m_matrix[compareIconIndex] && icon->getImageIndex() == m_matrix[compareIconIndex]->getImageIndex())
		{
			downCnt++;
		}
		else {
			break;
		}

		//�Ƚ���һ��
		compareIconIndex -= MATRIX_COLUMN;
	}

	//2.��鲢���
	auto cnt = upCnt + downCnt;
	if (cnt >= 3)
	{
		//���ҵ���ײ�һ��Ҫ������ͼ��
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
			//�����Ҫ�Ƴ���ͼ��
			if (m_matrix[index]->getCanMark())
			{
				m_matrix[index]->setCanRemove(true);
			}
			cnt--;
			//==========ע�⣺������
			index += MATRIX_COLUMN;
		}
		return true;
	}
	return false;
}

void GameScene::eliminateMatrix() {
	//���ڿ�����Ч��������
	bool isOne = true;
	//1.����������ͼ��Ĺ���
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

				//������������Ч��
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

	//2.��������ǵ�ͼ��
	//ͳ�ƿո�����
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
				//==========ע��2
				if (this->getChildByTag(CIRCLE_TAG) == nullptr)
				{
					changeState(CreateAndDown);
				}
			}), nullptr));

			m_matrix[i] = nullptr;
			m_spaceCount++;
		}
	}

	//���·�ֵ
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
	//��λ
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
				//�ƶ�����
				auto newRow = icon->getRow() - spaceCnt;
				m_matrix[newRow * MATRIX_COLUMN + icon->getCol()] = icon;
				m_matrix[icon->getRow() * MATRIX_COLUMN + icon->getCol()] = nullptr;
				//�����к�
				icon->setRow(newRow);

				//�ƶ�����
				auto oldPos = icon->getPosition();
				auto newPos = convertToPosition(newRow, col);
				auto time = (oldPos - newPos).y / VISIBLE_SIZE.height;
				icon->runAction(MoveTo::create(time, newPos));
			}
		}
		
		//������ͼ�꣨��ȱ��
		while (spaceCnt > 0) {
			auto row = MATRIX_ROW - spaceCnt;
			auto icon = Icon::create(row, col);
			icon->setPosition(convertToPosition(row, col) + Vec2(0, VISIBLE_SIZE.height));
			this->addChild(icon);
			auto index = row * MATRIX_COLUMN + col;
			m_matrix[index] = icon;

			//==========ע��4
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
	//������ྫ������Ϊ���գ����㵱ǰ��������λ��
	pos.x = m_positonOfLeftBottom.x + (m_iconWidth + SPACE_WIDTH) * col;
	pos.y = m_positonOfLeftBottom.y + (m_iconWidth + SPACE_WIDTH) * row;
	return pos;
}

void GameScene::up() {
	//1.����Ƕ��������轻��
	if (m_icon->getRow() == MATRIX_ROW - 1) return;

	//���ϣ��������ͼ��
	auto otherIndex = (m_icon->getRow() + 1) * MATRIX_COLUMN + m_icon->getCol();
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;

	//=========ע�⣺2.����˫���ھ����е�λ��
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex - MATRIX_COLUMN] = other;
	//����˫�����к�
	m_icon->setRow(m_icon->getRow() + 1);
	other->setRow(other->getRow() - 1);

	//3.����ƶ�֮���Ƿ���������
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	//4.���ݼ����������Ӧ�Ĵ���
	if (isEliminate)
	{
		//4.1.��������˵�������ǶԵģ�ֻ��ִ�н���λ�ö�����ִ������
		//ͨ����������˫���ڲ��е�λ��
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
		//4.2.���û��������˵����������Ч
		//4.2.1��ԭ˫���ھ����е�λ��
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex - MATRIX_COLUMN] = m_icon;
		m_icon->setRow(m_icon->getRow() - 1);
		other->setRow(other->getRow() + 1);

		//4.2.2ִ�н����������������û�
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
	//����Ƕ��������轻��
	if (m_icon->getRow() == 0) return;
	//���ϣ��������ͼ��
	auto otherIndex = (m_icon->getRow() - 1) * MATRIX_COLUMN + m_icon->getCol();
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========ע�⣺����˫���ھ����е�λ��
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex + MATRIX_COLUMN] = other;
	//����˫�����к�
	m_icon->setRow(m_icon->getRow() - 1);
	other->setRow(other->getRow() + 1);

	//����ƶ�֮���Ƿ���������
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//ͨ����������˫���ڲ��е�λ��
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
		//����˫���ھ����е�λ��
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex + MATRIX_COLUMN] = m_icon;
		m_icon->setRow(m_icon->getRow() + 1);
		other->setRow(other->getRow() - 1);

		//ͨ����������˫���ڲ��е�λ��
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
	//����Ƕ��������轻��
	if (m_icon->getCol() == 0) return;
	//���ϣ��������ͼ��
	auto otherIndex = m_icon->getRow() * MATRIX_COLUMN + m_icon->getCol() - 1;
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========ע�⣺����˫���ھ����е�λ��
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex + 1] = other;
	//����˫�����к�
	m_icon->setCol(m_icon->getCol() - 1);
	other->setCol(other->getCol() + 1);

	//����ƶ�֮���Ƿ���������
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//ͨ����������˫���ڲ��е�λ��
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
		//����˫���ھ����е�λ��
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex + 1] = m_icon;
		m_icon->setCol(m_icon->getCol() + 1);
		other->setCol(other->getCol() - 1);

		//ͨ����������˫���ڲ��е�λ��
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
	//����Ƕ��������轻��
	if (m_icon->getCol() == MATRIX_COLUMN - 1) return;
	//���ϣ��������ͼ��
	auto otherIndex = m_icon->getRow() * MATRIX_COLUMN + m_icon->getCol() + 1;
	auto other = m_matrix[otherIndex];
	if (other == nullptr) return;
	//=========ע�⣺����˫���ھ����е�λ��
	m_matrix[otherIndex] = m_icon;
	m_matrix[otherIndex - 1] = other;
	//����˫�����к�
	m_icon->setCol(m_icon->getCol() + 1);
	other->setCol(other->getCol() - 1);

	//����ƶ�֮���Ƿ���������
	bool verticalEliminate = verticalCheck(m_icon);
	bool horizontalEliminate = horizontalCheck(m_icon);
	bool vEliminate = verticalCheck(other);
	bool hEliminate = horizontalCheck(other);

	bool isEliminate = verticalEliminate || horizontalEliminate || vEliminate || hEliminate;

	if (isEliminate)
	{
		//ͨ����������˫���ڲ��е�λ��
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
		//����˫���ھ����е�λ��
		m_matrix[otherIndex] = other;
		m_matrix[otherIndex - 1] = m_icon;
		m_icon->setCol(m_icon->getCol() - 1);
		other->setCol(other->getCol() + 1);

		//ͨ����������˫���ڲ��е�λ��
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
	//�����״̬��֮ǰ��ͬ��������
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
			//��ǰͼ��
			auto currentIcon = m_matrix[row * MATRIX_COLUMN + col];
			m_tips.pushBack(currentIcon);
			//�Ƚ��Ҳ�ͼ��
			if (col + 1 < MATRIX_COLUMN)
			{
				auto rightIcon = m_matrix[row * MATRIX_COLUMN + (col + 1)];
				if (currentIcon->getImageIndex() == rightIcon->getImageIndex())
				{
					m_tips.pushBack(rightIcon);
					//���
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
						//����
						if (row + 1 < MATRIX_ROW)
						{
							auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//����
						if (row - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}

					//�Ҳ�
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
						//����
						if (row + 1 < MATRIX_ROW)
						{
							auto thirdIcon = m_matrix[(row + 1) * MATRIX_COLUMN + (col + 2)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//����
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

				//�ߵ������ʾ�����Ƶ�������������ԭ����Ϊһ��
				if (m_tips.size() == 2)
				{
					m_tips.popBack();
				}
			}

			//�Ƚ��ϲ��ͼ��
			if (row + 1 < MATRIX_ROW)
			{
				auto upIcon = m_matrix[(row + 1) * MATRIX_COLUMN + col];
				if (currentIcon->getImageIndex() == upIcon->getImageIndex()) {
					m_tips.pushBack(upIcon);
					//�²�
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
						//����
						if (col + 1 < MATRIX_COLUMN)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col + 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//����
						if (col - 1 >= 0)
						{
							auto thirdIcon = m_matrix[(row - 1) * MATRIX_COLUMN + (col - 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
					}

					//�ϲ�
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
						//����
						if (col + 1 < MATRIX_COLUMN)
						{
							auto thirdIcon = m_matrix[(row + 2) * MATRIX_COLUMN + (col + 1)];
							if (currentIcon->getImageIndex() == thirdIcon->getImageIndex()) {
								m_tips.pushBack(thirdIcon);
								return;
							}
						}
						//����
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

			//�Ƚ�ˮƽ����
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

			//�Ƚϴ�ֱ����
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

			//�ߵ�����˵��ʮ�����������ܶ�������
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
