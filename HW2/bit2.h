#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED
#include <bit.h>

#define T Bit2_T
typedef struct T *T;

extern T Bit2_new(int width, int height); //creator

extern int Bit2_width(T bit2); //observer
extern int Bit2_height(T bit2); //observer
extern int Bit2_size(T bit2); //observer
extern int Bit2_get(T bit2, int x, int y); //observer

extern int Bit2_put(T bit2, int x, int y, int bit); //mutator
extern void Bit2_map_col_major(T bit2, 
                void (*apply)(T bit2, int i, int j, int bitval, void *cl), 
                        void *cl); //mutator
extern void Bit2_map_row_major(T bit2,
                void (*apply)(T bit2, int i, int j, int bitval, void *cl),
                        void *cl); //mutator

extern void Bit2_free(T bit2); //destroyer

#undef T
#endif
