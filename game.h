/************************************************************
 *
 * game.h
 *
 * Holds the game state
 * Controls the flow of the game
 *
************************************************************/

#pragma once
#include "player.h"
#include "suspect.h"
#include "room.h"
#include "item.h"
#include <string>
#include <vector>

enum View {
    VIEW_TOWER,
    VIEW_ROOM,
    VIEW_INVENTORY
};

enum Difficulty {
    DIFF_EASY,      // -> Pickup all items as fast as possible. No time limit
    DIFF_MEDIUM,    // ->
    DIFF_HARD,      // ->
    DIFF_NIGHTMARE  // ->
};

class Game {
    public:
        Game(std::string playerName, Difficulty difficulty);

        void showStoryLine();
        void showHelpScreen();

        void displayView();
        void command();

        // Accesor methods
        bool getFoundKiller();
        bool getGameOver();

    private:
        // Private variables
        Player player;
        std::vector<Room> rooms;
        View view;
        Difficulty difficulty;
        bool gameOver;
        bool foundKiller;

        // Initializer methods
        void createRooms();
        void createItems();
        void createSuspects();

        // Game view methods
        void cycleView();
        void displayTower();
        void displayRoom();
        void displayInventory();

        // Utility commands
        void confirmQuit();
        void invalidCommand();
};
