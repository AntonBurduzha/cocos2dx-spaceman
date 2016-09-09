#include "GameScene.h"
#include "Defines.h"
#include "GameOverScene.h"
#include "WinScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    // 'layer' is an autorelease object
    auto layer = GameScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	backgroundGame1 = Sprite::create("GameBackground.png");
	backgroundGame1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	backgroundGame2 = Sprite::create("GameBackground.png");
	backgroundGame2->setPosition(Vec2(3 * visibleSize.width / 2, visibleSize.height / 2));

	this->addChild(backgroundGame1, 0);
	this->addChild(backgroundGame2, 0);

	this->scheduleUpdate();

	player = Sprite::create("Player.png"); //player
	player->setTag(12);
	player->setPosition(Point((visibleSize.width / 950) * 131, (visibleSize.height / 10) * 5));

	auto playerSize = player->getContentSize(); //player body
	auto playerBody = PhysicsBody::createBox(Size(playerSize.width / 2, playerSize.height / 2), PhysicsMaterial(0.1f, 1.0f, 0.0f));
	playerBody->setDynamic(true);
	playerBody->setCategoryBitmask(2);
	playerBody->setCollisionBitmask(1);
	playerBody->setContactTestBitmask(0xFFFFFFFF);
	player->setPhysicsBody(playerBody);

	this->addChild(player);

	playerFly = Sprite::create("playerAnim/frame001.png");
	playerFly->setPosition(Point((visibleSize.width / 950) * 20, (visibleSize.height / 10) * 5)); //fire animation

	Vector<SpriteFrame*>animFrames;
	animFrames.reserve(5);
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame001.png", Rect(0, 0, 128, 64)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame002.png", Rect(0, 0, 123, 64)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame003.png", Rect(0, 0, 123, 64)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame004.png", Rect(0, 0, 121, 64)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame005.png", Rect(0, 0, 111, 47)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame006.png", Rect(0, 0, 123, 64)));
	animFrames.pushBack(SpriteFrame::create("playerAnim/frame007.png", Rect(0, 0, 123, 64)));
	animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
	animate = Animate::create(animation);
	playerFly->runAction(RepeatForever::create(animate));
	this->addChild(playerFly);


	auto playerListener = EventListenerKeyboard::create();
	playerListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this); //player moving
	_eventDispatcher->addEventListenerWithSceneGraphPriority(playerListener, player);

	auto playerFlyListener = EventListenerKeyboard::create();
	playerFlyListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this); // fire moving
	_eventDispatcher->addEventListenerWithSceneGraphPriority(playerFlyListener, playerFly);

	curTime = getCurTime(); //time
	stopSpawningAsteroid = curTime + 6000;
	spawnEnemy = curTime + 7000;
	enemyHealth = 140;

	srand((unsigned int)time(nullptr));
	this->schedule(schedule_selector(GameScene::addAsteroid), 1);

	auto contactListener = EventListenerPhysicsContact::create(); //contact
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	health = 100;
	__String* tempHealth = __String::createWithFormat("Health:  %i", health); //health
	healthLabel = Label::createWithTTF(tempHealth->getCString(), "fonts/Marker Felt.ttf", 25);
	healthLabel->setColor(Color3B::WHITE);
	healthLabel->setPosition(Point((visibleSize.width / 20) * 2, (visibleSize.height / 20) * 19));
	this->addChild(healthLabel, 10000);

	score = 0;
	__String* tempScore = __String::createWithFormat("Score:  %i", score);  //score
	scoreLabel = Label::createWithTTF(tempScore->getCString(), "fonts/Marker Felt.ttf", 25);
	scoreLabel->setColor(Color3B::WHITE);
	scoreLabel->setPosition(Point((visibleSize.width / 20) * 18, (visibleSize.height / 20) * 19));
	this->addChild(scoreLabel, 10000);

	this->scheduleUpdate();

    return true;
}

void GameScene::update(float dt)
{
	auto position1 = backgroundGame1->getPosition();
	position1.x -= 250 * dt;
	auto position2 = backgroundGame2->getPosition();
	position2.x -= 250 * dt;

	if (position1.x < 0 - (backgroundGame1->getBoundingBox().size.width / 2))
	{
		position1.x = this->getBoundingBox().getMaxX() + backgroundGame1->getBoundingBox().size.width / 2;
	}
	if (position2.x < 0 - (backgroundGame2->getBoundingBox().size.width / 2))
	{
		position2.x = this->getBoundingBox().getMaxX() + backgroundGame2->getBoundingBox().size.width / 2;
	}
	backgroundGame1->setPosition(position1);
	backgroundGame2->setPosition(position2);

	curTime = getCurTime(); //time
	

	if (curTime > stopSpawningAsteroid)
	{
		this->unschedule(schedule_selector(GameScene::addAsteroid));
	}
	if (curTime > spawnEnemy)
	{
		GameScene::addEnemy();
		this->schedule(schedule_selector(GameScene::enemyFire), 1);
		spawnEnemy = curTime + 300000; // костыль для обхода времени
	}
}

