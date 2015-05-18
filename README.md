# Weil-Pairing
A simple implementation of Weil pairing


We implemented the Weil pairing of elliptic curve over $\mathbb{F}_{p^2}$, where $p$ is a prime such that $p\equiv 1\mod 4$.

**Edit**: We add a new interpreter for this library.

To build the interpreter, use the follow compilation command in gcc:

    g++ - c Miller.cpp interpreter.cpp elliptic.h
    g++ -o interpreter interpreter.o Miller.o

Copyright (c) 2015 <changlong1993@gamil.com>.

