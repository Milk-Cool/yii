#include "data.h"

// percent-codes:
// %n -- my name, %N -- the previous person's name, %a -- the next person's name
// %t -- randomly chosen thing

Activities activities{
    { ACTIVITY_INTRODUCTION, { // two dolls meeting for the first time
        { "Oh! Hello! We haven't met yet, have we?", "Hi! We haven't. My name's %n. And yours is?", "My name is %n. Nice to meet you, %N!" },
        { "Hi! Haven't seen you around before.", "Hello! My name is %n. And what should I call you?", "Oh, hi there, %N! You can call me %n. Nice to meet you!" },
    } },
    { ACTIVITY_GETTING_TO_KNOW_EACH_OTHER, { // a pair of dolls getting to know each other
        { "Hi %N! What would you say is your primary interest?", "Hello there %N! I'm very fond of %t.", "Oh, cool! I share your interest in %t.", "Woah, that's awesome!!" },
        { "Hi %N! Say, what do you do on the weekends?", "Hey! I mostly try to relax, get some sleep, and watch a lot of documentaries on %t.", "Huh, that's pretty cool!" },
        { "Hey there, %N!", "Ah! Hi %N! Didn't see you there.", "Oh! Sorry. Anyway, what do you think about %t?", "Oh, I love %t!! It is like, my favorite thing ever.", "Really? No way! I love %t too!!", "Guess we could be good friends then!" },
        { "Hello %N! How are you doing?", "Hi! I was just thinking about %t. Have you ever heard of it?", "No, not really... Sorry.", "It's okay!" },
        { "Heya! What have you been up to?", "Hey! I recently got into %t, it's lots of fun!", "Oh really? I should try learning about it too then.", "Awesome, %N!!" },
        { "Hi! What do you think about %t?", "%t? Never heard of it.", "Oh. That's too bad." },
        { "Hi %N!", "Hey! What have you been up to?", "Let me think... I recently read a book on %t. What do you know about %t?", "I know all about %t! You can ask me anything, anytime.", "Oh perfect! Seems like we could be friends!!" },
    } },
    { ACTIVITY_TALKING, { // talking in groups of 2 or more
       { "Heya %a!", "Hey! Have you ever heard of %t?", "Oh! I know about %t! I recently watched a short film on it.", "Really? That's so fun, %N!", "It really is!" }, 
       { "Hey, did you know that %t is actually a government hoax designed to control people?", "What? No, you're kidding, %N. It is not.", "I don't know... %t does seem really shady...", "Meow" },
       { "Hi! Do you think this world is even real?", "No, I think it's just a simulation.", "I personally think it is real, though I do believe in parallel unievrses.", "Well, both theories are valid, I guess...", "Fair enough.", "Let's switch topics." },
       { "Hi! What do you think about %t?", "It can be really fun at times, but I'm not really a fan of it.", "Huh... Am I the only %t fan here then?", "Yes you are, %N." },
       { "Hey %a! What kind of music do you listen to?", "I listen to drum and bass mostly.", "Oh, that's cool! I am more of a classical music person myself.", "Oh, I listen to classical music a lot too!", "Oh, great!", "By the way, what is your favorite classical composer?", "I'd say mine is Beethoven. His songs are still holding up well.", "Oh, that's pretty cool, %N!" },
       { "Hey! Do you play any musical instruments?", "Well, not anymore - I used to play bass, but I lost interest in it.", "Oh, that's sad. I used to play drums.", "Woah, drums are pretty cool!", "I know, right?" },
       { "%a! Do you have any dreams?", "I do! I would love to go to Paris one day.", "Oh! That's fun. I wanted to climb mount Everest one day.", "What! No way! That's too dangerous.", "It really is, but people have done it.", "Not to mention how fun it probably is!", "...it really isn't." },
    } }
};