void GameScene::enemyFire(float dt)
{
	enemyBullet = Sprite::create("EnemyRocket.png");
	enemyBullet->setTag(14);
	enemyBullet->setPosition(Vec2(enemy->getPosition().x - 64, enemy->getPosition().y));

	auto enemyBulletSize = enemyBullet->getContentSize();
	auto enemyBulletBody = PhysicsBody::createBox(Size(enemyBulletSize.width / 2, enemyBulletSize.height / 2), PhysicsMaterial(0.1f, 1.0f, 0.0f));
	enemyBulletBody->setDynamic(true);
	enemyBulletBody->setCategoryBitmask(1);
	enemyBulletBody->setCollisionBitmask(2);
	enemyBulletBody->setContactTestBitmask(0xFFFFFFFF);
	enemyBullet->setPhysicsBody(enemyBulletBody);

	this->addChild(enemyBullet);

	auto enemyBulletAction = MoveTo::create(2, Vec2(player->getPosition().x - 100, player->getPosition().y));
	auto enemyBulletRemove = RemoveSelf::create();
	enemyBullet->runAction(Sequence::create(enemyBulletAction, enemyBulletRemove, nullptr));
}

void GameScene::addEnemy()
{

	Size visSize = Director::getInstance()->getVisibleSize();

	enemy = Sprite::create("Enemy.png"); //enemy
	enemy->setTag(13);
	enemy->setPosition(Point((visSize.width / 950) * 950, (visSize.height / 10) * 6));

	auto enemySize = enemy->getContentSize(); //player body
	auto enemyBody = PhysicsBody::createBox(Size(enemySize.width / 2, enemySize.height / 2), PhysicsMaterial(0.1f, 1.0f, 0.0f));
	enemyBody->setDynamic(true);
	enemyBody->setCategoryBitmask(1);
	enemyBody->setCollisionBitmask(2);
	enemyBody->setContactTestBitmask(0xFFFFFFFF);
	enemy->setPhysicsBody(enemyBody);
	this->addChild(enemy);

	auto enemyInit = MoveBy::create(4, Vec2(-100, 0));
	enemy->runAction(enemyInit);


}

float GameScene::getCurTime()
{
	timeval time;
	gettimeofday(&time, NULL);
	unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (float)millisecs;
}

bool GameScene::onContactBegin(cocos2d::PhysicsContact &contact)
 {
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	if (nodeA && nodeB)
	{	
		if (nodeA->getTag() == 10)
		{
			nodeA->removeFromParentAndCleanup(true);
			auto asteroidBoom = Sprite::create("asterBoom/boom001.png");
			asteroidBoom->setPosition(Point(nodeB->getPosition().x, nodeB->getPosition().y));
			Vector<SpriteFrame*>asteroidFrames;
			asteroidFrames.reserve(5);
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom001.png", Rect(0, 0, 128, 128)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom002.png", Rect(0, 0, 91, 85)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom003.png", Rect(0, 0, 89, 88)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom004.png", Rect(0, 0, 97, 97)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom005.png", Rect(0, 0, 105, 104)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom006.png", Rect(0, 0, 121, 121)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom007.png", Rect(0, 0, 125, 124)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom008.png", Rect(0, 0, 123, 124)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom009.png", Rect(0, 0, 125, 123)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom010.png", Rect(0, 0, 123, 122)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom011.png", Rect(0, 0, 121, 121)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom012.png", Rect(0, 0, 117, 118)));
			asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom013.png", Rect(0, 0, 117, 115)));
			auto asteroidAnimation = Animation::createWithSpriteFrames(asteroidFrames, 0.1f);
			auto asteroidAnimate = Animate::create(asteroidAnimation);
			auto boomRemove = RemoveSelf::create();
			asteroidBoom->runAction(Sequence::create(asteroidAnimate, boomRemove, nullptr));
			this->addChild(asteroidBoom);
			score += 25;

			__String* tempScore = __String::createWithFormat("Score:  %i", score);
			scoreLabel->setString(tempScore->getCString());
			
		}
		else if (nodeA->getTag() == 14)
		{
			nodeA->removeFromParentAndCleanup(true);
		}

		if (nodeB->getTag() == 11)
		{
			nodeB->removeFromParentAndCleanup(true);
		}
		else if (nodeB->getTag() == 12)
		{
			health -= 20;

			__String* tempHealth = __String::createWithFormat("Health:  %i", health);
			healthLabel->setString(tempHealth->getCString());
			if (health < 20)
			{
				auto asteroidBoom = Sprite::create("asterBoom/boom001.png");
				asteroidBoom->setPosition(Point(nodeB->getPosition().x, nodeB->getPosition().y));
				Vector<SpriteFrame*>asteroidFrames;
				asteroidFrames.reserve(5);
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom001.png", Rect(0, 0, 128, 128)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom002.png", Rect(0, 0, 91, 85)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom003.png", Rect(0, 0, 89, 88)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom004.png", Rect(0, 0, 97, 97)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom005.png", Rect(0, 0, 105, 104)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom006.png", Rect(0, 0, 121, 121)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom007.png", Rect(0, 0, 125, 124)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom008.png", Rect(0, 0, 123, 124)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom009.png", Rect(0, 0, 125, 123)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom010.png", Rect(0, 0, 123, 122)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom011.png", Rect(0, 0, 121, 121)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom012.png", Rect(0, 0, 117, 118)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom013.png", Rect(0, 0, 117, 115)));
				auto asteroidAnimation = Animation::createWithSpriteFrames(asteroidFrames, 0.1f);
				auto asteroidAnimate = Animate::create(asteroidAnimation);
				auto boomRemove = RemoveSelf::create();
				asteroidBoom->runAction(Sequence::create(asteroidAnimate, boomRemove, nullptr));
				this->addChild(asteroidBoom);
				auto scene = GameOverScene::createScene(score);
				Director::getInstance()->replaceScene(TransitionFade::create(1, scene));
			}
		}
		else if (nodeB->getTag() == 13)
		{
			enemyHealth -= 20;

			if (enemyHealth < 20)
			{
				auto asteroidBoom = Sprite::create("asterBoom/boom001.png");
				asteroidBoom->setPosition(Point(nodeB->getPosition().x, nodeB->getPosition().y));
				Vector<SpriteFrame*>asteroidFrames;
				asteroidFrames.reserve(5);
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom001.png", Rect(0, 0, 128, 128)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom002.png", Rect(0, 0, 91, 85)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom003.png", Rect(0, 0, 89, 88)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom004.png", Rect(0, 0, 97, 97)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom005.png", Rect(0, 0, 105, 104)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom006.png", Rect(0, 0, 121, 121)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom007.png", Rect(0, 0, 125, 124)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom008.png", Rect(0, 0, 123, 124)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom009.png", Rect(0, 0, 125, 123)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom010.png", Rect(0, 0, 123, 122)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom011.png", Rect(0, 0, 121, 121)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom012.png", Rect(0, 0, 117, 118)));
				asteroidFrames.pushBack(SpriteFrame::create("asterBoom/boom013.png", Rect(0, 0, 117, 115)));
				auto asteroidAnimation = Animation::createWithSpriteFrames(asteroidFrames, 0.1f);
				auto asteroidAnimate = Animate::create(asteroidAnimation);
				auto boomRemove = RemoveSelf::create();
				asteroidBoom->runAction(Sequence::create(asteroidAnimate, boomRemove, nullptr));
				this->addChild(asteroidBoom);

				auto winScene = WinScene::createScene(score);
				Director::getInstance()->replaceScene(TransitionFade::create(1, winScene));
			}
		}
		
	}
	

	return true;
}

