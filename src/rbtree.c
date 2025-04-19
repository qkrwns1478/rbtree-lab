#include "rbtree.h"

#include <stdlib.h>

void left_rotate(rbtree* t, node_t *x);
void right_rotate(rbtree* t, node_t *y);
void rbtree_insert_fixup(rbtree *t, node_t *cur);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_erase_fixup(rbtree *t, node_t *cur);
void delete_rbtree_node(rbtree *t, node_t *p);
void rbtree_inorder_traversal(const rbtree *t, node_t *p, key_t *arr, int *idx, size_t n);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (p == NULL) return NULL;
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  if (nil != NULL) {
    nil->color = RBTREE_BLACK;
    p->nil = nil;
    p->root = nil;
  }
  return p;
}

void delete_rbtree(rbtree *t) {
  if (t != NULL) {
    delete_rbtree_node(t, t->root);
    free(t->nil);
    free(t);
    t = NULL;
  }
}

// 재귀로 BST처럼 노드를 삭제하는 메서드
void delete_rbtree_node(rbtree *t, node_t *p) {
  if (p != t->nil) {
    delete_rbtree_node(t, p->left);
    delete_rbtree_node(t, p->right);
    free(p);
    p = NULL;
  }
}

// x를 기준으로 왼쪽으로 회전하는 메서드
void left_rotate(rbtree *t, node_t *x) {
  if (t != NULL) {
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != t->nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == t->nil) t->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
  }
}

// y를 기준으로 오른쪽으로 회전하는 메서드
void right_rotate(rbtree *t, node_t *y) {
  if (t != NULL) {
    node_t *x = y->left;
    y->left = x->right;
    if (x->right != t->nil) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == t->nil) t->root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    x->right = y;
    y->parent = x;
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  if (t == NULL) return NULL;
  node_t *cur = t->root;
  node_t *prev = t->nil;
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  if (new_node != NULL) {
    new_node->color = RBTREE_RED;
    new_node->key = key;
    new_node->left = t->nil;
    new_node->right = t->nil;
    new_node->parent = t->nil;

    while (cur != t->nil) {
      prev = cur;
      if (new_node->key < cur->key) cur = cur->left;
      else cur = cur->right;
    }

    new_node->parent = prev;
    if (prev == t->nil) t->root = new_node;
    else if (new_node->key < prev->key) prev->left = new_node;
    else prev->right = new_node;

    rbtree_insert_fixup(t, new_node);
    return new_node;
  } else return NULL;
}

