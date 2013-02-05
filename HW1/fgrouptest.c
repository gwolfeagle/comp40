#include <stdio.h>
#include <stdlib.h>
#include <atom.h>
#include <list.h>
#include <table.h>
#include <ctype.h>
#include <string.h>


#define T Table_T
#define L List_T

FILE* process_args(int, char**);
void read_fgroups(FILE*, T*);
void store_name(char*, const char*, T*);
void print_fgroups(T*);

int main(int argc, char* argv[])
{
        FILE* fp = process_args(argc, argv);
        //printf("Program started \n");
        T table = Table_new(1000, NULL, NULL);
        read_fgroups(fp, &table);
        print_fgroups(&table);
}

FILE* process_args(int argc, char** argv)
{
        //Assignment states that input will be only from stdin
        if (argc > 1) {
                fprintf(stderr, "Could not read from %s. Exiting.", argv[0]);
                exit(1);
        }
        return stdin;
        
}

void read_fgroups(FILE* fp, T* table)
{
        printf("entering read_fgroups \n");
        int reading_fprint = 1;
        int hit_ws = 0;
        char fprint_buf[513];
        char name_buf[513];
        L name_list = List_list(NULL);
        
        int c, i = 0;
        do {
                c = getc(fp);
                printf("Just read in:%c\n", c);
                
                if (c == EOF) continue;
                else if (reading_fprint) {
                        if (isspace(c)) {
                                fprint_buf[i] = 0;
                                hit_ws = 1;
                                i = 0;
                                reading_fprint = 0;
                        } else {
                                if(i >= 512){
                                        fprintf(stderr, "Fingerprint exceeds max size, truncating fingerprint");
                                        while (!isspace(c)){
                                                c = getc(fp);
                                        }
                                } else {
                                fprint_buf[i] = (char) c;
                                i++;
                                }
                        }
                } else {
                        if (isspace(c) && hit_ws) {
                                continue;
                        }
                        if (c == '\n') {
                                name_buf[i] = 0;
                                
                                reading_fprint = 1;
                                hit_ws = 0;
                                /*
                                printf("Fingerprint: ");
                                for (int z=0; fprint_buf[z] && z<512; z++) {
                                        printf("%c", fprint_buf[z]);
                                }
                                printf(", Name: ");
                                for (int z=0; name_buf[z] && z<512; z++) {
                                        printf("%c", name_buf[z]);
                                }
                                printf("\n"); */
                                
                                const char* name = Atom_string(name_buf);
                                List_push(name_list, (void*) name);
                                printf("got here\n");
                                char **name_arr = (char**) List_toArray(name_list, NULL);
                                for(int z = 0; name_arr[z]; z++){
                                        printf("entering loop\n");
                                        if(strcmp(name_arr[z], name_buf) == 0){
                                                i = 0;
                                                printf("continuing because string compare is %d\n", strcmp(*name_arr, name_buf));
                                                continue;
                                        } else {
                                        store_name(fprint_buf, name, table);
                                        printf("storing name %s\n", name_buf);
                                        i = 0;
                                        }
                                }
                        } else {
                                //Add check
                                hit_ws = 0;
                                name_buf[i] = (char) c;
                                i++;
                        }
                }
        } while (c != EOF);
}

void store_name(char* fprint_buffer, const char* name, T* table)
{
        const char* fprint = Atom_string(fprint_buffer);
	printf("Just read fp: %s for: %s\n", fprint, name);
	L list = (L) Table_get(*table, fprint);
	if (list == NULL) {
		//printf("list did not exist, making list \n");
		list = List_list((char*) name, NULL);
		//char** charz = (char**) List_toArray(list, NULL);
		//printf("We just put %s in the new list at addr: %p\n", charz[0], (void*)list);
		//Table_put(*table, fprint, list);
	} else {
		//printf("list existed, adding to list at: %p\n", (void*)list);
		list = List_push(list, (void*) name);
                
	}
	Table_put(*table, fprint, list);
}

void print_fgroups(T* table)
{
        //printf("entering print_fgroups \n");
        char ** arr = (char **)Table_toArray(*table, NULL);
        int table_length = Table_length(*table) * 2;
        //printf("table length: %d \n", table_length);
        for (int i=0; i < table_length; i+=2){
                //char* fngr = arr[i];
                //printf("Printing fingerprint group for fp: %s\n", fngr);
                L list_ptr = ((L)arr[i+1]);
                int list_size = List_length(list_ptr);
                //printf("List (p:%p) %d size: %d\n", (void*)(list_ptr), i, list_size);
                if (list_size >= 1){
                        char **names = (char**) List_toArray(list_ptr, NULL);
                        for (int j = 0; names[j]; j++)
                                printf("%s\n", (char*)(names[j]));
                        printf("\n");
                        free(names);
                }
                List_free(&list_ptr);
        }
        free(arr);
        Table_free(table);
}

#undef L
#undef T
