#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == nullptr){
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return; 
    }

    AVLNode<Key, Value>* active = static_cast<AVLNode<Key, Value>*>(this->root_);
    Key aKey = new_item.first; 
    AVLNode<Key, Value>* activeTraverser; 
    while (true){
        if (active->getLeft() == nullptr && aKey < active->getKey()){
            active->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, active)); 
            activeTraverser = active->getLeft(); 
            break;
        }
        else if (active->getRight() == nullptr && aKey > active->getKey()){
            active->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, active)); 
            activeTraverser = active->getRight(); 
            break;
        }
        else if (active->getLeft() != nullptr && aKey < active->getKey()){
            active = active->getLeft(); 
        }
        else if (active->getRight() != nullptr && aKey > active->getKey()){
            active = active->getRight(); 
        }
        else {
            active->setValue(new_item.second);
            return;
        }
    }
    
    AVLNode<Key, Value>* child = activeTraverser;
    AVLNode<Key, Value>* parent = child->getParent();

    while (parent != nullptr) {
        
        if (parent->getLeft() == child) {
            parent->setBalance(parent->getBalance() - 1);
        } else {
            parent->setBalance(parent->getBalance() + 1);
        }

        if (parent->getBalance() == 0) {
            break; 
        }

        AVLNode<Key, Value>* activeTraverser = parent;

        if (activeTraverser->getBalance() == 2){ // heavier right side
            int rChild = activeTraverser->getRight()->getBalance(); 
            if (rChild == 1) { // RR rotation
                AVLNode<Key, Value>* child = activeTraverser->getRight();
                activeTraverser->setRight(child->getLeft());
                if (activeTraverser->getRight() != nullptr){
                    activeTraverser->getRight()->setParent(activeTraverser);
                }
                child->setLeft(activeTraverser); 

                if(activeTraverser->getParent() == nullptr){
                    child->setParent(nullptr);
                    this->root_ = child;
                }
                else {
                    child->setParent(activeTraverser->getParent());
                    if (child->getParent()->getLeft() == activeTraverser){
                        child->getParent()->setLeft(child);
                    }
                    else {
                        child->getParent()->setRight(child);
                    }
                }
                activeTraverser->setParent(child);
                activeTraverser->setBalance(0);
                child->setBalance(0); 
            }
            else { // RL rotation
                AVLNode<Key, Value>* z = activeTraverser;
                AVLNode<Key, Value>* c = z->getRight();
                AVLNode<Key, Value>* g = c->getLeft();
                
                AVLNode<Key, Value>* t2 = g->getLeft();
                AVLNode<Key, Value>* t3 = g->getRight();

                g->setLeft(z);
                g->setRight(c);
                z->setRight(t2);
                c->setLeft(t3); 

                if (t2 != nullptr) { t2->setParent(z); }
                if (t3 != nullptr) { t3->setParent(c); }

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) {
                    this->root_ = g;
                } else {
                    if (g->getParent()->getLeft() == z) g->getParent()->setLeft(g);
                    else g->getParent()->setRight(g);
                }
                
                z->setParent(g);
                c->setParent(g);

                if (g->getBalance() == -1) { z->setBalance(0); c->setBalance(1); }
                else if (g->getBalance() == 1) { z->setBalance(-1); c->setBalance(0); }
                else { z->setBalance(0); c->setBalance(0); }
                g->setBalance(0);
            }
            break; // A rotation always finishes the balancing for insert
        }
        else if (activeTraverser->getBalance() == -2){ // heavier left side
            int lChild = activeTraverser->getLeft()->getBalance(); 
            if (lChild == 1) { // LR rotation
                AVLNode<Key, Value>* z = activeTraverser;
                AVLNode<Key, Value>* c = z->getLeft();
                AVLNode<Key, Value>* g = c->getRight();
                
                AVLNode<Key, Value>* t2 = g->getLeft();
                AVLNode<Key, Value>* t3 = g->getRight();

                g->setLeft(c);
                g->setRight(z);
                c->setRight(t2);
                z->setLeft(t3); 

                if (t2 != nullptr) { t2->setParent(c); }
                if (t3 != nullptr) { t3->setParent(z); }

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) {
                    this->root_ = g;
                } else {
                    if (g->getParent()->getLeft() == z) g->getParent()->setLeft(g);
                    else g->getParent()->setRight(g);
                }

                z->setParent(g);
                c->setParent(g);

                if (g->getBalance() == 1) { z->setBalance(0); c->setBalance(-1); }
                else if (g->getBalance() == -1) { z->setBalance(1); c->setBalance(0); }
                else { z->setBalance(0); c->setBalance(0); }
                g->setBalance(0);
            }
            else { // LL rotation
                AVLNode<Key, Value>* child = activeTraverser->getLeft();
                activeTraverser->setLeft(child->getRight());
                if (activeTraverser->getLeft() != nullptr){
                    activeTraverser->getLeft()->setParent(activeTraverser);
                }
                child->setRight(activeTraverser); 

                if(activeTraverser->getParent() == nullptr){
                    child->setParent(nullptr);
                    this->root_ = child;
                }
                else {
                    child->setParent(activeTraverser->getParent());
                    if (child->getParent()->getLeft() == activeTraverser){
                        child->getParent()->setLeft(child);
                    }
                    else {
                        child->getParent()->setRight(child);
                    }
                }
                activeTraverser->setParent(child);
                activeTraverser->setBalance(0);
                child->setBalance(0); 
            }
            break; /
        }

        child = parent;
        parent = parent->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
     AVLNode<Key, Value>* foundKey = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (foundKey == nullptr) {
        return;
    }

    if (foundKey->getLeft() != nullptr && foundKey->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(foundKey));
        this->nodeSwap(foundKey, pred);
    }

    AVLNode<Key, Value>* child = nullptr;
    if (foundKey->getLeft() != nullptr) {
        child = foundKey->getLeft();
    } else {
        child = foundKey->getRight();
    }

    AVLNode<Key, Value>* parent = foundKey->getParent();
    int8_t diff = 0;

    if (parent == nullptr) {
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == foundKey) {
            parent->setLeft(child);
            diff = 1;
        } else {
            parent->setRight(child);
            diff = -1;
        }
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    delete foundKey;

    AVLNode<Key, Value>* curr = parent;
    while (curr != nullptr) {
        curr->updateBalance(diff);

        if (curr->getBalance() == 1 || curr->getBalance() == -1) {
            break; 
        }

        if (curr->getBalance() == 2) { // Right-heavy
            AVLNode<Key, Value>* c = curr->getRight(); // child
            
            if (c->getBalance() == 1 || c->getBalance() == 0) {
                AVLNode<Key, Value>* z = curr;
                z->setRight(c->getLeft());
                if (z->getRight() != nullptr){
                    z->getRight()->setParent(z);
                }
                c->setLeft(z); 

                if(z->getParent() == nullptr){
                    c->setParent(nullptr);
                    this->root_ = c;
                }
                else {
                    c->setParent(z->getParent());
                    if (c->getParent()->getLeft() == z){
                        c->getParent()->setLeft(c);
                    }
                    else {
                        c->getParent()->setRight(c);
                    }
                }
                z->setParent(c);
                
                if (c->getBalance() == 0) {
                    z->setBalance(1);
                    c->setBalance(-1);
                    break; // Height stabilized
                } else {
                    z->setBalance(0);
                    c->setBalance(0);
                }
            } 
            else { // RL Case
                AVLNode<Key, Value>* z = curr;
                AVLNode<Key, Value>* g = c->getLeft(); // grandchild

                c->setLeft(g->getRight());
                if (c->getLeft() != nullptr) { c->getLeft()->setParent(c); }
                g->setRight(c);
                c->setParent(g);

                z->setRight(g);
                g->setParent(z); 

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) { this->root_ = g; }
                else {
                    if (g->getParent()->getLeft() == z) g->getParent()->setLeft(g);
                    else g->getParent()->setRight(g);
                }
                z->setParent(g);

                if (g->getBalance() == -1) { z->setBalance(0); c->setBalance(1); }
                else if (g->getBalance() == 1) { z->setBalance(-1); c->setBalance(0); }
                else { z->setBalance(0); c->setBalance(0); }
                g->setBalance(0);
            }
        } 
        else if (curr->getBalance() == -2) { // Left-heavy
            AVLNode<Key, Value>* c = curr->getLeft(); // child
            
            // LL Case (or L0 case)
            if (c->getBalance() == -1 || c->getBalance() == 0) {
                AVLNode<Key, Value>* z = curr;

                z->setLeft(c->getRight());
                if (z->getLeft() != nullptr){
                    z->getLeft()->setParent(z);
                }
                c->setRight(z); 

                if(z->getParent() == nullptr){
                    c->setParent(nullptr);
                    this->root_ = c;
                }
                else {
                    c->setParent(z->getParent());
                    if (z->getParent()->getLeft() == z){
                        c->getParent()->setLeft(c);
                    }
                    else {
                        c->getParent()->setRight(c);
                    }
                }
                z->setParent(c);

                if (c->getBalance() == 0) {
                    z->setBalance(-1);
                    c->setBalance(1);
                    break; // Height stabilized
                } else {
                    z->setBalance(0);
                    c->setBalance(0);
                }
            } 
            else { // LR Case (c->getBalance() == 1)
                AVLNode<Key, Value>* z = curr;
                AVLNode<Key, Value>* g = c->getRight(); // grandchild

                c->setRight(g->getLeft());
                if (c->getRight() != nullptr) { c->getRight()->setParent(c); }
                g->setLeft(c);
                c->setParent(g);

                z->setLeft(g);
                g->setParent(z);

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) { this->root_ = g; }
                else {
                    if (g->getParent()->getLeft() == z) g->getParent()->setLeft(g);
                    else g->getParent()->setRight(g);
                }
                z->setParent(g);

                if (g->getBalance() == 1) { z->setBalance(0); c->setBalance(-1); }
                else if (g->getBalance() == -1) { z->setBalance(1); c->setBalance(0); }
                else { z->setBalance(0); c->setBalance(0); }
                g->setBalance(0);
            }
        }

        AVLNode<Key, Value>* nextParent = curr->getParent();
        if (nextParent != nullptr) {
            if (nextParent->getLeft() == curr) {
                diff = 1;
            } else {
                diff = -1;
            }
        }
        curr = nextParent;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
