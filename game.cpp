/************************************************************
 *
 * game.cpp
 *
 * Implementation of game.h
 *
 * Briano Goestiawan, 31482228
 *
************************************************************/

// NOTE: Can add upto 19 items before screen overflow in room display
// NOTE: Game mode idea: pickup all the items as fast as possible (maybe this could be easy mode)
//
// TODO:
// - Show leaderboard
// - Create suspects
// - Suspect interactions (talk, accuse)
// - Player move count
// - Write a reflection document 300-500 words
//
// TODO commands:
// - QUESTION suspects to determine alibis "I was with Bob and Charlie"
// - ACCUSE the suspect who thik is murderer

#include <iostream>
#include <sstream>
#include <algorithm>
#include "game.h"
#include "player.h"
#include "item.h"
#include "utils.h"
#define ROOM_ROWS 4
#define ROOM_COLS 3
#define ROOM_WIDTH 25    // Not including border
#define WINDOW_WIDTH 77  // Not including border
#define WINDOW_HEIGHT 39 // Not including border

using namespace std;
string readFile(string fileName);
string readInput(string prompt);

/****************************** PUBLIC METHODS ******************************/

Game::Game(string playerName, Difficulty difficulty) {
    // Create and allocate game objects
    this->createRooms();
    this->createItems();
    this->createSuspects();

    // Create player instance
    Room startingRoom = this->rooms[6];
    this->player = Player(playerName, &startingRoom);
    this->player.move(DIR_RIGHT);

    // Initialize variables
    this->gameOver = false;
    this->foundKiller = false;
    this->view = VIEW_TOWER;
    this->difficulty = difficulty;
}

// Display help screen. Wait for user before continuing
void Game::showStoryLine() {
    clearScreen();
    cout << readFile("assets/story_line.txt");
    pause();
}

// Display help screen. Wait for user before continuing
void Game::showHelpScreen() {
    clearScreen();
    cout << readFile("assets/help_screen.txt");
    pause();
}

// Draw image to screen
void Game::displayView() {
    clearScreen();
    switch (this->view) {
        case VIEW_TOWER:
            this->displayTower();
            break;
        case VIEW_ROOM:
            this->displayRoom();
            break;
        case VIEW_INVENTORY:
            this->displayInventory();
            break;
    }
}

// Validate and process user input
void Game::command() {
    // Anatomy of a command:
    //     command    argument (can be multiple words)
    //     \_____/    \______________________________/

    // Read input from user
    string input = toLower(readInput("//> "));
    string command = "";
    string argument = "";
    int spaceIndex = input.find(' ');
    if (spaceIndex == string::npos) {
        command = input.substr(0, spaceIndex);
    } else {
        command = input.substr(0, spaceIndex);
        argument = input.substr(spaceIndex + 1);
    }

    // Game view
    if (command == "view") this->cycleView();
    else if (command == "tower") this->view = VIEW_TOWER;
    else if (command == "room") this->view = VIEW_ROOM;
    else if (command == "inv") this->view = VIEW_INVENTORY;

    // Movement
    else if (command == "left") this->player.move(DIR_LEFT);
    else if (command == "right") this->player.move(DIR_RIGHT);
    else if (command == "up") this->player.move(DIR_UP);
    else if (command == "down") this->player.move(DIR_DOWN);

    // Item Interactions
    else if (command == "search") this->player.getRoom()->search();
    else if (command == "pickup") this->player.pickupItem(argument);
    else if (command == "drop") this->player.dropItem(argument);
    else if (command == "inspect") this->player.inspectItem(argument);

    // Supect Interactions
    else if (command == "talk") this->talk(argument);
    else if (command == "gather") this->gather();

    // Utility commands
    else if (command == "help") this->showHelpScreen();
    else if (command == "easter") cout << "egg" << endl, this->command();
    else if (command == "quit") this->confirmQuit();
    else this->invalidCommand();
}

bool Game::getFoundKiller() {
    return this->foundKiller;
}

bool Game::getGameOver() {
    return this->gameOver;
}

/****************************** PRIVATE METHODS ******************************/

