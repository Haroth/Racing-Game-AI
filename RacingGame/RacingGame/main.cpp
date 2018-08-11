#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <fstream>
#include <cstdio> // zegar
#include <ctime> // zegar

#include <cstdlib> // random

#include <ratio>  // clock
#include <chrono>

#include <vector>

#include "player.h"
#include "functions.h"

using namespace sf;

//std::cout << Vector2f(Mouse::getPosition(window)).x << " " << Vector2f(Mouse::getPosition(window)).y << std::endl;

int main()
{
	// ------------------------------------------------------- parameters to manipulate control version and window / simulation version
	bool imitationBotControl = false;
	bool playerControl = true;
	bool botControl = false;
	bool windowVersion = true;

	srand(time(NULL));


	// ------------------------------------------------------- joystick power check
	sf::Joystick::update();
	if (Joystick::isConnected(0) && playerControl == true)
		std::cout << "Joystick is connected" << std::endl;
	else if (!Joystick::isConnected(0) && playerControl == true)
		std::cout << "Joystick is not connected" << std::endl;


	// ------------------------------------------------------- textures and window shapes
	Texture carTexture;
	std::string texName = "Textures/car" + std::to_string(rand() % 8) + ".png";
	carTexture.loadFromFile(texName);

	Texture carCounterTexture;
	carCounterTexture.loadFromFile("Textures/carCounter.png");
	RectangleShape carCounter;
	carCounter.setTexture(&carCounterTexture);
	carCounter.setSize(Vector2f(354.f, 248.f));
	carCounter.setPosition(25.f, 525.f);

	ConvexShape carCounterPointer;
	carCounterPointer.setPointCount(4);
	carCounterPointer.setPoint(0, Vector2f(-2.f, 150.f));
	carCounterPointer.setPoint(1, Vector2f(6.f, 150.f));
	carCounterPointer.setPoint(2, Vector2f(3.f, 0.f));
	carCounterPointer.setPoint(3, Vector2f(1.f, 0.f));
	carCounterPointer.setOrigin(Vector2f(2.f, 150.f));
	carCounterPointer.setPosition(198.f, 696.f);
	carCounterPointer.setFillColor(Color::White);

	// ---------------------------------------------------------------------------------------------- texts

	Font PinkBlue;
	if (!PinkBlue.loadFromFile("Fonts/Pink Blue.ttf"))
		throw("Could not load font!");

	Text carCounterVelocityText;
	carCounterVelocityText.setCharacterSize(48);
	carCounterVelocityText.setFillColor(Color::White);
	carCounterVelocityText.setFont(PinkBlue);
	carCounterVelocityText.setPosition(175.f, 710.f);

	Text lapTimeText;
	lapTimeText.setCharacterSize(48);
	lapTimeText.setFillColor(Color::White);
	lapTimeText.setFont(PinkBlue);
	lapTimeText.setPosition(25.f, 25.f);

	Text lapTimeCenterText;
	lapTimeCenterText.setCharacterSize(200);
	lapTimeCenterText.setFillColor(Color::Magenta);
	lapTimeCenterText.setFont(PinkBlue);
	lapTimeCenterText.setPosition(420.f, 210.f);

	int lapTimeCenterFreezeTime = 0;

	Text lapsCounterText;
	lapsCounterText.setCharacterSize(48);
	lapsCounterText.setFillColor(Color::White);
	lapsCounterText.setFont(PinkBlue);
	lapsCounterText.setPosition(25.f, 75.f);


	// ---------------------------------------------------------------------------------------------- track and checkpoints

	Vector2f trackCheckpoints [100];
	std::ifstream fileIn;
	fileIn.open("checkpoints.txt");
	for (int i = 0; i < 100; i++)
	{
		float x, y;
		fileIn >> x >> y;
		trackCheckpoints[i] = Vector2f(x, y);
	}

	RectangleShape track[12];
	ConvexShape internalObstacle[3];

	constructTrack(track, internalObstacle);
	Vector2f trackCollisionPoints[19] = 
	{	Vector2f(67.f, 400.f),
		Vector2f(526.f, 40.f),
		Vector2f(811.f, 40.f),
		Vector2f(811.f, 101.f),
		Vector2f(1030.f, 227.f),
		Vector2f(1443.f, 52.f),
		Vector2f(1719.f, 222.f),
		Vector2f(1719.f, 610.f),
		Vector2f(1511.f, 757.f),
		Vector2f(1185.f, 756.f),
		Vector2f(1059.f, 668.f),
		Vector2f(558.f, 666.f),

		Vector2f(355.f, 388.f),
		Vector2f(595.f, 195.f),
		Vector2f(1126.f, 430.f),
		Vector2f(1205.f, 353.f),
		Vector2f(1412.f, 320.f),
		Vector2f(1488.f, 417.f),
		Vector2f(1294.f, 513.f)};


	Texture metaTexture;
	metaTexture.loadFromFile("Textures/meta.png");
	RectangleShape meta;
	meta.setSize(Vector2f(260.f, 50.f));
	meta.setRotation(90.f);
	meta.setPosition(600.f, 410.f);
	meta.setTexture(&metaTexture);



	// ---------------------------------------------------------------------------------------------- player creation

	const Vector2f spawnPosition = Vector2f(1500.f, 550.f);
	const Vector2f spawnDirection = Vector2f(-0.71f, 0.71f);

	std::vector <StateAndControl> stateAndControlHistory [5];
	float lapsTime[5] = { 0.f, 0.f, 0.f, 0.f, 0.f };
	bool lapsConfirmed[5] = { false, false, false, false, false };

	int lapsCounter = 0;
	int lapsGoal = 5;

	bool gameRunning = true;
	bool firstFrame = true;


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++ MAIN ++
	///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	if (windowVersion == true)
	{
		RenderWindow window(VideoMode(1800, 800), "Racing Game", Style::Default);
		window.setFramerateLimit(60);
		Player player(spawnPosition, spawnDirection, &carTexture);

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			static bool raceStarted = false;

			//==============================================================================================================
			//=============================================== JOYSTICK =====================================================
			//==============================================================================================================

			if (gameRunning == true)
			{
				if (playerControl == true)
				{
					Vector2f joystickDirection;
					bool joystickThrottle = false;
					bool joystickBrake = false;
					bool joystickTurnLeft = false;
					bool joystickTurnRight = false;

					joystickDirection.x = Joystick::getAxisPosition(0, sf::Joystick::X);
					if (joystickDirection.x < 25 && joystickDirection.x > -25)
						joystickDirection.x = 0;
					joystickDirection.y = Joystick::getAxisPosition(0, sf::Joystick::Y);
					if (joystickDirection.y < 25 && joystickDirection.y > -25)
						joystickDirection.y = 0;

					if (Joystick::getAxisPosition(0, sf::Joystick::Z) < -50.f)
						joystickThrottle = true;
					else if (Joystick::getAxisPosition(0, sf::Joystick::Z) > 50.f)
						joystickBrake = true;

					if (vectorLength(joystickDirection) > 25.f)
					{
						if (angleBetweenVectors(player.getDirection(), joystickDirection) < -0.1f)
							joystickTurnLeft = true;
						else if (angleBetweenVectors(player.getDirection(), joystickDirection) > 0.1f)
							joystickTurnRight = true;
					}
					player.move(joystickTurnLeft, joystickTurnRight, joystickThrottle, joystickBrake);


					// ------------------------------------------------------- aggregate state and control
					StateAndControl currentState;
					currentState.position = player.getPosition();
					currentState.direction = player.getDirection();
					currentState.velocity = player.getVelocity();

					for (int i = 0; i < RAYS_NUMBER; i++)
						currentState.raysDistance[i] = player.getRaysDistance()[i];
					currentState.controlTurnLeft = joystickTurnLeft;
					currentState.controlTurnRight = joystickTurnRight;
					currentState.controlThrottle = joystickThrottle;
					currentState.controlBrake = joystickBrake;

					if (raceStarted == true)
						stateAndControlHistory[lapsCounter].push_back(currentState);

					if (Joystick::isButtonPressed(0, 0) == true)
						lapsConfirmed[lapsCounter - 1] = true;
				}


				//==============================================================================================================
				//=============================================== IMITATION BOT ================================================
				//==============================================================================================================


				if (imitationBotControl == true && firstFrame == false)
				{
					bool imitationTurnLeft = false;
					bool imitationTurnRight = false;
					bool imitationThrottle = false;
					bool imitationBrake = false;

					printState(player, false, 0);
					readControl(imitationTurnLeft, imitationTurnRight, imitationThrottle, imitationBrake);

					player.move(imitationTurnLeft, imitationTurnRight, imitationThrottle, imitationBrake);
				}
				firstFrame = false;


				//==============================================================================================================
				//=============================================== BOT ==========================================================
				//==============================================================================================================


				if (botControl == true)
				{
					bool botTurnLeft = false;
					bool botTurnRight = false;
					bool botThrottle = false;
					bool botBrake = false;

					float botVelocity = player.getVelocity();
					float brakeLimit = 0.05 * botVelocity;

					if (-exp(-player.getRaysDistance()[5] / 500.f) + 1 < brakeLimit || -exp(-player.getRaysDistance()[4] / 500.f) + 1 < brakeLimit || -exp(-player.getRaysDistance()[6] / 500.f) + 1 < brakeLimit)
						if (botVelocity > 3.f)
							botBrake = true;

					float turnEpsilon = 0.02;
					if ((-exp(-player.getRaysDistance()[1] / 500.f) + 1) < (-exp(-player.getRaysDistance()[9] / 500.f) + 1) - turnEpsilon)
						botTurnRight = true;

					else if ((-exp(-player.getRaysDistance()[1] / 500.f) + 1) > (-exp(-player.getRaysDistance()[9] / 500.f) + 1) + turnEpsilon)
						botTurnLeft = true;

					else if (botVelocity < 8.f)
						botThrottle = true;

					player.move(botTurnLeft, botTurnRight, botThrottle, botBrake);

					// ------------------------------------------------------- aggregate state and control
					StateAndControl currentState;
					currentState.position = player.getPosition();
					currentState.direction = player.getDirection();
					currentState.velocity = player.getVelocity();
					for (int i = 0; i < RAYS_NUMBER; i++)
						currentState.raysDistance[i] = player.getRaysDistance()[i];
					currentState.controlTurnLeft = botTurnLeft;
					currentState.controlTurnRight = botTurnRight;
					currentState.controlThrottle = botThrottle;
					currentState.controlBrake = botBrake;

					if (raceStarted == true)
						stateAndControlHistory[lapsCounter].push_back(currentState);

					// ------------------------------------------------------- dislocate agent with some noise
					static bool prevJoystickButton2 = false;
					if (Joystick::isButtonPressed(0, 2) == true && prevJoystickButton2 == false)
						player.setPosition(player.getPosition() + Vector2f((rand() % 3 - 1) * 40.f, (rand() % 3 - 1) * 40.f));
					prevJoystickButton2 = Joystick::isButtonPressed(0, 2);
				}


				//==============================================================================================================
				//=============================================== COLISIONS AND LAP PASS CHECK =================================
				//==============================================================================================================

				
				// ------------------------------------------------------- lap pass check
				static bool halfTrackPassed = false;
				static int frameCounterLap = 0;
				int lapPassedPercentage = lapPassed(player, trackCheckpoints);

				if (raceStarted == false && lapPassedPercentage >= 0 && lapPassedPercentage <= 5)
					raceStarted = true;
				if (raceStarted)
					frameCounterLap++;
				int lapDurationMiliseconds = frameCounterLap * 16.667f;
					
				if (lapPassedPercentage > 50 && lapPassedPercentage < 60)
					halfTrackPassed = true;

				lapsCounterText.setString("Laps: " + std::to_string(lapsCounter + 1) + "/" + std::to_string(lapsGoal));
				lapTimeText.setString(milisecondsToString(lapDurationMiliseconds));

				// ------------------------------------------------------- meta passed
				if (halfTrackPassed == true && lapPassedPercentage >= 0 && lapPassedPercentage <= 5)
				{
					halfTrackPassed = false;
					lapTimeCenterFreezeTime = 90;
					
					lapTimeCenterText.setString(lapTimeText.getString());
					lapsTime[lapsCounter] = lapDurationMiliseconds / 1000.f;

					frameCounterLap = 0;
					lapsCounter++;

					if (lapsCounter == lapsGoal)
						gameRunning = false;
				}

				lapTimeCenterFreezeTime--;

				// ------------------------------------------------------- collisions and rays update
				setCarCounter(carCounterPointer, carCounterVelocityText, player);
				player.checkCollisions(trackCollisionPoints);
				player.raysUpdate(trackCollisionPoints);


				//==============================================================================================================
				//=============================================== DRAW =========================================================
				//==============================================================================================================


				window.clear();
				
				window.draw(meta);
				for (int i = 0; i < 12; i++)
					window.draw(track[i]);
				for (int i = 0; i < 3; i++)
					window.draw(internalObstacle[i]);

				window.draw(lapTimeText);
				window.draw(lapsCounterText);
				if (lapTimeCenterFreezeTime > 0)
					window.draw(lapTimeCenterText);

				window.draw(carCounter);
				window.draw(carCounterPointer);
				window.draw(carCounterVelocityText);
				window.draw(player.getTexture());
				window.display();
			}

			//==============================================================================================================
			//=============================================== ENDING WINDOW ================================================
			//==============================================================================================================
			else
			{
				static bool previousMouseClicked = false;
				if (Mouse::isButtonPressed(sf::Mouse::Left) && previousMouseClicked == false && Mouse::getPosition(window).x >= 870.f && Mouse::getPosition(window).x <= 920.f)
				{
					for (int i = 0; i < 5; i++)
					{
						if (Mouse::getPosition(window).y >= 260.f + 100.f * i && Mouse::getPosition(window).y <= 310.f + 100.f * i)
						{

							std::fstream file;
							file.open("input.txt", std::ios::out | std::ios::app);
							if (file.good() == true)
							{
								for (std::vector <StateAndControl>::iterator it = stateAndControlHistory[i].begin(); it != stateAndControlHistory[i].end(); it++)
								{
									file << it->position << " " << it->direction << " " << it->velocity << " ";
									for (int j = 0; j < RAYS_NUMBER; j++)
										file << it->raysDistance[j] << " ";
									file << it->controlTurnLeft << " " << it->controlTurnRight << " " << it->controlThrottle << " " << it->controlBrake << std::endl;
								}
								std::cout << "Lap " << i + 1 << " saved to file" << std::endl;
							}
							else
								std::cout << "Unable to open a file!" << std::endl;
						}
					}
				}
				previousMouseClicked = Mouse::isButtonPressed(sf::Mouse::Left);
				endingWindow(window, lapsTime, lapsConfirmed);
			}
		}
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++ SIMULATION ++++
	///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	else
	{
		// ------------------------------------------------------- 2000 repeats of 1 lap race
		for (int i = 0; i < 2000; i++)
		{
			Player player(spawnPosition, spawnDirection, &carTexture);
			int prevLapPassed = lapPassed(player, trackCheckpoints);
			int lapPassedPercentage = lapPassed(player, trackCheckpoints) - 100;
			bool raceStarted = false;
			int frameCounterLap = 0;
			bool firstFrame = true;
			bool raceFinished = false;


			while (true)
			{
				int lapDurationMiliseconds = frameCounterLap * 16.667f;
				// ------------------------------------------------------- bot control
				if (botControl == true)
				{

					bool botTurnLeft = false;
					bool botTurnRight = false;
					bool botThrottle = false;
					bool botBrake = false;

					float botVelocity = player.getVelocity();
					float brakeLimit = 0.05 * botVelocity;

					if (-exp(-player.getRaysDistance()[5] / 500.f) + 1 < brakeLimit || -exp(-player.getRaysDistance()[4] / 500.f) + 1 < brakeLimit || -exp(-player.getRaysDistance()[6] / 500.f) + 1 < brakeLimit)
						if (botVelocity > 3.f)
							botBrake = true;

					float turnEpsilon = 0.02;
					if ((-exp(-player.getRaysDistance()[1] / 500.f) + 1) < (-exp(-player.getRaysDistance()[9] / 500.f) + 1) - turnEpsilon)
						botTurnRight = true;

					else if ((-exp(-player.getRaysDistance()[1] / 500.f) + 1) > (-exp(-player.getRaysDistance()[9] / 500.f) + 1) + turnEpsilon)
						botTurnLeft = true;
					else if (botVelocity < 8.f)
						botThrottle = true;

					player.move(botTurnLeft, botTurnRight, botThrottle, botBrake);

					if (raceFinished == false)
						printState(player, raceFinished, reward(10001.f / 1000.f, 0));
					else
						printState(player, raceFinished, reward(lapDurationMiliseconds / 1000.f, lapPassedPercentage));
					if (raceFinished == true)
					{
						sleep(seconds(25));
						break;
					}
				}

				// ------------------------------------------------------- imitation bot control
				if (imitationBotControl == true && firstFrame == false)
				{

					bool imitationTurnLeft = false;
					bool imitationTurnRight = false;
					bool imitationThrottle = false;
					bool imitationBrake = false;

					if (raceFinished == false)
						printState(player, raceFinished, reward(10001.f / 1000.f, 0));
					else
						printState(player, raceFinished, reward(lapDurationMiliseconds / 1000.f, lapPassedPercentage));
					readControl(imitationTurnLeft, imitationTurnRight, imitationThrottle, imitationBrake);

					if (raceFinished == true)
						break;
						
					player.move(imitationTurnLeft, imitationTurnRight, imitationThrottle, imitationBrake);
				}
				firstFrame = false;

				//==============================================================================================================
				//=============================================== LAP CHECK ====================================================
				//==============================================================================================================
				
				int lapPassedValue = lapPassed(player, trackCheckpoints);
				int singleFrameLapPassed = lapPassedValue - prevLapPassed;
				if (singleFrameLapPassed < -50)
					singleFrameLapPassed += 100;
				else if (singleFrameLapPassed > 50)
					singleFrameLapPassed -= 100;
				
				lapPassedPercentage += singleFrameLapPassed;
				prevLapPassed = lapPassedValue;

				if (raceStarted == false && lapPassedPercentage >= 0)
				{
					raceStarted = true;
					frameCounterLap = 0;
				}
				frameCounterLap++;

				// ------------------------------------------------------- meta passed or time out
				if (lapPassedPercentage >= 100 || lapDurationMiliseconds > 10000)
					raceFinished = true;

				// ------------------------------------------------------- check collisions and rays update
				player.raysUpdate(trackCollisionPoints);
				player.checkCollisions(trackCollisionPoints);
			}
		}
	}
	return 0;
}