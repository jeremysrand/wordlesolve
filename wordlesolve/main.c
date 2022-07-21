/*
 * main.c
 * wordlesolve
 *
 * Created by Jeremy Rand on 2022-07-21.
 * Copyright (c) 2022 ___ORGANIZATIONNAME___. All rights reserved.
 *
 */

#pragma cda "wordlesolve" start shutdown

#include <Types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "words.h"

// Defines

#define WORD_LEN 5
#define NUM_LETTERS 26
#define LETTER_TO_INDEX(letter) ((letter) - 'A')
#define INDEX_TO_LETTER(index) ((index) + 'A')
#define MAX_GUESSES 6

#undef FIND_BEST_START_WORD
#define BEST_WORD "AEROS"


// Globals

uint16_t numWords;
char * words;
char * lastGuess;
char buffer[256];
Boolean * wordsEliminated;
uint16_t totalLetterCounts[WORD_LEN][NUM_LETTERS];

// Implementation

void updateLetterCount(uint16_t wordIndex, char * wordPtr)
{
    static uint16_t currentLetterCounts[NUM_LETTERS];
    uint16_t i;
    uint16_t letterIndex;
    
    memset(currentLetterCounts, 0, sizeof(currentLetterCounts));
    for (i = 0; i < WORD_LEN; i++, wordPtr++) {
        letterIndex = LETTER_TO_INDEX(*wordPtr);
        totalLetterCounts[currentLetterCounts[letterIndex]][letterIndex]++;
        currentLetterCounts[letterIndex]++;
    }
}

void printWord(char * wordPtr)
{
    uint16_t i;
    for (i = 0; i < WORD_LEN; i++, wordPtr++) {
        putc(*wordPtr, stdout);
    }
}

uint32_t scoreWord(uint16_t wordIndex, char * wordPtr)
{
    static uint16_t currentLetterCounts[NUM_LETTERS];
    uint16_t i;
    uint16_t letterIndex;
    uint32_t result = 0;
    
    memset(currentLetterCounts, 0, sizeof(currentLetterCounts));
    for (i = 0; i < WORD_LEN; i++, wordPtr++) {
        letterIndex = LETTER_TO_INDEX(*wordPtr);
        result += totalLetterCounts[currentLetterCounts[letterIndex]][letterIndex];
        currentLetterCounts[letterIndex]++;
    }
    return result;
}

#ifndef FIND_BEST_START_WORD
void solvePuzzle(void)
{
    uint16_t numGuesses;
    
    lastGuess = BEST_WORD;
    for (numGuesses = 0; numGuesses < MAX_GUESSES; numGuesses++) {
        printf("Guess:            ");
        printWord(lastGuess);
        printf("\nEnter match info: ");
        fgets(buffer, sizeof(buffer), stdin);
    }
}
#else
void findBestStartWord(void)
{
    uint16_t wordIndex;
    uint16_t i;
    char * wordPtr;
    uint32_t bestScore = 0;
    uint32_t currentScore;
    char * bestWord;
    
    memset(totalLetterCounts, 0, sizeof(totalLetterCounts));
    
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        updateLetterCount(wordIndex, wordPtr);
    }
    
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        currentScore = scoreWord(wordIndex, wordPtr);
        if (currentScore > bestScore)
        {
            bestScore = currentScore;
            bestWord = wordPtr;
        }
    }
    
    printf("\nBest Word: ");
    printWord(bestWord);
    printf("\n");
}
#endif

void start(void)
{
    uint16_t * ptrNumWords;
    
    // This is weird and I don't know why I need to do this but this makes it work.  Perhaps this triggers the segment to load.
    ptrNumWords = &wordData.numWords;
    numWords = *ptrNumWords;

#if 0 // Delete this debug...
    printf("numWords = %u\n", numWords);
    words = &(wordData.words[0]);
    printf("firstWord = %x %x %x %x %x\n", (uint16_t)words[0], (uint16_t)words[1], (uint16_t)words[2], (uint16_t)words[3], (uint16_t)words[4]);
#endif // To here...
    
    wordsEliminated = malloc(sizeof(Boolean) * numWords);
    memset(wordsEliminated, 0, sizeof(Boolean) * numWords);
    
#ifdef FIND_BEST_START_WORD
    findBestStartWord();
#else
    solvePuzzle();
#endif
    
    printf("\n\n   Press ENTER to quit...");
    
    fgets(buffer, sizeof(buffer), stdin);
    free(wordsEliminated);
}


void shutdown(void)
{
}

