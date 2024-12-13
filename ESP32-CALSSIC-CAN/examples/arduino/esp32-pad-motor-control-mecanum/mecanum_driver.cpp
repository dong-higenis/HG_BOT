#ifndef MECANUM_DRIVER_H
#define MECANUM_DRIVER_H

#include "Arduino.h"
#include "mecanum_driver.h"

namespace robot
{
    motor::motor(const esp_motor_pin_t &pins) : motorPins(pins), initialized(true)
    {
        initializePins();
    }

    void motor::initializePins()
    {
        if (!initialized)
            return;
        ledcSetup(motorPins.pwmChA, freq, resolution);
        ledcSetup(motorPins.pwmChB, freq, resolution);
        ledcAttachPin(motorPins.pinA, motorPins.pwmChA);
        ledcAttachPin(motorPins.pinB, motorPins.pwmChB);
    }

    void motor::move(int pwm)
    {
        pwm = constrain(pwm, -255, 255);
        if (pwm >= 0)
        {
            ledcWrite(motorPins.pwmChA, pwm);
            ledcWrite(motorPins.pwmChB, 0);
        }
        else
        {
            ledcWrite(motorPins.pwmChA, 0);
            ledcWrite(motorPins.pwmChB, -pwm);
        }
    }

    wheels::wheels(esp_motor_pin_t *espMotorPins)
    {
        frontLeft = motor(espMotorPins[0]);
        frontRight = motor(espMotorPins[1]);
        backLeft = motor(espMotorPins[2]);
        backRight = motor(espMotorPins[3]);
    }

    void wheels::control(int axisX, int axisY, int axisRx)
    {
        int lx = map(axisX, -511, 512, -255, 255);
        ;
        int ly = map(axisY, -511, 512, -255, 255);
        ;
        int rx = map(axisRx, -511, 512, -255, 255);

        // 메카넘 휠 속도 계산
        int frontLeftSpeed = ly + lx + rx;
        int frontRightSpeed = ly - lx - rx;
        int backLeftSpeed = ly - lx + rx;
        int backRightSpeed = ly + lx - rx;

        // 속도 정규화
        int maxSpeed = max(abs(frontLeftSpeed), max(abs(frontRightSpeed), max(abs(backLeftSpeed), abs(backRightSpeed))));
        if (maxSpeed > 255)
        {
            float scale = 255.0 / maxSpeed;
            frontLeftSpeed *= scale;
            frontRightSpeed *= scale;
            backLeftSpeed *= scale;
            backRightSpeed *= scale;
        }
        frontLeft.move(frontLeftSpeed);
        frontRight.move(frontRightSpeed);
        backLeft.move(backLeftSpeed);
        backRight.move(backRightSpeed);
    }
}

#endif
