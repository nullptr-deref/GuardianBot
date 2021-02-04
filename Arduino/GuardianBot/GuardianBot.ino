#include <Servo.h>

const int MAIN_PIN = 3;

Servo mainServo;

namespace util
{
    int power(int base, int p)
    {
        if (p == 0) return 1;

        int res = base;
        for (int i = 2; i <= p; i++)
        {
            res *= base;
        }

        return res;
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    mainServo.attach(MAIN_PIN);
    Serial.begin(9600);
}

void serialEvent()
{
    const String rotateCommand = "rotate";
    while (Serial.available())
    {
        const String readStr = Serial.readString();

        if (readStr.substring(0, 6) == rotateCommand)
        {
            digitalWrite(LED_BUILTIN, HIGH);
//            Debugging
//            digitalWrite(LED_BUILTIN, HIGH);
//            delay(1000);
//            digitalWrite(LED_BUILTIN, LOW);

            int angle = 0;
            String strAngle = readStr.substring(7, 10);
            for (int i = 0; i < strAngle.length(); i++)
            {
                if (!isDigit(strAngle[i])) strAngle.remove(i);
            }
            Serial.println(strAngle);
            Serial.println(strAngle.length());
            for (int i = 0; i < strAngle.length(); i++)
            {
                if (isDigit(strAngle[i])) angle += (int)(strAngle[i] - '0') * util::power(10, strAngle.length() - 1 - i);
            }
            //if (strAngle.length() == 2) angle /= 10;
            //if (strAngle.length() == 1) angle /= 100;
            Serial.println(angle);

            mainServo.write(angle);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}

void loop()
{

}
