#include "../common.h"

#define STACK_DEEP 256

int return_position_deep = STACK_DEEP;
List return_position_stack[STACK_DEEP];

int push_return_position_stack(List l)
{
  if(return_position_deep < 1)
  {
    return 0;
  }
  return_position_stack[--return_position_deep] = l;
  return 1;
}

List pop_return_position_stack()
{
  return return_position_stack[return_position_deep++];
}

List top_return_position_stack()
{
  return return_position_stack[return_position_deep];
}

/*  */
int return_val_deep = STACK_DEEP;
unsigned char return_val_stack[STACK_DEEP];

int push_return_val_stack(Symbol p)
{
  if(return_val_deep < get_symbol_align_size(p))
  {
    return 0;
  }
  return_val_deep -= get_symbol_align_size(p);
  return 1;
}

void pop_return_val_stack(Symbol p)
{
  return_val_deep += get_symbol_align_size(p);
}

void assign_return_val(Node n, Symbol p)
{
  assign_with_byte_unit(p->type->type_id, &return_val_stack[return_val_deep + p->offset], &(n->val));
}


void load_return_val(Node n, Symbol p)
{
  load_with_byte_unit(p->type->type_id, &return_val_stack[return_val_deep + p->offset], &(n->val));
}

/*  */
int local_deep = STACK_DEEP;
unsigned char local_stack[STACK_DEEP];

int push_local_stack(Symtab tab)
{
  if(local_deep < tab->local_size)
  {
    return 0;
  }
  local_deep -= tab->local_size;
  return 1;
}

void pop_local_stack(Symtab tab)
{
  local_deep += tab->local_size;
}

void assign_local(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  value tmp;

  if(p->type->type_id == TYPE_ARRAY)
  {
    baseOffset = p->offset;

    int eleOffset = get_symbol_align_size(p->type_link->last);

    int i;
    for(i = 0; i < strlen(n->val.s); i++)
    {
      if(i > p->type_link->num_ele - 1)
      {
        parse_error("assign_local array out of index", p->name);
        return;
      }

      tmp.c = n->val.s[i];
      assign_with_byte_unit(TYPE_CHAR, &local_stack[local_deep + baseOffset + i * eleOffset], &tmp); 
    }
  }
  else
  {
    if(q != NULL)
    {
      baseOffset = q->offset;
    }

    assign_with_byte_unit(p->type->type_id, &local_stack[local_deep + baseOffset + p->offset], &(n->val)); 
    // printf("assign_local %s offset:%d val:%d\n", p->name, local_deep + baseOffset + p->offset, local_stack[local_deep + baseOffset + p->offset].i);
  }
  
}

void load_local(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  if(q != NULL)
  {
    baseOffset = q->offset;
  }

  // printf("load_local %s offset:%d val:%d\n", p->name, local_deep + baseOffset + p->offset, local_stack[local_deep + baseOffset + p->offset].i);
  load_with_byte_unit(p->type->type_id, &local_stack[local_deep + baseOffset + p->offset], &(n->val));
}


/*  */
int args_deep = STACK_DEEP;
unsigned char args_stack[STACK_DEEP];

int push_args_stack(Symtab tab)
{
  if(args_deep < tab->args_size)
  {
    return 0;
  }
  args_deep -= tab->args_size;
  return 1;
}

void pop_args_stack(Symtab tab)
{
  args_deep += tab->args_size;
}

void assign_arg(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  value tmp;

  if(p->type->type_id == TYPE_ARRAY)
  {
    baseOffset = p->offset;

    int eleOffset = get_symbol_align_size(p->type_link->last);

    int i;
    for(i = 0; i < strlen(n->val.s); i++)
    {
      if(i > p->type_link->num_ele - 1)
      {
        parse_error("assign_local array out of index", p->name);
        return;
      }

      tmp.c = n->val.s[i];
      assign_with_byte_unit(TYPE_CHAR, &args_stack[args_deep + baseOffset + i * eleOffset], &tmp);
    }
  }
  else
  {
    if(q != NULL)
    {
      baseOffset = q->offset;
    }

    assign_with_byte_unit(p->type->type_id, &args_stack[args_deep + baseOffset + p->offset], &(n->val));
    // printf("assign_arg %s offset:%d val:%d\n", p->name, args_deep + baseOffset + p->offset, args_stack[args_deep + baseOffset + p->offset].i);
  }
}


void load_arg(Node n, Symbol p, Symbol q)
{
  int baseOffset = 0;
  if(q != NULL)
  {
    baseOffset = q->offset;
  }

  // printf("load_arg %s offset:%d val:%d\n", p->name, args_deep + baseOffset + p->offset, args_stack[args_deep + baseOffset + p->offset].i);
  load_with_byte_unit(p->type->type_id, &args_stack[args_deep + baseOffset + p->offset], &(n->val));
}