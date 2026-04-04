#include "data.h"

// percent-codes:
// %n -- my name, %N -- the previous person's name
// %t -- randomly chosen thing

Activities activities{
    { ACTIVITY_INTRODUCTION, {
        { "Oh! Hello! We haven't met yet, have we?", "Hi! We haven't. My name's %n. And yours is?", "My name is %n. Nice to meet you, %N!" },
        { "Hi! Haven't seen you around before.", "Hello! My name is %n. And what should I call you?", "Oh, hi there, %N! You can call me %n. Nice to meet you!" },
    } },
    { ACTIVITY_GETTING_TO_KNOW_EACH_OTHER, {
        { "Hi %N! What would you say is your primary interest?", "Hello there %N! I'm very fond of %t.", "Oh, cool! I share your interest in %t.", "Woah, that's awesome!!" },
        { "Hi %N! Say, what do you do on the weekends?", "Hey! I mostly try to relax, get some sleep, and watch a lot of documentaries on %t.", "Huh, that's pretty cool!" },
    } }
};