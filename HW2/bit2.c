#include <bit2.h>
#include <stdio.h>
#include <stdlib.h>

#define T Bit2_T

struct T
{
        int width, height;
        Bit_T *bitVects;
};

T Bit2_new(int width, int height)
{
	T bit2 = malloc(sizeof(struct T)); 
        bit2->width = width;
        bit2->height = height;
		
        if(width > 0){
		bit2->bitVects = calloc(width, sizeof(Bit_T));
		for(int i = 0; i <  width; i++)
			bit2->bitVects[i] = Bit_new(height);
	}
	else bit2->bitVects = NULL;
        
        return bit2;	       
}

int Bit2_width(T bit2)
{
        return bit2->width;    
}

int Bit2_height(T bit2){
        return bit2->height;
}

int Bit2_get(T bit2, int xcor, int bitNum)
{
        return Bit_get(bit2->bitVects[xcor], bitNum);
}

int Bit2_put(T bit2, int xcor, int ycor, int value)
{
        Bit_put(bit2->bitVects[xcor], ycor, value);
        return value;
}

void Bit2_map_row_major(T bit2, void (*apply)(T bit2, int i, int j, int bitval, void*cl), void *cl)
{
        for(int i = 0; i < bit2->height; i++){
                for(int j =0; j < bit2->width; j++){
                        int bitval = Bit2_get(bit2, j, i);
                        apply(bit2, j, i, bitval, cl);      
                }
        }
}

void Bit2_map_col_major(T bit2, void (*apply)(T bit2, int i, int j, int bitval, void*cl), void *cl)
{
        for(int i = 0; i < bit2->width; i++){
                for(int j = 0; j < bit2->height; j++){
                        int bitval = Bit2_get(bit2, i, j);
                        apply(bit2, i, j, bitval, cl);
                }
        } 
}

void Bit2_free(T bit2)
{
        for(int i = 0; i < bit2->width; i++){
                Bit_T * bitVectsPtr = &(bit2->bitVects[i]);
                Bit_free(bitVectsPtr);
        }
        free(bit2);
}

#undef T
