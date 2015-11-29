#include "Icon.h"
#include "CommonData.h"

Icon::Icon() :m_row(0), m_col(0)
, m_imageIndex(0), m_canRemove(false)
, m_type(NormalIcon), m_canMark(true)
, m_score(5){}

Icon *Icon::create() {
	return create(0, 0);
}

Icon *Icon::create(int row, int col) {
	std::vector<int> extends;
	return create(row, col, extends);
}

Icon *Icon::create(int row, int col, const std::vector<int> &extends) {
	auto icon = new Icon();
	if (icon && icon->initWithRowAndCol(row, col, extends))
	{
		icon->autorelease();
		return icon;
	}
	CC_SAFE_DELETE(icon);
	return nullptr;
}

//�����������ǲ��ܱ�ȡ����ͼ���±�ļ��ϣ�����ж��֣�����Ϊ�գ�
bool Icon::initWithRowAndCol(int row, int col, const std::vector<int> &extends) {
	do
	{
		//��������ʼ����������ͼ���±�ļ���
		std::vector<int> all;
		//�������п��ܵ�ͼ���±�
		for (auto i = 0; i < ICON_TYPE_COUNT; i++)
		{
			//���費�ᱻ�ų����ϰ���
			bool contains = false;
			//�����ų����ϣ�������ڵ�ͼ���±��뼯������һ��Ա��ͬ�����ܱ���ӵ�all��
			for (auto index : extends) {
				if ((i + 1) == index) {
					contains = true;
				}
			}
			if (contains) continue;
			all.push_back(i+1);
		}
		//�ٴ�all�������ȡ�±�
		auto randIndex = random(0, int(all.size()-1));
		m_imageIndex = all.at(randIndex);

		CC_BREAK_IF(!Sprite::initWithSpriteFrameName(StringUtils::format("icon%d.png", m_imageIndex)));

		this->m_row = row;
		this->m_col = col;

		return true;
	} while (0);
	return false;
}

void Icon::setType(const IconType &type) {
	if (m_type == type) return;
	m_type = type;
	switch (type)
	{
	case HorizontalIcon:
		setSpriteFrame(StringUtils::format("icon%d_h.png", m_imageIndex));
		m_canMark = false;
		m_score *= 3;
		break;
	case VerticalIcon:
		setSpriteFrame(StringUtils::format("icon%d_z.png", m_imageIndex));
		m_canMark = false;
		m_score *= 3;
		break;
	default:
		break;
	}
}
