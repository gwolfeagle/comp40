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
void store_name(char*, char*, T*, L*);
void print_fgroups(T*);

int main(int argc, char* argv[])
{
        FILE* fp = process_args(argc, argv);
        T table = Table_new(1000, NULL, NULL);
        read_fgroups(fp, &table);
        print_fgroups(&table);
}

FILE* process_args(int argc, char** argv)
{
        //Assignment states that input will be only from stdin
        if (argc > 1) {
                fprintf(stderr, "Could not read from %s. Exiting.\n", argv[0]);
                exit(1);
        }
        return stdin;
        
}

void read_fgroups(FILE* fp, T* table)
{
        int reading_fprint = 1;
        int hit_ws = 0;
        int skip_read = 0;
        
        char fprint_buf[513];
        char name_buf[513];
        L name_list = List_list(NULL);
        
        int c = 0, i = 0;
        do {
                if (!skip_read)
                        c = getc(fp);
                else
                        skip_read = 0;
                
                if (c == EOF) continue;
                else if (reading_fprint) {
                        if (isspace(c)) {
                                if (i == 0) {
                                        fprintf(stderr, "Input is badly formed. Discarding line\n");
                                        while (c != EOF && c != '\n') {
                                                c = getc(fp);
                                        }
                                        continue;
                                }
                                fprint_buf[i] = 0;
                                hit_ws = 1;
                                i = 0;
                                reading_fprint = 0;
                        } else {
                                if(i >= 512){
                                        fprintf(stderr, "Fingerprint exceeds max size, truncating fingerprint\n");
                                        while (c != EOF && !isspace(c)) {
                                                c = getc(fp);
                                        }
                                        skip_read = 1;
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

                                if (i == 0) continue;
                                store_name(fprint_buf, name_buf, table, &name_list);
                                i = 0;
                        } else {
                                if(i >= 512){
                                        fprintf(stderr, "Name exceeds max size, truncating name.\n");
                                        while (c != EOF && c != '\n'){
                                                c = getc(fp);
                                        }
                                        skip_read = 1;
                                } else {
                                        //Add check
                                        hit_ws = 0;
                                        name_buf[i] = (char) c;
                                        i++;
                                }
                        }
                }
        } while (c != EOF);
        List_free(&name_list);
}

void store_name(char* fprint_buffer, char* name_buf, T* table, L* name_list)
{
        const char* fprint = Atom_string(fprint_buffer);
        const char* name = Atom_string(name_buf);
        
        char **name_arr = (char**) List_toArray(*name_list, NULL);
        for(int z = 0; name_arr[z]; z++){
                if(name_arr[z] == name){ 
                        fprintf(stderr, "Duplicate name detected: Ignoring input\n");
                        free (name_arr);
                        return;
                }
        }
        free(name_arr);
        *name_list = List_push(*name_list, (void*) name);        
	L list = (L) Table_get(*table, fprint);
	if (list == NULL) {
		list = List_list((char*) name, NULL);
	} else {
		list = List_push(list, (void*) name);
                
	}
	Table_put(*table, fprint, list);
}

void print_fgroups(T* table)
{
        char ** arr = (char **)Table_toArray(*table, NULL);
        int table_length = Table_length(*table) * 2;
        for (int i=0; i < table_length; i+=2){
                L list_ptr = ((L)arr[i+1]);
                int list_size = List_length(list_ptr);
                if (list_size >= 2){
                        char **names = (char**) List_toArray(list_ptr, NULL);
                        for (int j = 0; names[j]; j++)
                                printf("%s\n", (char*)(names[j]));
                        if (2*i+2 < table_length)
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
