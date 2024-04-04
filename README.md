# ECS

An entity-component-system (ECS) written in pure C++ and libsdl.

## Building

You'll currently need to add in your own graphics. This repository is meant more for the ECS than the game itself.
The code is being built on the premise that the first game I am building is a side-scrolling shoot-em-up, so when time comes to implement mechanics, they will be tailored to that.
There are plans to also build various RPGs, as well as FPS engines, so one goal is for the ECS to be as versatile as possible.
This is still very early in the process, but if you want to build:

```
make clean && make
```

If you happen to have the graphics, you can run the game with: `./game`

## Motivations

I am looking to create a very tight development feedback loop, so building as fast as possible is important. Everything currently sits inside of `main.cpp` at the moment for this reason. I feel like if the source code is kept extremely clean and tight and as few LOC as possible, it will be easier to refactor and build upon in the future. This is a very experimental approach, but I am excited to see where it goes.

I also seek to offer an alternative method to gamedev. There are many existing engines out there, and plenty of others have "rolled-their-own" engine in C++, Java, Python, rust, and other languages, so why do it in C++ all over again?

- Educational purposes
- Alternative to existing engines
- Tight development feedback loop
- Versatile ECS for multiple game types
- Fun!

## Story

This project emerged from a competition held via the l0de Radio Hour on EFNet IRC between myself and [birdneststream](https://github.com/birdneststream/). The competition was initially to see if either of us could reproduce the classic game, "Tempest", in an alotted time. I created a variant on the game that later became a sidescrolling shooter. During initial development, I discovered an interesting mechanic I hadn't seen in other games, so I decided to start building the idea forward to see what it could become. The initial code was trash but worked. The idea and assets still exist and now I've finally decided to push it forward as far as I can.

## Other gamedevs to learn from

- [DaFluffyPotato](https://www.youtube.com/@DaFluffyPotato)
- [TokyoSpliff](https://www.youtube.com/@tokyospliff)
- [bisqwit](https://www.youtube.com/@Bisqwit)

-----

## Other developers to learn from

- [ThePrimeAgen](https://www.youtube.com/@ThePrimeTimeagen)
- [birdneststream](https://github.com/birdneststream/)

-----

## My Social Media

- [Twitter](https://twitter.com/evildojo666)
- [Twitch](https://twitch.tv/evildojo666)
- [YouTube](https://www.youtube.com/@evildojo666)
- [Github](https://www.github.com/mikedesu)

