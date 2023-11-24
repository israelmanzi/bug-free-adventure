#include <Arduino.h>
#include <AFMotor.h>

#define MAX_MOTOR_SPEED 120
#define MIN_MOTOR_SPEED 50
#define MOTOR_DELAY 20
#define RELEASE_DELAY 200
#define TURN_DELAY 2000

AF_DCMotor motors[4] = {
    AF_DCMotor(1),
    AF_DCMotor(2),
    AF_DCMotor(3),
    AF_DCMotor(4)};

unsigned long prev_direction_change_time = 0;

void init_motors();
void directional_control();
void release_motors();
void turn_vehicle(bool turn_left = true);
void set_motor_speed(uint8_t speed, uint8_t direction);
void log_motor_movement(const char *wheel, uint8_t direction);

void setup()
{
  Serial.begin(9600);
  init_motors();
}

void loop()
{
  directional_control();
}

void init_motors()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    motors[i].setSpeed(MIN_MOTOR_SPEED);
    motors[i].run(RELEASE);
  }
}

void directional_control()
{
  unsigned long current_time = millis();

  for (uint8_t i = MIN_MOTOR_SPEED; i < MAX_MOTOR_SPEED; i++)
  {
    set_motor_speed(i, FORWARD);
    delay(MOTOR_DELAY);
  }

  if (current_time - prev_direction_change_time > TURN_DELAY)
  {
    turn_vehicle();
    prev_direction_change_time = current_time;
  }

  for (uint8_t i = MAX_MOTOR_SPEED; i > MIN_MOTOR_SPEED; i--)
  {
    set_motor_speed(i, BACKWARD);
    delay(MOTOR_DELAY);
  }

  release_motors();
}

void turn_vehicle(bool turn_left)
{
  uint8_t turn_speed = map(MAX_MOTOR_SPEED, 0, 255, 40, 50);

  if (turn_left)
  {
    for (uint8_t i = 0; i < 2; i++)
    {
      set_motor_speed(turn_speed, FORWARD);
    }

    for (uint8_t i = 2; i < 4; i++)
    {
      set_motor_speed(turn_speed, BACKWARD);
    }
  }
  else
  {
    for (uint8_t i = 0; i < 2; i++)
    {
      set_motor_speed(turn_speed, BACKWARD);
    }

    for (uint8_t i = 2; i < 4; i++)
    {
      set_motor_speed(turn_speed, FORWARD);
    }
  }

  delay(TURN_DELAY);
  release_motors();
}

void release_motors()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    motors[i].run(RELEASE);
  }

  log_motor_movement("ALL", RELEASE);
  delay(RELEASE_DELAY);
}

void set_motor_speed(uint8_t speed, uint8_t direction)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    motors[i].setSpeed(speed);
    motors[i].run(direction);
    log_motor_movement((i == 0) ? "FL" : (i == 1) ? "FR"
                                     : (i == 2)   ? "BL"
                                                  : "BR",
                       direction);
    delay(MOTOR_DELAY);
  }
}

void log_motor_movement(const char *wheel, uint8_t direction)
{
  Serial.print(wheel);
  Serial.print(" ==> ");
  Serial.println(direction == FORWARD ? "FORWARD" : direction == BACKWARD ? "BACKWARD"
                                                                          : "RELEASE");
}