// 삽입 시 RB트리 속성을 위반했다면 재조정하는 메서드
void rbtree_insert_fixup(rbtree *t, node_t *cur) {
  node_t *uncle = t->nil;
  while (cur->parent->color == RBTREE_RED) {
    if (cur->parent == cur->parent->parent->left) { // 부모가 할아버지의 왼쪽 자식일 때
      uncle = cur->parent->parent->right;           // 삼촌은 할아버지의 오른쪽 자식
      // Case 1: 부모도 RED, 삼촌도 RED
      // → 부모와 삼촌을 BLACK으로 바꾸고 할아버지를 RED로 바꾼 후 할아버지에서 다시 확인
      if (uncle->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;          // 부모를 BLACK으로
        uncle->color = RBTREE_BLACK;                // 삼촌을 BLACK으로
        cur->parent->parent->color = RBTREE_RED;    // 할아버지를 RED로
        cur = cur->parent->parent;                  // 할아버지에서 다시 확인
      } else {
        // Case 2-1: 삽입된 노드가 부모의 오른쪽 자녀, 부모가 RED이고 할아버지의 왼쪽 자녀, 삼촌은 BLACK
        // → 부모를 기준으로 왼쪽으로 회전한 후 Case 3 방식으로 해결
        if (cur == cur->parent->right) {
          cur = cur->parent;                        // 부모를 기준으로
          left_rotate(t, cur);                      // 왼쪽으로 회전
        }
        // Case 3-1: 삽입된 노드가 부모의 왼쪽 자녀, 부모가 RED이고 할아버지의 왼쪽 자녀, 삼촌은 BLACK
        // → 부모와 할아버지의 색을 바꾼 후 할아버지 기준으로 오른쪽으로 회전
        color_t tmp = cur->parent->color;
        cur->parent->color = cur->parent->parent->color; // 부모의 색을 할아버지의 색으로
        cur->parent->parent->color = tmp;           // 할아버지의 색을 부모의 색으로
        right_rotate(t, cur->parent->parent);       // 할아버지를 기준으로 오른쪽으로 회전
      }
    } else {                                        // 부모가 할아버지의 오른쪽 자식일 때
      uncle = cur->parent->parent->left;            // 삼촌은 할아버지의 오른쪽 자식
      // Case 1: 부모도 RED, 삼촌도 RED
      // → 부모와 삼촌을 BLACK으로 바꾸고 할아버지를 RED로 바꾼 후 할아버지에서 다시 확인
      if (uncle->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;          // 부모를 BLACK으로
        uncle->color = RBTREE_BLACK;                // 삼촌을 BLACK으로
        cur->parent->parent->color = RBTREE_RED;    // 할아버지를 RED로
        cur = cur->parent->parent;                  // 할아버지에서 다시 확인
      } else {
        // Case 2-2: 삽입된 노드가 부모의 왼쪽 자녀, 부모가 RED이고 할아버지의 오른쪽 자녀, 삼촌은 BLACK
        // → 부모를 기준으로 오른쪽으로 회전한 뒤 Case 3의 방식으로 해결
        if (cur == cur->parent->left) {
          cur = cur->parent;                        // 부모를 기준으로
          right_rotate(t, cur);                     // 오른쪽으로 회전
        }
        // Case 3-2: 삽입된 노드가 부모의 오른쪽 자녀, 부모가 RED이고 할아버지의 오른쪽 자녀, 삼촌은 BLACK
        // → 부모와 할아버지의 색을 바꾼 후 할아버지 기준으로 왼쪽으로 회전
        color_t tmp = cur->parent->color;
        cur->parent->color = cur->parent->parent->color; // 부모의 색을 할아버지의 색으로
        cur->parent->parent->color = tmp;           // 할아버지의 색을 부모의 색으로
        left_rotate(t, cur->parent->parent);        // 할아버지를 기준으로 왼쪽으로 회전
      }
    }
  }
  t->root->color = RBTREE_BLACK;                    // 루트를 BLACK으로
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (t == NULL || t->root == t->nil) return NULL;
  node_t *cur = t->root;
  while (cur != t->nil) {
    if (cur->key > key) cur = cur->left;
    else if (cur->key < key) cur = cur->right;
    else return cur;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  if (t == NULL) return NULL;
  if (t->root == t->nil) return t->nil; 
  node_t *cur = t->root;
  while (cur->left != t->nil) cur = cur->left;
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  if (t == NULL) return NULL;
  if (t->root == t->nil) return t->nil; 
  node_t *cur = t->root;
  while (cur->right != t->nil) cur = cur->right;
  return cur;
}

// u를 v로 바꾸는 메서드
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (t == NULL) return;
  if (u->parent == t->nil) t->root = v;
  else if (u == u->parent->left) u->parent->left = v;
  else u->parent->right = v;
  v->parent = u->parent;
}

int rbtree_erase(rbtree *t, node_t *p) {
  if (t == NULL || t->root == t->nil) return 0;
  node_t *x = t->nil;                               // x는 y의 원래 자리로 이동하는 노드
  node_t *y = p;                                    // y는 p의 자리로 이동하는 노드
  color_t y_original_color = y->color;              // p의 자식이 하나 이하면 삭제되는 색은 p의 색
  // 1. 삭제하려는 노드의 자녀가 없거나 하나라면, 삭제되는 색 = 삭제되는 노드의 색
  if (p->left == t->nil) {
    // 1-1. 삭제 노드의 왼쪽 자식이 NIL일 때
    // → 삭제 노드를 오른쪽 자식으로 대체함
    x = p->right;
    rbtree_transplant(t, p, p->right);              // p를 오른쪽 자식으로 바꿈
  } else if (p->right == t->nil) {
    // 1-2. 삭제 노드의 오른쪽 자식이 NIL일 때
    // → 삭제 노드를 왼쪽 자식으로 대체함
    x = p->left;
    rbtree_transplant(t, p, p->left);               // p를 왼쪽 자식으로 바꿈
  } else {                                          // p의 자식이 두 개일 때…
    // 2. 삭제하려는 노드의 자녀가 둘이라면, 삭제되는 색 = 삭제되는 노드의 후임자의 색
    y = p->right;
    while (y->left != t->nil) y = y->left;          // y는 p의 후임자
    y_original_color = y->color;                    // 삭제되는 색은 후임자의 색
    x = y->right;
    if (y != p->right) {                            // 후임자가 p의 자식이 아닌 경우
      // 2-1. 삭제 노드의 후임자가 손자 이하일 때
      // → 삭제 노드를 후임자로, 후임자를 그 오른쪽 자식으로 대체함
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    } else {
      // 2-2. 삭제 노드의 오른쪽 자식이 후임자일 때
      // → 삭제 노드를 오른쪽 자식으로, 오른쪽 자식을 오른쪽 손자로 대체함
      x->parent = y;                                // x가 NIL인 경우 (?)
    }
    rbtree_transplant(t, p, y);                     // p를 후임자로 대체함
    y->left = p->left;                              // p의 왼쪽 자식을 y에 연결
    y->left->parent = y;                            // p의 왼쪽 자식에 y를 연결
    y->color = p->color;                            // 후임자를 p의 색으로
  }
  if (y_original_color == RBTREE_BLACK)             // 삭제 노드의 색이 BLACK이라면
    rbtree_erase_fixup(t, x);                       // → 재조정 수행
  free(p);
  p = NULL;
  return 1;
}

