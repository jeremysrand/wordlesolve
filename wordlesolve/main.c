/*
 * main.c
 * wordlesolve
 *
 * Created by Jeremy Rand on 2022-07-21.
 * Copyright (c) 2022 ___ORGANIZATIONNAME___. All rights reserved.
 *
 */

#pragma cda "Wordle Solver" start shutdown

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


// Typedefs

typedef struct tLetterCounts {
    uint16_t min;
    uint16_t max;
} tLetterCounts;

// Globals

uint16_t numWords;
char * words;
char * currentGuess;
char buffer[256];

Boolean * wordsEliminated;
tLetterCounts letterCounts[NUM_LETTERS];
char eliminatedLetters[WORD_LEN][NUM_LETTERS + 1];
char solvedLetters[WORD_LEN];
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

void blockCharAtPosition(char ch, uint16_t pos)
{
    char * ptr;
    
    for (ptr = eliminatedLetters[pos]; *ptr != '\0'; ptr++) {
        if (*ptr == ch)
            return;
    }
    *ptr = ch;
    ptr++;
    *ptr = '\0';
}

void updateKnowledge(char * hints)
{
    static Boolean letterVisited[NUM_LETTERS];
    uint16_t i, j;
    uint16_t letterIndex;
    Boolean capNumInstances;
    uint16_t minNumInstances;
    
    memset(letterVisited, 0, sizeof(letterVisited));
    for (i = 0; i < WORD_LEN; i++) {
        letterIndex = LETTER_TO_INDEX(currentGuess[i]);
        if (letterVisited[letterIndex])
            continue;
        
        letterVisited[letterIndex] = TRUE;
        capNumInstances = FALSE;
        minNumInstances = 0;
        
        for (j = i; j < WORD_LEN; j++) {
            if (LETTER_TO_INDEX(currentGuess[j]) != letterIndex)
                continue;
            
            switch (hints[j]) {
                case 'x':
                case 'X':
                    capNumInstances = TRUE;
                    break;
                    
                case '?':
                    blockCharAtPosition(currentGuess[j], j);
                    minNumInstances++;
                    break;
                    
                case '^':
                    solvedLetters[j] = currentGuess[j];
                    minNumInstances++;
                    break;
            }
        }
        
        letterCounts[letterIndex].min = minNumInstances;
        if (capNumInstances)
            letterCounts[letterIndex].max = minNumInstances;
    }
        
    
}

Boolean wordMatchesRules(char * wordPtr)
{
    static uint16_t currentLetterCounts[NUM_LETTERS];
    uint16_t i;
    uint16_t letterIndex;
    char ch;
    
    memset(currentLetterCounts, 0, sizeof(currentLetterCounts));
    for (i = 0; i < WORD_LEN; i++) {
        ch = wordPtr[i];
        letterIndex = LETTER_TO_INDEX(ch);
        currentLetterCounts[letterIndex]++;
        
        if ((solvedLetters[i] != '\0') &&
            (solvedLetters[i] != ch))
            return FALSE;
        
        if (strchr(eliminatedLetters[i], ch) != NULL)
            return FALSE;
    }
    
    for (i = 0; i < NUM_LETTERS; i++) {
        if ((currentLetterCounts[i] < letterCounts[i].min) ||
            (currentLetterCounts[i] > letterCounts[i].max))
            return FALSE;
    }
    
    return TRUE;
}

void maybeGuessAnEliminatedWord(void)
{
    static char lettersToFind[NUM_LETTERS + 1];
    char * wordPtr;
    char * prevWordPtr = NULL;
    char * lettersToFindPtr = lettersToFind;
    uint16_t wordIndex;
    uint16_t i;
    uint16_t bestScore = 1;
    uint16_t currScore;
    uint16_t numOccur;
    uint16_t letterChanging = WORD_LEN;
    char ch;
    
    // First figure out if only one letter is unknown at this point.  I could try to figure
    // that out from the things we have learned but easiest and most reliable to go to the
    // words that are left to find this.
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        if (wordsEliminated[wordIndex])
            continue;
        if (prevWordPtr != NULL) {
            for (i = 0; i < WORD_LEN; i++) {
                if (prevWordPtr[i] == wordPtr[i])
                    continue;
                
                if (letterChanging == WORD_LEN)
                    letterChanging = i;
                else if (letterChanging != i) // More than one letter changing in solution still
                    return;
                
                if (lettersToFind == lettersToFindPtr) {
                    *lettersToFindPtr = prevWordPtr[i];
                    lettersToFindPtr++;
                }
                *lettersToFindPtr = wordPtr[i];
                lettersToFindPtr++;
            }
        }
        prevWordPtr = wordPtr;
    }
    *lettersToFindPtr = '\0';
    
    // Now, we have the set of letters we need to eliminate in lettersToFind as a C string.
    // We need to go through all of the words and find the single word which has the most
    // of these candidate letters.
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        currScore = 0;
        for (lettersToFindPtr = lettersToFind; *lettersToFindPtr != '\0'; lettersToFindPtr++) {
            ch = *lettersToFindPtr;
            numOccur = 0;
            for (i = 0; i < WORD_LEN; i++) {
                if (wordPtr[i] == ch) {
                    numOccur++;
                    break;
                }
            }
            if (numOccur > letterCounts[ch].min)
                currScore++;
        }
        
        if (currScore > bestScore) {
            bestScore = currScore;
            currentGuess = wordPtr;
        }
    }
}

