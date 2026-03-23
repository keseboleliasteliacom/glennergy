#ifndef SIGNALHANDLER_H 
#define SIGNALHANDLER_H

/**
 * @file SignalHandler.h
 * @brief Interface for signal handling utilities.
 *
 * Provides initialization and status checking for process signal handling.
 */

/**
 * @brief Initializes signal handlers for the application.
 *
 * Sets up handling for termination and interrupt signals.
 */
void SignalHandler_Initialize();

/**
 * @brief Checks whether a stop signal has been received.
 *
 * @return int Returns non-zero if a stop signal was received, otherwise 0.
 */
int SignalHandler_Stop();

#endif