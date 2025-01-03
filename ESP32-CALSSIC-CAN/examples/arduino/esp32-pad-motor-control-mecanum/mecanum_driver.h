#include <stdint.h>

namespace robot
{
    struct esp_motor_pin_t
    {
        uint8_t pinA;
        uint8_t pinB;
        uint8_t pwmChA;
        uint8_t pwmChB;
    };

    class motor
    {
    public:
        motor() = default;
        motor(const esp_motor_pin_t &pins);
        void move(int pwm);

    private:
        void initializePins();
        int freq = 20000;   // 10khz
        int resolution = 8; // 8bit
        esp_motor_pin_t motorPins;
        bool initialized = false;
    };

    class wheels
    {
    public:
        wheels(esp_motor_pin_t *espMotorPins);
        void control(int axisX, int axisY, int axisRx);

    private:
        motor frontLeft;
        motor frontRight;
        motor backLeft;
        motor backRight;
    };
}