void makeNextGuess(char * hints, uint16_t numGuesses)
{
    uint16_t wordIndex;
    char * wordPtr;
    uint32_t bestScore = 0;
    uint32_t currentScore;
    uint16_t numWordsWithEqualScore = 0;
    
    printf("\n  ... Thinking ...\n");
    
    updateKnowledge(hints);
    
    memset(totalLetterCounts, 0, sizeof(totalLetterCounts));
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        if (wordsEliminated[wordIndex])
            continue;
        
        if (!wordMatchesRules(wordPtr)) {
            wordsEliminated[wordIndex] = TRUE;
            continue;
        }
        
        updateLetterCount(wordIndex, wordPtr);
    }
    
    currentGuess = NULL;
    wordPtr = words;
    for (wordIndex = 0; wordIndex < numWords; wordIndex++, wordPtr += 5) {
        if (wordsEliminated[wordIndex])
            continue;
        currentScore = scoreWord(wordIndex, wordPtr);
        if (currentScore > bestScore)
        {
            bestScore = currentScore;
            currentGuess = wordPtr;
            numWordsWithEqualScore = 0;
        } else if (currentScore == bestScore)
            numWordsWithEqualScore++;
    }
    
    // If we only have a single character to figure out and a few words left as possibility
    // and at least two more guesses, then consider making the next guess a word which
    // tries to figure which letter remains rather than trying to solve the puzzle.
    if ((numWordsWithEqualScore >= 2) &&
        (numGuesses < MAX_GUESSES - 1))
        maybeGuessAnEliminatedWord();
}

void promptToQuit(void)
{
    printf("\n\n   Press ENTER to quit...");
    fgets(buffer, sizeof(buffer), stdin);
}

void printInstructions(void)
{
    printf("  Use 'X' for letters not in the word.\n  Use '?' for letters in the word but in the wrong place.\n  Use '^' for correct letters.\n\n");
}

void getMatchInfo(uint16_t numGuesses)
{
    uint16_t i;
    Boolean isValid = TRUE;
    
    do {
        if (!isValid) {
            printf("\n\nInvalid entry.\n");
            printInstructions();
        }
        printf("\nGuess %u:          ", numGuesses + 1);
        printWord(currentGuess);
        printf("\nEnter match info: ");
        fgets(buffer, sizeof(buffer), stdin);
        
        if ((buffer[0] == 'q') ||
            (buffer[0] == 'Q'))
            return;
        
        isValid = FALSE;
        if ((strlen(buffer) == WORD_LEN + 1) &&
            (buffer[WORD_LEN] == '\n')) {
            buffer[WORD_LEN] == '\0';
            isValid = TRUE;
            for (i = 0; i < WORD_LEN; i++) {
                switch (buffer[i])
                {
                    case 'x':
                    case 'X':
                    case '?':
                    case '^':
                        break;
                    default:
                        isValid = FALSE;
                }
            }
        }
    } while (!isValid);
}

#ifndef FIND_BEST_START_WORD
void solvePuzzle(void)
{
    Boolean foundWord = FALSE;
    uint16_t numGuesses;
    uint16_t i;
    
    printf("Wordle Solver\n  By Jeremy Rand\n\nInstructions:\n");
    printInstructions();
    
    for (numGuesses = 0; numGuesses < MAX_GUESSES; numGuesses++) {
        if (numGuesses == 0)
            currentGuess = BEST_WORD;
        else
            makeNextGuess(buffer, numGuesses);
        
        if (currentGuess == NULL) {
            printf("\n\nCould not find a guess that mathches the criteria.\n  Did you give good feedback on the letters in the target word?\n");
            promptToQuit();
            return;
        }
        getMatchInfo(numGuesses);
        if ((buffer[0] == 'q') ||
            (buffer[0] == 'Q')) {
            return;
        }
        
        foundWord = TRUE;
        for (i = 0; i < WORD_LEN; i++) {
            if (buffer[i] != '^') {
                foundWord = FALSE;
                break;
            }
        }
        if (foundWord) {
            printf("\nI solved the Wordle.  It was ");
            printWord(currentGuess);
            promptToQuit();
            return;
        }
    }
    
    printf("\n\Ran out of guesses.\n  Did you give good feedback on the letters in the target word?\n");
    promptToQuit();
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
    promptToQuit();
}
#endif

void initState(void)
{
    uint16_t i;
    
    memset(wordsEliminated, 0, sizeof(Boolean) * numWords);
    memset(eliminatedLetters, 0, sizeof(eliminatedLetters));
    memset(solvedLetters, 0, sizeof(solvedLetters));
    for (i = 0; i < NUM_LETTERS; i++) {
        letterCounts[i].min = 0;
        letterCounts[i].max = WORD_LEN;
    }
}

void start(void)
{
    uint16_t * ptrNumWords;
    
    // This is weird and I don't know why I need to do this but this makes it work.  Perhaps this triggers the segment to load.
    ptrNumWords = &wordData.numWords;
    numWords = *ptrNumWords;
    words = &(wordData.words[0]);
    
    wordsEliminated = malloc(sizeof(Boolean) * numWords);
    initState();
    
#ifdef FIND_BEST_START_WORD
    findBestStartWord();
#else
    solvePuzzle();
#endif
    
    free(wordsEliminated);
}


void shutdown(void)
{
}

