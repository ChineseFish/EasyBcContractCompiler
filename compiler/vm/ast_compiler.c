#include  <stdio.h>
#include  "../common.h"
#include  "../ast/symtab.h"
#include  "../parser/error.h"
#include  "../parser/rule.h"

List g_cp;
List g_routine_forest;

/*  */
#define CODE_MAX_NUM 1024 * 48
int code_byte_index = 0;
unsigned char code_byte_sequence[CODE_MAX_NUM];
int push_data(Type t, Value v)
{
  /*  */
  assign_with_byte_unit(t->type_id, &code_byte_sequence[code_byte_index], v);

  /*  */
  int data_size = get_type_align_size(t);
  code_byte_index += data_size;

  /*  */
  if(code_byte_index >= CODE_MAX_NUM)
  {
    printf("code_byte_sequence overflow\n");
    return 0;
  }
  return 1;
}
int push_command(int code)
{
  /*  */
  if(code_byte_index >= CODE_MAX_NUM)
  {
    printf("code_byte_sequence overflow\n");
    return 0;
  }

  /*  */
  code_byte_sequence[code_byte_index++] = code;

  return 1;
}

/*  */
typedef struct _label_detail_ *LabelDetail;
struct _label_detail_
{
  int code_index;
  char name[NAME_LEN];
};
static int label_detail_index = 0;
static LabelDetail label_detail_sequence[CODE_MAX_NUM];
static void push_label(char *name, int code_index)
{
  NEW0(label_detail_sequence[label_detail_index], PERM);

  /*  */
  label_detail_sequence[label_detail_index]->code_index = code_index;
  strncpy(label_detail_sequence[label_detail_index]->name, name, NAME_LEN);

  label_detail_index ++;
}
static int get_label_index(char *name)
{
  int i = 0;
  while(i < label_detail_index)
  {
    if(!strcmp(label_detail_sequence[i]->name, name))
    {
      return label_detail_sequence[i]->code_index;
    }

    i++;
  }
  return -1;
}

/*  */
typedef struct _function_detail_ *FunctionDetail;
struct _function_detail_
{
  int code_index;
  Symtab ptab;
};
static int function_detail_index = 0;
static FunctionDetail function_detail_sequence[CODE_MAX_NUM];
static void push_function(Symtab ptab, int code_index)
{
  NEW0(function_detail_sequence[function_detail_index], PERM);

  /*  */
  function_detail_sequence[function_detail_index]->code_index = code_index;
  function_detail_sequence[function_detail_index]->ptab = ptab;

  function_detail_index ++;
}
static int get_function_index(Symtab ptab)
{
  int i = 0;
  while(i < function_detail_index)
  {
    if(function_detail_sequence[i]->ptab == ptab)
    {
      return function_detail_sequence[i]->code_index;
    }

    i++;
  }
  return -1;
}

/*  */
struct _jump_detail_
{
  int code_index;
  char name[NAME_LEN]; /* normal label */
  Symtab ptab; /* function label */
};
typedef struct _jump_detail_ *JumpDetail;
static int jump_detail_index = 0;
static JumpDetail jump_detail_sequence[CODE_MAX_NUM];
static void push_jump_detail(char *name, int index, Symtab ptab)
{
  NEW0(jump_detail_sequence[jump_detail_index], PERM);

  if(name)
  {
    strncpy(jump_detail_sequence[jump_detail_index]->name, name, NAME_LEN);
    jump_detail_sequence[jump_detail_index]->code_index = index;
  }
  else
  {
    jump_detail_sequence[jump_detail_index]->ptab = ptab;
  }

  jump_detail_index++;
}

/*  */
void ast_compile(List routine_forest, List dag)
{
  printf("\n********************\nbegin compiler to code bytes\n********************\n");

  g_routine_forest = routine_forest;

  /*  */
  push_symtab_stack(Global_symtab);

  /*  */
  for(g_cp = dag->link; g_cp != NULL; g_cp = g_cp->link)
  {
    node_compile((Node)(g_cp->x));

    if(g_cp == NULL)
    {
      break;
    }
  }

  /*  */
  push_symtab_stack(Global_symtab);
  /*  */
  int i = 0;
  int label_index;
  int jump_index;
  while(i < jump_detail_index)
  {
    if(jump_detail_sequence[i]->ptab == NULL)
    {
      /*  */
      char *label_name = jump_detail_sequence[i]->name;
      /*  */
      label_index = get_label_index(label_name);
    }
    else
    {
      Symtab ptab = jump_detail_sequence[i]->ptab;
      /*  */
      label_index = get_function_index(ptab);
    }

    jump_index = jump_detail_sequence[i]->code_index;

    /*  */
    value v_jump_code;
    v_jump_code.i = label_index;

    assign_with_byte_unit(TYPE_INTEGER, &code_byte_sequence[jump_index], &v_jump_code);

    i++;
  }

  code_byte_sequence[code_byte_index] = '\0';
}

