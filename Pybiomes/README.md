# Pybiomes

This is an unfinished, limited port of some of [Cubiomes](https://www.github.com/Cubitect/cubiomes)' features to Python. It is not intended as a "proper" seedfinding tool, since a program written in C will run incomparably faster compared to the same program written in Python. Instead, this exists
1. for use by people who are interested in seedfinding but who aren't familiar with any other programming languages;
1. as an exercise for myself for writing a full type-safe Python library, and for porting fixed-width C code to infinite-width Python; and
3. to refactor certain Cubiomes constructs that I personally wish had been implemented differently.

Note that there are multiple syntactical and semantical differences between this library and the original Cubiomes library. One should also not expect this library to necessarily be regularly updated whenever Cubiomes is.