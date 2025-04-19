#include "rbtree.h"

#include <stdlib.h>

void left_rotate(rbtree* t, node_t *x);
void right_rotate(rbtree* t, node_t *y);
void rbtree_insert_fixup(rbtree *t, node_t *cur);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_erase_fixup(rbtree *t, node_t *cur);
void delete_rbtree_node(node_t *p);
void rbtree_inorder_traversal(node_t *p, key_t *arr, int idx, size_t n);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (p != NULL) p->root = NULL;
  return p;
}

void delete_rbtree(rbtree *t) {
  if(t != NULL) {
    delete_rbtree_node(t->root);
    free(t);
    t = NULL;
  }
}

void delete_rbtree_node(node_t *p) {
  while (p != NULL) {
    if (p->left != NULL) delete_rbtree_node(p->left);
    if (p->right != NULL) delete_rbtree_node(p->right);
    free(p);
    p = NULL;
  }
}

void left_rotate(rbtree* t, node_t *x) {
  if (t != NULL) {
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL) t->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
  }
}

void right_rotate(rbtree* t, node_t *y) {
  if (t != NULL) {
    node_t *x = y->left;
    y->left = x->right;
    if (x->right != NULL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL) t->root = x;
    else if (y == y->parent->left) x->parent->left = y;
    else x->parent->right = y;
    x->right = y;
    y->parent = x;
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  if (t == NULL) return NULL;
  node_t *cur = t->root;
  node_t *prev = NULL;
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
      if (new_node != NULL) {
        new_node->color = RBTREE_RED;
        new_node->key = key;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = NULL;

        while (cur != NULL) {
          prev = cur;
          if (new_node->key < cur->key) cur = cur->left;
          else cur = cur->right;
        }

        new_node->parent = prev;
        if (prev == NULL) t->root = new_node;
        else if (new_node->key < prev->key) prev->left = new_node;
        else prev->right = new_node;

        rbtree_insert_fixup(t, new_node);
        t->root->color = RBTREE_BLACK;
      }
  return t->root;
}

void rbtree_insert_fixup(rbtree *t, node_t *cur) {
  node_t *uncle = NULL;
  while (cur->parent->color == RBTREE_RED) {
    if (cur->parent == cur->parent->parent->left) {
      uncle = cur->parent->parent->right;
      // Case 1: 부모도 RED, 삼촌도 RED
      // → 부모와 삼촌을 BLACK으로 바꾸고 할아버지를 RED로 바꾼 후 할아버지에서 다시 확인
      if (uncle->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        cur = cur->parent->parent;
      } else {
        // Case 2-1: 삽입된 노드가 부모의 오른쪽 자녀, 부모가 RED이고 할아버지의 왼쪽 자녀, 삼촌은 BLACK
        // → 부모를 기준으로 왼쪽으로 회전한 후 Case 3 방식으로 해결
        if (cur == cur->parent->right) {
          cur = cur->parent;
          left_rotate(t, cur);
        }
        // Case 3-1: 삽입된 노드가 부모의 왼쪽 자녀, 부모가 RED이고 할아버지의 왼쪽 자녀, 삼촌은 BLACK
        // → 부모와 할아버지의 색을 바꾼 후 할아버지 기준으로 오른쪽으로 회전
        cur->parent->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        right_rotate(t, cur->parent->parent);
      }
    } else {
      uncle = cur->parent->parent->left;
      if (uncle->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        cur = cur->parent->parent;
      } else {
        // Case 2-2: 삽입된 노드가 부모의 왼쪽 자녀, 부모가 RED이고 할아버지의 오른쪽 자녀, 삼촌은 BLACK
        // → 부모를 기준으로 오른쪽으로 회전한 뒤 Case 3의 방식으로 해결
        if (cur == cur->parent->left) {
          cur = cur->parent;
          right_rotate(t, cur);
        }
        // Case 3-2: 삽입된 노드가 부모의 오른쪽 자녀, 부모가 RED이고 할아버지의 오른쪽 자녀, 삼촌은 BLACK
        // → 부모와 할아버지의 색을 바꾼 후 할아버지 기준으로 왼쪽으로 회전
        cur->parent->parent->color = RBTREE_BLACK;
        left_rotate(t, cur->parent->parent);
      }
    }
  }
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (t == NULL || t->root == NULL) return NULL; 
  node_t *cur = t->root;
  while (cur->left != NULL || cur->right != NULL) {
    if (cur->key > key) cur = cur->left;
    else if (cur->key < key) cur = cur->right;
    else return cur;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  if (t == NULL || t->root == NULL) return NULL; 
  node_t *cur = t->root;
  while (cur->left != NULL) cur = cur->left;
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  if (t == NULL || t->root == NULL) return NULL; 
  node_t *cur = t->root;
  while (cur->right != NULL) cur = cur->right;
  return cur;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (t != NULL) {
    if (u->parent == NULL) t->root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
  }
}

int rbtree_erase(rbtree *t, node_t *p) {
  if (t == NULL || t->root == NULL) return 1;
  node_t *x = NULL;
  node_t *y = p;
  node_t *successor;
  color_t y_original_color = y->color;
  if (p->left == NULL) {
    x = p->right;
    rbtree_transplant(t, p, p->right);
  } else if (p->right == NULL) {
    x = p->left;
    rbtree_transplant(t, p, p->left);
  } else {
    // y = rbtree_min(p->right); // y는 p의 후임자
    successor = p;
    while (successor->left != NULL) successor = successor->left;
    y = successor;
    y_original_color = y->color;
    x = y->right;
    if (y != p->right) {
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    } else x->parent = y;
    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
    if (y_original_color == RBTREE_BLACK) rbtree_erase_fixup(t, x);
  }
  return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *cur) {
  node_t *sibling = NULL;
  while (cur != t->root && cur->color == RBTREE_BLACK) {
    if (cur->parent->left) {
      sibling = cur->parent->right;
      // Case 1-1: DOUBLY BLACK의 오른쪽 형제가 RED일 때
      // → 부모와 형제의 색을 바꾸고 부모를 기준으로 왼쪽으로 회전한 뒤, DOUBLY BLACK을 기준으로 Case 2, 3, 4 중 하나로 해결
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        cur->parent->color = RBTREE_RED;
        left_rotate(t, cur->parent);
        sibling = cur->parent->right;
      }
      // Case 2: DOUBLY BLACK의 형제가 BLACK and 그 형제의 두 자녀가 모두 BLACK일 때
      // → DOUBLY BLACK과 그 형제의 BLACK을 모두 모아서 부모에게 전달해서 부모가 EXTRA BLACK을 해결하도록 한다
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        cur = cur->parent;
      } else {
        // Case 3-1: DOUBLY BLACK의 오른쪽 형제가 BLACK and 그 형제의 왼쪽 자녀가 RED and 오른쪽 자녀가 BLACK일 때:
        // → DOUBLY BLACK의 형제의 오른쪽 자녀를 RED가 되게 만들어서 이후에는 Case 4를 적용하여 해결
        if (sibling->right->color == RBTREE_BLACK) {
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t, sibling);
          sibling = cur->parent->right;
        }
        // Case 4-1:DOUBLY BLACK의 오른쪽 형제가 BLACK and 그 형제의 오른쪽 자녀가 RED일 때
        // → 오른쪽 형제는 부모의 색으로, 오른쪽 형제의 오른쪽 자녀는 BLACK으로, 부모는 BLACK으로 바꾼 후에 부모를 기준으로 왼쪽으로 회전하면 해결
        sibling->color = cur->parent->color;
        cur->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t, cur->parent);
        cur = t->root;
      }
    } else {
      sibling = cur->parent->left;
      // Case 1-2: DOUBLY BLACK의 왼쪽 형제가 RED일 때
      // → 부모와 형제의 색을 바꾸고 부모를 기준으로 오른쪽으로 회전한 뒤 DOUBLY BLACK을 기준으로 Case 2, 3, 4 중 하나로 해결
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;
        cur->parent->color = RBTREE_RED;
        right_rotate(t, cur->parent);
        sibling = cur->parent->left;
      }
      // Case 2: DOUBLY BLACK의 형제가 BLACK and 그 형제의 두 자녀가 모두 BLACK일 때
      // → DOUBLY BLACK과 그 형제의 BLACK을 모두 모아서 부모에게 전달해서 부모가 EXTRA BLACK을 해결하도록 한다
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        cur = cur->parent;
      } else {
        // Case 3-2: DOUBLY BLACK의 왼쪽 형제가 BLACK and 그 형제의 오른쪽 자녀가 RED and 왼쪽 자녀가 BLACK일 때
        // → DOUBLY BLACK의 형제의 왼쪽 자녀를 RED가 되게 만들어서 이후에는 Case 4를 적용하여 해결
        if (sibling->left->color == RBTREE_BLACK) {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = cur->parent->left;
        }
        // Case 4-2: DOUBLY BLACK의 왼쪽 형제가 BLACK and 그 형제의 왼쪽 자녀가 RED일 때
        // → 왼쪽 형제는 부모의 색으로, 왼쪽 형제의 왼쪽 자녀는 BLACK으로, 부모는 BLACK으로 바꾼 후에 부모를 기준으로 오른쪽으로 회전하면 해결
        sibling->color = cur->parent->color;
        cur->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, cur->parent);
        cur = t->root;
      }
    }
  }
  cur->color = RBTREE_BLACK;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t == NULL || t->root == NULL || arr == NULL || n == 0) return 1;
  rbtree_inorder_traversal(t->root, arr, 0, n);
  return 0;
}

void rbtree_inorder_traversal(node_t *p, key_t *arr, int idx, size_t n) {
  if (p != NULL && idx < n) {
    rbtree_inorder_traversal(p->left, arr, idx, n);
    arr[idx] = p->key;
    idx++;
    rbtree_inorder_traversal(p->right, arr, idx, n);
  }
}