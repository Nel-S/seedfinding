# Pybiomes

This is an unfinished, limited port of some of [Cubiomes](https://www.github.com/Cubitect/cubiomes)' features to Python. It is not intended as a "proper" seedfinding tool, since a program written in C will run incomparably faster compared to the same program written in Python. Instead, this exists
1. as an exercise for myself of porting fixed-width C code to infinite-width Python, and
2. for use by seedfinders who are interested in the field but who aren't familiar with any other programming languages.

This program requires installing the [multimethod module](https://pypi.org/project/multimethod/) to function.<br>
Note that there are multiple syntactical and semantical differences between this library and the original Cubiomes library, and one should not expect this library to necessary be updated whenever Cubiomes is.