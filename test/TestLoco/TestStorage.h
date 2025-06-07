/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include "Settings.h"
#include "Storage.h"

#if defined(ARDUINO_ARCH_ESP32)
#define F(x) (x)
#endif

const char testStr[] = "1234567890abcd";
const char testStr2[] = "ABCDEFGHIJKLMNO";

void printResult(const char *expected, char *actual)
{
    if (strcmp(expected, actual) == 0) {
        Serial.println("ok");
    } else {
        Serial.print(F("FAIL. Expected: "));
        Serial.print(expected);
        Serial.print(F(" Actual: "));
        Serial.println(actual);
    }
}

void test00()
{
    // Test read multiple
    char buffer[20];
    const int c = 50;
    int s = 0;
    storage.write("/test01", testStr, sizeof(testStr));
    long start = millis();
    for (int i = 0; i < c; i++)
        s = storage.read("/test01", buffer, sizeof(testStr));
    long duration = millis() - start;
    if ((strcmp(testStr, buffer) == 0) && duration < 500) {
        Serial.println("ok");
    } else {
        Serial.print(F("FAIL, "));
        Serial.println(duration);
    }
}

void test01()
{
    // Test size and offset
    char buffer[20];
    storage.write("/test02", testStr, sizeof(testStr));
    int s = storage.read("/test02", buffer, 11, 7);
    buffer[s] = '\0';
    String t = String(testStr);
    t = t.substring(7, t.length());
    printResult(t.c_str(), buffer);
}

void test02()
{
    // Test offset bigger than size
    char buffer[20];
    storage.write("/test02", testStr, sizeof(testStr));
    int s = storage.read("/test02", buffer, 11, 20);
    if (s == 0)
        Serial.println("ok");
    else
        Serial.println(F("FAIL"));
}

void test03()
{
    // Test cache
    settings.put("testvalue", "101.1");
    const int c = 10000;
    long start = millis();
    float r = 0;
    for (int i = 0; i < c; i++)
        r = settings.getCachedFloat("testvalue");
    long duration = millis() - start;
    if ((int)(r * 10) == 1011 && duration < 100) {
        Serial.println("ok");
    } else {
        Serial.print(F("FAIL, "));
        Serial.println(duration);
    }
}

void test04()
{
    // Test exist
    storage.write("/test01", testStr, sizeof(testStr));
    bool s = storage.exists("/test01");
    if (s)
        Serial.println("ok");
    else
        Serial.println("FAIL");
}

void (*tests[])() = {
    test00, test01, test02, test03, test04,
};