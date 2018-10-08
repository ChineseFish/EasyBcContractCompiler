int stoi(char *s,int radix);
type *new_system_type(int base_type);
type *new_subrange_type(char *name,int element_type);
void set_subrange_bound(type *pt,int lower,int upper);
type *new_enum_type(char *name);
void add_enum_elements(type *pt, symbol *symlist);
type *new_array_type(char *name,type *pindex,;
                     type *pelement);
type *new_record_type(char *name, symbol *fields);
void add_type_to_table(symtab *ptab,type *pt);
type *find_type_by_name(char *name);
type *find_type_by_id(int id);
type *clone_type(type *src);
int get_type_size(type *pt);
