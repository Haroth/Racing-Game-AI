#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <fstream>

#define PLAYER_SIZE 15.f // size of car texture
#define PLAYER_CIRCLE_RADIUS 10.f // size of car circle
#define PLAYER_MAX_VELOCITY 15.f // maximum car velocity 
#define PLAYER_ACCELERATION 0.15f // car acceleration
#define PLAYER_BRAKING 0.4f // car braking
#define PLAYER_TURNING_ANGLE 2.f // car turning
#define TRACK_ANGLE_BOUNCE 45.f // maximum angle of track bounce
#define TRACK_BOUNCE_POWER 1.2f // track bounce power

#define RAYS_NUMBER 11

using namespace sf;

class Player
{
	Vector2f position; 
	Vector2f direction;
	float velocity;
	float raysDistance[RAYS_NUMBER]; // distance of rays to obstacles
	
	RectangleShape texture;

public:
	void move(bool controlTurnLeft, bool controlTurnRight, bool controlThrottle, bool controlBrake); // car move according to controls
	bool checkCollisions(Vector2f* trackCollisionPoints); // returns logic value of the car collisions with obstacles
	void raysUpdate(Vector2f* trackCollisionPoints); // update rays distance to obstacles
	Player(const Vector2f& spawnPosition, const Vector2f& spawnDirection, Texture* const playerTex);

	inline const RectangleShape& getTexture() const { return texture; }
	inline const Vector2f& getPosition() const { return position; }
	inline const Vector2f& getDirection() const { return direction; }
	inline float getVelocity() const { return velocity; }
	inline const float* const getRaysDistance() const { return raysDistance; }
	inline void setPosition(const Vector2f& position_) { position = position_; }
};

struct StateAndControl // structure to aggregate agent's state and control
{
	Vector2f position;
	Vector2f direction;
	float velocity;
	float raysDistance[RAYS_NUMBER];

	bool controlTurnLeft;
	bool controlTurnRight;
	bool controlThrottle;
	bool controlBrake;
};

const float vectorLength(const Vector2f& vector); // returns length of a vector
const Vector2f normalizeVector(const Vector2f& vector); // returns normalized vector
const float angleBetweenVectors(const Vector2f& direction1, const Vector2f& direction2); // returns angle between vectors in radians
const Vector2f rotateVector(Vector2f vector, float clockwiseAngle); // returns rotated vector. The angle argument is in degrees
bool collisionSegmentCircle(Vector2f segmentBeggining, Vector2f segmentEnding, Vector2f circlePosition); // returns logic value of a segment collision with a circle
bool segmentsCollision(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2); // returns logic value of collision of two segments
Vector2f linesIntersection(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2); // returns intersection point of two lines

inline std::ostream & operator<< (std::ostream &out, const Vector2f& vec) { return out << vec.x << " " << vec.y; } // overload << operator to print a vector

#endif