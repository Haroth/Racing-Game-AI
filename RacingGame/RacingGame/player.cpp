#include "player.h"

//=======================================================================================================================================================
//=============================================== CONSTRUCTOR ===========================================================================================
//=======================================================================================================================================================


Player::Player(const Vector2f& spawnPosition, const Vector2f& spawnDirection, Texture* const playerTex)
{
	texture.setSize(Vector2f(PLAYER_SIZE * 2 * 2.31f, PLAYER_SIZE * 2));
	texture.setOrigin(Vector2f(PLAYER_SIZE * 2.31f, PLAYER_SIZE));
	texture.setPosition(spawnPosition);
	texture.setTexture(playerTex);

	const float PI = 3.14159265f;
	const float angle = atan2(spawnDirection.y, spawnDirection.x) * 180.f / PI;
	texture.setRotation(angle);

	position = spawnPosition;
	direction = spawnDirection;
	velocity = 7.f;
}


//=======================================================================================================================================================
//=============================================== MOVE =========================================================================================
//=======================================================================================================================================================


void Player::move(bool controlTurnLeft, bool controlTurnRight, bool controlThrottle, bool controlBrake)
{
	// ------------------------------------------------------- check for interferences in control and updating car properties

	if (controlThrottle == true && controlBrake == false)
	{
		if (velocity < PLAYER_MAX_VELOCITY - PLAYER_ACCELERATION)
			velocity += PLAYER_ACCELERATION;
		else
			velocity = PLAYER_MAX_VELOCITY;
	}
	else if (controlBrake == true && controlThrottle == false)
	{
		if (velocity > 0.f + PLAYER_BRAKING)
			velocity -= PLAYER_BRAKING;
		else
			velocity = 0;
	}

	if (controlTurnLeft == false || controlTurnRight == false)
	{
		if (controlTurnLeft == true)
		{
			direction = rotateVector(direction, -PLAYER_TURNING_ANGLE);
			texture.rotate(-PLAYER_TURNING_ANGLE);
		}
		else if (controlTurnRight == true)
		{
			direction = rotateVector(direction, PLAYER_TURNING_ANGLE);
			texture.rotate(PLAYER_TURNING_ANGLE);
		}
	}
	
	
	position += direction * velocity;
	texture.setPosition(position);
}


//=======================================================================================================================================================
//=============================================== CHECK COLLISIONS ======================================================================================
//=======================================================================================================================================================


bool Player::checkCollisions(Vector2f* trackCollisionPoints)
{
	const float PI = 3.14159265f;
	// ------------------------------------------------------- check for external track
	for (int i = 0; i < 12; i++) 
	{
		if (collisionSegmentCircle(trackCollisionPoints[i], trackCollisionPoints[(i + 1) % 12], position))
		{
			float angle = angleBetweenVectors(trackCollisionPoints[(i + 1) % 12] - trackCollisionPoints[i], direction) * 180 / PI;
			position = position - direction * velocity * 1.5f;

			if (angle < TRACK_ANGLE_BOUNCE && angle > -TRACK_ANGLE_BOUNCE)
			{
				texture.rotate(-angle * TRACK_BOUNCE_POWER);
				direction = rotateVector(direction, -angle * TRACK_BOUNCE_POWER);
				velocity = velocity / 4;
			}
			else
				velocity = 0;

			return true;
		}
	}
	// ------------------------------------------------------- check for internal track
	for (int i = 0; i < 7; i++)
	{
		if (collisionSegmentCircle(trackCollisionPoints[i + 12], trackCollisionPoints[(i + 1) % 7 + 12], position))
		{
			float angle = angleBetweenVectors(trackCollisionPoints[(i + 1) % 7 + 12] - trackCollisionPoints[i + 12], direction) * 180 / PI;
			position = position - direction * velocity * 1.5f;

			if (angle < TRACK_ANGLE_BOUNCE && angle > -TRACK_ANGLE_BOUNCE)
			{
				texture.rotate(-angle * TRACK_BOUNCE_POWER);
				direction = rotateVector(direction, -angle * TRACK_BOUNCE_POWER);
				velocity = velocity / 4;
			}
			else
				velocity = 0;
			return true;
		}
	}
	// ------------------------------------------------------- check for internal vertices
	for (int i = 0; i < 7; i++)
	{
		if (vectorLength(trackCollisionPoints[i + 12] - position) < PLAYER_SIZE + 5.f)
		{
			float angle = angleBetweenVectors(trackCollisionPoints[i + 12] - trackCollisionPoints[(i + 6) % 7 + 12], direction) * 180 / PI;
			position = position - direction * velocity * 1.5f;

			if (angle < 120.f && angle > 0.f)
				position = position - rotateVector(direction, 90.f) * 5.f;
			else
				position = position - direction * 25.f;
			
			velocity = velocity / 4;
			texture.setPosition(position);

			return true;
		}
	}
	return false;
}


//=======================================================================================================================================================
//=============================================== RAYS UPDATE ===========================================================================================
//=======================================================================================================================================================


