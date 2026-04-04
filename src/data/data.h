#pragma once
#include <Arduino.h>
#include <vector>
#include <map>

typedef const char* Dialogue;
typedef std::vector<Dialogue> Variant;
typedef std::vector<Variant> Activity;
typedef std::map<uint8_t, Activity> Activities;

extern Activities activities;
#define ACTIVITY_NONE 0
#define ACTIVITY_INTRODUCTION 1