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

extern FILE *get_file(int, char**);
extern Bit2_T pbmwrite(FILE *, Bit2_T);
extern void buildEdgeSet(Bit2_T, Seq_T);
extern void identifyBlackEdgePixel(Bit2_T, Seq_T, int x, int y);
extern void processEdgePixels(Bit2_T, Seq_T);
extern void addEdgeNeighbors(Bit2_T, Seq_T, coordinates*);
extern void printBitmap(Bit2_T, int, int, int, void *);

int main(int argc, char* argv[])
{
        FILE *fp = get_file(argc, argv);
        Seq_T edgePixels = Seq_new(0);

        Bit2_T bitmap = NULL;
        bitmap = pbmwrite(fp, bitmap);
        buildEdgeSet(bitmap, edgePixels);
       
        processEdgePixels(bitmap, edgePixels);
        Seq_free(&edgePixels);
        fprintf(stdout, "P1 %d %d ", Bit2_width(bitmap), Bit2_height(bitmap));
        Bit2_map_row_major(bitmap, printBitmap, NULL);
        Bit2_free(bitmap);// Bit2_width(bitmap));

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

Bit2_T pbmwrite(FILE *fp, Bit2_T bitmap)
{
        Pnmrdr_T bitmapRdr = Pnmrdr_new(fp);
        Pnmrdr_mapdata bitmapData = Pnmrdr_data(bitmapRdr);
        bitmap = Bit2_new(bitmapData.width, bitmapData.height);
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
        (void) bitmap;
        (void) x;
        (void) y;
        (void) cl;
        fprintf(stdout, "%d", bitval);
} 

void buildEdgeSet(Bit2_T bitmap, Seq_T edgePixels)
{
        for(int i = 0; i < Bit2_height(bitmap); i++){
                int index = i;
                identifyBlackEdgePixel(bitmap, edgePixels, 0, index);
                identifyBlackEdgePixel(bitmap, edgePixels, Bit2_width(bitmap) - 1, index);
        }
        for(int i = 1; i < Bit2_width(bitmap) - 1; i++){
                int index = i;
                identifyBlackEdgePixel(bitmap, edgePixels, index, 0);
                identifyBlackEdgePixel(bitmap, edgePixels, index, Bit2_height(bitmap) - 1);
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

                coordinates* currPixel = (coordinates*)Seq_remlo(edgePixels);
                addEdgeNeighbors(bitmap, edgePixels, currPixel);
                Bit2_put(bitmap, currPixel->x, currPixel->y, 0);
        }
}

void addEdgeNeighbors(Bit2_T bitmap, Seq_T edgePixels, coordinates* currPixel)
{
        if(Bit2_get(bitmap, currPixel->x, currPixel->y) == 0)
                return;
                
        coordinates* neighborPixel = malloc(sizeof(coordinates));
        
        if(currPixel->x < Bit2_width(bitmap) - 1){ //Checks right
                neighborPixel->x = currPixel->x + 1; 
                neighborPixel->y = currPixel->y;
        
                if(Bit2_get(bitmap, neighborPixel->x, neighborPixel->y))
                        Seq_addlo(edgePixels, neighborPixel);
        }
           
        if(currPixel->x > 0){ //Check left
                neighborPixel->x = currPixel->x - 1; 
                neighborPixel->y = currPixel->y;
                
                if(Bit2_get(bitmap, neighborPixel->x, neighborPixel->y))
                        Seq_addlo(edgePixels, neighborPixel);
        }
        
        if(currPixel->y < Bit2_height(bitmap) - 1){ //Checks up
                neighborPixel->x = currPixel->x; 
                neighborPixel->y = currPixel->y + 1;
                
                if(Bit2_get(bitmap, neighborPixel->x, neighborPixel->y))
                        Seq_addlo(edgePixels, neighborPixel);
        }
                       
        if(currPixel->y > 0){ //Checks down
                neighborPixel->x = currPixel->x; 
                neighborPixel->y = currPixel->y-1;
                
                if(Bit2_get(bitmap, neighborPixel->x, neighborPixel->y))
                        Seq_addlo(edgePixels, neighborPixel);                
        }          
}

#undef T
