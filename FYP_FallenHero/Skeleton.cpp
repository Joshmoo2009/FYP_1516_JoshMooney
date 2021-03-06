#include "stdafx.h"
#include "Skeleton.hpp"
#include "vHelper.hpp"

Skeleton::Skeleton() {
	init();
	setPosition(-500, -500);
	setTexture(sf::Texture());
}
Skeleton::Skeleton(b2Body *b, bool dir, AI ai) {
	b->SetUserData(this);
	e_box_body = b;
	m_ai = ai;
	init();
	alineSprite();

	//After Default initalisation
	e_direction = dir;
	setDirection(e_direction);
	projectile_mgr = nullptr;
}
Skeleton::Skeleton(b2Body *b, bool dir, AI ai, ProjectileManager *p) {
	b->SetUserData(this);
	e_box_body = b;
	m_ai = ai;
	init();
	alineSprite();

	//After Default initalisation
	e_direction = dir;
	setDirection(e_direction);
	projectile_mgr = p;

	low_lob = b2Vec2(65, -25);
	high_lob = b2Vec2(30, -60);
}
Skeleton::Skeleton(b2World * world) {
	/*
	b2BodyDef myBodyDef;
	myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
	myBodyDef.position.Set(200, 100); //set the starting position
	myBodyDef.angle = 0; //set the starting angle
	myBodyDef.userData = this;

	e_body_active = true;
	e_box_body = world->CreateBody(&myBodyDef);

	//Define the shape of the body
	b2PolygonShape shape;
	//shape.SetAsBox(m_text_size.x / 32.0f, m_text_size.y / 32.0f);
	shape.SetAsBox(m_text_size.x / 2.0f, m_text_size.y / 2.0f);

	b2FixtureDef myFixtureDef;
	myFixtureDef.density = 1.0f;
	myFixtureDef.friction = 1.5f;
	myFixtureDef.shape = &shape;

	e_box_body->CreateFixture(&myFixtureDef);
	*/
}

void Skeleton::init() {
	e_sword_col = false;
	m_speed = 4.0f;
	e_direction = 1;	//true = 1 = Looing right and vice versa
	speedFactor = 0;
	e_body_active = true;
	e_can_despawn = false;

	m_current_state = WALKING;
	m_previous_state = m_current_state;

	loadMedia();

	//Entity Initalisation
	switch (m_ai) {
	case WHITE:
		//Do nothing sprite is already white
		e_hp = 10;
		break;
	case GREY:
		setColor(sf::Color(95, 95, 95, 255));
		e_hp = 20;
		break;
	case BLACK:
		setColor(sf::Color(48, 48, 48, 255));
		e_hp = 30;
		break;
	}
	if (touching_terr == nullptr) {
		m_current_state = IDLE;
	}

	m_fire_clock.restart();
	cooldown_time = 1.75f;
	can_fire = false;
}

void Skeleton::ChangeState(STATE s) {
	/*switch (s) {
	case IDLE:
		m_current_state = IDLE;
		m_animator.playAnimation(IDLE);
		break;
	case DEATH:
		m_current_state = DEATH;
		m_animator.playAnimation(DEATH);
		break;
	case WALKING:
		m_current_state = WALKING;
		m_animator.playAnimation(WALKING);
		break;
	case ATTACKING:
		m_current_state = ATTACKING;
		m_animator.playAnimation(ATTACKING);
		break;
	}*/
}

void Skeleton::loadMedia(){
	e_texture = "Assets/Game/OG_Skeleton.png";
	setTexture(ResourceManager<sf::Texture>::instance()->get(e_texture));
	sf::Texture l_texture = ResourceManager<sf::Texture>::instance()->get(e_texture);
 	m_text_size = sf::Vector2u(27, 40);
	setOrigin(m_text_size.x / 2, m_text_size.y / 2);

	addFrames(frame_death, 0, 0, 7, 38, 47, 1.0f);
	addFrames(frame_idle, 1, 0, 4, m_text_size.x, m_text_size.y, 1.0f);
	addFrames(frame_walk, 2, 0, 4, m_text_size.x, m_text_size.y, 1.0f);
	addFrames(frame_attack, 3, 0, 4, 29, m_text_size.y, 1.0f);
	
	m_animator.addAnimation(DEATH, frame_death, sf::seconds(0.5f));
	m_animator.addAnimation(IDLE, frame_idle, sf::seconds(0.5f));
	m_animator.addAnimation(WALKING, frame_walk, sf::seconds(0.5f));
	m_animator.addAnimation(ATTACKING, frame_attack, sf::seconds(0.15f));

	s_death = "Assets/Audio/Game/skeleton_kill.wav";
	m_death.setBuffer(ResourceManager<sf::SoundBuffer>::instance()->get(s_death));
}

void Skeleton::checkAnimation() {
	if (m_previous_state != m_current_state) {
 		m_previous_state = m_current_state;
		m_animator.playAnimation(m_current_state);
	}
	if(m_current_state == WALKING && !m_animator.isPlayingAnimation())
		m_animator.playAnimation(WALKING);
	if (m_current_state == IDLE && !m_animator.isPlayingAnimation())
		m_animator.playAnimation(IDLE);
	if (m_current_state == ATTACKING && !m_animator.isPlayingAnimation()) {
		m_current_state = WALKING;
		ChangeDirection();
		move();
	}
}

