#include "stdafx.h"
#include "Projectile.hpp"
#include "Thor\Vectors.hpp"

Projectile::Projectile() {		}

Projectile::Projectile(b2Body * b, bool dir, STATE type) {
	b->SetUserData(this);
	m_box_body = b;
	m_current_state = type;

	if (dir)
		m_direction = sf::Vector2f(1, 0);
	else
		m_direction = sf::Vector2f(1, 0);

	init();

	if (dir)
		setScale(-1, 1);
	else
		setScale(1, 1);

	loadMedia();
	alineSprite();
	applySpeed();

	m_animator.playAnimation(m_current_state);
}

Projectile::Projectile(b2Body* b, sf::Vector2f dir) {
	b->SetUserData(this);
	m_box_body = b;
	m_direction = dir;
	m_current_state = FIRE;

	init();

	loadMedia();
	alineSprite();
	applySpeed();

	m_animator.playAnimation(m_current_state);
}
Projectile::Projectile(b2Body* b, sf::Vector2f dir, STATE type) {
	b->SetUserData(this);
	m_box_body = b;
	m_current_state = type;

	m_direction = dir;

	init();

	loadMedia();
	alineSprite();
	applySpeed();	

	m_animator.playAnimation(m_current_state);
}
Projectile::Projectile(b2Body* b, sf::Vector2f dir, STATE type, string type_id) {
	b->SetUserData(this);
	m_box_body = b;
	m_current_state = type;

	init();

	loadMedia();
	alineSprite();

	m_direction = thor::unitVector(dir);
	if (type_id == "At-Player") {
		m_speed = 90;
		correctRotation(m_direction);
	}
	else if (type_id == "Up") {
		m_speed = 120.0;
	}
	else {
		m_speed = 150.0;
	}

	
	applySpeed();
	m_animator.playAnimation(m_current_state);
}
Projectile::~Projectile() {		}

void Projectile::init() {
	switch (m_current_state) {
	case FIRE:
		m_speed = 75.0f;
		break;
	case RED:
		m_speed = 90.0f;
		break;
	case BLUE:
		m_speed = 120.0f;
		break;
	case BOSS:
		m_speed = 150.0f;
		break;

	case WEED_L1:
		m_speed = 75.0f;
		break;
	case WEED_L2:
		m_speed = 90.0f;
		break;
	case WEED_L3:
		m_speed = 120.0f;
		break;
	}
	m_body_active = true;
	can_despawn = false;
	m_spawn_point = vHelper::toSF(getBody()->GetPosition());
	if (m_current_state != BOSS) {
		if (m_direction.x > 0)	
			setScale(1, 1);
		else                                    
			setScale(-1, 1);
	}
}

void Projectile::update() {
	if(m_body_active)
		alineSprite();
	checkAnimation();
}
void Projectile::correctRotation(sf::Vector2f dir) {
	float desired_dir = atan2f(-dir.y, dir.x);
	float des_degrees = tan(desired_dir);
	des_degrees = desired_dir * 180 / 3.14159265;
	setRotation(desired_dir);
	m_box_body->SetTransform(m_box_body->GetPosition(), desired_dir);
}
void Projectile::render(sf::RenderWindow & w, sf::Time frames) {
	m_animator.update(frames);
	m_animator.animate(*this);
}

