/************************************************************
 *
 * utils.h
 *
 * A collection of helper functions
 *
************************************************************/

#pragma once
#include <string>

// Returns the content of a file specified by fileName
std::string readFile(std::string fileName);

// Print promp to screen then read and return input line
std::string readInput(std::string prompt = "");

// Pause the control flow until the user press enter
void pause();

// Clear the output screen
void clearScreen();

// Return true if value is an integer string else return false
bool isInteger(std::string value);

// Returns a copy of value with all the uppercase characters replaced with its
// lowercase equivalent
// TODO think of a better argument name than value
std::string toLower(std::string value);

// TODO: implement a function takes string as input and prints it in a frame
void printFramed(std::string value);