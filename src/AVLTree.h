/*
 * 3dfm - 3D File Manager
 * $Id: AVLTree.h,v 1.6 2003/04/03 01:12:27 adam Exp $
 */

#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#include <stdio.h>
//#include <FFolder.h>
class FFSObject;

#define AVL_CMP_ALL  0  /* compare on all criteria */
#define AVL_CMP_NAME 1  /* compare only on name */

typedef struct AVLNode_t {
    FFSObject *elem;
    int bal;
    struct AVLNode_t *left;
    struct AVLNode_t *right;
} AVLNode;

typedef struct AVLListNode_t {
    FFSObject *f;
    struct AVLListNode_t *next;
} AVLListNode;

class AVLTree {
 public:
    AVLTree();
    void insert(FFSObject *e);
#ifdef _DEBUG_AVL
    static int list_cnt;
    static int tree_cnt;
#endif
    void makeList(void);
    FFSObject *getNext(void);
    int getCount(void);
    void makeEmpty(void);
    void setCompare(int compareType);
    void checkMRUs(void);
 private:
    int compareType;
    AVLNode *root;
    int count;
    int madeList;
    AVLListNode *head;
    AVLListNode *last;
    AVLListNode *curr;
    void rotateLeft(AVLNode* &p);
    void rotateRight(AVLNode* &p);
    int ins(AVLNode* &p, FFSObject *e);
    void addToList(FFSObject *elem);
    void traverse(const AVLNode *p);
    void empty(AVLNode* &p);
};

#endif /* AVL */

