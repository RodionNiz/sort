#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

enum PrintStrReason 
{
    DebugingPrint = -1,
    PrintAns = 1,
    PrintOriginal = 2
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

TextStates ParseText    (FILE* readingFile, char* fileName);
char*   ReadFile        (size_t* fileSize, FILE* file, char* fileName);
size_t  CountStrings    (char* text, size_t textSize);
size_t  FillIndex       (char*** originalTextIndexes, char* text, size_t textSize);

void    QSort           (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
size_t  Partition       (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
void    ByteSwap        (void* first, void* second, size_t nOfBytes);

int     CompareDirect   (const void* cmpStrIndex, const void* pivotIndex);
int     CompareReverse  (const void* cmpStrIndex, const void* pivotIndex);

void    PrintStrings    (char** index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile);

void    ClearMem        (char* text, char** stringsPtrs);


int main (int argc, char* argv [])
{
    if (argc < 2)
    {
        char fileName [11] = "Onegin.txt";
        argv [1] = fileName;
    }

    if (argc > 2)
    {
        printf ("Unsupported number of arguments");
        abort ();
    }

    FILE* file = fopen (argv [1], "r");
    FILE* sortedFile = fopen ("SortedText.txt", "w");

    assert (file != nullptr);
    assert (sortedFile != nullptr);

    TextStates states = ParseText (file, argv[1]);
    
    QSort (states.textPtrs, states.nOfGoodStr, sizeof (states.textPtrs [0]) , &CompareDirect);

    PrintStrings (states.textPtrs, states.nOfGoodStr, PrintAns, sortedFile);

    qsort (states.textPtrs, states.nOfGoodStr, sizeof (states.textPtrs [0]), &CompareReverse);
    
    PrintStrings (states.textPtrs, states.nOfGoodStr, PrintAns, sortedFile);

    PrintStrings (&states.text, states.nOfGoodStr, PrintOriginal, sortedFile);

    ClearMem (states.text, states.textPtrs);

    fclose (file);
    fclose (sortedFile);
}


TextStates ParseText (FILE* readingFile, char* const fileName)
{
    assert (readingFile != nullptr);
    assert (fileName != nullptr);

    TextStates states = {};

    states.text = ReadFile (&(states.textLen), readingFile, fileName);
    
    states.nOfStr = CountStrings (states.text, states.textLen);

    states.textPtrs = (char**) calloc (states.nOfStr, sizeof (states.textPtrs [0]));

    states.nOfGoodStr = FillIndex (&states.textPtrs, states.text, states.textLen);

    states.textPtrs = (char**) realloc (states.textPtrs, states.nOfGoodStr * sizeof (states.textPtrs [0]));

    return states;
}


char* ReadFile (size_t* fileSize, FILE* file, char* const fileName)
{
    assert (file != nullptr);
    assert (fileSize != nullptr);
    assert (fileName != nullptr);
    
    struct stat fileStat = {};

    assert (stat (fileName, &fileStat) != -1);

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
    assert (textSize != 0);

    size_t count = 0;
    size_t strCount = 0;

    char endOfStrMarker = '\n';

    while (count < textSize)
    {
        if (text [count] == endOfStrMarker)
        {
            strCount++;
        }

        count++;
    }

    if (text [textSize - 1] != endOfStrMarker)
    {
        strCount++;
    }

    return strCount;
}


size_t FillIndex (char*** originalTextIndexes, char* text, size_t textSize)
{
    assert (originalTextIndexes != nullptr);
    assert (text != nullptr);
    assert (textSize != 0);

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
    assert (srtData != nullptr);
    assert (cmpFunc != nullptr);
    assert (nOfElements != 0);
    assert (sizeofElement != 0);

    size_t partitionResult = Partition (srtData, nOfElements, sizeofElement, cmpFunc);
    

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
    assert (srtData != nullptr);
    assert (cmpFunc != nullptr);
    assert (nOfElements != 0);
    assert (sizeofElement != 0);

    size_t pivotIndex = (nOfElements - 1) / 2;

    size_t lIndex = 0;
    size_t rIndex = nOfElements - 1;

    while (lIndex < rIndex)
    {
        while (lIndex < rIndex && cmpFunc ((void*) ((char*) srtData + lIndex * sizeofElement),
                                           (void*) ((char*) srtData + pivotIndex * sizeofElement)) < 0)
        {
            lIndex++;
        }

        while (lIndex < rIndex && cmpFunc ((void*) ((char*) srtData + rIndex * sizeofElement),
                                           (void*) ((char*) srtData + pivotIndex * sizeofElement)) > 0)
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
    assert (first  != nullptr);
    assert (second != nullptr);
    assert (nOfBytes != 0);

    for (size_t i = 0; i < nOfBytes; i++) 
    {
        char tempchar = ((char*) first) [i];
        ((char*) first)  [i] = ((char*) second) [i];
        ((char*) second) [i] = tempchar;
    }
}


int CompareDirect (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const char* cmpStrCasted = *((const char* const*) cmpStrIndex);
    const char* pivotCasted  = *((const char* const*) pivotIndex);

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


int CompareReverse (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const char* cmpStrCasted = *((const char* const*) cmpStrIndex);
    const char* pivotCasted  = *((const char* const*) pivotIndex);

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
    assert (nOfStr != 0);

    char* text = *index;

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

        case PrintOriginal:
            for (size_t i = 0; i < nOfStr; i++)
            {
                fprintf (sortedFile, "%s\n", text);
                text += strlen (text) + 1;
            }
            break;
        
        default:
            break;
    }
}


void ClearMem (char* text, char** stringsPtrs)
{
    assert (text != nullptr);
    assert (stringsPtrs != nullptr);

    free (text);
    free (stringsPtrs);
}