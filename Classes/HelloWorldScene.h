#pragma once
#include "cocos2d.h"

USING_NS_CC;

class HelloWorld : public Layer {
public:
	HelloWorld();

	static Scene *createScene();
	CREATE_FUNC(HelloWorld);
	virtual bool init() override;
private:
	Vector<Sprite *> m_stages;
	int m_currentStageIndex;
	bool m_heroMoving;

	void initStage();
};