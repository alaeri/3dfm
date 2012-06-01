/*
 * 3dfm - 3D File Manager
 * $Id: AVLTree.cpp,v 1.7 2003/04/09 01:55:38 adam Exp $
 */

/* system includes */
#include <stdio.h>

/* 3dfm includes */
#include <AVLTree.h>
#include <FFSObject.h>
#include <Prefs.h>

#ifdef _DEBUG_AVL
int AVLTree::list_cnt = 0;
int AVLTree::tree_cnt = 0;
#endif

AVLTree::AVLTree() : root(NULL), count(0), madeList(0), 
		     compareType(AVL_CMP_ALL) {
}

void AVLTree::insert(FFSObject *e) {
    if (madeList) { 
	return;
    }
    ins(root,e);
    count++;
}

void AVLTree::makeEmpty(void) {
    if (madeList) {
	AVLListNode *p = head;
	AVLListNode *q;
	while (p != NULL) {
	    q = p->next;
	    delete p;
#ifdef _DEBUG_AVL
	    AVLTree::list_cnt--;
#endif
	    p = q;
	}
    }
    empty(root);
}

void AVLTree::setCompare(int compareType) {
    this->compareType = compareType;
}

void AVLTree::rotateLeft(AVLNode* &p) {
    AVLNode *q = p;
    p = p->right;
    q->right = p->left;
    p->left = q;
    q->bal--;
    if (p->bal > 0) {
	q->bal -= p->bal;
    }
    p->bal--;
    if (q->bal < 0) {
	p->bal += q->bal;
    }
}

void AVLTree::rotateRight(AVLNode* &p) {
    AVLNode *q = p;
    p = p->left;
    q->left = p->right;
    p->right = q;
    q->bal++;
    if (p->bal < 0) {
	q->bal -= p->bal;
    }
    if (q->bal > 0) {
	p->bal += q->bal;
    }
}

void AVLTree::empty(AVLNode* &p) {
    if (p != NULL) {
	empty(p->left);
	empty(p->right);
	if (p->elem) {
	    delete p->elem;
	}
	delete p;
#ifdef _DEBUG_AVL
	AVLTree::tree_cnt--;
#endif
    }
    p = NULL;
}

int AVLTree::ins(AVLNode* &p, FFSObject *e) {
    int deltaH = 0;
    if (p == NULL) {
	p = new AVLNode;
#ifdef _DEBUG_AVL
	AVLTree::tree_cnt++;
#endif
	p->elem = e;
	p->bal = 0;
	p->left = p->right = NULL;
	deltaH = 1;  /* tree hight increased by 1 */
#ifdef _DEBUG_AVL
	fprintf(stderr, "added %s to AVLTree (count %d)\n", 
		e->getName(), count);
#endif
    } else if (compareType == AVL_CMP_NAME
	       ? e->compareName(p->elem) > 0
	       : e->compareAll(p->elem) > 0) {
	if (ins(p->right, e)) {
	    p->bal++; /* height of right subtree increased */
	    if (p->bal == 1) {
		deltaH = 1;
	    } else if (p->bal == 2) {
		if (p->right->bal == -1) {
		    rotateRight(p->right);
		}
		rotateLeft(p);
	    }
	}
    } else /* if (e->compareAll(p->elem) <= 0)*/ {
	if (ins(p->left, e)) {
	    p->bal--;
	    if (p->bal == -1) {
		deltaH = 1;
	    } else if (p->bal == -2) {
		if (p->left->bal == 1) {
		    rotateLeft(p->left);
		}
		rotateRight(p);
	    }
	}
    }
    return deltaH;
}
	
void AVLTree::makeList(void) {

    /* if we already made a list, just rewind it */
    if (madeList) {
	curr = head->next;
	return;
    }

    /* made a list */
    if (root != NULL) {
	head = new AVLListNode;
#ifdef _DEBUG_AVL
	AVLTree::list_cnt++;
#endif
	head->f = root->elem;
	head->next = NULL;
	last = head;
	traverse(root);
	curr = head->next;
	madeList = 1;
	checkMRUs();
    } else {
	curr = NULL;
    }

}

void AVLTree::traverse(const AVLNode *p) {
    if (p != NULL) {
	traverse(p->left);
	addToList(p->elem);
	traverse(p->right);
    }
}

void AVLTree::addToList(FFSObject *elem) {
    last->next = new AVLListNode;
#ifdef _DEBUG_AVL
    AVLTree::list_cnt++;
#endif
    last = last->next;
    last->f = elem;
    last->next = NULL;
}

FFSObject *AVLTree::getNext(void) {
    /* do not allow on empty lists */
    if (!madeList) {
	return NULL;
    }

    if (curr != NULL) {
	AVLListNode *p = curr;
	curr = curr->next;
	return p->f;
    } else {
	return NULL;
    }
}

int AVLTree::getCount(void) {
    return count;
}

void AVLTree::checkMRUs(void) {

    if (!madeList) {
	return;
    }

    int mru_cnt = 0;
    AVLListNode *curr = head;
    AVLListNode *prev = head;
    AVLListNode *offender = NULL;
    while (curr != NULL) {
	if ((Prefs::getMRU(curr->f->getName()) != 0) 
 	    && (++mru_cnt > Prefs::mru_max)) {
 	    offender = curr;
 	    prev->next = curr->next;
 	    break;
 	}
 	prev = curr;
 	curr = curr->next;
    }

    /* if we don't have an offender, we are done checking */
    if (offender == NULL) {
 	return;
    }

    Prefs::removeMRU(offender->f->getName());

    curr = curr->next;
    while ((curr != NULL) && (curr->f->compareName(offender->f) <= 0)) {
 	prev = curr;
 	curr = curr->next;
    }

    if (curr == prev) {
 	head = offender;
 	offender->next = curr;
     } else {
 	offender->next = curr;
 	prev->next = offender;
    }

}
