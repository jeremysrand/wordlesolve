//
//  words.h
//  wordlesolve
//
//  Created by Jeremy Rand on 2022-07-21.
//

#ifndef words_h
#define words_h

#include <stdint.h>

typedef struct tWordData {
    uint16_t numWords;
    char words[1];
} tWordData;

extern tWordData wordData;

extern void wordCode(void);

#endif /* words_h */
