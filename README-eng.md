# Word Frequrency Rating
[![License](https://img.shields.io/badge/LICENSE-The%20Unlicense-green?style=flat-square)](/LICENSE)  [![Version](https://img.shields.io/badge/VERSION-RELEASE%20--%201.0-green?style=flat-square)](https://github.com/averov90/Word-Frequrency/releases)
### :small_orange_diamond: [Русская версия](/README.md)
This code allows you to build a rating of words by frequency of use in the source text, which should be in the *test file.txt* in the source folder (the folder where the program is located).  
The *unordered dictionary* structure optimized for this task is used to calculate the frequency. By using the structure instead of passing through the array in a loop, you can significantly speed up the program. The threshold for creating a new block (*UD_BLACK_THRESHOLD*) can be changed for a specific situation, as well as the size of the structure block (by default, 1024 words). You may also need to replace the changed hash function because the current one was chosen for reasons of calculation speed on a large number of architectures (including microcontrollers) and a relatively acceptable frequency of collisions.
The winning algorithm *unordered dictionary* in this situation is the lack of necessity pass through the array of words in finding the right - first from the word compute hash function (*efficiency - O(1)*), and then this value is used as index in a hash table (*efficiency - O(1) time in the worst case is O(B * N), where B is as quantitative units, N is the number of words in each unit*). The probability of the worst case (and close to it) in the efficiency of hash tables is extremely small, so the most common will be the efficiency of O(1). The worst case of a hash table is equivalent to the worst case of an array (*the array has o(n)*).

### File format
For the program to work with files, it must contain a set of words separated by the following punctuation marks ```.,:;-?!"()``` or a space, or a line break. After the last word, there should also be a punctuation mark. Any number of punctuation marks can stand in a row. You can correct the rules for defining punctuation marks without any problems in the program code (in *switch*).

Example:
```
hello, hello,some.test here?
```