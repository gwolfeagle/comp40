#include <uarray2.h>
#include <stdio.h>
#include <stdlib.h>

#define T UArray2_T

struct T
{
        int width, height, size;
        UArray_T* uarrs;
};


T Uarray2_new(int width, int height, int size){
	
	T uarr2 = malloc(sizeof(struct T)); 
			
		uarr2->width = width;
		uarr2->height = height;
		uarr2->size = size;
                
	if(width > 0){	
		uarr2->uarrs = calloc(width, sizeof(UArray_T));
		
		for(int i = 0; i <  width; i++){
			uarr2->uarrs[i] = UArray_new(height, size);
		}
	}

	else uarr2->uarrs = NULL;
        
        return uarr2;	       
}

int Uarray2_width(T arr){
        return arr->width;    
}

int Uarray2_height(T arr){
        return arr->height;
}

int Uarray2_size(T arr){
        return arr->size;    
}

void *Uarray2_at(T arr, int xcor, int ycor){
        return UArray_at((arr->uarrs[xcor]), ycor);
}

void Uarray2_map_row_major(T arr, void (*apply)(T arr, int i, 
                                int j, void *value, void *cl), void *cl)
{
        for(int i = 0; i < arr->height; i++){
                for(int j = 0; j < arr->width; j++){
                        void *p = Uarray2_at(arr, j, i);
                        apply(arr, j, i, p, cl);
                }
        }
}

void Uarray2_map_col_major(T arr, void (*apply)(T arr, int i, 
                                int j, void *value, void *cl), void *cl)
{

        for(int i = 0; i < arr->width; i++){
                for(int j = 0; j < arr->height; j++){
                        void *p = Uarray2_at(arr, i, j);
                        apply(arr, i, j, p, cl);
                }
        }
}

void Uarray2_free(T arr){
        for(int i = 0; i < arr->width; i++)
                UArray_free(&(arr->uarrs[i]));
                
        free(arr);
}
