#include "joker_glob.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glob.h>
#include <wordexp.h>


void checkJoker(char*** sequence){
    wordexp_t result;
    char** command;
    command = sequence[0
    for(int j = 0 ; command[j]!=NULL ; j++){
        if(j==0){
            wordexp(command[j],&result,0);
        }else{
            wordexp(command[j],&result,WRDE_APPEND);
        }
        
        free(command[j]);
    }
    
    free(sequence[0]);
    sequence[0] = result.we_wordv;
}


