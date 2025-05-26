#pragma once
#include "Settings.h"
#include "Storage.h"

const char testStr[]  = "1234567890abcd";
const char testStr2[] = "ABCDEFGHIJKLMNO";

void printResult(const char *expected, char* actual)
{
    if (strcmp(expected, actual) == 0) {
        Serial.println("ok");
    } else {
        Serial.print("FAIL. Expected: ");
        Serial.print(expected);
        Serial.print(" Actual: ");
        Serial.println(actual);
    }
}

void test00()
{
    //Test write/read with extra size
    char buffer[64];
    storage.write("test01", testStr, sizeof(testStr));
    int s = storage.read("test01", buffer, sizeof(buffer));
    // buffer[s] = '\0';
    printResult(testStr, buffer);
}

void test01()
{
    //Test read only
    char buffer[64];
    int s = storage.read("test01", buffer, 12);
    buffer[s] = '\0';
    String t = String(testStr);
    t = t.substring(0, 12);
    printResult(t.c_str(), buffer);
}

void test02()
{
    //Test read multiple
    char buffer[64];    
    const int c = 100;
    int s = 0;
    long start = millis();
    for (int i = 0; i < c; i++)
        s = storage.read("test01", buffer, 12);
    long duration = millis() - start;
    buffer[s] = '\0';
    // Serial.println(String("Duration of ") + c + " runs = " + duration);
    String t = String(testStr);
    t = t.substring(0, 12);
    printResult(t.c_str(), buffer);
}

void test03()
{
    //Test size and offset
    char buffer[64];    
    storage.write("test02", testStr, sizeof(testStr));
    int s = storage.read("test02", buffer, 11, 7);
    buffer[s] = '\0';
    String t = String(testStr);
    t = t.substring(7, t.length());
    printResult(t.c_str(), buffer);
}

void test04()
{
    //Test offset bigger than size
    char buffer[64];    
    storage.write("test02", testStr, sizeof(testStr));
    int s = storage.read("test02", buffer, 11, 20);
    if (s == 0)
        Serial.println("ok");
    else
        Serial.println("FAIL");
}

void test05()
{
    //Test re-write
    char buffer[64];    
    storage.write("test01", testStr2, sizeof(testStr2));
    int s = storage.read("test01", buffer, sizeof(testStr2));
    buffer[s] = '\0';
    printResult(testStr2, buffer);
}

void test06()
{
    //Test setting create
    settings.create("test05", testStr);
    String r = settings.get("test05");
    Serial.println(r);

    printResult(testStr, (char*)r.c_str());
}

void test07()
{
    //Test setting read
    String r = settings.get("test05");
    printResult(testStr, (char*)r.c_str());
}

void test08()
{
    //Test settings re-write
    settings.put("test05", testStr2);
    String r = settings.get("test05");
    printResult(testStr2, (char*)r.c_str());
}

void test09()
{
    //Test reading defaults
    String r = settings.get("loconame");
    printResult("RCC", (char*)r.c_str());
}

void test10()
{
    //Test cache
    settings.put("testvalue", "101.1");
    const int c = 10000;
    long start = millis();
    float r = 0;
    for (int i = 0; i < c; i++)
        r = settings.getCachedFloat("testvalue");
    long duration = millis() - start;
    // Serial.println(String("Duration of ") + c + " runs = " + duration + ", value = " + r);
    if ((int)(r*10) == 1011 && duration < 100)
        Serial.println("ok");
    else
        Serial.println("FAIL");
}

void test11()
{
    //Test exist
    storage.write("test01", testStr, sizeof(testStr));
    bool s = storage.exists("test01");
    if (s)
        Serial.println("ok");
    else
        Serial.println("FAIL");
}

void test12()
{
    //Test clean & valid
    settings.put("loconame", "something");
    storage.clear();
    String r = settings.get("loconame");
    printResult("RCC", (char*)r.c_str());
}

const void (*tests[])() = {
    test00,
    test01,
    test02,
    test03,
    test04,
    test05,
    test06,
    test07,
    test08,
    test09,
    test10,
    test11,
    test12,
};