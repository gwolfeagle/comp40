#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>

#define T Pnmrdr_T
#define T_mapdata Pnmrdr_mapdata

void average_brightness(FILE *);
FILE* get_file_handle(int, char**);

int main(int argc, char *argv[])
{
    FILE * fp = get_file_handle(argc, argv);
    average_brightness(fp);
        fclose(fp);
}

void average_brightness(FILE * fp)
{
        TRY
            T rdr = Pnmrdr_new (fp);
            T_mapdata map = Pnmrdr_data(rdr);
    
            int area = map.width * map.height;
            int max = map.denominator;
            int total_brightness = 0;
        
            for (int i=0; i<area; i++) {
                total_brightness += Pnmrdr_get(rdr);
            }
            double avg_brightness =
        ((double) total_brightness)/((double) (area*max));
            int truncated = (int) (avg_brightness * 1000);
            
            printf("%g\n", ((float) truncated/1000.0));
               Pnmrdr_free(&rdr);
        EXCEPT (Pnmrdr_Badformat)
                fprintf(stderr, "Input file type is not supported by Pnmrdr\n");
        END_TRY;
}

FILE * get_file_handle(int argc, char *argv[])
{
    FILE *fp = NULL;
    if(argc == 1){
        fp = stdin;
    } else if(argc == 2) {    
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: %s %s %s\n", argv[0],
"Could not open file ", argv[1], "for reading");
            exit(1);
        }
    } else {
                fprintf(stderr, "More than one argument supplied. Terminating program.\n");
                exit(1);
        }
    return fp;
}

#undef T
#undef T_mapdata
