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
#define ACTIVITY_GETTING_TO_KNOW_EACH_OTHER 2
#define ACTIVITY_TALKING 3

#define DOLLS_LIMIT(activity) (activity == ACTIVITY_INTRODUCTION || activity == ACTIVITY_GETTING_TO_KNOW_EACH_OTHER \
    ? 2 \
    : 255)

#define RELATION_GETTING_FAMILIAR_MIN 0
#define RELATION_GETTING_FAMILIAR_MAX 2
#define RELATION_ACQUAINTANCES 3

typedef const char* Thing;
typedef std::vector<Thing> Things;
extern Things things;