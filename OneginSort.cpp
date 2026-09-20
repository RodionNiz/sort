#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

enum PrintStrReason 
{
    DebugingPrint = -1,
    PrintAns = 1
};


typedef int (*CompareFuncType) (const void* cmpStrIndex, const void* pivotIndex);


struct TextStates 
{
    size_t nOfStr;
    size_t nOfGoodStr;
    size_t textLen;
    char*  text; 
    char** textPtrs;
};


char*   ReadFile        (size_t* fileSize, FILE* file);
size_t  FillIndex       (char*** originalTextIndexes, char* text, size_t textSize);
size_t  strlenToN       (const char* str);
size_t  CountStrings    (char* text, size_t textSize);
TextStates ParseText    (FILE* readingFile);

void    QSort           (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
size_t  Partition       (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
void    ByteSwap        (void* first, void* second, size_t nOfBytes);

int     CompareStr      (const void* cmpStrIndex, const void* pivotIndex);
int     CompareStandard (const void* cmpStrIndex, const void* pivotIndex);

void    PrintStrings    (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile);


int main ()
{
    FILE* file = fopen("Onegin.txt", "r");
    FILE* sortedFile = fopen("SortedOnegin.txt", "w");

    TextStates states = ParseText (file);

    char** index = (char**) calloc (states.nOfGoodStr, sizeof (states.textPtrs [0]));

    for (size_t i = 0; i < states.nOfGoodStr; i++)
    {
        index [i] = states.textPtrs [i];
    }
    
    QSort (index, states.nOfGoodStr, sizeof (index [0]) , &CompareStr);

    PrintStrings (index, states.nOfGoodStr, PrintAns, sortedFile);

    qsort (index, states.nOfGoodStr, sizeof (index [0]), &CompareStandard);
    
    PrintStrings (index, states.nOfGoodStr, PrintAns, sortedFile);

    PrintStrings (states.textPtrs, states.nOfGoodStr, PrintAns, sortedFile);
    

    free (index);
    free (states.textPtrs);
    free (states.text);

    fclose (file);
    fclose (sortedFile);
}


TextStates ParseText (FILE* readingFile)
{
    TextStates states = {};

    states.text = ReadFile(&(states.textLen), readingFile);
    
    states.nOfStr = CountStrings(states.text, states.textLen);

    states.textPtrs = (char**) calloc (states.nOfStr, sizeof (states.textPtrs [0]));

    states.nOfGoodStr = FillIndex (&states.textPtrs, states.text, states.textLen);

    states.textPtrs = (char**) realloc (states.textPtrs, states.nOfGoodStr * sizeof (states.textPtrs [0]));

    return states;
}


char* ReadFile (size_t* fileSize, FILE* file)
{
    assert (file != nullptr);
    assert (fileSize != nullptr);
    
    struct stat fileStat = {};

    stat ("Onegin.txt", &fileStat);

    size_t approxFileSize = fileStat.st_size;

    char* text = (char*) calloc (approxFileSize + 1, sizeof (text [0]));

    *fileSize = fread (text, sizeof (text [0]), approxFileSize, file);

    text = (char*) realloc (text, *fileSize * sizeof (text [0]) + 1);

    text [*fileSize] = 0;

    assert (text != nullptr);

    return text;
}


size_t CountStrings (char* text, size_t textSize)
{
    assert (text != nullptr);

    size_t count = 0;
    size_t strCount = 0;

    while (count < textSize)
    {
        if (text [count] == '\n')
        {
            strCount++;
        }

        count++;
    }

    if (text [textSize -1] != '\n')
    {
        strCount++;
    }

    return strCount;
}


size_t FillIndex (char*** originalTextIndexes, char* text, size_t textSize)
{
    assert (originalTextIndexes != nullptr);
    assert (text != nullptr);

    size_t count = 0;
    size_t strCount = 0;

    while (count < textSize)
    {
        unsigned int wasLetter = 0;

        while (!wasLetter && text [count] != '\n' && count < textSize)
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
            size_t countDup = count;

            while (text [count] != '\n')
            {
                count++;
            }

            text [count] = '\0';

            (*originalTextIndexes) [strCount] = (text + countDup);
            strCount++;
        }

        count++;
    }

    return strCount;
}


void QSort (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc)
{
    assert (srtData);
    assert (cmpFunc);
    assert (nOfElements != 0);
    assert (sizeofElement != 0);

    size_t partitionResult = Partition(srtData, nOfElements, sizeofElement, cmpFunc);
    

    if (partitionResult > 0)
    {
        QSort (srtData, partitionResult, sizeofElement, cmpFunc);
    }

    if (partitionResult + 1 < nOfElements - 1)
    {
        QSort ((void*) ((char*) srtData + (partitionResult) * sizeofElement ), nOfElements - partitionResult, sizeofElement, cmpFunc);
    }
}


size_t Partition (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc)
{
    size_t pivotIndex = (nOfElements - 1) / 2;

    size_t lIndex = 0;
    size_t rIndex = nOfElements - 1;

    while (lIndex < rIndex)
    {
        while (lIndex < rIndex && cmpFunc ((void*) ((char*) srtData + lIndex * sizeofElement), (void*) ((char*) srtData + pivotIndex * sizeofElement)) < 0)
        {
            lIndex++;
        }

        while (lIndex < rIndex && cmpFunc ((void*) ((char*) srtData + rIndex * sizeofElement), (void*) ((char*) srtData + pivotIndex * sizeofElement)) > 0)
        {
            rIndex--;
        }

        if (lIndex >= rIndex)
        {
             break;   
        }

        if (pivotIndex == lIndex)
        {
            pivotIndex = rIndex;
        }

        else if (pivotIndex == rIndex)
        {
            pivotIndex = lIndex;
        }

        ByteSwap ((void*) ((char*) srtData + lIndex * sizeofElement), (void*) ((char*) srtData + rIndex * sizeofElement), sizeofElement);

        lIndex++;
        rIndex--;
    }

    return rIndex;
}


void ByteSwap (void* first, void* second, size_t nOfBytes)
{
    for (size_t i = 0; i < nOfBytes; i++) 
    {
        char tempchar = ((char*) first) [i];
        ((char*) first) [i] = ((char*) second) [i];
        ((char*) second) [i] = tempchar;
    }
}


int CompareStr (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-qual"

    const char* cmpStrCasted = *((const char**) cmpStrIndex);
    const char* pivotCasted  = *((const char**) pivotIndex);

    #pragma GCC diagnostic pop

    size_t cmpCount = 0;
    size_t pivotCount = 0;

    while (cmpStrCasted [cmpCount] != '\0' && pivotCasted [pivotCount] != '\0')
    {
        
        while (!isalpha (cmpStrCasted [cmpCount]) && cmpStrCasted [cmpCount] != '\0')
        {
            cmpCount++;
        }

        while (!isalpha (pivotCasted [pivotCount]) && pivotCasted [pivotCount] != '\0')
        {
            pivotCount++;
        }

        if (cmpStrCasted [cmpCount] == '\0' || pivotCasted [pivotCount] == '\0')
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

size_t strlenToN (const char* str)
{
    assert (str != nullptr);

    const char* startAddress = str;

    while (*(str++) != '\n')
    {
    }

    return (size_t)(str - startAddress - 1);
}


int CompareStandard (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-qual"

    const char* cmpStrCasted = *((const char**) cmpStrIndex);
    const char* pivotCasted  = *((const char**) pivotIndex);

    #pragma GCC diagnostic pop

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


void PrintStrings (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile)
{
    assert (index != nullptr);
    assert (sortedFile != nullptr);

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

            fprintf (sortedFile, "************************************\n\n\n\n\n\n\n\n\n");
            break;
        
        default:
            break;
    }
}
