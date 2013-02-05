#include <bit2.h>
#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <seq.h>
#include <set.h>
#include <atom.h>

#define T Bit2_T

typedef struct{
        int x, y;
}coordinates;

FILE *get_file(int, char**);
void pbmwrite(FILE *, Bit2_T);
void buildEdgeSet(Bit2_T, Seq_T, Set_T);
void identifyBlackEdgePixel(Bit2_T, Seq_T, Set_T, int x, int y);
void processEdgePixels(Bit2_T, Seq_T, Set_T);
void addEdgeNeighbors(Bit2_T, Seq_T, Set_T, coordinates);
const char *createAtom(int, int);
//void enque(Bit2_T, Seq_T, Set_T, coordinates);
//void Bit2_T pbmread(const char *);

int main(int argc, char* argv[])
{
        FILE *fp = get_file(argc, argv);

        Seq_T edgePixels = Seq_new(0);
        Set_T IDset = Set_new(0, NULL, NULL);

        Bit2_T bitmap = NULL;
        pbmwrite(fp, bitmap);
        buildEdgeSet(bitmap, edgePixels, IDset);
       
        processEdgePixels(bitmap, edgePixels, IDset);
        return 0;
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
                        fprintf(stderr, "%s: Could not open file %s for reading", argv[0], argv[1]);
                        exit(1);
                }
        }
        else{
                fprintf(stderr, "Too many arguments provided in addition to %s", argv[1]);
                exit(1);
        }
        return fp;    
}

void pbmwrite(FILE *fp, Bit2_T bitmap)
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
}

void buildEdgeSet(Bit2_T bitmap, Seq_T edgePixels, Set_T IDset)
{
        for(int i = 0; i < Bit2_height(bitmap); i++){
                identifyBlackEdgePixel(bitmap, edgePixels, IDset, 0, i);
                identifyBlackEdgePixel(bitmap, edgePixels, IDset, Bit2_width(bitmap), i);
        }
        
        for(int i = 0; i < Bit2_width(bitmap); i++){
                identifyBlackEdgePixel(bitmap, edgePixels, IDset, i, 0);
                identifyBlackEdgePixel(bitmap, edgePixels, IDset, i, Bit2_height(bitmap));
         }
}

void identifyBlackEdgePixel(Bit2_T bitmap, Seq_T blackPixels, Set_T IDset, int x, int y)
{
        int val = Bit2_get(bitmap, x, y);
        
        if(val == 1)
        {
                coordinates* currCords;
                currCords->x = x;
                currCords->y = y;
                Seq_addhi(blackPixels, currCords);                
                const char *xyAtom = createAtom(x, y);
                Set_put(IDset, xyAtom); 
        }
}

void processEdgePixels(Bit2_T bitmap, Seq_T edgePixels, Set_T IDset)
{
        while(Seq_length(edgePixels) != 0){

                coordinates currPixel = *(coordinates*)Seq_remlo(edgePixels);
                addEdgeNeighbors(bitmap, edgePixels, IDset, currPixel);
                Bit2_put(bitmap, currPixel.x, currPixel.y, 0);
        }
}

const char* createAtom(int x, int y)
{
        char xstring[40], ystring[40];
        sprintf(xstring, "%d", x);
        sprintf(ystring, "%d", y);
        int size = sizeof(xstring) + sizeof(ystring);
        const char* xyAtom = Atom_new(strcat(xstring, ystring), size);
        return xyAtom;
}

void addEdgeNeighbors(Bit2_T bitmap, Seq_T edgePixels, Set_T IDset, coordinates currPixel)
{
        coordinates neighborPixel;
        
        if(currPixel.x < Bit2_width(bitmap) - 1){
                neighborPixel.x = currPixel.x+1; 
                neighborPixel.y = currPixel.y;
        
                if(Bit2_get(bitmap, neighborPixel.x, neighborPixel.y)){
                        const char *testAtom = createAtom(neighborPixel.x, neighborPixel.y);
                        if(!Set_member(IDset, testAtom)){
                                Set_put(IDset, testAtom);
                                Seq_addlo(edgePixels, &neighborPixel);
                        }
                }
        }
           
        if(currPixel.x > 0){
                neighborPixel.x = currPixel.x-1; 
                neighborPixel.y = currPixel.y;
                
                if(Bit2_get(bitmap, neighborPixel.x, neighborPixel.y)){
                        const char *testAtom = createAtom(neighborPixel.x, neighborPixel.y);
                        if(!Set_member(IDset, testAtom)){
                                Set_put(IDset, testAtom);
                                Seq_addlo(edgePixels, &neighborPixel);
                        }
                }
        }
        
        if(currPixel.y < Bit2_height(bitmap) - 1){
                neighborPixel.x = currPixel.x; 
                neighborPixel.y = currPixel.y+1;
                
                if(Bit2_get(bitmap, neighborPixel.x, neighborPixel.y)){
                        const char *testAtom = createAtom(neighborPixel.x, neighborPixel.y);
                        if(!Set_member(IDset, testAtom)){
                                Set_put(IDset, testAtom);
                                Seq_addlo(edgePixels, &neighborPixel);
                        }
                }
        }
                       
        if(currPixel.y > 0){
                neighborPixel.x = currPixel.x; 
                neighborPixel.y = currPixel.y-1;
                
                //enque(bitmap, edgePixels, IDset, neighborPixel);
                if(Bit2_get(bitmap, neighborPixel.x, neighborPixel.y)){
                        const char *testAtom = createAtom(neighborPixel.x, neighborPixel.y);
                        if(!Set_member(IDset, testAtom)){
                                Set_put(IDset, testAtom);
                                Seq_addlo(edgePixels, &neighborPixel);
                        }
                }      
        }          
}

#if 0
void enque(Bit2_T bitmap, Seq_T edgePixels, Set_T IDset, coordinates Pixel)
{
        if(Bit2_get(bitmap, Pixel.x, Pixel.y)){
                const char *testAtom = createAtom(Pixel.x, Pixel.y);
                if(!Set_member(IDset, testAtom)){
                        Set_put(IDset, testAtom);
                        Seq_addlo(edgePixels, &Pixel);
                }
        }
}
#endif
#undef T
