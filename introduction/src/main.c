#include "grammar.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

// #define DEBUG

#define max(a, b) a > b ? a : b

typedef struct table *Table_;
struct table {
  string id;
  int value;
  Table_ tail;
};

Table_ Table(string id, int value, struct table *table);
int lookup(Table_ t, string key);
void printTable(struct table *table);

struct IntAndTable {
  int i;
  Table_ t;
};

typedef struct int_list *IntList_;
struct int_list {
  int value;
  IntList_ tail;
};

IntList_ IntList(int value, struct int_list *list);

struct IntListAndTable {
  IntList_ l;
  Table_ t;
};

int inspect_exp(A_exp exp);
int count(A_expList list);
int maxargs(A_stm stm);

void interp(A_stm stm);
Table_ interpStm(A_stm stm, Table_ t);
struct IntAndTable interpExp(A_exp exp, Table_ t);
struct IntListAndTable interpExpList(A_expList list, Table_ t);

Table_ Table(string id, int value, struct table *table) {
  Table_ t = checked_malloc(sizeof(*t));
  t->id = id;
  t->value = value;
  t->tail = table;
  return t;
}

int lookup(Table_ t, string key) {
  while (t) {
    if (strcmp(key, t->id) == 0)
      return t->value;
    t = t->tail;
  }
  assert(0);
}

void printTable(struct table *table) {
  printf("Current table state:\n");
  while (table) {
    printf("Key: %s, Value: %d\n", table->id, table->value);
    table = table->tail;
  }
}

IntList_ IntList(int value, struct int_list *list) {
  IntList_ l = checked_malloc(sizeof(*l));
  l->value = value;
  l->tail = list;
  return l;
}

void interp(A_stm stm) {
  Table_ t = NULL;
  interpStm(stm, t);
#ifdef DEBUG
  printTable(t);
#endif
}

Table_ interpStm(A_stm stm, Table_ t) {
#ifdef DEBUG
  printTable(t);
#endif
  switch (stm->kind) {
  case A_assignStm: {
    struct IntAndTable res = interpExp(stm->u.assign.exp, t);
    return Table(stm->u.assign.id, res.i, res.t);
  }
  case A_compoundStm: {
    Table_ t2 = interpStm(stm->u.compound.stm1, t);
    return interpStm(stm->u.compound.stm2, t2);
  }
  case A_printStm: {
    struct IntListAndTable res = interpExpList(stm->u.print.exps, t);
    while (res.l) {
      printf("%d ", res.l->value);
      res.l = res.l->tail;
    }
    printf("\n");
    return res.t;
  }
  }
  assert(0);
}

struct IntAndTable interpExp(A_exp exp, Table_ t) {
#ifdef DEBUG
  printTable(t);
#endif
  switch (exp->kind) {
  case A_idExp: {
    int val = lookup(t, exp->u.id);
    struct IntAndTable res = {.t = t, .i = val};
#ifdef DEBUG
    printf("Evaluated %d\n", res.i);
#endif
    return res;
  }
  case A_numExp: {
    struct IntAndTable res = {.t = t, .i = exp->u.num};
#ifdef DEBUG
    printf("Evaluated %d\n", res.i);
#endif
    return res;
  }
  case A_opExp: {
    struct IntAndTable left = interpExp(exp->u.op.left, t);
    struct IntAndTable right = interpExp(exp->u.op.right, left.t);
#ifdef DEBUG
    printf("Operating %d and %d\n", left.i, right.i);
#endif
    switch (exp->u.op.oper) {
    case A_plus: {
      struct IntAndTable res = {.t = right.t, .i = left.i + right.i};
      return res;
    }
    case A_minus: {
      struct IntAndTable res = {.t = right.t, .i = left.i - right.i};
      return res;
    }
    case A_times: {
      struct IntAndTable res = {.t = right.t, .i = left.i * right.i};
      return res;
    }
    case A_div: {
      struct IntAndTable res = {.t = right.t, .i = left.i / right.i};
      return res;
    }
    }
    assert(0);
  }
  case A_eseqExp: {
    struct IntAndTable first = interpExp(exp->u.eseq.exp, t);
    Table_ t3 = interpStm(exp->u.eseq.stm, first.t);
    struct IntAndTable res = {.t = t3, .i = first.i};
#ifdef DEBUG
    printf("Evaluated %d\n", res.i);
#endif
    return res;
  }
  }
  assert(0);
}

struct IntListAndTable interpExpList(A_expList list, Table_ t) {
#ifdef DEBUG
  printTable(t);
#endif
  switch (list->kind) {
  case A_pairExpList: {
    struct IntAndTable eval = interpExp(list->u.pair.head, t);
    struct IntListAndTable res = interpExpList(list->u.pair.tail, eval.t);
    struct IntListAndTable output = {.l = IntList(eval.i, res.l), .t = res.t};
    return output;
  }
  case A_lastExpList: {
    struct IntAndTable eval = interpExp(list->u.last, t);
    IntList_ l = IntList(eval.i, NULL);
    struct IntListAndTable res = {.l = l, .t = eval.t};
    return res;
  }
  }
  assert(0);
}

int inspect_exp(A_exp exp) {
  switch (exp->kind) {
  case A_idExp:
  case A_numExp:
    return 0;
  case A_opExp: {
    int a = inspect_exp(exp->u.op.right);
    int b = inspect_exp(exp->u.op.left);
    return max(a, b);
  }
  case A_eseqExp: {
    int a = inspect_exp(exp->u.eseq.exp);
    int b = maxargs(exp->u.eseq.stm);
    return max(a, b);
  }
  }
  assert(0);
}

int count(A_expList list) {
  switch (list->kind) {
  case A_pairExpList:
    return count(list->u.pair.tail) + 1;
  case A_lastExpList:
    return 1;
  }
  assert(0);
}

// Write a function int maxargs(A_stm) that tells the maximum number
// of arguments of any print statement within any subexpression of a given
// statement. For example, maxargs(prog) is 2.
int maxargs(A_stm stm) {
  switch (stm->kind) {
  case A_compoundStm: {
    int a = maxargs(stm->u.compound.stm1);
    int b = maxargs(stm->u.compound.stm2);
    return a > b ? a : b;
  }
  case A_assignStm:
    return inspect_exp(stm->u.assign.exp);
  case A_printStm:
    return count(stm->u.print.exps);
  }
  assert(0);
}

int main(int argc, char *argv[]) {

  A_stm prog = A_CompoundStm(
      A_AssignStm("a", A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
      A_CompoundStm(
          A_AssignStm("b",
                      A_EseqExp(A_PrintStm(A_PairExpList(
                                    A_IdExp("a"),
                                    A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                                          A_NumExp(1))))),
                                A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
          A_PrintStm(A_LastExpList(A_IdExp("b")))));

  assert(maxargs(prog) == 2);

  interp(prog);

  return 0;
}
