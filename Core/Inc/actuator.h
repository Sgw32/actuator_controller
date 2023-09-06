#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>

enum ActuatorState {
    IDLE,
    CALIBRATION_HOME_FORWARD,
    CALIBRATION_HOME_BACKWARD,
    CALIBRATION_SPEED_FORWARD,
    CALIBRATION_SPEED_BACKWARD,
    MOVING_MIDDLE,
    ACTUATOR_MIDDLE,
    POWERUP_UNKNOWN,
    ACTUATOR_ERROR
};

#define NO_ENDSWITCH 0
#define ENDSWITCH_BACKWARD 1
#define ENDSWITCH_FORWARD 2
#define ENDSWITCH_ERROR 3

#define ACTUATOR_TIMEOUT_IN_TICKS 10000 //10s
#define MOVE_ACTUATOR_IDLE 0
#define MOVE_ACTUATOR_BACKWARD 1
#define MOVE_ACTUATOR_FORWARD 2


void actuator_iteration(void);

void process_unknown(uint8_t endswitch_state);
void process_calibration_home_backward(uint8_t endswitch_state);
void process_calibration_home_forward(uint8_t endswitch_state);
void process_calibration_speed_forward(uint8_t endswitch_state);
void process_calibration_speed_backward(uint8_t endswitch_state);
void process_moving_middle(uint8_t endswitch_state);

void move_actuator(uint8_t operation);
uint8_t get_end_switch_state(void);

#endif
