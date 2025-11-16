#define LASER_PIN 4
#define BUTTON_PIN 5

const unsigned int FREQUENCY_HZ = 5;

void setup()
{
    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        digitalWrite(LASER_PIN, !digitalRead(LASER_PIN));
        delay(500 / FREQUENCY_HZ);
    }
    else
    {
        digitalWrite(LASER_PIN, LOW);
    }
}