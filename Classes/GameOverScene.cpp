#include "GameOverScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"

USING_NS_CC;

int score;

Scene* GameOverScene::createScene(int tempScore)
{
	score = tempScore;
    // 'scene' is an autorelease object
	auto scene = Scene::create();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    // 'layer' is an autorelease object
    auto layer = GameOverScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backgroundSprite = Sprite::create("Background3.png");
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);

	auto menu_item1 = MenuItemFont::create("Retry", CC_CALLBACK_1(GameOverScene::Retry, this));
	auto menu_item2 = MenuItemFont::create("Main menu", CC_CALLBACK_1(GameOverScene::MainMenu, this));

	menu_item1->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 10) * 4));
	menu_item2->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 10) * 5));

	auto* menu = Menu::create(menu_item1, menu_item2, NULL);
	menu->setPosition(Point(0, 0));
	this->addChild(menu);


	__String* tempScore = __String::createWithFormat("Score:  %i", score);
	auto currentScore = LabelTTF::create(tempScore->getCString(), "fonts/Marker Felt.ttf", 60);
	currentScore->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 10) * 6));
	this->addChild(currentScore);
    return true;
}

void GameOverScene::Retry(cocos2d::Ref* pSender)
{
	auto gameScene = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, gameScene));
}
void GameOverScene::MainMenu(cocos2d::Ref* pSender)
{
	auto menuScene = MainMenuScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, menuScene));
}
