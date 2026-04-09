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
#define ACTIVITY_BECOMING_FRIENDS 4
#define ACTIVITY_CONFESSING_SUCCESSFUL 5
#define ACTIVITY_CONFESSING_UNSUCCESSFUL 6

#define DOLLS_LIMIT(activity) (activity == ACTIVITY_INTRODUCTION || activity == ACTIVITY_GETTING_TO_KNOW_EACH_OTHER \
    || activity == ACTIVITY_BECOMING_FRIENDS || activity == ACTIVITY_CONFESSING_SUCCESSFUL || activity == ACTIVITY_CONFESSING_UNSUCCESSFUL \
    ? 2 \
    : 255)

#define RELATION_GETTING_FAMILIAR_MIN 0
#define RELATION_GETTING_FAMILIAR_MAX 2
#define RELATION_ACQUAINTANCES 3
#define RELATION_FRIENDS_MIN 4
#define RELATION_FRIENDS_MAX 9
#define RELATION_LOVERS 10

typedef const char* Thing;
typedef std::vector<Thing> Things;

extern Things things[];
#define THING_CAT_THINGS 0
#define THING_CAT_MUSIC_GENRE 1
#define THING_CAT_MUSIC_INSTRUMENT 2
#define THING_CAT_CITY 3
#define THING_CAT_FOOD 4
#define THING_CAT_PICKUP_LINES 5