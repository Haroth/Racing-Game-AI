#include "functions.h"


//=======================================================================================================================================================
//=============================================== LAP PASSED ============================================================================================
//=======================================================================================================================================================


int lapPassed(const Player& player, Vector2f* trackCheckpoints)
{
	static int nearestCheckpoint = 0;

	int minDistance = vectorLength(trackCheckpoints[nearestCheckpoint] - player.getPosition());

	for (int i = nearestCheckpoint - 10; i < nearestCheckpoint + 10; i++)
	{
		int j = i;
		if (i < 0)
			j = j + 100;
		else if (i >= 100)
			j = j % 100;
		if (vectorLength(trackCheckpoints[j] - player.getPosition()) < minDistance)
		{
			nearestCheckpoint = j;
			minDistance = vectorLength(trackCheckpoints[j] - player.getPosition());
		}
	}
	return nearestCheckpoint;
}


//=======================================================================================================================================================
//=============================================== REWARD ================================================================================================
//=======================================================================================================================================================


double reward(double lapTime, int lapPassed)
{
	if (lapPassed == 100 && lapTime < 10.f)
		return 11 - lapTime; // [1, 21]
	else
		return lapPassed / 100.f; // [-1, 1)
}


//=======================================================================================================================================================
//=============================================== PRINT STATE ===========================================================================================
//=======================================================================================================================================================


void printState(const Player& player, bool raceFinished, double reward)
{
	std::cout << player.getPosition() << " " << player.getDirection() << " " << player.getVelocity() << " ";
	for (int j = 0; j < RAYS_NUMBER; j++)
		std::cout << player.getRaysDistance()[j] << " ";
	std::cout << raceFinished << " " << reward << " ";
	std::cout << std::endl;
	std::cout.flush();
}


//=======================================================================================================================================================
//=============================================== READ CONTROL ==========================================================================================
//=======================================================================================================================================================


void readControl(bool& imitationTurnLeft, bool& imitationTurnRight, bool& imitationThrottle, bool& imitationBrake)
{
	/*
	float controlChance[4];
	for (int i = 0; i < 4; i++)
		std::cin >> controlChance[i];
	float random = rand() % 1000 / 1000.f;
	if (random < controlChance[0])
		imitationTurnLeft = true;
	random = rand() % 1000 / 1000.f;
	if (random < controlChance[1])
		imitationTurnRight = true;
	random = rand() % 1000 / 1000.f;
	if (random < controlChance[2])
		imitationThrottle = true;
	random = rand() % 1000 / 1000.f;
	if (random < controlChance[3])
		imitationBrake = true;
	*/

	float controlChance[4];
	for (int i = 0; i < 4; i++)
		std::cin >> controlChance[i];
	imitationTurnLeft = controlChance[0];
	imitationTurnRight = controlChance[1];
	imitationThrottle = controlChance[2];
	imitationBrake = controlChance[3];

	//std::cin >> imitationTurnLeft >> imitationTurnRight >> imitationThrottle >> imitationBrake;
	std::cin.clear();
}


//=======================================================================================================================================================
//=============================================== SET CAR COUNTER =======================================================================================
//=======================================================================================================================================================


void setCarCounter(ConvexShape& carCounterPointer, Text& carCounterVelocityText, const Player& player)
{
	int carVelocity = player.getVelocity() / PLAYER_MAX_VELOCITY * 220.f;
	carCounterVelocityText.setString(std::to_string(carVelocity));
	carCounterPointer.setRotation(251.5f + 216.f * carVelocity / 220.f);
}


//=======================================================================================================================================================
//=============================================== MS TO STRING ==========================================================================================
//=======================================================================================================================================================


std::string milisecondsToString(int miliseconds)
{
	std::string formatedString;
	if (miliseconds % 1000 < 10)
		formatedString = std::to_string(miliseconds / 1000) + ".00" + std::to_string(miliseconds % 1000);
	else if (miliseconds % 1000 < 100)
		formatedString = std::to_string(miliseconds / 1000) + ".0" + std::to_string(miliseconds % 1000);
	else
		formatedString = std::to_string(miliseconds / 1000) + "." + std::to_string(miliseconds % 1000);
	return formatedString;
}


//=======================================================================================================================================================
//=============================================== CONSTRUCT TRACK =======================================================================================
//=======================================================================================================================================================


