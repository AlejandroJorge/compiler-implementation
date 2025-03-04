#include "persistent_tree.h"
#include <stdio.h>
#include <string.h>

T_tree Tree(T_tree l, string k, void *binding, T_tree r) {
  T_tree t = checked_malloc(sizeof(*t));
  t->left = l;
  t->key = k;
  t->right = r;
  return t;
}

T_tree insert(string key, void *binding, T_tree t) {
  if (t == NULL)
    return Tree(NULL, key, binding, NULL);
  else if (strcmp(key, t->key) < 0)
    return Tree(insert(key, binding, t->left), t->key, t->binding, t->right);
  else if (strcmp(key, t->key) > 0)
    return Tree(t->left, t->key, t->binding, insert(key, binding, t->right));
  else
    return Tree(t->left, key, binding, t->right);
}

char exists(string key, T_tree t) {
  if (t == NULL)
    return 0;

  if (strcmp(key, t->key) == 0)
    return 1;
  else if (strcmp(key, t->key) < 0)
    return exists(key, t->left);
  else
    return exists(key, t->right);
}

void *t_lookup(string key, T_tree t) {
  if (t == NULL)
    return NULL;

  if (strcmp(key, t->key) == 0)
    return t->binding;
  else if (strcmp(key, t->key) < 0)
    return t_lookup(key, t->left);
  else
    return t_lookup(key, t->right);
}
