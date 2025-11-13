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
            break;
        }
    }
    
    while(activeTraverser->getParent() != nullptr){
        if (activeTraverser->getParent()->getLeft() == activeTraverser){
            activeTraverser = activeTraverser->getParent(); 
            activeTraverser->setBalance(activeTraverser->getBalance() - 1);
        }
        else {
            activeTraverser = activeTraverser->getParent(); 
            activeTraverser->setBalance(activeTraverser->getBalance() + 1);
        }

        if (activeTraverser->getBalance() == 0){
            break; // tree is balanced yippee!
        }
        else if (activeTraverser->getBalance() == 2){ // heavier right side
            int rChild = activeTraverser->getRight()->getBalance(); 
            if (rChild == 1) { // LL rotation
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
                    activeTraverser->setParent(child);
                }
                activeTraverser->setBalance(0);
                child->setBalance(0); 
            }
            else { // RL rotation
                AVLNode<Key, Value>* z = activeTraverser;
                AVLNode<Key, Value>* c = z->getRight();
                AVLNode<Key, Value>* g = c->getLeft();

                c->setLeft(g->getRight());
                if (c->getLeft() != nullptr) {
                    c->getLeft()->setParent(c);
                }
                g->setRight(c);
                c->setParent(g);

                z->setRight(g);
                g->setParent(z); 

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) {
                    this->root_ = g;
                }
                else {
                    if (g->getParent()->getLeft() == z) {
                        g->getParent()->setLeft(g);
                    } else {
                        g->getParent()->setRight(g);
                    }
                }

                z->setParent(g);
                c->setParent(g); // c's parent was already set to g

                if (g->getBalance() == -1) { // g was left-heavy
                    z->setBalance(0);
                    c->setBalance(1);
                } else if (g->getBalance() == 1) { // g was right-heavy
                    z->setBalance(-1);
                    c->setBalance(0);
                } else { // g was 0 (the new node itself)
                    z->setBalance(0);
                    c->setBalance(0);
                }
                g->setBalance(0); // The new root of this subtree is now balanced
            }
        }
        else if (activeTraverser->getBalance() == -2){ // heavier left side
            int lChild = activeTraverser->getLeft()->getBalance(); 
            if (lChild == 1) { // LR rotation
                AVLNode<Key, Value>* z = activeTraverser;
                AVLNode<Key, Value>* c = z->getLeft();
                AVLNode<Key, Value>* g = c->getRight();

                c->setRight(g->getLeft());
                if (c->getRight() != nullptr) {
                    c->getRight()->setParent(c);
                }
                g->setLeft(c);
                c->setParent(g);

                z->setLeft(g);
                g->setParent(z); 

                g->setParent(z->getParent());
                if (g->getParent() == nullptr) {
                    this->root_ = g; // g is the new root of the whole tree
                } 
                else {
                    if (g->getParent()->getLeft() == z) {
                        g->getParent()->setLeft(g);
                    } else {
                        g->getParent()->setRight(g);
                    }
                }

                z->setParent(g);
                c->setParent(g); // c's parent was already set to g

                if (g->getBalance() == 1) { // g was right-heavy
                    z->setBalance(0);
                    c->setBalance(-1);
                } else if (g->getBalance() == -1) { // g was left-heavy
                    z->setBalance(1);
                    c->setBalance(0);
                } else { // g was 0 (the new node itself)
                    z->setBalance(0);
                    c->setBalance(0);
                }
                g->setBalance(0); // The new root of this subtree is now balanced
            }
            else { // RR rotation
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
                    if (child->getParent()->getRight() == activeTraverser){
                        child->getParent()->setLeft(child);
                    }
                    else {
                        child->getParent()->setLeft(child);
                    }
                    activeTraverser->setParent(child);
                }
                activeTraverser->setBalance(0);
                child->setBalance(0); 
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
     Node<Key, Value>* foundKey = internalFind(key);

    if (foundKey == nullptr) { // empty case
        return;
    }

    if (foundKey->getLeft() != nullptr && foundKey->getRight() != nullptr) { // two child case, swap and then no longer two child
        Node<Key, Value>* pred = predecessor(foundKey);
        nodeSwap(foundKey, pred);
    }

    Node<Key, Value>* child = nullptr; // child since will only have one or zero child
    if (foundKey->getLeft() != nullptr) {
        child = foundKey->getLeft();
    } else {
        child = foundKey->getRight();
    }

    Node<Key, Value>* parent = foundKey->getParent();

    if (parent == nullptr) { // root case
        root_ = child;
    }
    else { // not root
        if (parent->getLeft() == foundKey) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }

    if (child != nullptr) { // sets child up
        child->setParent(parent);
    }

    delete foundKey;

    
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
