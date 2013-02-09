#include <bit2.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <seq.h>
#include <atom.h>

#define T Bit2_T

typedef struct{
        int x, y;
}coordinates;

FILE *get_file(int, char**);
Bit2_T pbm_to_bit2(FILE *);
void buildEdgeSet(Bit2_T, Seq_T);
void identifyBlackEdgePixel(Bit2_T, Seq_T, int x, int y);
void processEdgePixels(Bit2_T, Seq_T);
void addEdgeNeighbors(Bit2_T, Seq_T, coordinates*);
void printBitmap(Bit2_T, int, int, int, void *);

int main(int argc, char* argv[])
{
        FILE *fp = get_file(argc, argv);
        Seq_T edgePixels = Seq_new(0);

        Bit2_T bitmap = NULL;
        bitmap = pbm_to_bit2(fp);
        buildEdgeSet(bitmap, edgePixels);
       
        processEdgePixels(bitmap, edgePixels);
        Seq_free(&edgePixels);
        fprintf(stdout, "P1 %d %d ", Bit2_width(bitmap), Bit2_height(bitmap));
        Bit2_map_row_major(bitmap, printBitmap, NULL);
        Bit2_free(bitmap);

}

FILE *get_file(int argc, char** argv)
{
        FILE *fp = NULL;
        if(argc == 1){
                fp = stdin;
        }
        else if(argc == 2){
                fp = fopen(argv[1], "r");
                if(fp == NULL){
                        fprintf(stderr, 
                                "%s: Could not open file %s for reading",
                                argv[0], argv[1]);
                        exit(1);
                }
        }
        else{
                fprintf(stderr, 
                "Too many arguments provided in addition to %s", argv[1]);
                exit(1);
        }
        return fp;    
}

Bit2_T pbm_to_bit2(FILE *fp)
{
        Pnmrdr_T bitmapRdr = Pnmrdr_new(fp); //Makes Pnm reader; Loads file
        Pnmrdr_mapdata bitmapData = Pnmrdr_data(bitmapRdr);
        Bit2_T bitmap = Bit2_new(bitmapData.width, bitmapData.height);
        unsigned int val = 0;
                
        for(unsigned int i = 0; i < bitmapData.height; i++){
                for(unsigned int j = 0; j < bitmapData.width; j++){ 
                        val = Pnmrdr_get(bitmapRdr);
                        Bit2_put(bitmap, j, i, val);
                }
        }
        Pnmrdr_free(&bitmapRdr);
        return bitmap;
}

void printBitmap(Bit2_T bitmap, int x, int y, int bitval, void * cl)
{
        (void) bitmap; (void) x; (void) y; (void) cl;
        fprintf(stdout, "%d", bitval);
} 

void buildEdgeSet(Bit2_T bitmap, Seq_T edgePixels)
{
        //Iterates down the two edge columns
        for(int i = 0; i < Bit2_height(bitmap); i++){
                int index = i;
                identifyBlackEdgePixel(bitmap, edgePixels, 0, index);
                identifyBlackEdgePixel(bitmap, edgePixels, 
                                       Bit2_width(bitmap) - 1, index);
        }
        //Iterates across the two edge rows
        for(int i = 1; i < Bit2_width(bitmap) - 1; i++){
                int index = i;
                identifyBlackEdgePixel(bitmap, edgePixels, index, 0);
                identifyBlackEdgePixel(bitmap, edgePixels, index, 
                                       Bit2_height(bitmap) - 1);
        }
}

void identifyBlackEdgePixel(Bit2_T bitmap, Seq_T blackPixels, int x, int y)
{
        int val = Bit2_get(bitmap, x, y);
        
        if(val == 1)
        {
                coordinates* currCords = malloc(sizeof(coordinates));
                currCords->x = x;
                currCords->y = y;
                Seq_addhi(blackPixels, currCords); 
        }
}

void processEdgePixels(Bit2_T bitmap, Seq_T edgePixels)
{
        while(Seq_length(edgePixels) > 0){

                coordinates* currPixel = Seq_remlo(edgePixels);
                //Checks if pixel is already white to avoid duplicates
                if(Bit2_get(bitmap, currPixel->x, currPixel->y) == 0){
                        free(currPixel);
                        continue;
                }
                addEdgeNeighbors(bitmap, edgePixels, currPixel);
                Bit2_put(bitmap, currPixel->x, currPixel->y, 0);
                free(currPixel);
        }
}

void addEdgeNeighbors(Bit2_T bitmap, Seq_T edgePixels, coordinates* currPixel)
{
        if(currPixel->x < Bit2_width(bitmap) - 1){ //Checks right
                if(Bit2_get(bitmap, currPixel->x + 1, currPixel->y)){
                        coordinates* neighborPixel = 
                                malloc(sizeof(*neighborPixel));
                        neighborPixel->x = currPixel->x + 1; 
                        neighborPixel->y = currPixel->y;
                        Seq_addlo(edgePixels, neighborPixel);
                }
        }
           
        if(currPixel->x > 0){ //Check left
                if(Bit2_get(bitmap, currPixel->x - 1, currPixel->y)){
                        coordinates* neighborPixel = 
                                malloc(sizeof(*neighborPixel));
                        neighborPixel->x = currPixel->x - 1; 
                        neighborPixel->y = currPixel->y;
                        Seq_addlo(edgePixels, neighborPixel);
                }
        }
        
        if(currPixel->y < Bit2_height(bitmap) - 1){ //Checks up
                if(Bit2_get(bitmap, currPixel->x, currPixel->y + 1)){
                        coordinates* neighborPixel = 
                                malloc(sizeof(*neighborPixel));
                        neighborPixel->x = currPixel->x; 
                        neighborPixel->y = currPixel->y + 1;
                        Seq_addlo(edgePixels, neighborPixel);
                }
        }
                       
        if(currPixel->y > 0){ //Checks down
                if(Bit2_get(bitmap, currPixel->x, currPixel->y - 1)){
                        coordinates* neighborPixel = 
                                malloc(sizeof(*neighborPixel));
                        neighborPixel->x = currPixel->x; 
                        neighborPixel->y = currPixel->y-1;
                        Seq_addlo(edgePixels, neighborPixel); 
                }               
        }          
}

#undef T