void node_compile(Node node)
{
  /* 流程控制相关 */
  switch (generic(node->op))
  {
  case LABEL:
  {
    Symbol p = node->syms[0];

    /*  */
    push_label(p->name, code_byte_index);
  }
  break;
  case JUMP:
  {
    Symbol p = node->syms[0];

    /* record the jump position */
    push_jump_detail(p->name, code_byte_index, NULL);

    /*  */
    int command_push_code = get_op_code_by_name("PUSH");
    push_command(command_push_code);
    /*  */
    value jump_position;
    jump_position.i = -1;
    push_data(find_type_by_id(TYPE_INTEGER), &jump_position);
    /*  */
    int jump_code = get_op_code_by_name("JUMP");
    push_command(jump_code);
  }
  break;
  case COND:
  {
    node_compile(node->kids[0]);

    /* record the jump position */
    Symbol p = node->u.cond.label;
    push_jump_detail(p->name, code_byte_index, NULL);

    /*  */
    switch(node->kids[0]->type->type_id)
    {
    case TYPE_INTEGER:
    case TYPE_BOOLEAN:
    {
      /*  */
      int command_push_code = get_op_code_by_name("PUSH");
      push_command(command_push_code);

      /*  */
      value cond;
      if(node->u.cond.true_or_false == true)
      {
        /*  */
        cond.b = true;
      }
      else
      {
        /*  */
        cond.b = false;
      }

      /*  */
      push_data(find_type_by_id(TYPE_BOOLEAN), &cond);
    }
    break;
    case TYPE_CHAR:
    {
      parse_error("COND expression can not be char", "");
    }
    break;
    case TYPE_REAL:
    {
      parse_error("COND expression can not be real", "");
    }
    break;
    case TYPE_STRING:
    {
      parse_error("COND expression can not be string", "");
    }
    break;
    }

    /*  */
    int command_push_code = get_op_code_by_name("PUSH");
    push_command(command_push_code);
    /*  */
    value jump_position;
    jump_position.i = -1;
    push_data(find_type_by_id(TYPE_INTEGER), &jump_position);
    /*  */
    int jump_code = get_op_code_by_name("COND");
    push_command(jump_code);
  }
  break;
  }

  /* 函数调用相关 */
  switch (generic(node->op))
  {
  case SYS:
  {
    /*  */
    node_compile(node->kids[0]);

    int command_function_code = 0;
    switch (node->u.sys_id)
    {
      case pREADLN:
      {
        /*  */
        command_function_code = get_op_code_by_name("READLN");
      }
      break;
      case pWRITELN:
      {
        /*  */
        command_function_code = get_op_code_by_name("WRITELN");
      }
      break;
    }

    push_command(command_function_code);
  }
  break;
  case CALL:
  {
    /*  */
    vm_push_function_call_stack(node->symtab);

    /* 记录返回地址 */
    vm_set_return_index(code_byte_index);

    /* 实参 */
    if (node->kids[0] != NULL)
    {
      node_compile(node->kids[0]);
    }

    /*  */
    push_jump_detail(NULL, -1, node->symtab);

    /*  */
    int command_push_code = get_op_code_by_name("PUSH");
    push_command(command_push_code);
    /*  */
    value function_index;
    function_index.i = -1;
    push_data(find_type_by_id(TYPE_INTEGER), &function_index);
    /*  */
    int jump_code = get_op_code_by_name("JUMP");
    push_command(jump_code);
  }
  break;
  case RIGHT:
  case ARG:
  {
    Symbol arg = node->syms[0];

    
    if(arg)
    {
      if(arg->type->type_id == TYPE_ARRAY)
      {
        vm_assign_function_call_stack_val(&(node->kids[0]->syms[0]->v), arg);
      }
      else
      {
        /* address */
        int code = get_op_code_by_name("PUSH");
        push_command(code);
        value s_offset;
        s_offset.i = arg->offset;
        push_data(find_type_by_id(TYPE_INTEGER), &s_offset);

        /* val */
        node_compile(node->kids[0]);

        vm_assign_function_call_stack_val(NULL, NULL);
      }
    }
    else
    {
      /* syscall, no symtab, no arg sym */
      node_compile(node->kids[0]);
    }
    

    /* 计算其余参数的值 */
    if(node->kids[1] != NULL)
    {
      node_compile(node->kids[1]);
    }
  }
  break;
  }

  /* 取值赋值相关 */
  switch (generic(node->op))
  {
    case CNST:
    {
      Symbol p = node->syms[0];

      if(p->type->type_id != TYPE_STRING)
      {
        /*  */
        int code = get_op_code_by_name("PUSH");
        /*  */
        push_command(code);
        /*  */
        push_data(p->type, &(p->v));
      }
    }
    break;
    case FIELD:
    {
      Symbol record = node->syms[0];
      Symbol field = node->syms[1];
      
      /* compute field offset base on record */
      int code = get_op_code_by_name("PUSH");
      push_command(code);
      value filed_offset;
      filed_offset.i = field->offset;
      push_data(find_type_by_id(TYPE_INTEGER), &filed_offset);

      /* compute record offset base on stack*/
      code = get_op_code_by_name("PUSH");
      push_command(code);
      value record_offset;
      record_offset.i = record->offset;
      push_data(find_type_by_id(TYPE_INTEGER), &record_offset);

      /* compute field offset base on stack */
      code = get_op_code_by_name("ADD");
      push_command(code);
    }
    break;
    case ARRAY:
    {
      /* 数组下标 */
      node_compile(node->kids[0]);

      Symbol array = node->syms[0];
      /* compute element offset base on array */
      int code = get_op_code_by_name("PUSH");
      push_command(code);
      value startIndex;
      startIndex.i = array->type_link->first->v.i;
      push_data(find_type_by_id(TYPE_INTEGER), &startIndex);
      /*  */
      code = get_op_code_by_name("SUB");
      push_command(code);
      /*  */
      code = get_op_code_by_name("PUSH");
      push_command(code);
      value ele_size;
      ele_size.i = get_symbol_align_size(array->type_link->last);
      push_data(find_type_by_id(TYPE_INTEGER), &ele_size);
      /*  */
      code = get_op_code_by_name("MUL");
      push_command(code);

      /* compute array offset base on stack */
      code = get_op_code_by_name("PUSH");
      push_command(code);
      value base_offset;
      base_offset.i = array->offset;
      push_data(find_type_by_id(TYPE_INTEGER), &base_offset);

      /* compute element offset base on stack*/
      code = get_op_code_by_name("ADD");
      push_command(code);
    }
    break;
    case ADDRG:
    {
      Symbol sym = node->syms[0];
      
      if(sym->type->type_id != TYPE_ARRAY)
      {
        /* compute sym offset */
        int code = get_op_code_by_name("PUSH");
        push_command(code);
        value sym_offset;
        sym_offset.i = sym->offset;
        push_data(find_type_by_id(TYPE_INTEGER), &sym_offset);
      }
    }
    break;
    case LOAD:
    {
      Symbol p = node->syms[0];

      if(node->kids[0])
      {
        /* 地址节点 */
        node_compile(node->kids[0]);
      }

      /* 全局变量 */
      if(top_symtab_stack()->level == 0)
      {
        if(p && p->defn == DEF_ENUM_ELEMENT)
        {
          /*  */
          int code = get_op_code_by_name("PUSH");
          push_command(code);
          push_data(p->type, &(p->v));
        }
        else
        {
          if(p)
          {
            /*  */
            int code = get_op_code_by_name("PUSH");
            push_command(code);
            value sym_offset;
            sym_offset.i = p->offset;
            push_data(p->type, &sym_offset);
          }

          vm_load_global();
        }
      }
      else
      {
        if(p && p->defn == DEF_ENUM_ELEMENT)
        {
          /*  */
          int code = get_op_code_by_name("PUSH");
          /*  */
          push_command(code);
          /*  */
          push_data(p->type, &(p->v));
        }
        else {
          if(p)
          {
            /*  */
            int code = get_op_code_by_name("PUSH");
            push_command(code);
            value sym_offset;
            sym_offset.i = p->offset;
            push_data(p->type, &sym_offset);
          }

          /* 普通局部变量，从栈取值 */
          vm_load_function_call_stack_val();
        }
      }
    }
    break;
    case ASGN:
    {
      Symbol p = NULL;
      if(generic(node->kids[0]->op) == ADDRG && node->kids[0]->syms[0]->type->type_id == TYPE_ARRAY)
      {
        p = node->kids[0]->syms[0];
      }

      /* address */
      node_compile(node->kids[0]);

      /* 表达式AST节点 */
      node_compile(node->kids[1]);

      if(top_symtab_stack()->level == 0)
      {
        if(p)
        {
          /*  */
          vm_assign_global(&(node->kids[1]->syms[0]->v), p);
        }
        else
        {
          vm_assign_global(NULL, NULL);
        }
      }
      else
      {
        if(p->defn == DEF_PROC)
        {
          parse_error("proc can not have return val", p->name);
          g_cp = NULL;
        }
        else
        {
          if(p)
          {
            /* 局部变量赋值 */
            vm_assign_function_call_stack_val(&(node->kids[1]->syms[0]->v), p);
          }
          else
          {
            vm_assign_function_call_stack_val(NULL, NULL);
          }
        }
      }
    }
    break;
  }

  /* 二元数学运算 */
  switch (generic(node->op))
  {
    case BAND:
    case BOR:
    case BXOR:
    {

    }
    case LSH:
    case RSH:
    {

    }
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case MOD:
    {

    }
    case AND:
    case OR:
    {

    }
    case EQ:
    case NE:
    {

    }
    case GE:
    case GT:
    case LE:
    case LT:
    {
      /* 计算左表达式AST节点对应的值 */
      if(node->kids[0] != NULL)
      {
        node_compile(node->kids[0]);
      }

      /* 计算右表达式AST节点对应的值 */
      if(node->kids[1] != NULL)
      {
        node_compile(node->kids[1]);
      }

      /*  */
      char *code_name = get_op_name(generic(node->op));
      /*  */
      int code = get_op_code_by_name(code_name);
      /*  */
      push_command(code);
    }
    break;
  }

  /* 一元数学元算 */
  switch (generic(node->op))
  {
    case BCOM:
    case NOT:
    case CVF:
    case CVI:
    case NEG:
    {
      node_compile(node->kids[0]);

      /*  */
      char *code_name = get_op_name(generic(node->op));
      /*  */
      int code = get_op_code_by_name(code_name);
      /*  */
      push_command(code);
    }
    case INCR:
    case DECR:
    {
      Symbol p = node->kids[0]->syms[0];

      /* address used by binaray operation */
      node_compile(node->kids[0]);

      /* address use by assignment */
      node_compile(node->kids[0]);

      /*  */
      if(top_symtab_stack()->level == 0)
      {
        vm_load_global(NULL, NULL);
      }
      else
      {
        vm_load_function_call_stack_val(NULL, NULL);
      }

      /*  */
      char *code_name = get_op_name(generic(node->op));
      /*  */
      int code = get_op_code_by_name(code_name);
      /*  */
      push_command(code);

      /*  */
      if(top_symtab_stack()->level == 0)
      {
        vm_assign_global(NULL, NULL);
      }
      else
      {
        vm_assign_function_call_stack_val(NULL, NULL);
      }
    }
    break;
  }

  /* 代码块标记 */
  switch (generic(node->op))
  {
    case HEADER: /* 表示过程以及函数定义的开始 */  
    {
       /* 符号表压栈 */
      push_symtab_stack(node->symtab);
      /* record function */
      push_function(node->symtab, code_byte_index);
    }
    break;
    case TAIL: /* 表示过程以及函数定义的结束 */
    {
      /*  */
      vm_get_return_index();
     
      /*  */
      Symtab ptab = top_symtab_stack();
      vm_pop_function_call_stack(ptab);

      /*  */
      int jump_code = get_op_code_by_name("JUMP");
      push_command(jump_code);

      /*  */
      pop_symtab_stack();
    }
    break;
    case BLOCKBEG:
    case BLOCKEND:
        break;
  }
}