void Projectile::loadMedia() {
	s_texture = "Assets/Game/projectile.png";
	setTexture(ResourceManager<sf::Texture>::instance()->get(s_texture));
	m_size = sf::Vector2f(16, 16);
	setOrigin(m_size.x / 2, m_size.y / 2);

	switch (m_current_state) {
	case FIRE:
		addFrames(frame_fire, 2, 0, 1, 16, 16, 1.0f);
		addFrames(frame_explode, 3, 0, 5, 28, 29, 1.0f);

		m_animator.addAnimation(FIRE, frame_fire, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_explode, sf::seconds(0.25f));
		break;
	case RED:
		addFrames(frame_fire_rd, 0, 0, 3, 28, 20, 1.0f);
		addFrames(frame_explode, 3, 0, 5, 28, 29, 1.0f);

		m_animator.addAnimation(RED, frame_fire_rd, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_explode, sf::seconds(0.25f));
		break;
	case BLUE:
		addFrames(frame_fire_bu, 1, 0, 3, 28, 20, 1.0f);
		addFrames(frame_explode, 3, 0, 5, 28, 29, 1.0f);

		m_animator.addAnimation(BLUE, frame_fire_bu, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_explode, sf::seconds(0.25f));
		break;
	case BOSS:
		addFrames(frame_fire_bk, 4, 0, 3, 28, 20, 1.0f);
		addFrames(frame_explode, 3, 0, 5, 28, 29, 1.0f);

		m_animator.addAnimation(BOSS, frame_fire_bk, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_explode, sf::seconds(0.25f));
		break;
	case WEED_L1:
		addFrames(frame_weed_l1,		5, 0, 2, 24, 30, 1.0f);
		addFrames(frame_weed_explode,	6, 0, 4, 24, 30, 1.0f);

		m_animator.addAnimation(WEED_L1, frame_weed_l1, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_weed_explode, sf::seconds(0.25f));
		break;
	case WEED_L2:
		setColor(sf::Color::Red);
		addFrames(frame_weed_l2,		5, 0, 2, 24, 30, 1.0f);
		addFrames(frame_weed_explode,	6, 0, 4, 24, 30, 1.0f);

		m_animator.addAnimation(WEED_L2, frame_weed_l2, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_weed_explode, sf::seconds(0.25f));
		break;
	case WEED_L3:
		setColor(sf::Color::Blue);
		addFrames(frame_weed_l3,		5, 0, 2, 24, 30, 1.0f);
		addFrames(frame_weed_explode,	6, 0, 4, 24, 30, 1.0f);

		m_animator.addAnimation(WEED_L3, frame_weed_l3, sf::seconds(0.25f));
		m_animator.addAnimation(EXPLODE, frame_weed_explode, sf::seconds(0.25f));
		break;
	}
}
void Projectile::addFrames(thor::FrameAnimation& animation, int y, int xFirst, int xLast, int xSep, int ySep, float duration) {
	if (y == 0)
		y = 0;
	else if (y == 1)
		y = 20;
	else if (y == 2)
		y = 40;
	else if (y == 3)
		y = 56;
	else if (y == 4)
		y = 84;
	else if (y == 5)
		y = 104;
	else if (y == 6)
		y = 134;

	for (int x = xFirst; x != xLast; x += 1)
		animation.addFrame(duration, sf::IntRect(xSep * x, y, xSep, ySep));
}

void Projectile::checkAnimation() {
	if (m_current_state != m_previous_state) {
		m_previous_state = m_current_state;
		m_animator.playAnimation(m_current_state);
	}
	if (m_current_state == EXPLODE && !m_animator.isPlayingAnimation())
		can_despawn = true;
	if (m_current_state != EXPLODE && !m_body_active)
		m_current_state = EXPLODE;
	if (m_current_state != EXPLODE && !m_animator.isPlayingAnimation())
		m_animator.playAnimation(m_current_state);
}
void Projectile::Die() {
	m_body_active = false;
	getBody()->GetFixtureList()->SetSensor(true);
	setOrigin(28.0f / 2.0f, 28.0f / 2.0f); 
	alineSprite();
}

void Projectile::alineSprite() {
	setPosition(vHelper::toSF(m_box_body->GetPosition()));

	//Set Rotation
	float desired_dir = atan2f(-m_direction.y, -m_direction.x);
	float des_degrees = tan(desired_dir);
	des_degrees = desired_dir * 180 / 3.14159265;
	setRotation(des_degrees);
}
void Projectile::applySpeed() {
	sf::Vector2f force = m_direction * m_speed;
	m_box_body->SetLinearVelocity(vHelper::toB2(force));
}
bool Projectile::getBoolDirection() {
	if (m_direction == sf::Vector2f(1, 0))
		return true;
	if (m_direction == sf::Vector2f(-1, 0))
		return false;
	if (m_direction == sf::Vector2f(0, 1))
		return true;
	if (m_direction == sf::Vector2f(0, -1))
		return false;
}