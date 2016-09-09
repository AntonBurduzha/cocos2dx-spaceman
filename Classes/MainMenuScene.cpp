#include "MainMenuScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* MainMenuScene::createScene()
{
    // 'scene' is an autorelease object
	auto scene = Scene::create();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    // 'layer' is an autorelease object
    auto layer = MainMenuScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backgroundSprite = Sprite::create("Background2.png");
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(backgroundSprite);

	auto titleSprite = Sprite::create("TitleSprite.png");
	titleSprite->setPosition(Point((visibleSize.width / 4) * 1, (visibleSize.height / 5) * 3));
	this->addChild(titleSprite);

	auto menu_item1 = MenuItemFont::create("Play", CC_CALLBACK_1(MainMenuScene::Play, this));
	auto menu_item2 = MenuItemFont::create("Exit", CC_CALLBACK_1(MainMenuScene::Exit, this));

	menu_item1->setPosition(Point((visibleSize.width / 4) * 2, (visibleSize.height / 5) * 3));
	menu_item2->setPosition(Point((visibleSize.width / 4) * 2, (visibleSize.height / 5) * 2));

	auto* menu = Menu::create(menu_item1, menu_item2, NULL);
	menu->setPosition(Point(0, 0));
	this->addChild(menu);

    return true;
}

void MainMenuScene::Play(cocos2d::Ref* pSender)
{
	auto scene = GameScene::createScene();
	Director::getInstance()->pushScene(scene);
}
void MainMenuScene::Exit(cocos2d::Ref* pSender)
{
	Director::sharedDirector()->end();
	exit(0);
}