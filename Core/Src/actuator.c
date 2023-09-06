#include "actuator.h"
#include "main.h"

static enum ActuatorState state = POWERUP_UNKNOWN;

//Test blink:
//static enum ActuatorState state = ACTUATOR_ERROR;
uint8_t errorblink = 0;
uint32_t operationTimestamp = 0;
uint32_t targetTimestamp = 0;
uint32_t timeForward = 0;
uint32_t timeBackward = 0;


void process_unknown(uint8_t endswitch_state)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); //To fade the led on bluepill, we need to set it. 
    //Determine current position after power on:
    if (endswitch_state == ENDSWITCH_BACKWARD)
    { 
        //We are on backward endpoint, move forward
        state = CALIBRATION_HOME_FORWARD;
        move_actuator(MOVE_ACTUATOR_FORWARD);
        operationTimestamp = HAL_GetTick();
        return;
    }
    if (endswitch_state == ENDSWITCH_FORWARD)
    { 
        //We are on forward endpoint, move backward
        state = CALIBRATION_HOME_BACKWARD;
        move_actuator(MOVE_ACTUATOR_BACKWARD);
        operationTimestamp = HAL_GetTick();
        return;
    }
    //Move backward otherwise
    state = CALIBRATION_HOME_BACKWARD;
    move_actuator(MOVE_ACTUATOR_BACKWARD);
}

void process_calibration_home_backward(uint8_t endswitch_state)
{
    uint32_t tB = HAL_GetTick() - operationTimestamp;
    if (tB>ACTUATOR_TIMEOUT_IN_TICKS)
    {
        move_actuator(MOVE_ACTUATOR_IDLE);
        state = ACTUATOR_ERROR;
        return;
    }
    if (endswitch_state==ENDSWITCH_BACKWARD) 
    {
        //On home backward
        move_actuator(MOVE_ACTUATOR_IDLE); //idle
        state = CALIBRATION_SPEED_FORWARD;
        operationTimestamp = HAL_GetTick();
        move_actuator(MOVE_ACTUATOR_FORWARD); //move forward
    }
}

void process_calibration_home_forward(uint8_t endswitch_state)
{
    uint32_t tF = HAL_GetTick() - operationTimestamp;
    if (tF>ACTUATOR_TIMEOUT_IN_TICKS)
    {
        move_actuator(MOVE_ACTUATOR_IDLE);
        state = ACTUATOR_ERROR;
        return;
    }
    if (endswitch_state==ENDSWITCH_FORWARD) 
    {
        //On home forward
        move_actuator(MOVE_ACTUATOR_IDLE); //idle
        state = CALIBRATION_SPEED_BACKWARD;
        operationTimestamp = HAL_GetTick();
        move_actuator(MOVE_ACTUATOR_BACKWARD); //move backward
    }
}

void process_calibration_speed_forward(uint8_t endswitch_state)
{
    uint32_t tF = HAL_GetTick() - operationTimestamp;
    if (tF>ACTUATOR_TIMEOUT_IN_TICKS)
    {
        move_actuator(MOVE_ACTUATOR_IDLE);
        state = ACTUATOR_ERROR;
        return;
    }
    if (endswitch_state==ENDSWITCH_FORWARD) 
    {
        //On home forward
        move_actuator(MOVE_ACTUATOR_IDLE); //idle
        timeForward = tF;
        if (!timeBackward)
        {
            state = CALIBRATION_SPEED_BACKWARD;
            operationTimestamp = HAL_GetTick();
            move_actuator(MOVE_ACTUATOR_BACKWARD); //move backward
        }
        else
        {
            //We are ready to do the goal task
            state = MOVING_MIDDLE;
            move_actuator(MOVE_ACTUATOR_BACKWARD); //move backward
            targetTimestamp = HAL_GetTick()+timeBackward/2;
        }
    }
}

