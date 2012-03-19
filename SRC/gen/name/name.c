// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

char **SURNAME_LIST;
int    SURNAME_COUNT;

void init_surnames(void)
{
        int i, n;             // Incremented variables.
        FILE *fp;             // Points to the surname text file.
        char buf[MAX_LENGTH]; // For counting strings.
        char **namelist;      // Dynamic string array.

        // Open the file and count the number of strings it contains.
        fp = fopen("/home/hubert/src/arawak/SRC/gen/name/surnames-english", "r");

        for (n=0; fscanf(fp, "%s", buf) != EOF; n++);

        // Size the dynamic array to the count obtained.
        namelist = malloc(n * sizeof(char *));
        for (i=0; i<n; i++) {
                namelist[i] = malloc(MAX_LENGTH * sizeof(char));
        }

        // Rewind the file pointer and scan the strings into the array.
        rewind(fp);

        for (i=0; fscanf(fp, "%s", namelist[i]) != EOF; i++);

        // Close the file and set globals.
        fclose(fp);

        SURNAME_LIST  = namelist;
        SURNAME_COUNT = n;
}

const char *pick_surname(void)
{
        return (SURNAME_LIST[(roll_fair(SURNAME_COUNT-1))]);
}
