#include "../common.h"

#define GLOBAL_LENGTH 256

/*  */
value global_queue[GLOBAL_LENGTH];

void assign_global(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  if(p->type->type_id == TYPE_ARRAY)
  {
    value tmp;

    baseOffset = p->offset;

    int eleOffset = get_symbol_align_size(p->type->last);

    int i;
    for(i = 0; i < strlen(n->val.s); i++)
    {
      if(i > p->type->num_ele - 1)
      {
        parse_error("assign_global array out of index", p->name);
        return;
      }

      tmp.c = n->val.s[i];
      assign_with_byte_unit(TYPE_CHAR, &global_queue[baseOffset + i * eleOffset], &tmp);
    }
  }
  else
  {
    if(q != NULL)
    {
      baseOffset = q->offset;
    }
    
    assign_with_byte_unit(p->type->type_id, &global_queue[baseOffset + p->offset], &(n->val)); 
  }
  
}

void load_global(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  if(q != NULL)
  {
    baseOffset = q->offset;
  }
  
  load_with_byte_unit(p->type->type_id, &global_queue[baseOffset + p->offset], &(n->val));
}