#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mmalloc.h"

#define SUPER "Supercalifragilisticexpialidocious"

int main(void) {
    char *str, *str_bis, *strcp, *str_bis_bis, *str_b, *str_another, *str_test, *str_free, *str_free_bis, *str_test_free, *str_test_free_bis, *str_test_free_bis_bis;

    str_free = malloc(84);
    free(str_free);

    str_free_bis = malloc(44);
    free(str_free_bis);

    str_test_free = malloc(4);
    str_test_free_bis = malloc(4);
    str_test_free_bis_bis = malloc(4);
    free(str_test_free_bis_bis);
    str_test_free_bis_bis = malloc(4);

    /* Un appel direct à malloc */
    str = malloc(44); /* pourquoi 9 de trop ? pourquoi pas ! */
    str_bis = malloc(14);
    str_bis_bis = malloc(84);
    str_b = malloc(54);
    str_another = malloc(64);
    str_test = malloc(4);

    strcpy(str, SUPER);
    strcpy(str_bis,"TEST");
    strcpy(str_bis_bis,"TESTBIS");
    strcpy(str_b,"THIS IS A TEST");
    strcpy(str_another,"0123456789");
    strcpy(str_test,"ok");

    /* Un appel a malloc dans cet appel */
    strcp = strdup(str);

    /* Print is not debug, nevertheless */
    printf("%s\n%s\n%s\n%s\n%s\n%s\n", str, str_bis, strcp, str_bis_bis, str_b, str_another);

    /* On libere la memoire allouee sur str est strcp*/
    free(str);
    free(str_bis);
    free(strcp);
    free(str_bis_bis);
    free(str_b);
    free(str_test);
    free(str_another);

    exit(EXIT_SUCCESS);
}