void GameScene::addAsteroid(float dt)
{
	
	asteroid = Sprite::create("Asteroid.png");
	asteroid->setTag(10);
	auto asteroidSize = asteroid->getContentSize();
	auto asteroidBody = PhysicsBody::createCircle(asteroidSize.width / 3);
	asteroidBody->setDynamic(true);
	asteroidBody->setCategoryBitmask(1);
	asteroidBody->setCollisionBitmask(2);
	asteroidBody->setContactTestBitmask(0xFFFFFFFF);
	asteroid->setPhysicsBody(asteroidBody);

	auto asteroidContentSize = asteroid->getContentSize();
	auto selfContentSize = this->getContentSize();
	int minY = asteroidContentSize.height / 2;
	int maxY = selfContentSize.height - asteroidContentSize.height / 2;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	
	asteroid->setPosition(Vec2(selfContentSize.width + asteroidContentSize.width / 2, randomY));
	this->addChild(asteroid);

	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;
 
	auto asteroidMove = MoveTo::create(randomDuration, Vec2(-asteroidContentSize.width/2, randomY));
	auto asteroidRemove = RemoveSelf::create();
	asteroid->runAction(Sequence::create(asteroidMove, asteroidRemove, nullptr));
	
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	Vec2 location = event->getCurrentTarget()->getPosition();

	switch(keyCode)
	{
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		event->getCurrentTarget()->setPosition(location.x, location.y += 30);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		event->getCurrentTarget()->setPosition(location.x, location.y -= 30);
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
			bullet = Sprite::create("RocketFly.png");
			bullet->setTag(11);
			bullet->setPosition(Vec2(player->getPosition().x + 64, player->getPosition().y));
			
			auto bulletSize = bullet->getContentSize();
			auto bulletBody = PhysicsBody::createBox(Size(bulletSize.width / 2, bulletSize.height / 2), PhysicsMaterial(0.1f, 1.0f, 0.0f));
			bulletBody->setDynamic(true);
			bulletBody->setCategoryBitmask(2);
			bulletBody->setCollisionBitmask(1);
			bulletBody->setContactTestBitmask(0xFFFFFFFF);
			bullet->setPhysicsBody(bulletBody);

			this->addChild(bullet);

			auto bulletAction = MoveBy::create(4, Vec2(1000, 0));
			auto bulletRemove = RemoveSelf::create();
			bullet->runAction(Sequence::create(bulletAction, bulletRemove, nullptr));
			break;
	}
}