// Generate room objects
void Game::createRooms() {
    // Create room instances and append to this->rooms
    this->rooms.push_back(Room("CONTROL CENTER", "assets/room_control_center.txt"));
    this->rooms.push_back(Room("OFFICE",         "assets/room_office.txt"));
    this->rooms.push_back(Room("SPA",            "assets/room_spa.txt"));
    this->rooms.push_back(Room("LABORATORY",     "assets/room_laboratory.txt"));
    this->rooms.push_back(Room("LIBRARY",        "assets/room_library.txt"));
    this->rooms.push_back(Room("GIFT SHOP",      "assets/room_gift_shop.txt"));
    this->rooms.push_back(Room("CAFETARIA",      "assets/room_cafetaria.txt"));
    this->rooms.push_back(Room("LOBBY",          "assets/room_lobby.txt"));
    this->rooms.push_back(Room("TOILET",         "assets/room_toilet.txt"));
    this->rooms.push_back(Room("SERVER ROOM",    "assets/room_server_room.txt"));
    this->rooms.push_back(Room("CAR PARK",       "assets/room_car_park.txt"));
    this->rooms.push_back(Room("PLUMBING ROOM",  "assets/room_plumbing_room.txt"));

    // Set one of the room to be the murder room
    this->rooms[rand() % this->rooms.size()].setMurderRoom();

    // Set room neighbours. for each room in rooms, set its left, right, up and
    // down neighbouring room if possible (not wall)
    for (int row = 0; row < ROOM_ROWS; row++) {
        for (int col = 0; col < ROOM_COLS; col++) {
            int index = row * ROOM_COLS + col;

            // Set left
            if (col > 0) {
                this->rooms[index].setNeighbour(DIR_LEFT, &this->rooms[index - 1]);
            }

            // Set right
            if (col < ROOM_COLS - 1) {
                this->rooms[index].setNeighbour(DIR_RIGHT, &this->rooms[index + 1]);
            }

            // Set up
            if (row > 0) {
                this->rooms[index].setNeighbour(DIR_UP, &this->rooms[index - ROOM_COLS]);
            }

            // Set down
            if (row < ROOM_ROWS - 1) {
                this->rooms[index].setNeighbour(DIR_DOWN, &this->rooms[index + ROOM_COLS]);
            }
        }
    }
}

// Generate items objects
void Game::createItems() {
    // Create items
    vector<Item> items;
    items.push_back(Item("Knife", "assets/item_knife.txt"));
    items.push_back(Item("Fork", "assets/item_fork.txt"));
    items.push_back(Item("Stick", "assets/item_stick.txt"));
    items.push_back(Item("Scissors", "assets/item_scissors.txt"));
    items.push_back(Item("Bowling ball", "assets/item_bowling_ball.txt"));
    items.push_back(Item("Screwdriver", "assets/item_screwdriver.txt"));
    items.push_back(Item("Chair", "assets/item_chair.txt"));
    items.push_back(Item("Vase", "assets/item_vase.txt"));

    // Put all item in items to a randomly selected room
    for (int i = 0; i < items.size(); i++) {
        Room *randomRoom = &this->rooms[rand() % this->rooms.size()];
        randomRoom->addItem(items[i]);
    }
}