void Skeleton::addFrames(thor::FrameAnimation& animation, int y, int xFirst, int xLast, int xSep, int ySep, float duration) {
	if (y == 0)
		y = 0;
	else if (y == 1)
		y = 47;
	else if (y == 2)
		y = 91;
	else if (y == 3)
		y = 135;

	for (int x = xFirst; x != xLast; x += 1)
		animation.addFrame(duration, sf::IntRect(xSep * x, y, xSep, ySep));
}

void Skeleton::update(FTS fts, Player *p) {
	checkAnimation();
	if (e_body_active) {
		if (m_ai == BLACK)
			CheckFire();
		back_and_forth = false;
		float dis_to_player = vHelper::distance(getPosition(), p->getPosition());
		if (m_ai == BLACK && dis_to_player < 300) {
			back_and_forth = true;
			if (p->getPosition().x > getPosition().x)
				setDirection(true);
			else 
				setDirection(false);
			if (can_fire && projectile_mgr != nullptr) {
				
				b2Vec2 toss;

				if (dis_to_player < 150)		toss = low_lob;
				else                            toss = high_lob;

				if (!e_direction)		toss.x *= -1;

				projectile_mgr->lobBone(getPosition(), toss);
				can_fire = false;
				m_fire_clock.restart();
			}
		}
		else if(m_current_state == WALKING)
			move();
		// If no other animation is playing, play idle animation
		//if (!m_animator.isPlayingAnimation())
		//	m_animator.playAnimation(IDLE);
		alineSprite();
	}
	else	{
		if (!m_animator.isPlayingAnimation())
			e_can_despawn = true;
	}
}
void Skeleton::render(sf::RenderWindow &w, sf::Time frames) {
	m_animator.update(frames);
	m_animator.animate(*this);
}

void Skeleton::TakeDamage() {
	if (!is_hit) {
		is_hit = true;
		e_hp -= 10;
		if (e_hp <= 0)
			Die();
	}
}

void Skeleton::Die() {
	m_death.play();
	e_box_body->GetFixtureList()->SetSensor(true);
	e_body_active = false;
	e_can_despawn = false;
	m_current_state = DEATH;
}

void Skeleton::move() {
	if (touching_terr == nullptr) {
		//In the air
		//m_animator.playAnimation(IDLE);
		if (e_body_active) {
			m_current_state = IDLE;
		}
	}
	else if (m_ai == BLACK && back_and_forth) {
		sf::FloatRect b = getBounds();
		if (e_direction) {
			if (touching_terr->geometry.left + touching_terr->geometry.width > b.left + b.width + 50) {
				int i = rand() % 2;
				if (i == 0)			e_box_body->SetLinearVelocity(b2Vec2(m_speed *.75f, e_box_body->GetLinearVelocity().y));
				else				e_box_body->SetLinearVelocity(b2Vec2(-m_speed *.75f, e_box_body->GetLinearVelocity().y));
			}
			else
				ReachedEdge();
		}
		else {
			if (touching_terr->geometry.left < b.left - 50) {
				int i = rand() % 2;
				if (i == 0)			e_box_body->SetLinearVelocity(b2Vec2(m_speed *.75f, e_box_body->GetLinearVelocity().y));
				else				e_box_body->SetLinearVelocity(b2Vec2(-m_speed *.75f, e_box_body->GetLinearVelocity().y));
			}
			else
				ReachedEdge();
		}
	}
		
	else {
		sf::FloatRect b = getBounds();
		if (e_direction) {
			if (touching_terr->geometry.left + touching_terr->geometry.width > b.left + b.width + 50) {
				if (speedFactor < 1.f)
					speedFactor += 0.02f;
				else if (speedFactor > 1.f)
					speedFactor = 1.f;

				e_box_body->SetLinearVelocity(b2Vec2(m_speed * speedFactor, e_box_body->GetLinearVelocity().y));
			}
			else
				ReachedEdge();
		}
		else {
			if (touching_terr->geometry.left < b.left - 50) {
				if (speedFactor > -1.f)
					speedFactor -= 0.02;
				else if (speedFactor < -1.f)
					speedFactor = -1.f;

				e_box_body->SetLinearVelocity(b2Vec2(m_speed * speedFactor, e_box_body->GetLinearVelocity().y));
			}
			else
				ReachedEdge();
		}
	}
}

void Skeleton::attack() {
	if (e_body_active) {
		m_current_state = ATTACKING;
	}
}

void Skeleton::alineSprite() {
	setPosition(vHelper::toSF(e_box_body->GetPosition()));
}

void Skeleton::ChangeDirection() {
	e_direction = !e_direction;
	if (e_direction)	setScale(1, 1);
	else setScale(-1, 1);
}

void Skeleton::setDirection(bool b) {
	e_direction = b;
	if (e_direction)	setScale(1, 1);
	else setScale(-1, 1);
}

void Skeleton::isTouching(Terrain* t) { 
	if (e_body_active) {
		touching_terr = t;
		m_current_state = WALKING;
	}
}

void Skeleton::ReachedEdge() {
	ChangeDirection();
	e_box_body->SetLinearVelocity(b2Vec2(0, 0));
}

void Skeleton::ReachPlayer() {
	attack();
	//ChangeDirection();
	//move();
}

void Skeleton::ReachWall(){
	//e_direction = !e_direction;

	ChangeDirection();
}

void Skeleton::CheckFire() {
	sf::Time elapsed = m_fire_clock.getElapsedTime();
	if (elapsed.asSeconds() > cooldown_time) {
		m_current_state = ATTACKING;
		m_fire_clock.restart();
		can_fire = true;
	}
}
