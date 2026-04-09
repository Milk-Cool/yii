#include "data.h"

// percent-codes:
// %n -- my name, %N -- the previous person's name, %a -- the next person's name
// %0 - %9 -- thing category (see data.h; replaced with ""; default is 0)
// %t -- randomly chosen thing #1, $T -- randomly chosen thing #2

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
        { "Hi! Do you think this world is even real?", "No, I think it's just a simulation.", "I personally think it is real, though I do believe in parallel universes.", "Well, both theories are valid, I guess...", "Fair enough.", "Let's switch topics." },
        { "Hi! What do you think about %t?", "It can be really fun at times, but I'm not really a fan of it.", "Huh... Am I the only %t fan here then?", "Yes you are, %N." },
        { "%1Hey %a! What kind of music do you listen to?", "I listen to %t mostly.", "Oh, that's cool! I am more of a %T music person myself.", "Oh, I listen to %T music a lot too!", "Oh, great!", "By the way, what is your favorite %T musician?", "I have one but forgot their name. Their songs are still holding up well though!", "Oh, that's pretty cool, %N!" },
        { "%2Hey! Do you play any musical instruments?", "Well, not anymore - I used to play %t, but I lost interest in it.", "Oh, that's sad. I used to play %T.", "Woah, %T are pretty cool!", "I know, right?" },
        { "%3%a! Do you have any dreams?", "I do! I would love to go to %t one day.", "Oh! That's fun. I wanted to climb mount Everest one day.", "What! No way! That's too dangerous.", "It really is, but people have done it.", "Not to mention how fun it probably is!", "...it really isn't." },
        { "%4%a! Hey there! Do you happen to have a favorite food?", "Why, yes I do! It's %t.", "%t is pretty good, yeah! I enjoy %T more though.", "Well, %T is pretty good in my opinion, %N.", "Yes, it is pretty tasty." },
        { "%3Hey %a! Where are you from?", "Hi %N! I'm actually from %t. But my parents are from %T.", "Oh, that's fun!! My parents are actually also from %T.", "Yeah, that is fun! I want to go there one day.", "Me too." },
        { "Hey! Do you think %t or %T is more interesting?", "Hello %N! I personally think %t is more interesting.", "Oh! Why's that? I've been more of a %T fan myself.", "Well, %T is also pretty fun, I don't know..." },
    } },
    { ACTIVITY_BECOMING_FRIENDS, {
        { "Hey there %N!", "Hi! What did you wanna talk about?", "So you know %t?", "Yes, I went to a %t movie to a cinema recently!", "No way! Me too!", "Oh wow! Wanna be frends, %N? I'm sure we'll have a lot to talk about.", "Sure!!" },
        { "%4Hi %N!", "Hi %N! What is it?", "What's your favorite food?", "Oh, mine is %t! I would eat it every breakfast, lunch and dinner if i had the chance. What about you?", "Mine is also %t! Followed by %T, of course.", "Oh wow!! We sure do have similar tastes in food.", "Cool! Wanna be friends?", "Of course!" },
        { "%1Heya %N, what music do you listen to?", "Hi %N! I listen to %t. What about you?", "I'm more of a %T fan, though %t is pretty cool too.", "Oh okay. Cool that you also enjoy %t. Wanna be friends?", "Yes! Of course!" },
    } },
    { ACTIVITY_CONFESSING_SUCCESSFUL, {
        { "%5Hey %N. I wanna tell you something.", "Yes, what is it?", "%t Will you go out with me?", "Yes!", "Yay!" },
        { "%5Hello %N. I wanna ask you something.", "What is it, %N?", "%t Will you go out with me?", "Yes, of course!", "Hooray!" },
        { "%5Hello %N. I would like to discuss something with you", "What is it, %N?", "I kind of like you. Will you go out with me?", "Yes, of course!", "Yay!" },
    } },
    { ACTIVITY_CONFESSING_UNSUCCESSFUL, {
        { "%4Hey %N. I wanna tell you something.", "Yes, what is it?", "You shine birght like a %t. Will you go out with me?", "What? What do you mean?", "I... don't know. Sorry." },
        { "%2Hello %N. I wanna ask you something.", "What is it, %N?", "You are... loud... just like %t... Will you go out with me? Please?", "No way. What even is that pick-up line?", "Well this is awkward! Sorry." },
        { "Hello %N. I would like to discuss something with you", "What is it, %N?", "I kind of like you. Will you go out with me?", "No, sorry...", "Damn..." },
    } },
};