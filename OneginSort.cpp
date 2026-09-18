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
char*   ReadFile        (size_t* fileSize, FILE* file);
void    FillIndex       (char*** index, char*** indexCopy, size_t indexElemSize, size_t* strCount, char* text, size_t textSize);


//TODO сделать все строки в один буффер
int main ()
{
    FILE* file = fopen("Onegin.txt", "r");
    FILE* sortedFile = fopen("SortedOnegin.txt", "w");

    char** index = (char**) calloc (1, sizeof (index [0]));
    char** indexCopy = (char**) calloc (1, sizeof (indexCopy [0]));
    
    size_t strCount = 0;

    size_t fileSize  = 0;

    char* text = ReadFile (&fileSize, file);

    FillIndex (&index, &indexCopy, sizeof (index [0]), &strCount, text, fileSize);

    QSortStr (index, 0, strCount - 1, &CompareStr);

    qsort (indexCopy, strCount, sizeof (index [0]), &CompareStandard);

    printf ("qsort finished\n");

    PrintStrings (index, strCount, PrintAns, sortedFile);
    
    PrintStrings (indexCopy, strCount, PrintAns, sortedFile);

    

    free (index);
    free (indexCopy);
    free (text);

    fclose (file);
    fclose (sortedFile);
}


char* ReadFile (size_t* fileSize, FILE* file)
{
    assert (file != nullptr);
    assert (fileSize != nullptr);
    
    fseek(file, 0, SEEK_END);

    size_t approxFileSize = ftell(file);
    
    fseek(file, 0, SEEK_SET);

    char* text = (char*) calloc (approxFileSize + 1, sizeof (text [0]));

    *fileSize = fread (text, sizeof (text [0]), approxFileSize, file);

    return text;
}


void FillIndex (char*** index, char*** indexCopy, size_t indexElemSize, size_t* strCount, char* text, size_t textSize)
{
    assert (index != nullptr);
    assert (indexCopy != nullptr);
    assert (strCount != nullptr);
    assert (text != nullptr);

    size_t count = 0;

    while (count < textSize)
    {
        unsigned int wasLetter = 0;

        while (!wasLetter && text [count] != '\n')
        {
            if (isalpha (text [count]))
            {
                wasLetter = 1;
                break;
            }
            count++;
        }

        if (wasLetter)
        {
            (*strCount)++;

            *index = (char**) realloc (*index, *strCount * indexElemSize);
            *indexCopy = (char**) realloc (*indexCopy, *strCount * indexElemSize);

            (*index) [*strCount - 1] = (text + count);
            (*indexCopy) [*strCount - 1] = (text + count);
            
            while (text [count] != '\n')
            {
                count++;
            }

            text [count] = '\0';
        }

        count++;
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
        
        if (tolower (cmpStrCasted [cmpCount]) != tolower (pivotCasted [pivotCount]))
        {
            return tolower (cmpStrCasted [cmpCount]) - tolower (pivotCasted [pivotCount]);
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
        
        if (tolower (cmpStrCasted [cmpCount]) != tolower (pivotCasted [pivotCount]))
        {
            return tolower (cmpStrCasted [cmpCount]) - tolower (pivotCasted [pivotCount]);
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


/*
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
*/

void PrintStrings (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile)
{
    switch (reason)
    {
        case DebugingPrint:
            for (size_t i = 0; i < nOfStr; i++)
            {
                printf ("str indexed %zu has len %zu and ptr %p is \'%s\'\n", i, strlen (index [i]), index [i],  index [i]);
            }
            break;
        
        case PrintAns:

            for (size_t i = 0; i < nOfStr; i++)
            {
                fprintf (sortedFile, "%s\n", index [i]);
            }
            
            fprintf (sortedFile, "************************************\n\n");
            break;
        
        default:
            break;
    }
}
