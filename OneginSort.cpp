#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define len 1000
#define maxCharsInStr 1000


#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"


enum PrintDataReason 
{
    GoLeft = 0,
    GoRight = 1,
    RewriteLeft = 2,
    RewriteRight = 3
};


enum PrintStrReason 
{
    DebugingPrint = -1,
    PrintAns = 1
};


typedef int (*CompareFuncType) (const void* cmpStrIndex, const void* pivotIndex);


int     CompareStr      (const void* cmpStrIndex, const void* pivotIndex);
int     CompareStandard (const void* cmpStrIndex, const void* pivotIndex);
void    QSortStr        (char** sortingData, const size_t leftBorder, const size_t rightBorder, CompareFuncType cmpFunc);
void    PrintData       (char** data, size_t lIndex, size_t rIndex, char* pivotValue, PrintDataReason reason);
char*   strdupreverse   (char *src);
void    PrintStrings    (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile);


int main ()
{
    FILE* file = fopen("Onegin.txt", "r");
    FILE* sortedFile = fopen("SortedOnegin.txt", "w");

    char** index = (char**) calloc (1, sizeof (index [0]));
    char** indexCopy = (char**) calloc (1, sizeof (index [0]));
    char buffer [maxCharsInStr] = {};

    size_t strCount = 0;

    while (fgets (buffer, maxCharsInStr, file))
    {
        strCount++;

        index = (char**) realloc (index, strCount * sizeof (index [0]));
        indexCopy = (char**) realloc (indexCopy, strCount * sizeof (indexCopy [0]));

        index [strCount - 1] = strdup (buffer);
        indexCopy [strCount - 1] = strdup (buffer);
        //indexCopy [strCount - 1] = strdupreverse (buffer);
    }

    QSortStr (index, 0, strCount - 1, &CompareStr);

    PrintStrings (index, strCount, PrintAns, sortedFile);

    qsort (indexCopy, strCount, sizeof (index [0]), &CompareStandard);
    
    PrintStrings (indexCopy, strCount, PrintAns, sortedFile);

    free (index);
    free (indexCopy);
}


void QSortStr (char** sortingData, const size_t leftBorder, const size_t rightBorder, CompareFuncType cmpFunc)
{
    assert (sortingData != nullptr);

    size_t lIndex = leftBorder;
    size_t rIndex = rightBorder;

    const char* pivotIndex = sortingData [leftBorder];

    while (lIndex < rIndex)
    {
        while (cmpFunc (&(sortingData [rIndex]), &pivotIndex) > 0 && lIndex < rIndex)
        {
            rIndex--;
        }

        if (lIndex < rIndex)
        {
            sortingData [lIndex] = sortingData [rIndex];
        }

        while (cmpFunc (&(sortingData [lIndex]), &pivotIndex) < 0 && lIndex < rIndex)
        {
            lIndex++;
        }

        if (lIndex < rIndex)
        {
            sortingData [rIndex] = sortingData [lIndex];
            rIndex--;
        }


        assert (lIndex <= rIndex);
        assert (lIndex >= leftBorder);
        assert (rIndex <= rightBorder);
    }

    assert (lIndex == rIndex);

    sortingData [lIndex] = (char*) pivotIndex;
    
    if (lIndex != leftBorder)
    {
        QSortStr (sortingData, leftBorder, lIndex - 1, cmpFunc);
    }

    if (rIndex != rightBorder)
    {
        QSortStr (sortingData, rIndex + 1, rightBorder, cmpFunc);
    }
}


int CompareStr (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const char* cmpStrCasted = *((const char**) cmpStrIndex);
    const char* pivotCasted  = *((const char**) pivotIndex);

    size_t cmpCount = 0;
    size_t pivotCount = 0;

    while (cmpStrCasted [cmpCount] != '\n' && pivotCasted [pivotCount] != '\n')
    {
        while (!isalpha (cmpStrCasted [cmpCount]) && cmpStrCasted [cmpCount] != '\n')
        {
            cmpCount++;
        }

        while (!isalpha (pivotCasted [pivotCount]) && pivotCasted [pivotCount] != '\0')
        {
            pivotCount++;
        }

        if (cmpStrCasted [cmpCount] == '\n' || pivotCasted [pivotCount] == '\0')
        {
            break;
        }
        
        if (cmpStrCasted [cmpCount] != pivotCasted [pivotCount])
        {
            return cmpStrCasted [cmpCount] - pivotCasted [pivotCount];
        }

        cmpCount++;
        pivotCount++;
    }

    return cmpStrCasted [cmpCount] - pivotCasted [pivotCount];
}


int CompareStandard (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const char* cmpStrCasted = *((const char**) cmpStrIndex);
    const char* pivotCasted  = *((const char**) pivotIndex);

    int cmpCount   = strlen (cmpStrCasted);
    int pivotCount = strlen (pivotCasted);

    while (cmpCount >= 0 && pivotCount >= 0)
    {
        while (cmpCount >= 0 && isalpha (cmpStrCasted [cmpCount]) == 0)
        {
            cmpCount--;
        }

        while (pivotCount >= 0 && isalpha (pivotCasted [pivotCount]) == 0)
        {
            pivotCount--;
        }

        if (cmpCount == -1 || pivotCount == -1)
        {
            break;
        }
        
        if (cmpStrCasted [cmpCount] != pivotCasted [pivotCount])
        {
            return cmpStrCasted [cmpCount] - pivotCasted [pivotCount];
        }

        cmpCount--;
        pivotCount--;
    }

    return cmpCount - pivotCount;
}

/*
char* strdupreverse (char *src)
{
    assert (src != nullptr);

    size_t inputStrSize = strlen(src) + 1;

    char* srcDup = (char*) calloc (inputStrSize, sizeof (char));

    size_t count = 0;
    inputStrSize -= 2;

    while (*(src + inputStrSize) != 0)
    {
        *(srcDup + count) = *(src + inputStrSize);
        inputStrSize--;
        count++;
    }

    *(srcDup + count) = 0;

    assert (srcDup != nullptr);

    return srcDup;
}*/

void PrintData (char** data, size_t lIndex, size_t rIndex, char* pivotValue, PrintDataReason reason)
{
    printf ("Now data is: \n" GREEN);

    for (size_t i = 0; i < len; i++)
    {
        if (i == lIndex + 1)
        {
            printf (RESET);
        }

        if (i == rIndex)
        {
            printf (RED);
        }

        printf ("%s ", data [i]);
    }

    printf (RESET "\nleft = %zu, right = %zu\n", lIndex, rIndex);

    switch (reason)
    {
        case GoLeft:
            printf ("Right pointer moved left\n");
            break;

        case GoRight:
            printf ("Left pointer moved right\n");
            break;

        case RewriteLeft:
            printf ("Value from left side was rewrite to the right side\n");
            break;

        case RewriteRight:
            printf ("Value from right side was rewrite to the left side\n");
            break;

        default:
            break;
    }

    getchar ();
}

void PrintStrings (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile)
{
    switch (reason)
    {
        case DebugingPrint:
            for (size_t i = 0; i < nOfStr; i++)
            {
                printf ("str indexed %zu has len %zu and ptr %p is \'%s\'", i, strlen (index [i]), index [i],  index [i]);
            }
            break;
        
        case PrintAns:
            for (size_t i = 0; i < nOfStr; i++)
            {
                fprintf (sortedFile, "%s", index [i]);
            }
            break;
        
        default:
            break;
    }
}
