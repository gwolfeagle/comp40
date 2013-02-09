#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED
#include <bit.h>

#define T Bit2_T
typedef struct T *T;

//Description: Creates and returns a Bit2_T
extern T Bit2_new(int width, int height); //creator

//Description: Observers return given values
extern int Bit2_width(T bit2); //observer
extern int Bit2_height(T bit2); //observer
extern int Bit2_size(T bit2); //observer
//Description: Returns an int at a given coordinate in a Bit_2
extern int Bit2_get(T bit2, int x, int y); //observer

//Description: Puts a given bit into given coordinates
extern int Bit2_put(T bit2, int x, int y, int bit); //mutator
//Description: Call the apply function on all bits in the structure
extern void Bit2_map_col_major(T bit2, 
                void (*apply)(T bit2, int i, int j, int bitval, void *cl), 
                        void *cl); //mutator
extern void Bit2_map_row_major(T bit2,
                void (*apply)(T bit2, int i, int j, int bitval, void *cl),
                        void *cl); //mutator

//Description: Frees allocated memory
extern void Bit2_free(T bit2); //destroyer

#undef T
#endif
