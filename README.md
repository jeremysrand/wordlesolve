#  Wordle Solver

This is a classic desk accessory for the Apple IIgs which solves Wordle puzzles for you.  Rather than being the game, this can be used to solve the puzzle for you by telling you the best guesses (hopefully).

![Wordle Solver Screenshot](/wordlesolve.png "Wordle Solver Screenshot")

It will give you its first guess of a five letter word.  Then, you are prompted to indicate which letters are correct.  You do so by trying a character for each letter:
* If the letter is in the correct position, then use a '^' character to inidicate that letter is right.
* If the letter exists in the word somewhere else but is in the wrong position, use a '?' character to indicate this.
* If the letter does not exist in the word at all, use a 'x' or 'X' character to indicate this.

Note that if the letter appears multiple times in the solution, then it is possible to have multiple letters marked as correct or correct and in the wrong position.  If the guess has more letters of that type in it, then one should be marked as wrong.

[YouTube video demo on real HW](https://youtu.be/_eNhsdsFjkI)

[Download a disk image](https://github.com/jeremysrand/wordlesolve/releases/download/1.0/wordlesolve.2mg)

[Download a SHK archive](https://github.com/jeremysrand/wordlesolve/releases/download/1.0/wordlesolve.shk)

## Implementation

In terms of implementation, this is a brute force approach that was done quickly during KansasFest 2022.  The algorithm could almost definitely be made better by being smart about how words are represented in memory and eliminating large sets of words as impossible in bulk rather than one at a time.  But it does work and that was my goal.

With the brute force implementation, it does take a few seconds for an unaccelerated GS to make its guess.  It may take about 15 seconds for it to make its second guess.  A more efficient algorithm would bring this down and maybe make it more or less instantaneous.  This is an exercise left to the reader.

Also, if you give bad feedback to the solver, perhaps saying a letter is present and then later say that it isn't present, the solver will behave oddly and likely fail to find a solution.  There is no checking to see if you are inconsistent in the feedback but in theory, there could be a check for that.

I am using the standard C fgets() function for reading the input from the user and it seems like you cannot edit your responses using fgets.  So, the input parser is pretty picky and unless you type things carefully, it will likely reject what you enter and ask you to try again.  Perhaps I should be using the text tools directly rather than the C standard library.

## Future

I would like to turn this into a desktop application or new desk accessory.  Probably a new desk accessory.  The text based input
