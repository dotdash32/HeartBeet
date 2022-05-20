/**
 * @file keepAlive.h
 * @author Josh DeWitt (jndewitt@staford.edu)
 * @brief Post a message to say Arduino is still alive
 * 
 * Now also 
 * 
 * @version 0.1
 * @date 2022-05-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

void keepAliveMessages();
void setupWatchdog(void);
void feedWatchdog(void);