// Copyright 2010 owners of the AI Challenge project
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at http://www.apache.org/licenses/LICENSE-2.0 . Unless
// required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// Author:	Jeff Cameron (jeff@jpcameron.com)
// ported to C++ by Albert Zeyer
//
// Stores the game state.

#ifndef __PW__GAME_H__
#define __PW__GAME_H__

#include <string>
#include <ostream>
#include <vector>
#include <list>
#include "PlanetWars.h"

struct Game {
	// Store all the planets and fleets. OMG we wouldn't wanna lose all the
	// planets and fleets, would we!?
	typedef std::vector<Planet> Planets;
	typedef std::vector<Fleet> Fleets;
	Planets planets;
	Fleets fleets;
	
	// The maximum length of the game in turns. After this many turns, the game
	// will end, with whoever has the most ships as the winner. If there is no
	// player with the most ships, then the game is a draw.
	int maxGameLength;
	int numTurns;
	
	// This is the game playback string. It's a complete description of the
	// game. It can be read by a visualization program to visualize the game.
	std::ostream* gamePlayback;
	
	// This is the name of the file in which to write log messages.
	std::ostream* logFile;

    // This constructor does not actually initialize the game object. You must
    // always call Init() before the game object will be in any kind of
    // coherent state.
	Game(int _maxGameLength = 0, std::ostream* _gamePlayback = NULL, std::ostream* _logFile = NULL)
	: maxGameLength(_maxGameLength), numTurns(0),
	gamePlayback(_gamePlayback), logFile(_logFile) {}

	// Parses a game state from a string. On success, returns true. On failure, returns false.
	bool ParseGameState(const std::string& s);
	bool ParseGamePlaybackInitial(const std::string& s);
	bool ParseGamePlaybackChunk(const std::string& s, const Game& initialGame);
	
	// Loads a map from a text file. The text file contains a description of
	// the starting state of a game. See the project wiki for a description of
	// the file format. It should be called the Planet Wars Point-in-Time
	// format. On success, return 1. On failure, returns 0.
	int LoadMapFromFile(const std::string& mapFilename);
		
	// Returns the number of planets. Planets are numbered starting with 0.
	int NumPlanets() { return planets.size(); }
		
    // Returns the number of fleets.
    int NumFleets() { return fleets.size(); }
		
	// Writes a string which represents the current game state. No point-of-
    // view switching is performed.
    std::string toString() { return PovRepresentation(-1); }
	
    // Writes a string which represents the current game state. This string
    // conforms to the Point-in-Time format from the project Wiki.
    //
    // Optionally, you may specify the pov (Point of View) parameter. The pov
    // parameter is a player number. If specified, the player numbers 1 and pov
    // will be swapped in the game state output. This is used when sending the
    // game state to individual players, so that they can always assume that
    // they are player number 1.
    std::string PovRepresentation(int pov);
	
    // Carries out the point-of-view switch operation, so that each player can
    // always assume that he is player number 1. There are three cases.
    // 1. If pov < 0 then no pov switching is being used. Return player_id.
    // 2. If player_id == pov then return 1 so that each player thinks he is
    //    player number 1.
    // 3. If player_id == 1 then return pov so that the real player 1 looks
    //    like he is player number "pov".
    // 4. Otherwise return player_id, since players other than 1 and pov are
    //    unaffected by the pov switch.
    static int PovSwitch(int pov, int playerID);
			
    // Returns the distance between two planets, rounded up to the next highest
    // integer. This is the number of discrete time steps it takes to get
    // between the two planets.
    int Distance(int sourcePlanet, int destinationPlanet);
	
	// Executes one time step.
	//   * Planet bonuses are added to non-neutral planets.
	//   * Fleets are advanced towards their destinations.
	//   * Fleets that arrive at their destination are dealt with.
	void DoTimeStep();
	
	// Issue an order. This function takes num_ships off the source_planet,
	// puts them into a newly-created fleet, calculates the distance to the
	// destination_planet, and sets the fleet's total trip time to that
	// distance. Checks that the given player_id is allowed to give the given
	// order. If not, the offending player is kicked from the game. If the
	// order was carried out without any issue, and everything is peachy, then
	// 0 is returned. Otherwise, -1 is returned.
	int IssueOrder(int playerID,
						  int sourcePlanet,
						  int destinationPlanet,
						  int numShips);		

	// Behaves just like the longer form of IssueOrder, but takes a string
	// of the form "source_planet destination_planet num_ships". That is, three
	// integers separated by space characters.
	int IssueOrder(int playerID, const std::string& order);
		
	void AddFleet(const Fleet& f) { fleets.push_back(f); }
	
	// Kicks a player out of the game. This is used in cases where a player
	// tries to give an illegal order or runs over the time limit.
	void DropPlayer(int playerID);		

	// Returns true if the named player owns at least one planet or fleet.
	// Otherwise, the player is deemed to be dead and false is returned.
	bool IsAlive(int playerID);
	
	// If the game is not yet over (ie: at least two players have planets or
	// fleets remaining), returns -1. If the game is over (ie: only one player
	// is left) then that player's number is returned. If there are no
	// remaining players, then the game is a draw and 0 is returned.
	int Winner();
			
	// Returns the number of ships that the current player has, either located
	// on planets or in flight.
	int NumShips(int playerID);
	
	//Resolves the battle at planet p, if there is one.
    //* Removes all fleets involved in the battle
    //* Sets the number of ships and owner of the planet according the outcome
    void __FightBattle(Planet& p);
	
	void WriteLogMessage(const std::string& message) {
		if(logFile) *logFile << message << std::endl;
	}
};

#endif
