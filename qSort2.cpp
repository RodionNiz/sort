#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define dataSize 3

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

#define DoubleCmpAccuracy 0.0001

enum PrintReason 
{
    GoLeft = 0,
    GoRight = 1,
    Rewrite = 2
};

typedef int (*CompareFuncType) (const void* compareNum, const void* pivot);

void    QSort               (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);
void    PrintData           (int* data, size_t lIndex, size_t rIndex, size_t pivotIndex, PrintReason reason);
int     CompareUpInt        (const void* compNum, const void* pivot);
int     CompareDownInt      (const void* compNum, const void* pivot);
int     CompareUpFloat      (const void* compNum, const void* pivot);
int     CompareDownFloat    (const void* compNum, const void* pivot);
void    ByteSwap            (void* first, void* second, size_t nOfBytes);
size_t  Partition           (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc);

int main ()
{
    srand (time (NULL));
    int data [dataSize] = {5, 5, 5};
    
    printf ("\n");


    QSort(data, dataSize, sizeof (int), &CompareUpInt);

    for (size_t i = 0; i < dataSize; i++)
    {
        printf ("%d ", data [i]);
    }

    printf ("\n");
}


void QSort (void* srtData, const size_t nOfElements, const size_t sizeofElement, CompareFuncType cmpFunc)
{
    assert (srtData);
    assert (cmpFunc);
    assert (nOfElements != 0);
    assert (sizeofElement != 0);

    size_t pivotIndex = (nOfElements - 1) / 2;

    size_t lIndex = 0;
    size_t rIndex = nOfElements - 1;

    size_t partitionResult = Partition(srtData, nOfElements, sizeofElement, cmpFunc);
    
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
        if (rIndex)
        {
            rIndex--;
        }
    }

    if (rIndex > 0)
    {
        QSort (srtData, rIndex + 1, sizeofElement, cmpFunc);
    }

    if (lIndex < nOfElements - 1 && lIndex)
    {
        QSort ((void*) ((char*) srtData + lIndex * sizeofElement ), nOfElements - lIndex, sizeofElement, cmpFunc);
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


int CompareUpInt (const void* compNum, const void* pivot)
{
    return *(const int*) compNum - *(const int*) pivot;
}

int CompareDownInt (const void* compNum, const void* pivot)
{
    return *(const int*) pivot - *(const int*) compNum;
}


int CompareUpFloat (const void* compNum, const void* pivot)
{
    double difference = *(const double*) compNum - *(const double*) pivot;

    if (difference > DoubleCmpAccuracy)
    {
        return 1;
    }
    if (difference < DoubleCmpAccuracy)
    {
        return -1;
    }
    return 0;
}


int CompareDownFloat (const void* compNum, const void* pivot)
{
    double difference = *(const double*) compNum - *(const double*) pivot;

    if (difference < DoubleCmpAccuracy)
    {
        return 1;
    }
    if (difference > DoubleCmpAccuracy)
    {
        return -1;
    }
    return 0;
}


void PrintData (int* data, size_t lIndex, size_t rIndex, size_t pivotIndex, PrintReason reason)
{
    printf ("Now data is: \n" GREEN);

    for (size_t i = 0; i < dataSize; i++)
    {
        if (i == lIndex + 1)
        {
            printf (RESET);
        }

        if (i == rIndex)
        {
            printf (RED);
        }

        printf ("%d ", data [i]);
    }

    printf(RESET "\nleft = %zu, right = %zu, pivot = %zu\n", lIndex, rIndex, pivotIndex);

    switch (reason)
    {
        case GoLeft:
            printf ("Left pointer moved right\n");
            break;
        case GoRight: 
            printf ("Right pointer moved left\n");
            break;
        case Rewrite:
            printf("Value was swapped\n");
            break;
        default:
            break;
    }
    getchar ();
}