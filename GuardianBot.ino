#include <Servo.h>

const int MAIN_PIN = 3;

Servo main;

void setup()
{
    main.attach(MAIN_PIN);
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
            const int rotateVal = readStr.substring(7).toInt();
            Serial.println("Received rotate command, executing...");
            Serial.print("[COMMAND INFO] Absolute rotation = ");
            Serial.println(rotateVal);

            main.write(rotateVal);
            delay(100);
        }
    }
}

void loop()
{

}
