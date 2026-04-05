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
        { "Hey there, %N!", "Ah! Hi %N! Didn't see you there.", "Oh! Sorry. Anyway, what do you think about %t?", "Oh, I love %t!! It is like, my favorite thing ever.", "Really? No way! I love %t too!!", "Guess we could be good friends then!" },
        { "Hello %N! How are you doing?", "Hi! I was just thinking about %t. Have you ever heard of it?", "No, not really... Sorry.", "It's okay!" },
        { "Heya! What have you been up to?", "Hey! I recently got into %t, it's lots of fun!", "Oh really? I should try learning about it too then.", "Awesome, %N!!" },
        { "Hi! What do you think about %t?", "%t? Never heard of it.", "Oh. That's too bad." },
        { "Hi %N!", "Hey! What have you been up to?", "Let me think... I recently read a book on %t. What do you know about %t?", "I know all about %t! You can ask me anything, anytime.", "Oh perfect! Seems like we could be friends!!" },
    } }
};