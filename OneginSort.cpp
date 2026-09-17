#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>


#define nOfStr 1000
#define maxCharsInStr 100


#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"


enum PrintReason 
{
    GoLeft = 0,
    GoRight = 1,
    RewriteLeft = 2,
    RewriteRight = 3
};


typedef int (*CompareFuncType) (const void* cmpStrIndex, const void* pivotIndex);


int     CompareStr      (const void* cmpStrIndex, const void* pivotIndex);
int     CompareStandard (const void* cmpStrIndex, const void* pivotIndex);
void    QSortStr        (char** sortingData, const size_t leftBorder, const size_t rightBorder, CompareFuncType cmpFunc);
void    PrintData       (char** data, size_t lIndex, size_t rIndex, char* pivotValue, PrintReason reason);


int main ()
{
    FILE *file = fopen("Onegin.txt", "r");
    FILE *sortedFile = fopen("SortedOnegin.txt", "w");

    char** index = (char**) calloc (1, sizeof (index [0]));
    char buffer [maxCharsInStr] = {};

    size_t strCount = 0;

    while (fgets (buffer, maxCharsInStr, file))
    {
        strCount++;
        index = (char**) realloc (index, strCount * sizeof (index [0]));
        index [strCount - 1] = strdup (buffer);
    }

    //QSortStr (index, 0, strCount - 1, &CompareStr);

    qsort (index, strCount, sizeof (index [0]), &CompareStr);
    
    for (size_t i = 0; i < strCount; i++)
    {
        fprintf (sortedFile, "%s", (const char*) index [i]);
    }
}


void QSortStr (char** sortingData, const size_t leftBorder, const size_t rightBorder, CompareFuncType cmpFunc)
{
    assert (sortingData != nullptr);

    size_t lIndex = leftBorder;
    size_t rIndex = rightBorder;

    const char* pivotIndex = sortingData [leftBorder];

    while (lIndex < rIndex)
    {
        //while (sortingData [rIndex] > pivot && lIndex < rIndex)
        while (cmpFunc (&(sortingData [rIndex]), &pivotIndex) > 0 && lIndex < rIndex)
        {
            rIndex--;
            //PrintData (sortingData, lIndex, rIndex, (char*) pivotIndex, GoLeft);
            //printf("right = %zu, left = %zu\n", rIndex, lIndex);
        }

        if (lIndex < rIndex)
        {
            sortingData [lIndex] = sortingData [rIndex];
            //PrintData (sortingData, lIndex, rIndex, (char*) pivotIndex, RewriteRight);
        }

        //while (sortingData [lIndex] < pivot && lIndex < rIndex)
        while (cmpFunc (&(sortingData [lIndex]), &pivotIndex) < 0 && lIndex < rIndex)
        {
            lIndex++;
            //PrintData (sortingData, lIndex, rIndex, (char*) pivotIndex, GoRight);
        }

        if (lIndex < rIndex)
        {
            sortingData [rIndex] = sortingData [lIndex];
            //PrintData (sortingData, lIndex, rIndex, (char*) pivotIndex, RewriteLeft);
            rIndex--;
        }


        assert (lIndex <= rIndex);
    }
    //printf("left = %zu, right = %zu\n", lIndex, rIndex);
    //printf ("pivot = %d\n", pivot);

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
    const char* cmpStrCasted = *((const char**) cmpStrIndex);
    const char* pivotCasted  = *((const char**) pivotIndex);

    size_t cmpCount = 0;
    size_t pivotCount = 0;

    while (cmpStrCasted [cmpCount] != 0 && pivotCasted [pivotCount] != 0)
    {
        while (isalpha (cmpStrCasted [cmpCount]) == 0 && cmpStrCasted [cmpCount] != 0)
        {
            cmpCount++;
        }

        while (isalpha (pivotCasted [pivotCount]) == 0 && pivotCasted [pivotCount] != 0)
        {
            pivotCount++;
        }
        
        if (cmpStrCasted [cmpCount] != pivotCasted [pivotCount])
        {
            return cmpStrCasted [cmpCount] - pivotCasted [pivotCount];
        }

        if (cmpStrCasted [cmpCount] != 0 && pivotCasted [pivotCount] != 0)
        {
            cmpCount++;
            pivotCount++;
        }
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

    if (!cmpCount)
    {
        cmpCount--;
    }
    
    if (!pivotCount)
    {
        pivotCount--;
    }

    assert (cmpCount >= 0);
    assert (pivotCount >= 0);

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

        if (cmpCount >= 0 && pivotCount >= 0)
        {
            cmpCount--;
            pivotCount--;
        }
    }

    return cmpCount - pivotCount;
}


void PrintData (char** data, size_t lIndex, size_t rIndex, char* pivotValue, PrintReason reason)
{
    printf ("Now data is: \n" GREEN);

    for (size_t i = 0; i < nOfStr; i++)
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
