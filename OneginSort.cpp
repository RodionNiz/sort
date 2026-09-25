#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

enum ErrsCode
{
    NofArgErr = -1,
    FileNameErr = -2
};


enum PrintStrReason 
{
    DebugingPrint = -1,
    PrintAns = 1
};


typedef int (*CompareFuncType) (const void* cmpStrIndex, const void* pivotIndex);


struct String 
{
    char* strPtr;
    size_t len;
};


struct TextStates 
{
    size_t nOfStr;
    size_t nOfGoodStr;
    size_t textLen;
    char*  text; 
    String* strings;
};


TextStates ParseText    (FILE* readingFile, char* fileName);
size_t  CountFileSize   (char* const fileName);
char*   ReadFile        (size_t* fileSize, FILE* file);
size_t  CountStrings    (char* text, size_t textSize);
size_t  FillIndex       (String* originalTextStrings, char* text, size_t textSize);

void    QSort           (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
size_t  Partition       (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
void    ByteSwap        (void* first, void* second, size_t nOfBytes);

int     CompareDirect   (const void* cmpStrIndex, const void* pivotIndex);
int     CompareReverse  (const void* cmpStrIndex, const void* pivotIndex);
int     ComparePtrs     (const void* cmpStrIndex, const void* pivotIndex);

void    PrintStrings    (String* index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile);

void    ClearMem        (char* text, String* stringsPtrs);


int main (int argc, char* argv [])
{
    //TODO имя сохраняемого файла
    if (argv [1] == nullptr)
    {
        char fileName [] = "Onegin.txt";
        argv [1] = fileName;
    }

    if (argv [2] == nullptr)
    {
        char sortedFileName [] = "SortedText.txt";
        argv [2] = sortedFileName;
    }

    if (argc > 3)
    {
        printf ("Error: %s",strerror (E2BIG));
        return E2BIG;
    }

    //TODO коды ошибок
    //TODO strerror errno
    FILE* file = fopen (argv [1], "r");
    FILE* sortedFile = fopen (argv [2], "w");

    if (file == nullptr)
    {
        printf ("Error: %s", strerror (ENOENT));
        return ENOENT;
    }

    if (sortedFile == nullptr)
    {
        printf ("Error: %s", strerror (ENOSPC));
        return ENOSPC;
    }

    TextStates states = ParseText (file, argv[1]);

    QSort (states.strings, states.nOfGoodStr, sizeof (states.strings [0]) , &CompareDirect);

    PrintStrings (states.strings, states.nOfGoodStr, DebugingPrint, sortedFile);

    qsort (states.strings, states.nOfGoodStr, sizeof (states.strings [0]), &CompareReverse);

    PrintStrings (states.strings, states.nOfGoodStr, PrintAns, sortedFile);

    qsort (states.strings, states.nOfGoodStr, sizeof (states.strings [0]), &ComparePtrs);

    PrintStrings (states.strings, states.nOfGoodStr, PrintAns, sortedFile);

    ClearMem (states.text, states.strings);

    fclose (file);
    fclose (sortedFile);
}


TextStates ParseText (FILE* readingFile, char* const fileName)
{
    assert (readingFile != nullptr);
    assert (fileName != nullptr);

    TextStates states = {};

    states.textLen = CountFileSize (fileName);

    states.text = ReadFile (&(states.textLen), readingFile);
    
    states.nOfStr = CountStrings (states.text, states.textLen);

    states.strings = (String*) calloc (states.nOfStr, sizeof (states.strings [0]));

    states.nOfGoodStr = FillIndex (states.strings, states.text, states.textLen);

    states.strings = (String*) realloc (states.strings, states.nOfGoodStr * sizeof (states.strings [0]));

    return states;
}


size_t CountFileSize (char* const fileName)
{
    assert (fileName != nullptr);

    struct stat fileStat = {};

    assert (stat (fileName, &fileStat) != -1);

    return  fileStat.st_size;
}


//TODO отдельная функция подсчета размера файла
char* ReadFile (size_t* fileSize, FILE* file)
{
    assert (file != nullptr);
    assert (fileSize != nullptr);

    char* text = (char*) calloc (*fileSize + 1, sizeof (text [0]));

    *fileSize = fread (text, sizeof (text [0]), *fileSize, file);

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


size_t FillIndex (String* originalTextStrings, char* text, size_t textSize)
{
    assert (originalTextStrings != nullptr);
    assert (text != nullptr);
    assert (textSize != 0);

    size_t count = 0;
    size_t strCount = 0;

    while (count < textSize)
    {
        unsigned int wasLetter = 0;

        //скип пустых строк
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

            while (text [count] != '\n' && text [count] != '\0')
            {
                count++;
            }

            text [count] = '\0';

            (originalTextStrings [strCount]).strPtr = (text + countDup);
            (originalTextStrings [strCount]).len = count - countDup;

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

    const String* cmpStrCasted = (const String*) cmpStrIndex;
    const String* pivotCasted  = (const String*) pivotIndex;

    size_t cmpCount = 0;
    size_t pivotCount = 0;

    while (cmpStrCasted->strPtr [cmpCount] != '\0' && pivotCasted->strPtr [pivotCount] != '\0')
    {
        
        while (!isalpha (cmpStrCasted->strPtr [cmpCount]) && cmpStrCasted->strPtr [cmpCount] != '\0')
        {
            cmpCount++;
        }

        while (!isalpha (pivotCasted->strPtr [pivotCount]) && pivotCasted->strPtr [pivotCount] != '\0')
        {
            pivotCount++;
        }

        if (cmpStrCasted->strPtr [cmpCount] == '\0' || pivotCasted->strPtr [pivotCount] == '\0')
        {
            break;
        }
        
        if (tolower (cmpStrCasted->strPtr [cmpCount]) != tolower (pivotCasted->strPtr [pivotCount]))
        {
            return tolower (cmpStrCasted->strPtr [cmpCount]) - tolower (pivotCasted->strPtr [pivotCount]);
        }

        cmpCount++;
        pivotCount++;
    }

    return cmpStrCasted->strPtr [cmpCount] - pivotCasted->strPtr [pivotCount];
}


int CompareReverse (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const String* cmpStrCasted = (const String*) cmpStrIndex;
    const String* pivotCasted  = (const String*) pivotIndex;

    int cmpCount   = cmpStrCasted->len;
    int pivotCount = pivotCasted->len;

    
    //TODO struct {ptr, len};
    while (cmpCount >= 0 && pivotCount >= 0)
    {
        
        while (cmpCount >= 0 && isalpha (cmpStrCasted->strPtr [cmpCount] == 0))
        {
            cmpCount--;
        }

        while (pivotCount >= 0 && isalpha (pivotCasted->strPtr [pivotCount]) == 0)
        {
            pivotCount--;
        }

        if (cmpCount == -1 || pivotCount == -1)
        {
            break;
        }
        
        if (tolower (cmpStrCasted->strPtr [cmpCount]) != tolower (pivotCasted->strPtr [pivotCount]))
        {
            return tolower (cmpStrCasted->strPtr [cmpCount]) - tolower (pivotCasted->strPtr [pivotCount]);
        }

        cmpCount--;
        pivotCount--;
    }

    return cmpCount - pivotCount;
}


int ComparePtrs (const void* cmpStrIndex, const void* pivotIndex)
{
    assert (cmpStrIndex != nullptr);
    assert (pivotIndex  != nullptr);

    const String* cmpStrCasted = (const String*) cmpStrIndex;
    const String* pivotCasted  = (const String*) pivotIndex;

    return cmpStrCasted->strPtr - pivotCasted->strPtr;
}


void PrintStrings (String* index, size_t nOfStr, PrintStrReason reason, FILE* sortedFile)
{
    assert (index != nullptr);
    assert (sortedFile != nullptr);
    assert (nOfStr != 0);

    switch (reason)
    {
        case DebugingPrint:
            for (size_t i = 0; i < nOfStr; i++)
            {
                printf ("str indexed %zu has len %zu and ptr %p is \'%s\'\n", i, (index [i]).len , (index [i]).strPtr,  (index [i]).strPtr);
            }
            break;
        
        case PrintAns:
            for (size_t i = 0; i < nOfStr; i++)
            {
                fprintf (sortedFile, "%s\n", (index [i]).strPtr);
            }

            fprintf (sortedFile, "************************************\n\n\n\n\n\n\n\n\n");
            break;

        default:
            break;
    }
}


void ClearMem (char* text, String* stringsPtrs)
{
    assert (text != nullptr);
    assert (stringsPtrs != nullptr);

    free (text);
    free (stringsPtrs);
}