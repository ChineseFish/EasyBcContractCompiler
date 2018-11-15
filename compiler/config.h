#ifndef __CONFIG_H_
#define __CONFIG_H_

// #define DAG_DEBUG			(2)
// #define CONST_FOLDING_DEBUG	(4)
// #define COMMON_EXPR_DEBUG	(8)
// #define SYSTEM_CALL_DEBUG	(16)

#define DAG_DEBUG			(0)
#define CONST_FOLDING_DEBUG	(0)
#define COMMON_EXPR_DEBUG	(0)
#define SYSTEM_CALL_DEBUG	(0)

#define DEBUG				(DAG_DEBUG | CONST_FOLDING_DEBUG | COMMON_EXPR_DEBUG | SYSTEM_CALL_DEBUG)

#endif

