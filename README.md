# AC_hacks
AC_hacks is a small project where I explored hacking the game AssaultCube using x86 assembly.

In this project, I hooked some of my code to execute when you type something into the chat. My code takes some of the parameters and writes what is typed in the chat into a text file.

![HelloWorldRevereEngineering](https://github.com/NedasR/AC_hacks/assets/129998724/b71780fd-6f9e-4cca-aea6-5f721b8d1d1e)

This is done by injecting a DLL that contains my code. Additionally, the DLL includes some extra code that automatically overwrites memory to create a jump (jmp) to my function. After executing my code, it jumps back to the injection point, 
one memory address below the jump to my CodeCave. My DLL also overwrites the CodeCave to preserve the registers that were changed when my function ran.