void constructTrack(RectangleShape* track, ConvexShape* internalObstacle)
{
	// ------------------------------------------------------- external track

	track[0].setPosition(50.f, 400.f);
	track[0].setSize(Vector2f(600.f, 10.f));
	track[0].setFillColor(Color::White);
	track[0].setRotation(-38.f);

	track[1].setPosition(522.f, 30.f);
	track[1].setSize(Vector2f(300.f, 10.f));
	track[1].setFillColor(Color::White);

	track[2].setPosition(822.f, 30.f);
	track[2].setSize(Vector2f(72.f, 10.f));
	track[2].setFillColor(Color::White);
	track[2].setRotation(90.f);

	track[3].setPosition(821.f, 94.f);
	track[3].setSize(Vector2f(250.f, 10.f));
	track[3].setFillColor(Color::White);
	track[3].setRotation(30.f);

	track[4].setPosition(1029.f, 217.f);
	track[4].setSize(Vector2f(450.f, 10.f));
	track[4].setFillColor(Color::White);
	track[4].setRotation(-23.f);

	track[5].setPosition(1443.f, 40.f);
	track[5].setSize(Vector2f(336.f, 10.f));
	track[5].setFillColor(Color::White);
	track[5].setRotation(31.5f);

	track[6].setPosition(1730.f, 217.f);
	track[6].setSize(Vector2f(400.f, 10.f));
	track[6].setFillColor(Color::White);
	track[6].setRotation(90.f);

	track[7].setPosition(1727.f, 618.f);
	track[7].setSize(Vector2f(257.f, 10.f));
	track[7].setFillColor(Color::White);
	track[7].setRotation(145.f);


	track[8].setPosition(1516.f, 767.f);
	track[8].setSize(Vector2f(335.f, 10.f));
	track[8].setFillColor(Color::White);
	track[8].setRotation(180.f);

	track[9].setPosition(1182.f, 767.f);
	track[9].setSize(Vector2f(155.f, 10.f));
	track[9].setFillColor(Color::White);
	track[9].setRotation(215.f);

	track[10].setPosition(1060.f, 678.f);
	track[10].setSize(Vector2f(505.f, 10.f));
	track[10].setFillColor(Color::White);
	track[10].setRotation(180.f);

	track[11].setPosition(557.f, 678.f);
	track[11].setSize(Vector2f(575.f, 10.f));
	track[11].setFillColor(Color::White);
	track[11].setRotation(208.5f);


	// ------------------------------------------------------- internal obstacle

	internalObstacle[0].setPointCount(3);
	internalObstacle[0].setPoint(0, Vector2f(355.f, 388.f));
	internalObstacle[0].setPoint(1, Vector2f(595.f, 195.f));
	internalObstacle[0].setPoint(2, Vector2f(1126.f, 430.f));
	internalObstacle[0].setFillColor(Color::White);

	internalObstacle[1].setPointCount(3);
	internalObstacle[1].setPoint(0, Vector2f(355.f, 388.f));
	internalObstacle[1].setPoint(1, Vector2f(1126.f, 430.f));
	internalObstacle[1].setPoint(2, Vector2f(1294.f, 513.f));
	internalObstacle[1].setFillColor(Color::White);

	internalObstacle[2].setPointCount(5);
	internalObstacle[2].setPoint(0, Vector2f(1126.f, 430.f));
	internalObstacle[2].setPoint(1, Vector2f(1205.f, 353.f));
	internalObstacle[2].setPoint(2, Vector2f(1412.f, 320.f));
	internalObstacle[2].setPoint(3, Vector2f(1488.f, 417.f));
	internalObstacle[2].setPoint(4, Vector2f(1294.f, 513.f));
	internalObstacle[2].setFillColor(Color::White);
}


//=======================================================================================================================================================
//=============================================== ENDING WINDOW =========================================================================================
//=======================================================================================================================================================


void endingWindow(RenderWindow& window, float* const lapsTime, bool* lapsConfirmed)
{
	// ------------------------------------------------------- initialize

	Font PinkBlue;
	if (!PinkBlue.loadFromFile("Fonts/Pink Blue.ttf"))
		throw("Could not load font!");

	Text gameTitleText;
	gameTitleText.setCharacterSize(120);
	gameTitleText.setFillColor(Color::Magenta);
	gameTitleText.setFont(PinkBlue);
	gameTitleText.setPosition(325.f, 50.f);
	gameTitleText.setString("One Last Ride Toretto");

	Text lapNumberText;
	lapNumberText.setCharacterSize(60);
	lapNumberText.setFillColor(Color::White);
	lapNumberText.setFont(PinkBlue);

	Texture saveButtonTexture;
	saveButtonTexture.loadFromFile("Textures/save.png");
	RectangleShape saveButton;
	saveButton.setSize(Vector2f(50.f, 50.f));
	saveButton.setTexture(&saveButtonTexture);


	// ------------------------------------------------------- draw

	window.clear(Color::Black);

	for (int i = 0; i < 5; i++)
	{
		if (lapsConfirmed[i] == true)
			lapNumberText.setFillColor(Color::Green);
		else
			lapNumberText.setFillColor(Color::White);
		int durationInt = lapsTime[i] * 1000;
		if (durationInt % 1000 < 10)
			lapNumberText.setString("Lap " + std::to_string(i + 1) + ": " + std::to_string(durationInt / 1000) + ".00" + std::to_string(durationInt % 1000));
		else if (durationInt % 1000 < 100)
			lapNumberText.setString("Lap " + std::to_string(i + 1) + ": " + std::to_string(durationInt / 1000) + ".0" + std::to_string(durationInt % 1000));
		else
			lapNumberText.setString("Lap " + std::to_string(i + 1) + ": " + std::to_string(durationInt / 1000) + "." + std::to_string(durationInt % 1000));
		lapNumberText.setPosition(535.f, 250.f + 100.f * i);
		window.draw(lapNumberText);

		saveButton.setPosition(870.f, 260.f + 100.f * i);
		window.draw(saveButton);
	}

	window.draw(gameTitleText);
	window.display();
}