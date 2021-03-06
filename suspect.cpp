/************************************************************
 *
 * suspect.cpp
 *
 * Represents the suspects in the game
 * A suspect might be a killer, victim or neither (normal)
 * Provides interface to get and set fields
 *
 * Briano Goestiawan, 31482228
 *
************************************************************/

#include <iostream>
#include "suspect.h"
#include "utils.h"

using namespace std;

// Suspect constructor
Suspect::Suspect(string name) {
    this->name = name;
    this->type = SUS_NORMAL;
    this->alibi = NULL;
}

// Return the name of the suspect
string Suspect::getName() {
    return this->name;
}

// Return a pointer to suspect's location
Room *Suspect::getRoom() {
    return this->room;
}

// Set the location of the suspect
void Suspect::setRoom(Room *room) {
    this->room = room;
}

// Get the type of suspect
SuspectType Suspect::getType() {
    return this->type;
}

// Mutator method to set the type of the suspect
void Suspect::setType(SuspectType type) {
    this->type = type;
}

// Set the alibi of the suspect
void Suspect::setAlibi(Suspect *alibi) {
    this->alibi = alibi;
}

// Display messages from the suspect
void Suspect::talk(string playerName) {
    switch (this->type) {
        // Normal or killer acts the same way by mentioning their alibi or
        // stating that their alone
        case SUS_NORMAL:
        case SUS_KILLER:
            cout << "Hi " << playerName << ", ";
            if (this->alibi == NULL) {
                cout << "I was alone" << endl;
            } else {
                cout << "I was with " << this->alibi->getName() << endl;
            }
            break;
        // Dead
        case SUS_VICTIM:
            cout << "X_X" << endl;
            break;
    }
}

// Move to neighbouring room based on given direction
void Suspect::move(Direction direction) {
    Room *destination = this->room->getNeighbour(direction);
    // Move to destination if it exists
    if (destination) this->room = destination;
}