void process_calibration_speed_backward(uint8_t endswitch_state)
{
    uint32_t tB = HAL_GetTick() - operationTimestamp;
    if (tB>ACTUATOR_TIMEOUT_IN_TICKS)
    {
        move_actuator(MOVE_ACTUATOR_IDLE);
        state = ACTUATOR_ERROR;
        return;
    }
    if (endswitch_state==ENDSWITCH_BACKWARD) 
    {
        //On home forward
        move_actuator(MOVE_ACTUATOR_IDLE); //idle
        timeBackward = tB;
        if (!timeForward)
        {
            state = CALIBRATION_SPEED_FORWARD;
            operationTimestamp = HAL_GetTick();
            move_actuator(MOVE_ACTUATOR_FORWARD); //move forward
        }
        else
        {
            //We are ready to do the goal task
            state = MOVING_MIDDLE;
            move_actuator(MOVE_ACTUATOR_FORWARD); //move forward
            targetTimestamp = HAL_GetTick()+timeForward/2;
        }
    }
}

void process_moving_middle(uint8_t endswitch_state)
{
    if (HAL_GetTick()>targetTimestamp)
    {
        state = ACTUATOR_MIDDLE;
        move_actuator(MOVE_ACTUATOR_IDLE); //idle
        return;
    }
}

void actuator_iteration(void)
{
    uint8_t endswitch_state = get_end_switch_state();
    if (endswitch_state == ENDSWITCH_ERROR)  //Something is wrong
    {
                move_actuator(MOVE_ACTUATOR_IDLE);
                state = ACTUATOR_ERROR;
    }
    switch (state) {
        case IDLE:
            move_actuator(MOVE_ACTUATOR_IDLE);
            break;
        case POWERUP_UNKNOWN:
            process_unknown(endswitch_state);
            break;
        case CALIBRATION_HOME_BACKWARD:
            process_calibration_home_backward(endswitch_state);
            break;
        case CALIBRATION_HOME_FORWARD:
            process_calibration_home_forward(endswitch_state);
            break;
        case CALIBRATION_SPEED_FORWARD:
            process_calibration_speed_forward(endswitch_state);
            break;
        case CALIBRATION_SPEED_BACKWARD:
            process_calibration_speed_backward(endswitch_state);
            break;
        case MOVING_MIDDLE:
            process_moving_middle(endswitch_state);
            break;
        case ACTUATOR_MIDDLE:
            // Actuator is now at the middle position
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
            break;
        case ACTUATOR_ERROR:
            // Actuator is now at the middle position
            if (errorblink==0)
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
						errorblink++;
            break;
    }
}

void move_actuator(uint8_t operation)
{
    switch (operation)
    {
        case MOVE_ACTUATOR_IDLE: //IDLE
            HAL_GPIO_WritePin(MOT_BACKWARD_GPIO_Port, MOT_BACKWARD_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOT_FORWARD_GPIO_Port, MOT_FORWARD_Pin, GPIO_PIN_RESET);
            break;
        case MOVE_ACTUATOR_BACKWARD: //BACKWARD
            HAL_GPIO_WritePin(MOT_FORWARD_GPIO_Port, MOT_FORWARD_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOT_BACKWARD_GPIO_Port, MOT_BACKWARD_Pin, GPIO_PIN_SET);
            break;
        case MOVE_ACTUATOR_FORWARD: //FORWARD
            HAL_GPIO_WritePin(MOT_BACKWARD_GPIO_Port, MOT_BACKWARD_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOT_FORWARD_GPIO_Port, MOT_FORWARD_Pin, GPIO_PIN_SET);
            break;
    }
}

uint8_t get_end_switch_state(void)
{
    uint8_t backward = 0;
    for (uint8_t debounce=0;debounce!=20;debounce++) //20 reads debounce
        if (!HAL_GPIO_ReadPin(ENDSWITCH_BACKWARD_GPIO_Port, ENDSWITCH_BACKWARD_Pin)) backward =1; //Commonly, endpoint sensors tie signal to ground
    uint8_t forward = 0;
    for (uint8_t debounce=0;debounce!=20;debounce++) //20 reads debounce
        if (!HAL_GPIO_ReadPin(ENDSWITCH_FORWARD_GPIO_Port, ENDSWITCH_FORWARD_Pin)) forward  =1; //Commonly, endpoint sensors tie signal to ground
    if (backward&&forward)
        return ENDSWITCH_ERROR;
    if (backward)
        return ENDSWITCH_BACKWARD;
    if (forward)
        return ENDSWITCH_FORWARD;
    return NO_ENDSWITCH;
}
