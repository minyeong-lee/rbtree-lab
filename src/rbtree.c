#include "rbtree.h"

#include <stdlib.h>

//RB트리를 만들어서, nill 초기화를 목적으로 함
rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));  //동적으로 할당
  // TODO: initialize struct if needed
  t->nil = NULL;
  t->root = NULL;

  //nill 포인터 생성
  node_t * nil = (node_t *)malloc(sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->parent = NULL;
  nil->left = NULL;
  nil->right = NULL;

  //연결
  t->root = nil;
  t->nil = nil;

  return t;
}

void remove_node(rbtree *t ,node_t * x) {
  //자식이 nil 노드인지 여부 검사
  if (x->left != t->nil)
    remove_node(t, x->left);
  if (x->right != t->nil)
    remove_node(t, x->right);
  free(x);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  //특정 노드가 있는 트리인 상태에서 삭제하려 할 때
  if (t->root != t->nil) {
    remove_node(t, t->root);
  }
  //비어있지 않은 트리 상태
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  //한 노드 생성
  node_t * new_node = (node_t *)malloc(sizeof(node_t));
  new_node->parent = t->nil;
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->key = key;
  new_node->color = RBTREE_RED;

  //아무것도 rbtree의 노드가 없을 때 첫 노드 삽입
  if (t->root == t->nil) {
    t->root = new_node;
    t->root->color = RBTREE_BLACK;
    return t->root;
  }

  //BST 위치 탐색
  //기존에 최소 하나의 노드 이상 존재하는 경우
  node_t * current = t->root;
  node_t * prev = t->nil;
  while (current != t->nil) {
    if (key > current->key) {
      prev = current;
      current = current->right;
    } else {
      prev = current;
      current = current->left;
    }
  }

  //current에는 insert할 위치 노드를 가리키고 있는 상태
  new_node->parent = prev;
  if (new_node->key < prev->key) {
    prev->left = new_node;
  } else {
    prev->right = new_node;
  }

  //새로 들어온 노드 색을 확인하고, 특정 로직 수행
  rb_insert_fixup(t, new_node);
 
  return t->root;
}

//고쳐주는 함수
void rb_insert_fixup(rbtree *t, node_t *z) {
  //이 안에서 회전함
  //이미 삽입을 할 때, RBTree 조건이 만족하는 상태에서 들어오는 것이므로
  node_t * y; 

  while(z->parent->color == RBTREE_RED) {
    // if (z->parent->color == RBTREE_BLACK) {
    //   break;
    // }
    //방향 확인
    if (z->parent == z->parent->parent->left) { //부모가 할아버지의 왼쪽 자식
      y = z->parent->parent->right; //삼촌
      
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        //할아버지에서 다시 확인 필요
        z = z->parent->parent;
      } else if (z == z->parent->right) { //삼촌 색이 검정색인데, 꺾여있는 경우
        //LEFT-ROTATE 함수의 매개변수가 부모 노드 포인터이므로 z를 업데이트
          z = z->parent;
          left_rotate(t, z);
      }
      z->parent->parent->color = RBTREE_RED;
      z->parent->color = RBTREE_BLACK;
      right_rotate(t, z->parent->parent);
    } else {
        y = z->parent->parent->left; // 삼촌

        if (y->color == RBTREE_RED) {
          z->parent->color = RBTREE_BLACK;
          y->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          //할아버지에서 다시 확인 필요
          z = z->parent->parent;
        } else if (z == z->parent->left) {
            z = z->parent;
            right_rotate(t, z);
        }
        z->parent->parent->color = RBTREE_RED;
        z->parent->color = RBTREE_BLACK;
        left_rotate(t, z->parent->parent);
    } 
  }
    
  //루트를 바꿔도 다른 조건들에 영향을 끼치지 않으므로
  //마지막에 레드인 경우만 색상 변경함
  t->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *t, node_t *x) {
  node_t* y = x->right;
  x->right = y->left;
  //자식의 원래 자식 확인(nil 여부)
  if (y->left != t->nil) {
    y->left->parent = x;
  } 
  //부모 바꿔주기
  y->parent = x->parent;

  //맨 위 노드가 루트 노드인지 여부 확인
  if (x->parent == t->nil) {  //원래 최상단 노드가 루트노드
    t->root = y;
  } else if (x == x->parent->left) {  //원래 최상단 부모랑 x가 어느 방향 연결인지 확인. 일단 만약 left에 x가 연결되어 있다면
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y; 
}

void right_rotate(rbtree *t, node_t *y) {
  node_t* x = y->left;
  y->left = x->right;
  //자식의 원래 자식 확인(nil 여부)
  if (x->right != t->nil) {
    x->right->parent = y; 
  }
  //부모 바꿔주기
  x->parent = y->parent;

  //맨 위 노드가 루트 노드인지 여부인지 확인
  if (y->parent == t->nil) {
    t->root = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}



//검색
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t * temp = t->root;

  while (temp != t->nil) {
    if (key == temp->key)
      return temp;
       
    if (temp->key < key) {
      temp = temp->right;
    } else {
      temp = temp->left;
    }
  }
  return temp;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t * temp = t->root;
  while (temp->left != t->nil) {
    temp = temp->left;
    // if (temp->left != NULL) {
    //   temp = temp->left;
    // }
  }
  return temp;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t * temp = t->root;
  while (temp->right != t->nil) {
    temp = temp->right;
  }
  return temp;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