// 삭제 시 RB트리 속성을 위반했다면 재조정하는 메서드
void rbtree_erase_fixup(rbtree *t, node_t *cur) {
  node_t *sibling = t->nil;
  while (cur != t->root && cur->color == RBTREE_BLACK) {
    if (cur == cur->parent->left) {                 // cur가 부모의 왼쪽 자식일 때
      sibling = cur->parent->right;                 // 형제는 부모의 오른쪽 자식
      // Case 1-1: DOUBLY BLACK의 오른쪽 형제가 RED일 때
      // → 부모와 형제의 색을 바꾸고 부모를 기준으로 왼쪽으로 회전한 뒤, DOUBLY BLACK을 기준으로 Case 2, 3, 4 중 하나로 해결
      if (sibling->color == RBTREE_RED) {
        color_t tmp = sibling->color;
        sibling->color = sibling->parent->color;    // 형제를 부모의 색으로
        sibling->parent->color = tmp;               // 부모를 형제의 색으로
        left_rotate(t, cur->parent);                // 부모를 기준으로 왼쪽으로 회전
        sibling = cur->parent->right;               // 새로운 형제를 기준으로 Case 2, 3, 4 중 하나로 해결
      }
      // Case 2: DOUBLY BLACK의 형제가 BLACK and 그 형제의 두 자녀가 모두 BLACK일 때
      // → DOUBLY BLACK과 그 형제의 BLACK을 모두 모아서 부모에게 전달해서 부모가 EXTRA BLACK을 해결하도록 한다
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;                // (cur를 순수한 BLACK으로 만들고) 형제를 RED로
        cur = cur->parent;                          // (부모에게 EXTRA BLACK을 전달해) 부모를 기준으로 확인
      } else {
        // Case 3-1: DOUBLY BLACK의 오른쪽 형제가 BLACK and 그 형제의 왼쪽 자녀가 RED and 오른쪽 자녀가 BLACK일 때
        // → (DOUBLY BLACK의 형제의 오른쪽 자녀를 RED가 되게 만들어서)
        // → 형제와 형제의 왼쪽 자식 색을 바꾸고 형제를 기준으로 오른쪽으로 회전하면 형제는 RED인 오른쪽 자식을 가진 BLACK이 됨
        // → 이후에는 Case 4를 적용하여 해결
        if (sibling->right->color == RBTREE_BLACK) {
          sibling->left->color = RBTREE_BLACK;      // 형제의 왼쪽 자식을 BLACK으로
          sibling->color = RBTREE_RED;              // 형제를 RED로
          right_rotate(t, sibling);                 // 형제를 기준으로 오른쪽으로 회전
          sibling = cur->parent->right;             // 새로운 형제를 기준으로 Case 4로 해결
        }
        // Case 4-1: DOUBLY BLACK의 오른쪽 형제가 BLACK and 그 형제의 오른쪽 자녀가 RED일 때
        // → 오른쪽 형제는 부모의 색으로, 오른쪽 형제의 오른쪽 자녀는 BLACK으로, 부모는 BLACK으로 바꾼 후에 부모를 기준으로 왼쪽으로 회전하면 해결
        sibling->color = cur->parent->color;        // 형제를 부모의 색으로
        sibling->right->color = RBTREE_BLACK;       // 형제의 오른쪽 자식을 BLACK으로
        cur->parent->color = RBTREE_BLACK;          // 부모를 BLACK으로
        left_rotate(t, cur->parent);                // 부모를 기준으로 왼쪽으로 회전
        cur = t->root;                              // cur가 루트가 되면 루프 조건 검사 때 while 루프가 종료됨
      }
    } else {                                        // cur가 부모의 오른쪽 자식일 때
      sibling = cur->parent->left;
      // Case 1-2: DOUBLY BLACK의 왼쪽 형제가 RED일 때
      // → 부모와 형제의 색을 바꾸고 부모를 기준으로 오른쪽으로 회전한 뒤 DOUBLY BLACK을 기준으로 Case 2, 3, 4 중 하나로 해결
      if (sibling->color == RBTREE_RED) {
        color_t tmp = sibling->color;
        sibling->color = sibling->parent->color;    // 형제를 부모의 색으로
        sibling->parent->color = tmp;               // 부모를 형제의 색으로
        right_rotate(t, cur->parent);               // 부모를 기준으로 오른쪽으로 회전
        sibling = cur->parent->left;                // 새로운 형제를 기준으로 Case 2, 3, 4 중 하나로 해결
      }
      // Case 2: DOUBLY BLACK의 형제가 BLACK and 그 형제의 두 자녀가 모두 BLACK일 때
      // → DOUBLY BLACK과 그 형제의 BLACK을 모두 모아서 부모에게 전달해서 부모가 EXTRA BLACK을 해결하도록 한다
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;                // (cur를 순수한 BLACK으로 만들고) 형제를 RED로
        cur = cur->parent;                          // (부모에게 EXTRA BLACK을 전달해) 부모를 기준으로 확인
      } else {
        // Case 3-2: DOUBLY BLACK의 왼쪽 형제가 BLACK and 그 형제의 오른쪽 자녀가 RED and 왼쪽 자녀가 BLACK일 때
        // → (DOUBLY BLACK의 형제의 왼쪽 자녀를 RED가 되게 만들어서)
        // → 형제와 형제의 오른쪽 자식의 색을 바꾸고 형제를 기준으로 왼쪽으로 회전하면 형제는 RED인 왼쪽 자식을 가진 BLACK이 됨
        // → 이후에는 Case 4를 적용하여 해결
        if (sibling->left->color == RBTREE_BLACK) {
          sibling->right->color = RBTREE_BLACK;     // 형제의 오른쪽 자식을 BLACK으로
          sibling->color = RBTREE_RED;              // 형제를 RED로
          left_rotate(t, sibling);                  // 형제를 기준으로 왼쪽으로 회전
          sibling = cur->parent->left;              // 새로운 형제를 기준으로 Case 4로 해결
        }
        // Case 4-2: DOUBLY BLACK의 왼쪽 형제가 BLACK and 그 형제의 왼쪽 자녀가 RED일 때
        // → 왼쪽 형제는 부모의 색으로, 왼쪽 형제의 왼쪽 자녀는 BLACK으로, 부모는 BLACK으로 바꾼 후에 부모를 기준으로 오른쪽으로 회전하면 해결
        sibling->color = cur->parent->color;        // 형제를 부모의 색으로
        sibling->left->color = RBTREE_BLACK;        // 형제의 왼쪽 자식을 BLACK으로
        cur->parent->color = RBTREE_BLACK;          // 부모를 BLACK으로
        right_rotate(t, cur->parent);               // 부모를 기준으로 오른쪽으로 회전
        cur = t->root;                              // cur가 루트가 되면 루프 조건 검사 때 while 루프가 종료됨
      }
    }
  }
  cur->color = RBTREE_BLACK;                        // 루트를 BLACK으로
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t == NULL || arr == NULL || n == 0) return 1;
  int idx = 0;
  rbtree_inorder_traversal(t, t->root, arr, &idx, n);
  return 0;
}

// RB 트리를 중위 순회하는 메서드
void rbtree_inorder_traversal(const rbtree *t, node_t *p, key_t *arr, int *idx, size_t n) {
  if (p != t->nil && *idx < n) {
    rbtree_inorder_traversal(t, p->left, arr, idx, n);
    arr[(*idx)++] = p->key;
    rbtree_inorder_traversal(t, p->right, arr, idx, n);
  }
}