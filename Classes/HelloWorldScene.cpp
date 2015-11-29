#include "HelloWorldScene.h"
#include"CommandDate.h"

USING_NS_CC;

HelloWorld::HelloWorld() {
	m_currentStageIndex = 0;
	m_heroMoving = false;
}

Scene *HelloWorld::createScene() {
	auto layer = HelloWorld::create();
	auto scene = Scene::create();
	scene->addChild(layer);
	return scene;
}

bool HelloWorld::init() {
	do
	{
		CC_BREAK_IF(!Layer::init());

		//添加背景
		int bgIndex = 2;
		auto bg = Sprite::create(StringUtils::format("bg%d.jpg", bgIndex));
		bg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		this->addChild(bg, -1);

		//添加月亮
		if (bgIndex == 2)
		{
			auto moon = Sprite::create("moon.png");
			moon->setPosition(-moon->getContentSize().width / 2, visibleSize.height / 2);
			this->addChild(moon);

			ccBezierConfig config;
			config.controlPoint_1 = Vec2(0, visibleSize.height);
			config.controlPoint_2 = Vec2(visibleSize);
			config.endPosition = Vec2(visibleSize.width + moon->getContentSize().width / 2, visibleSize.height / 3 * 2);
			moon->runAction(BezierTo::create(360, config));
		}

		//添加柱子
		initStage();

		//添加英雄
		auto heroIndex = random(1, 4);
		auto hero = Sprite::create(StringUtils::format("hero%d.png", heroIndex));
		hero->setPosition(m_stages.at(m_currentStageIndex)->getPositionX(), m_stages.at(m_currentStageIndex)->getContentSize().height + hero->getContentSize().height / 2);
		this->addChild(hero, 1);

		auto kickAni = Animation::create();
		auto stayAni = Animation::create();
		auto walkAni = Animation::create();
		for (int i = 0; i < 5; i++)
		{
			auto kickPng = StringUtils::format("anim%d/kick%d.png", heroIndex, i + 1);
			auto stayPng = StringUtils::format("anim%d/stay%d.png", heroIndex, i + 1);
			auto walkPng = StringUtils::format("anim%d/walk%d.png", heroIndex, i + 1);
			kickAni->addSpriteFrameWithFileName(kickPng);
			stayAni->addSpriteFrameWithFileName(stayPng);
			walkAni->addSpriteFrameWithFileName(walkPng);
		}
		kickAni->setDelayPerUnit(0.2f);
		stayAni->setDelayPerUnit(0.2f);
		walkAni->setDelayPerUnit(0.2f);
		kickAni->setLoops(-1);
		stayAni->setLoops(-1);
		walkAni->setLoops(-1);

		AnimationCache::getInstance()->addAnimation(kickAni, "KICK_ANIMATION");
		AnimationCache::getInstance()->addAnimation(stayAni, "STAY_ANIMATION");
		AnimationCache::getInstance()->addAnimation(walkAni, "WALK_ANIMATION");

		hero->runAction(Animate::create(stayAni));

		//触屏长棍子
		auto stick = Sprite::create("stick1.png");
		stick->setScaleX(4);
		stick->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
		stick->setPosition(m_stages.at(0)->getPositionX() + m_stages.at(0)->getContentSize().width * m_stages.at(0)->getScaleX() / 2, m_stages.at(0)->getContentSize().height - stick->getContentSize().width * stick->getScaleX());
		this->addChild(stick, 1);
		stick->setVisible(false);

		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [=](Touch *, Event *) {
			if (m_heroMoving) return false;
			stick->setRotation(0);
			stick->setScaleY(1);
			stick->setPositionX(m_stages.at(m_currentStageIndex)->getPositionX() + m_stages.at(m_currentStageIndex)->getContentSize().width * m_stages.at(m_currentStageIndex)->getScaleX() / 2);
			stick->setVisible(true);
			stick->runAction(EaseIn::create(ScaleTo::create(1, 4, 200), 2));
			return true;
		};
		listener->onTouchEnded = [=](Touch *, Event *) {
			stick->stopAllActions();
			m_heroMoving = true;
			auto rotateStick = RotateBy::create(0.5f, 90);
			auto moveCallFunc = CallFunc::create([=]() {
				auto nextIndex = (m_currentStageIndex + 1) % 3;
				auto nextX = m_stages.at(nextIndex)->getPositionX() - m_stages.at(m_currentStageIndex)->getPositionX();
				auto stickLength = stick->getContentSize().height*stick->getScaleY();
				auto minX = nextX - (m_stages.at(nextIndex)->getContentSize().width * m_stages.at(nextIndex)->getScaleX()) / 2
					- (m_stages.at(m_currentStageIndex)->getContentSize().width * m_stages.at(m_currentStageIndex)->getScaleX()) / 2;
				auto maxX = minX + (m_stages.at(nextIndex)->getContentSize().width * m_stages.at(nextIndex)->getScaleX());
				if (stickLength < minX || stickLength > maxX)
				{
					auto heroX = (m_stages.at(m_currentStageIndex)->getContentSize().width * m_stages.at(m_currentStageIndex)->getScaleX()) / 2 + stickLength;
					auto move1 = MoveBy::create(1, Vec2(heroX, 0));
					auto rotate = RotateBy::create(0.01f, 90);
					auto move2 = MoveTo::create(0.5f, Vec2(hero->getPositionX() + heroX, -hero->getContentSize().width / 2));
					auto callFunc = [=](){
					auto layer = LayerColor::create(Color4B(70, 70, 70, 255));
					auto label = Label::create();
					label->setPosition(visibleSize / 2);
					label->setString("Game Over");
					label->setScale(10);
					label->setColor(Color3B::ORANGE);
					layer->addChild(label);
					this->addChild(layer, 99); 
					};
					hero->runAction(Sequence::create(move1, rotate, move2, callFunc, nullptr));

					
					
					
				}
				else{
					auto move = MoveBy::create(1, Vec2(-nextX, 0));
					for (auto stage : m_stages)
					{
						stage->runAction(move->clone());
					}
					auto changeCallFunc = CallFunc::create([=]() {
						m_stages.at(m_currentStageIndex)->setScaleX(random(10, 50));

						auto preIndex = (m_currentStageIndex + 2) % 3;
						auto newX = m_stages.at(preIndex)->getPositionX()
							+ (m_stages.at(preIndex)->getContentSize().width *	m_stages.at(preIndex)->getScaleX()) / 2
							+ (m_stages.at(m_currentStageIndex)->getContentSize().width *	m_stages.at(m_currentStageIndex)->getScaleX()) / 2
							+ random(30, 100);
						m_stages.at(m_currentStageIndex)->setPosition(Vec2(newX, 0));

						stick->setVisible(false);
						m_currentStageIndex = ++m_currentStageIndex % 3;
						m_heroMoving = false;
						hero->stopAllActions();
						hero->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("STAY_ANIMATION")));
					});
					stick->runAction(Sequence::create(move, changeCallFunc, nullptr));
					hero->stopAllActions();
					hero->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("WALK_ANIMATION")));
				}
			});
			stick->runAction(Sequence::create(rotateStick, moveCallFunc, nullptr));

		};

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		return true;
	} while (0);

	return false;
}

void HelloWorld::initStage() {
	//添加柱子
	for (int i = 0; i < 3; i++)
	{
		auto stage = Sprite::create("stage1.png");
		stage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		this->addChild(stage, 1);
		stage->setScaleX(random(10, 50));
		m_stages.pushBack(stage);
	}
	m_stages.at(0)->setPosition(visibleSize.width / 5, 0);
	auto stageX1 = m_stages.at(0)->getPositionX()
		+ (m_stages.at(0)->getContentSize().width *	m_stages.at(0)->getScaleX()) / 2
		+ (m_stages.at(1)->getContentSize().width *	m_stages.at(1)->getScaleX()) / 2
		+ random(30, 100);	//随机间隙
	m_stages.at(1)->setPosition(Vec2(stageX1, 0));
	auto stageX2 = m_stages.at(1)->getPositionX()
		+ (m_stages.at(1)->getContentSize().width *	m_stages.at(1)->getScaleX()) / 2
		+ (m_stages.at(2)->getContentSize().width *	m_stages.at(2)->getScaleX()) / 2
		+ random(30, 100);
	m_stages.at(2)->setPosition(Vec2(stageX2, 0));
}