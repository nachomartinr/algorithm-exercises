#include <unistd.h> /* getopt */
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>
#include <limits.h>
#include <errno.h>

#define MAX_FILENAME_LEN 32
#define DEFAULT_DATA_SIZE 128

typedef enum {
    E_QUICKSORT_PIVOT_FIRST = 0,
    E_QUICKSORT_PIVOT_LAST = 1,
    E_QUICKSORT_PIVOT_MEDIAN = 2,
    E_QUICKSORT_PIVOT_RANDOM = 3
} EPivotOptions;

static void PrintArray(int Input[], int Left, int Right) {
    
    int i = Left;

    printf("[", Input[i]);
    
    if (Right > Left) {
        
        for (i = Left; i <= Right - 1; ++i) {
            printf("%d, ", Input[i]);
        }
    }
    
    printf("%d]", Input[i]);
}

static void Swap(int * A, int * B) {
    int Temp = *A;
    *A = *B;
    *B = Temp;
}

static int GetPivot(int Input[], int Left, int Right, EPivotOptions Options) {
    
    int LeftVal = Input[Left];
    int RightVal =  Input[Right];
    int CenterVal = Input[(Left + Right)/2];
    int pivotIdx = 0;
    switch(Options) {
        case E_QUICKSORT_PIVOT_FIRST:
            break;
        case E_QUICKSORT_PIVOT_LAST:
            Swap(&Input[Left], &Input[Right]);
            break;
        case E_QUICKSORT_PIVOT_MEDIAN:
            if (((RightVal > CenterVal) && (CenterVal > LeftVal)) || 
                ((LeftVal > CenterVal) && (CenterVal > RightVal))) {
                Swap(&Input[Left], &Input[(Left + Right)/2]);
            }
            else if (((LeftVal > RightVal) && (RightVal > CenterVal)) || 
                ((CenterVal > RightVal) && (RightVal > LeftVal))) {
                Swap(&Input[Left], &Input[Right]);
            }
            break;
        case E_QUICKSORT_PIVOT_RANDOM:            
            pivotIdx = Left + rand() % (Right - Left + 1);
            Swap(&Input[Left], &Input[pivotIdx]);
            break;
        default:
            break;
    }
    int Pivot = Input[Left];   
    return Pivot;
}

static int Partition(int Input[], int Left, int Right, EPivotOptions Options) {
    
    int i = Left + 1;
    int j = Left + 1;
    int Pivot = GetPivot(Input, Left, Right, Options);

    for (j = Left + 1; j <= Right; ++j){
        if (Input[j] < Pivot) {
            Swap(&Input[j], &Input[i]);
            ++i;
        }  
    }
    Swap(&Input[Left], &Input[i - 1]);
    return i - 1;
}



static uint64_t QuickSort(int Input[], int Left, int Right, EPivotOptions Options){
    
    int len = Right - Left + 1;
    uint64_t comparisons = 0;
    if (len > 1) { /* recurse while n > 1 */
        comparisons = len - 1;
        if (Options == E_QUICKSORT_PIVOT_MEDIAN) {
            /* keep track of the comparisons made in identifying the median */
            comparisons += 3; /* although we use 4 comparisons, the median of three can be optimized to use only 3 comparisons */
        }
        int PivotPos = Partition(Input, Left, Right, Options);

        comparisons += QuickSort(Input, Left, PivotPos - 1, Options);        
        comparisons += QuickSort(Input, PivotPos + 1, Right, Options);

   }

   return comparisons;
}

int main(int argc, char **argv) {

    char filename[MAX_FILENAME_LEN];
    int c;
    char * pvalue;
    int verbose = 0;
    EPivotOptions options = E_QUICKSORT_PIVOT_FIRST;

    while ((c = getopt (argc, argv, "vp:")) != -1) {
        if (c == 'p') {
            pvalue = optarg;
            if (strncmp(pvalue, "fisrt", 6) == 0) {
                options = E_QUICKSORT_PIVOT_FIRST;    
            }
            else if (strncmp(pvalue, "last", 5) == 0) {
                options = E_QUICKSORT_PIVOT_LAST;    
            }
            else if (strncmp(pvalue, "median", 7) == 0) {
                options = E_QUICKSORT_PIVOT_MEDIAN;    
            }
            else if (strncmp(pvalue, "random", 7) == 0) {
                options = E_QUICKSORT_PIVOT_RANDOM;    
            }
            else {
                printf("Unknown option %s.\n", pvalue);    
            }
        }
        if (c == 'v') {
            verbose = 1;
        }
    }

    if (optind < argc) {
        strncpy(filename, argv[optind], MAX_FILENAME_LEN);
        filename[MAX_FILENAME_LEN - 1] = '\0';

        FILE * fd = fopen(filename, "r");
        if (verbose) {
            printf("File opened read: %s\n", filename);
        }
        
        if (fd != NULL) {
            int err = 0;
            ssize_t read;
            size_t len = 0;
            char * line = NULL;
            int i = 0;
            int * data = malloc(DEFAULT_DATA_SIZE * sizeof(int));

            if (data != NULL) {
                int data_size = DEFAULT_DATA_SIZE * sizeof(int);
                if (verbose) {
                    printf("Unordered:");
                }
                while ((read = getline(&line, &len, fd)) != -1) {
                    char * temp;                    
                    long val = strtol(line, &temp, 0);                     

                    if ((temp == line) ||
                        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
                        continue;
                    }
                    else {
                        data[i] = val;
                    }

                    if (verbose) {
                        printf("\n%d", data[i]);
                    }
                    
                    ++i;

                    if ((i * sizeof(int)) >= data_size) {
                        int new_data_size = 2 * data_size; 
                        void * new_data_ptr = realloc(data, new_data_size);
                        if (new_data_ptr != NULL ) {
                            data = new_data_ptr;
                            data_size = new_data_size;  
                            
                        }
                        else {
                            fprintf(stderr, "\n\nNot enough memory\n");
                            err = 1;
                            break;
                        }
                    }
                }
                if (verbose) {
                    printf("\n\nMemory allocated: %d bytes\n\n", data_size);
                }
                free(line);

                if ((err == 0 && i > 0)) { 
                    srand(time(NULL));                                       
                    uint64_t comparisons = QuickSort(data, 0, i - 1, options);
                    if (verbose) { 
                        printf("Ordered:\n");
                    }
                    for (int j = 0; j < i; ++j) {
                        printf("%d", data[j]);
                        if (j != (i - 1)) {
                            printf("\n");
                        }
                    }

                    if (verbose) {                   
                        printf("\n\nComparisons: %llu", comparisons);
                    }
                }
                
                free(data);
            }
            else {
                fprintf(stderr, "\n\nNot enough memory\n");
                err = 1;
            }

            fclose(fd);
            return err;
        }
        else {
            fprintf(stderr, "Failed to open %s\n", filename);
            return 1;
        }
    }
    else {

        fprintf(stderr, "File name not defined.\n");
        /* PrintUsage(); */
        return 1;
    }
}