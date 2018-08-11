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

using namespace sf;

int lapPassed(const Player& player, Vector2f* trackCheckpoints); // returns how many percent of lap is passed (require 100 checkpoints)
double reward(double lapTime, int lapPassed); // returns reward value of passing lap. Value interval [-1, 11]. Depends on percent of lap passed and time of race.
void printState(const Player& player, bool raceFinished, double reward); // cout current player state
void readControl(bool& imitationTurnLeft, bool& imitationTurnRight, bool& imitationThrottle, bool& imitationBrake); // cin player control
void setCarCounter(ConvexShape& carCounterPointer, Text& carCounterVelocityText, const Player& player); // set car counter pointer and car velocity text
std::string milisecondsToString(int miliseconds); // convert miliseconds to formated string (eg: 6056 -> 6.056)
void constructTrack(RectangleShape* track, ConvexShape* internalObstacle); // construct track
void endingWindow(RenderWindow& window, float* const lapsTime, bool* lapsConfirmed); // print ending windows with laps time and enable to save state and control history