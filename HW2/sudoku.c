#include <stdio.h>
#include <stdlib.h>
#include <uarray2.h>
#include <pnmrdr.h>
#include <assert.h>

static const int SUBSETSIZE = 9;

extern FILE* get_file(int, char**);
extern void readInput(UArray2_T, int, int, void *, void *);
extern int unique_Subset(UArray2_T, int *, int, int, int, int); 
extern int solution_Valid(UArray2_T);
extern UArray2_T pgmwrite(FILE *, UArray2_T);

int main (int argc, char* argv[])
{
        UArray2_T solution = NULL;
        FILE *fp = get_file(argc, argv);
        assert(fp);
        solution = pgmwrite(fp, solution);
        
        if(solution_Valid(solution)){
                printf("return 0 good\n");
                
                return 0;
        }

        else {
                printf("return 1 bad\n");
                return 1;
        }
}

FILE* get_file(int argc, char** argv)
{
        FILE* fp = NULL;
        if(argc == 1){
                fp = stdin;
        }
        else if(argc == 2){
                fp = fopen(argv[1], "r");
                if(fp == NULL){
                        fprintf(stderr,
                                "%s: %s %s %s\n",
                                argv[0], "Could not open file ",
                                argv[1], "for reading");
                        exit(1);
                }
        } else{
                fprintf(stderr, 
                "Error: too many files provided beyond %s\n", argv[1]);
                exit(1);
        }
        return fp;
}

UArray2_T pgmwrite(FILE* fp, UArray2_T solution)
{
        Pnmrdr_T bitmapRdr;
        Pnmrdr_T *bitmapPtr;
        bitmapRdr = Pnmrdr_new(fp);

        bitmapPtr = &bitmapRdr;
        Pnmrdr_mapdata bitmapData = Pnmrdr_data(bitmapRdr);
        //test denominator and if greater than 9x9
        if(bitmapData.denominator > 9)
                exit(1);
        if(bitmapData.height > 9 || bitmapData.width > 9)
                exit(1);
        
        solution = Uarray2_new(bitmapData.width, bitmapData.height, sizeof(int));
        Uarray2_map_row_major(solution, readInput, bitmapRdr);
        Pnmrdr_free(bitmapPtr);
        return solution;
}

void readInput(UArray2_T solution, int i, int j, void *pos, void *cl)
{
        (void) i;
        (void) j;
        (void) solution; 
        Pnmrdr_T bitmapRdr = cl;
        unsigned int val = Pnmrdr_get(bitmapRdr);
        unsigned int* p = pos;
        *p = val;
}

int solution_Valid(UArray2_T solution)
{
        int valid = 1; //Used as a bool
        int *subset = calloc(SUBSETSIZE, sizeof(int));
        
        int width = Uarray2_width(solution);
        int height = Uarray2_height(solution);
        
        //check vertical columns
        int lowRow = 0;
        int highRow = 1;
        int lowCol = 0;
        int highCol = width;

        while(valid && highRow <= height){
                valid = unique_Subset(solution, subset, lowRow, highRow, lowCol, highCol);
                for(int i = 0; i < SUBSETSIZE; i++)
                        memset(&subset[i], 0, sizeof(int));                
                lowRow++;
                highRow++;
        }
        //check horizontal rows
        lowRow = 0;
        highRow = height;
        lowCol = 0;
        highCol = 1;

        while(valid && highCol <= width){
                valid = unique_Subset(solution, subset, lowRow, highRow, lowCol, highCol);
                for(int i = 0; i < SUBSETSIZE; i++)
                        memset(&subset[i], 0, sizeof(int));  
                        
                lowCol++;
                highCol++;  
        }
        //check quadrants     
        lowCol = 0;
        highCol = 3;

        while(valid && highCol <= width){
                lowRow = 0;
                highRow = 3; 
                while(valid && highRow <= height){
                        valid = unique_Subset(solution, subset, lowRow, highRow, lowCol, highCol);
                        for(int i = 0; i < SUBSETSIZE; i++)
                                memset(&subset[i], 0, sizeof(int));
                        lowRow = lowRow+3;
                        highRow = highRow+3;
                }

                lowCol = lowCol+3;
                highCol = highCol+3;
        }
        free(subset);
        Uarray2_free(solution);
        
        return valid;
}

int unique_Subset(UArray2_T source, int *subset, int lowRow, int hiRow, int lowCol, int hiCol)
{
        for(int currRow = lowRow; currRow < hiRow; currRow++){
                for(int currCol = lowCol; currCol < hiCol; currCol++){
                        int val = *((int*)Uarray2_at(source, currRow, currCol)); //beware
                        printf("value of val: %d \n", val);
                        if (subset[val - 1] > 0)
                                return 0;
                        
                        else subset[val - 1]++;
                }
        }
        return 1;
}
