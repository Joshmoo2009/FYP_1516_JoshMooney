#ifndef _WEED_HPP
#define _WEED_HPP
#include "stdafx.h"
#include "Enemy.hpp"
#include "Player.hpp"
#include "ProjectileManager.hpp"

class Weed : public Enemy {
private:
	sf::Vector2u m_text_size;
	float speedFactor;
	float m_jump;

	float m_shoot_dis;
	sf::Clock m_fire_clock;
	bool can_fire;
	float cooldown_time;
	
	bool m_dir;
	//string s_death;
	//sf::Sound m_death;

	//thor::FrameAnimation frame_jump;
	//thor::FrameAnimation frame_land;
	thor::FrameAnimation frame_attack;
	thor::FrameAnimation frame_idle;
	thor::FrameAnimation frame_death;
	bool m_can_be_hurt;
	ProjectileManager *m_gun;
public:
	enum AI { GREEN, BLUE, RED };
	AI m_ai;
	enum STATE { ATTACK, IDLE, DEATH, JUMP, LAND };
	STATE m_current_state, m_previous_state;
	thor::Animator<sf::Sprite, STATE> m_animator;

	Weed();
	Weed(b2Body *b, bool dir, ProjectileManager *g);
	Weed(b2Body *b, bool dir, ProjectileManager *g, AI type);
	virtual ~Weed() {
		cLog::inst()->print("Weed Deconstructor called");
	}

	void TakeDamage() override;
	void Die() override;

	void checkAnimation();
	void addFrames(thor::FrameAnimation& animation, int y, int xFirst, int xLast, int xSep, int ySep, float duration);
	void loadMedia();
	void init();

	void update(FTS fts, Player *p);
	void render(sf::RenderWindow &w, sf::Time frames);
	void assignBody(b2Body *b) { e_box_body = b; }
	void attack();
	void alineSprite();

	sf::Vector2f getCenter() { return vHelper::toSF(e_box_body->GetPosition()); }
	sf::Vector2u getTextureSize() { return m_text_size; }
	sf::FloatRect getBounds() override {
		sf::Vector2f position(getPosition().x - (m_text_size.x / 2), getPosition().y - (m_text_size.y / 2));
		return sf::FloatRect{ position.x, position.y, (float)m_text_size.x, (float)m_text_size.y };
	}
};

#endif