void Player::raysUpdate(Vector2f* trackCollisionPoints)
{
	const float PI = 3.14159265f;

	Vector2f raysDirection[11];
	raysDirection[0] = rotateVector(direction, -90.f);
	raysDirection[1] = rotateVector(direction, -60.f);
	raysDirection[2] = rotateVector(direction, -45.f);
	raysDirection[3] = rotateVector(direction, -30.f);
	raysDirection[4] = rotateVector(direction, -15.f);
	raysDirection[5] = rotateVector(direction, -0.f);
	raysDirection[6] = rotateVector(direction, 15.f);
	raysDirection[7] = rotateVector(direction, 30.f);
	raysDirection[8] = rotateVector(direction, 45.f);
	raysDirection[9] = rotateVector(direction, 60.f);
	raysDirection[10] = rotateVector(direction, 90.f);

	for (int i = 0; i < RAYS_NUMBER; i++)
		raysDistance[i] = 1700.f;

	// ------------------------------------------------------- check for external track
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < RAYS_NUMBER; j++)
		{
			if (segmentsCollision(trackCollisionPoints[i], trackCollisionPoints[(i + 1) % 12], position, position + raysDirection[j] * raysDistance[j]))
			{
				Vector2f intersection = linesIntersection(trackCollisionPoints[i], trackCollisionPoints[(i + 1) % 12], position, position + raysDirection[j] * raysDistance[j]);
				float distanceToIntersection = vectorLength(intersection - position);
				if (distanceToIntersection < raysDistance[j])
					raysDistance[j] = distanceToIntersection;
			}
		}
	}
	// ------------------------------------------------------- check for internal track
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < RAYS_NUMBER; j++)
		{
			if (segmentsCollision(trackCollisionPoints[i + 12], trackCollisionPoints[(i + 1) % 7 + 12], position, position + raysDirection[j] * raysDistance[j]))
			{
				Vector2f intersection = linesIntersection(trackCollisionPoints[i + 12], trackCollisionPoints[(i + 1) % 7 + 12], position, position + raysDirection[j] * raysDistance[j]);
				float distanceToIntersection = vectorLength(intersection - position);
				if (distanceToIntersection < raysDistance[j])
					raysDistance[j] = distanceToIntersection;
			}
		}
	}
}


//=======================================================================================================================================================
//=============================================== VECTOR LENGTH =========================================================================================
//=======================================================================================================================================================


const float vectorLength(const Vector2f& vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y);
}


//=======================================================================================================================================================
//=============================================== NORMALIZE VECTOR ======================================================================================
//=======================================================================================================================================================


const Vector2f normalizeVector(const Vector2f& vector)
{
	return vector / vectorLength(vector);
}


//=======================================================================================================================================================
//=============================================== AGNLE BETWEEN VECTORS =================================================================================
//=======================================================================================================================================================


const float angleBetweenVectors(const Vector2f& vector1, const Vector2f& vector2)
{
	const float dot = vector1.x * vector2.x + vector1.y * vector2.y;
	const float det = vector1.x * vector2.y - vector1.y * vector2.x;
	return atan2(det, dot);
}


//=======================================================================================================================================================
//=============================================== ROTATE VECTOR =========================================================================================
//=======================================================================================================================================================


const Vector2f rotateVector(Vector2f vector, float clockwiseAngle)
{
	const float PI = 3.14159265f;
	Vector2f rotatedVector;
	rotatedVector.x = vector.x * cos(clockwiseAngle * PI / 180) - vector.y * sin(clockwiseAngle * PI / 180);
	rotatedVector.y = vector.x * sin(clockwiseAngle * PI / 180) + vector.y * cos(clockwiseAngle * PI / 180);
	return rotatedVector;
}


//=======================================================================================================================================================
//=============================================== COLLISON SEGMENT CIRCLE ===============================================================================
//=======================================================================================================================================================


bool collisionSegmentCircle(Vector2f segmentBeggining, Vector2f segmentEnding, Vector2f circlePosition)
{
	const float PI = 3.14159265f;
	const float angle = abs(angleBetweenVectors(segmentEnding - segmentBeggining, circlePosition - segmentBeggining));
	if (angle > PI / 2)
		return false;

	float distanceToCircle = vectorLength(circlePosition - segmentBeggining);
	float segmentLength = vectorLength(segmentEnding - segmentBeggining);

	if (cos(angle) * distanceToCircle > segmentLength)
		return false;
	else if (sin(angle) * distanceToCircle > PLAYER_CIRCLE_RADIUS)
		return false;
	else
		return true;
}


//=======================================================================================================================================================
//=============================================== SEGMENTS COLLISION0 ===================================================================================
//=======================================================================================================================================================


bool segmentsCollision(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2)
{
	const float uA = ((end2.x - begin2.x)*(begin1.y - begin2.y) - (end2.y - begin2.y)*(begin1.x - begin2.x)) / ((end2.y - begin2.y)*(end1.x - begin1.x) - (end2.x - begin2.x)*(end1.y - begin1.y));
	const float uB = ((end1.x - begin1.x)*(begin1.y - begin2.y) - (end1.y - begin1.y)*(begin1.x - begin2.x)) / ((end2.y - begin2.y)*(end1.x - begin1.x) - (end2.x - begin2.x)*(end1.y - begin1.y));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
		return true;
	else
		return false;
}


//=======================================================================================================================================================
//=============================================== LINES INTERSECTION ====================================================================================
//=======================================================================================================================================================


Vector2f linesIntersection(const Vector2f& begin1, const Vector2f& end1, const Vector2f& begin2, const Vector2f& end2)
{
	Vector2f intersection;
	const float denominator = (begin1.x - end1.x) * (begin2.y - end2.y) - (begin1.y - end1.y) * (begin2.x - end2.x);
	intersection.x = ((begin1.x * end1.y - begin1.y * end1.x) * (begin2.x - end2.x) - (begin1.x - end1.x) * (begin2.x * end2.y - begin2.y * end2.x)) / denominator;
	intersection.y = ((begin1.x * end1.y - begin1.y * end1.x) * (begin2.y - end2.y) - (begin1.y - end1.y) * (begin2.x * end2.y - begin2.y * end2.x)) / denominator;
	return intersection;
}