// Generate suspects objects
void Game::createSuspects() {
    // Create suspects
    this->suspects.push_back(Suspect("Anna", "assets/suspect_anna.txt"));
    this->suspects.push_back(Suspect("Bob", "assets/suspect_bob.txt"));
    this->suspects.push_back(Suspect("Charlie", "assets/suspect_charlie.txt"));
    this->suspects.push_back(Suspect("Daniel", "assets/suspect_daniel.txt"));
    this->suspects.push_back(Suspect("Emma", "assets/suspect_emma.txt"));
    this->suspects.push_back(Suspect("Felix", "assets/suspect_felix.txt"));
    this->suspects.push_back(Suspect("George", "assets/suspect_george.txt"));

    // Create a vector of pointer to all suspects (for picking purpose)
    vector<Suspect*> suspectsNotPicked;
    for (int i = 0; i < this->suspects.size(); i++) {
        suspectsNotPicked.push_back(&this->suspects[i]);
    }
    /* random_shuffle(suspectsNotPicked.begin(), suspectsNotPicked.end()); */

    // Victim
    suspectsNotPicked.back()->setType(SUS_VICTIM);
    suspectsNotPicked.pop_back();

    // Killer
    suspectsNotPicked.back()->setType(SUS_KILLER);
    suspectsNotPicked.back()->setAlibi(suspectsNotPicked[rand() % suspectsNotPicked.size()]);
    suspectsNotPicked.pop_back();

    // Set alibi pairs
    while (suspectsNotPicked.size() > 1) {
        Suspect *A = suspectsNotPicked.back();
        suspectsNotPicked.pop_back();
        Suspect *B = suspectsNotPicked.back();
        suspectsNotPicked.pop_back();

        A->setAlibi(B);
        B->setAlibi(A);
    }

    // Put all suspect in suspects to a randomly selected room
    for (int i = 0; i < this->suspects.size(); i++) {
        Room *randomRoom = &this->rooms[rand() % this->rooms.size()];
        randomRoom->addSuspect(&this->suspects[i]);
    }
}

// Cycle through all the different views
void Game::cycleView() {
    switch (this->view) {
        case VIEW_TOWER:
            this->view = VIEW_ROOM;
            break;
        case VIEW_ROOM:
            this->view = VIEW_INVENTORY;
            break;
        case VIEW_INVENTORY:
            this->view = VIEW_TOWER;
            break;
    }
}

// Display the tower including the player character where it is located in the tower
void Game::displayTower() {
    // Print tower roof
    cout << "   +-----------------------------------------------------------------------+   " << endl;
    cout << "  /                                                                         \\ " << endl;
    cout << " /                            B R U M P   T O W E R                          \\" << endl;
    cout << "|                                                                             |" << endl;
    cout << "+-----------------------------------------------------------------------------+" << endl;

    // For each floor in the tower
    for (int row = 0; row < ROOM_ROWS; row++) {

        // Status bar on top of each room
        cout << '|';
        for (int col = 0; col < ROOM_COLS; col++) {
            // Display a * for each item in room
            Room currentRoom = this->rooms[row * ROOM_COLS + col];
            string itemStars = "";
            if (currentRoom.getItemHidden()) {
                itemStars = "? ";
            } else {
                int itemCount = currentRoom.getItems().size();
                for (int i = 0; i < itemCount; i++) {
                    itemStars += "* ";
                }
            }

            // Display the first letter of each suspect in the room
            vector<Suspect*> suspectsInRoom = this->rooms[row * ROOM_COLS + col].getSuspects();
            string suspectLetters = "";
            for (int i = 0; i < suspectsInRoom.size(); i++) {
                suspectLetters.append(" " + suspectsInRoom[i]->getName().substr(0, 1));
            }

            // Example display format: | A B C D E F G * * * * * |
            int blankCount = ROOM_WIDTH - itemStars.length() - suspectLetters.length();
            cout << suspectLetters << fixedWidth("", ' ', blankCount) << itemStars << '|';
        }
        cout << '\n';

        // Display the main section of the room (where the character might be)
        for (int i = 0; i < 6; i++) {
            cout << '|';
            for (int col = 0; col < ROOM_COLS; col++) {
                string content = "";

                Room *currentRoom = &this->rooms[row * ROOM_COLS + col];
                if (currentRoom == this->player.getRoom()) {
                    content = this->player.getImage()[i];
                }

                cout << fixedWidth(content, ' ', ROOM_WIDTH) << '|';
            }
            cout << '\n';
        }

        // Print names of rooms in the current floor
        cout << '|';
        for (int col = 0; col < ROOM_COLS; col++) {
            string roomName = this->rooms[row * ROOM_COLS + col].getName();
            cout << fixedWidth(" " + roomName, ' ', ROOM_WIDTH) << '|';
        }
        cout << '\n';

        cout << "+-----------------------------------------------------------------------------+" << endl;
    }
}

