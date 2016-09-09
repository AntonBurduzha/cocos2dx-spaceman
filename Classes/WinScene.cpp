#include "WinScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"

USING_NS_CC;

int scores;

Scene* WinScene::createScene(int tmpScore)
{
	scores = tmpScore;
    // 'scene' is an autorelease object
	auto scene = Scene::create();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    // 'layer' is an autorelease object
    auto layer = WinScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool WinScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backgroundSprite = Sprite::create("Background4.png");
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);

	auto menu_item = MenuItemFont::create("Main menu", CC_CALLBACK_1(WinScene::MainMenu, this));
	menu_item->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 10) * 5));

	auto* menu = Menu::create(menu_item, NULL);
	menu->setPosition(Point(0, 0));
	this->addChild(menu);


	__String* tmpScore = __String::createWithFormat("Score:  %i", scores);
	auto currentScore = LabelTTF::create(tmpScore->getCString(), "fonts/Marker Felt.ttf", 60);
	currentScore->setPosition(Point(visibleSize.width / 2, (visibleSize.height / 10) * 6));
	this->addChild(currentScore);
    return true;
}

void WinScene::MainMenu(cocos2d::Ref* pSender)
{
	auto menuScene = MainMenuScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, menuScene));
}
