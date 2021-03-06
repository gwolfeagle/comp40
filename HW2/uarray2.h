#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED
#include <uarray.h>

#define T UArray2_T
typedef struct T *T;

//Description: Creates and returns a Uarray_2
extern T Uarray2_new(int width, int height, int size); //creator

//Description: Observers return given values
extern int Uarray2_width(T arr); //observer
extern int Uarray2_height(T arr); //observer
extern int Uarray2_size(T arr); //observer
//Description: Returns a pointer to given index at given coordinates
extern void *Uarray2_at(T arr, int x, int y); //observer

//Description: Call the apply function on all indexes in the structure
extern void Uarray2_map_col_major(T arr, 
                void (*apply)(T arr, int i, int j,void *value, void *cl),
                        void *cl); //morpher
extern void Uarray2_map_row_major(T arr,
                void (*apply)(T arr, int i, int j, void *value, void *cl),
                        void *cl); //morpher
                        
//Description: Frees allocated memory
extern void Uarray2_free(T arr); //destroyer

#undef T
#endif
