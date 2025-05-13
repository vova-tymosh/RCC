#pragma once
#include "Settings.h"
#include "Storage.h"

char testStr[]  = "1234567890abcd";
char testStr2[] = "ABCDEFGHIJKLMNO";
char buffer[64];

void printResult(const char *expected, char* actual)
{
    if (strcmp(expected, actual) == 0) {
        Serial.println("ok");
    } else {
        Serial.print(String("FAIL. Expected: ") + expected);
        Serial.println(String(" Actual: ") + actual);
    }
}

void test00()
{
    storage.write("test01", testStr, sizeof(testStr));
    int s = storage.read("test01", buffer, sizeof(testStr));
    buffer[s] = '\0';
    printResult(testStr, buffer);
}

void test01()
{
    int s = storage.read("test01", buffer, 12);
    buffer[s] = '\0';
    String t = String(testStr);
    t = t.substring(0, 12);
    printResult(t.c_str(), buffer);
}

void test02()
{
    storage.write("test02", testStr, sizeof(testStr));
    int s = storage.read("test02", buffer, 11, 2);
    buffer[s] = '\0';
    String t = String(testStr);
    t = t.substring(2, 13);
    printResult(t.c_str(), buffer);
}

void test03()
{
    storage.write("test01", testStr2, sizeof(testStr2));
    int s = storage.read("test01", buffer, sizeof(testStr2));
    buffer[s] = '\0';
    printResult(testStr2, buffer);
}

void test04()
{
    settings.create("test05", testStr);
    String r = settings.get("test05");
    printResult(testStr, (char*)r.c_str());
}

void test05()
{
    String r = settings.get("test05");
    printResult(testStr, (char*)r.c_str());
}

void test06()
{
    settings.put("test05", testStr2);
    String r = settings.get("test05");
    printResult(testStr2, (char*)r.c_str());
}

void test07()
{
    String r = settings.get("loconame");
    printResult("RCC", (char*)r.c_str());
}



void (*tests[])() = {
    test00,
    test01,
    test02,
    test03,
    test04,
    test05,
    test06,
    test07,
};