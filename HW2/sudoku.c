#include <stdio.h>
#include <stdlib.h>
#include <uarray2.h>
#include <pnmrdr.h>

static const int SUBSETSIZE = 9;

extern void readInput(UArray2_T, int, int, void *, void *);
extern int unique_Subset(UArray2_T, int *, int, int, int, int); 
extern int solution_Valid(UArray2_T);

int main (int argc, char* argv[])
{
         printf("Starting program\n");
        UArray2_T solution;
        Pnmrdr_T bitmapRdr;
        Pnmrdr_T *bitmapPtr;
        FILE *fp = NULL;

        if(argc == 1){
                fp = stdin;
                printf("One arg provided\n");
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
                 printf("Two args provided\n");
        } else{
                fprintf(stderr, "Error: too many files provided beyond %s\n", argv[1]);
                exit(1);
        }
                
                bitmapRdr = Pnmrdr_new(fp);
                /*if(Pnmrdr_Badformat){
                        fprintf(stderr, "Error: Bad format\n");
                        exit(1);
                }*/
                bitmapPtr = &bitmapRdr;
                Pnmrdr_mapdata bitmapData = Pnmrdr_data(bitmapRdr);
                printf("Checking bitmap data\n");
                //test denominator and if greater than 9x9
                if(bitmapData.denominator > 9)
                        exit(1);
                if(bitmapData.height > 9 || bitmapData.width > 9)
                        exit(1);
                
                printf("Assigning solution\n");
                solution = Uarray2_new(bitmapData.height, bitmapData.width, sizeof(int));
                Uarray2_map_row_major(solution, readInput, bitmapRdr);//(bitmapPtr, solution, NULL)
                printf("Uarray2 made and loaded\n");
                
        Pnmrdr_free(bitmapPtr);
        if(solution_Valid(solution)){
                printf("Solution good\n");
                return 0;
        }

        else {
                printf("solution did not work\n");
                return 1;
        }
}

void readInput(UArray2_T solution, int i, int j, void *pos, void *cl)
{
        (void) i;
        (void) j;
        (void) solution; //See if there's a more efficient way
        Pnmrdr_T bitmapRdr = cl;
        unsigned int val = Pnmrdr_get(bitmapRdr);
        unsigned int* p = pos;
        *p = val;
}


int solution_Valid(UArray2_T solution)
{
        printf("Entering solution finder\n");
        int valid = 1; //Used as a bool
        int *subset = calloc(SUBSETSIZE, sizeof(int));
        
        int width = Uarray2_width(solution);
        int height = Uarray2_height(solution);
        
        //check vertical columns
        int lowRow = 0;
        int highRow = 1;
        int lowCol = 0;
        int highCol = width;

        printf("Checking vertical columns\n");
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

        printf("Checking horizontal rows\n");
        while(valid && highCol <= width){ //Valid gets set to 0 when highCol is 8 or 9
                valid = unique_Subset(solution, subset, lowRow, highRow, lowCol, highCol);
                for(int i = 0; i < SUBSETSIZE; i++)
                        memset(&subset[i], 0, sizeof(int));  
                        
                lowCol++;
                highCol++;  
        }
         
        //check quadrants     
        lowCol = 0;
        highCol = 4;

        printf("Checking quadrants\n");
        while(valid && highCol <= width){
                lowRow = 0;
                highRow = 4; 
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
        printf("returning unique now\n");
        return 1;
}