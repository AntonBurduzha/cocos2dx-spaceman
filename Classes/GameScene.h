#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

	void update(float dt);
	float getCurTime();
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void addAsteroid(float dt);
	bool onContactBegin(cocos2d::PhysicsContact &contact);
	void addEnemy();
	void enemyFire(float dt);

private:

	cocos2d::Sprite* backgroundGame1;
	cocos2d::Sprite* backgroundGame2; 

	cocos2d::Sprite* player;
	cocos2d::Sprite* playerFly;
	cocos2d::Sprite* bullet;

	cocos2d::Sprite* enemy;
	cocos2d::Sprite* enemyBullet;
	

	cocos2d::Animation* animation;
	cocos2d::Animate* animate;

	cocos2d::Sprite* asteroid;
	
	int score;
	int health;
	int enemyHealth;
	cocos2d::Label* scoreLabel;
	cocos2d::Label* healthLabel;

	float curTime;
	float stopSpawningAsteroid;
	float spawnEnemy;

};

#endif // __HELLOWORLD_SCENE_H__
