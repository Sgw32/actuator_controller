/**
 * @file actuator.h
 * @author Fedor Zagumennov
 * @brief Actuator header file for actuator controller project
 * @version 0.1
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>

/// Actuator state is for iterative processing of actuator actions
enum ActuatorState {
    IDLE, ///< Idle, no movement
    CALIBRATION_HOME_FORWARD, ///< Home to forward after power on
    CALIBRATION_HOME_BACKWARD, ///< Home to backward after power on
    CALIBRATION_SPEED_FORWARD, ///< Calibrate time of backward to forward movement
    CALIBRATION_SPEED_BACKWARD, ///< Calibrate time of forward to backward movement
    MOVING_MIDDLE, ///< Waiting for middle point
    ACTUATOR_MIDDLE, ///< State of being in middle position
    POWERUP_UNKNOWN, ///< State after powerup
    ACTUATOR_ERROR ///< Calibrate time of backward to forward movement
};

#define NO_ENDSWITCH 0 ///< State of no endswitch signal
#define ENDSWITCH_BACKWARD 1 ///< Backward endswitch signal
#define ENDSWITCH_FORWARD 2 ///< Forward endswitch signal
#define ENDSWITCH_ERROR 3 ///< Endswitch error

#define ACTUATOR_TIMEOUT_IN_TICKS 10000 //10s
#define MOVE_ACTUATOR_IDLE 0 ///< Idle operation
#define MOVE_ACTUATOR_BACKWARD 1 ///< Backward movement operation
#define MOVE_ACTUATOR_FORWARD 2 ///< Forward movement operation

/**
 * @brief Main function for processing actuator events
 * 
 */
void actuator_iteration(void);

/**
 * @brief Function to process unknown(powerup) state of actuator
 * 
 * @param endswitch_state 
 */
void process_unknown(uint8_t endswitch_state);
/**
 * @brief Function to home actuator backward after poweron
 * 
 * @param endswitch_state 
 */
void process_calibration_home_backward(uint8_t endswitch_state);
/**
 * @brief Function to home actuator forward after poweron
 * 
 * @param endswitch_state 
 */
void process_calibration_home_forward(uint8_t endswitch_state);
/**
 * @brief Function to calibrate forward speed
 * 
 * @param endswitch_state 
 */
void process_calibration_speed_forward(uint8_t endswitch_state);
/**
 * @brief Function to calibrate backward speed
 * 
 * @param endswitch_state 
 */
void process_calibration_speed_backward(uint8_t endswitch_state);
/**
 * @brief Function to center actuator between two endpoints
 * 
 * @param endswitch_state 
 */
void process_moving_middle(uint8_t endswitch_state);

/**
 * @brief Function to set actuator outputs
 * 
 * @param operation 
 */
void move_actuator(uint8_t operation);
/**
 * @brief Get the end switch state object
 * 
 * @return uint8_t 
 */
uint8_t get_end_switch_state(void);

#endif