// Display the room where the player is in
void Game::displayRoom() {
    Room *currentRoom = this->player.getRoom();

    // Display the current room's image and description
    cout << currentRoom->getImage();
    cout << "|                                                                             |" << endl;

    // Display each item that exists in the current room
    vector<Item> items = currentRoom->getItems();
    string itemsString = " ITEMS: ";
    if (currentRoom->getItemHidden()) {
        itemsString += '?';
    } else if (items.size() > 0) {
        itemsString += items[0].getName();
        for (int i = 1; i < items.size(); i++) {
            itemsString += ", " + items[i].getName();
        }
    } else {
        itemsString += '-';
    }
    cout << '|' << fixedWidth(itemsString, ' ', WINDOW_WIDTH) << '|' << endl;
    cout << "|                                                                             |" << endl;

    // Display all suspect in the current room
    vector<Suspect*> suspects = currentRoom->getSuspects();
    string suspectsString = " SUSPECTS: ";
    if (suspects.size() > 0) {
        suspectsString += suspects[0]->getName();
        for (int i = 1; i < suspects.size(); i++) {
            suspectsString += ", " + suspects[i]->getName();
        }
    } else {
        suspectsString += '-';
    }
    cout << '|' << fixedWidth(suspectsString, ' ', WINDOW_WIDTH) << '|' << endl;
    cout << "|                                                                             |" << endl;

    cout << "+-----------------------------------------------------------------------------+" << endl;
}

// Display player's inventory full screen
void Game::displayInventory() {
    cout << "+-----------------------------------------------------------------------------+" << endl;

    // Print title
    cout << '|' << fixedWidth("", ' ', WINDOW_WIDTH) << '|' << endl;
    cout << '|' << fixedWidth("  INVENTORY", ' ', WINDOW_WIDTH) << '|' << endl;

    // Print items in player's inventory
    vector<Item> inventory = this->player.getInventory();
    for (int i = 0; i < inventory.size(); i++) {
        cout << '|' << fixedWidth("", ' ', WINDOW_WIDTH) << '|' << endl;
        cout << '|' << fixedWidth("  - " + inventory[i].getName(), ' ', WINDOW_WIDTH) << '|' << endl;
    }

    // Print blank lines
    for (int i = 0; i < WINDOW_HEIGHT - this->player.getInventory().size() * 2 - 2; i++) {
        if (i == 1 && inventory.size() == 0) {
            // Print empty. if nothing in inventory
            cout << '|' << fixedWidth("  Empty.", ' ', WINDOW_WIDTH) << '|' << endl;
        } else {
            cout << '|' << fixedWidth("", ' ', WINDOW_WIDTH) << '|' << endl;
        }
    }

    cout << "+-----------------------------------------------------------------------------+" << endl;
}

// Get confirmation from user if they want to quit
void Game::confirmQuit() {
    string answer = readInput("Are you sure? (Y/n) ");
    if (answer == "y" || answer == "Y") {
        this->gameOver = true;
    }
}

// If the user puts an invalid command suggest them to read the help screen
void Game::invalidCommand() {
    cout << "Get some '//> help'\n" << endl;
    this->command();  // Prompt for command again. No re-render
}

// Display a response from the suspect named suspectName
void Game::talk(string suspectName) {
    int suspectIndex = this->player.getRoom()->searchSuspect(suspectName);

    if (suspectIndex >= 0) {
        this->player.getRoom()->getSuspects()[suspectIndex]->talk(this->player.getName());
    }

    this->command();
}

// Move all suspect to the room where the player is in
void Game::gather() {
    for (int roomIndex = 0; roomIndex < this->rooms.size(); roomIndex++) {
        for (int suspectIndex = this->rooms[roomIndex].getSuspects().size() - 1; suspectIndex >= 0; suspectIndex--) {
            Suspect suspect = *this->rooms[roomIndex].getSuspects()[suspectIndex];
            this->player.getRoom()->addSuspect(&suspect);
            this->rooms[roomIndex].removeSuspect(suspectIndex);
        }
    }
}
