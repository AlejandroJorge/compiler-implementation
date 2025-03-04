#include "util.h"

typedef struct tree *T_tree;
struct tree {
  T_tree left;
  string key;
  void *binding;
  T_tree right;
};

T_tree Tree(T_tree l, string k, void *binding, T_tree r);

T_tree insert(string key, void *binding, T_tree t);

void *t_lookup(string key, T_tree t);

char exists(string key, T_tree t);
