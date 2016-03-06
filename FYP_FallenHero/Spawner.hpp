#ifndef _SPAWNER_HPP
#define _SPAWNER_HPP
#include "stdafx.h"

#include "SFML\Graphics.hpp"
#include "Box2D\Box2D.h"
#include "Enemy.hpp"
#include "Skeleton.hpp"
#include "CrumbleBlock.hpp"

#include "Player.hpp"

class Spawner {
private:
	float distanceToPlayer(sf::Vector2f entity, sf::Vector2f player);
	float update_dist;
	b2World *m_world;

	Skeleton* prototype_Skeleton;
	vector<Enemy *> m_enemies;
	vector<CrumbleBlock *> m_blocks;

public:
	enum SPAWN_TYPE { SKELETON };
	Spawner()	{		}
	Spawner(b2World *world);
	~Spawner();

	b2Body* GenerateBody(SPAWN_TYPE type);

	void SpawnSkeleton(sf::Vector2f pos);
	void SpawnBlock(sf::Vector2f pos, CrumbleBlock::TYPE t, CrumbleBlock::SIZE s);

	void CullBodies();
	void DespawnObject();
	void CullInActiveEnemies();

	void update(FTS fts, Player *p);
	void render(sf::RenderWindow &w, sf::Time frames);
	void clear();
};

#endif