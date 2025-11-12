#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathLength(Node* node){
    if (node == nullptr){ // empty node
        return 0; 
    }
    else {
        int left = pathLength(node->left); 
        int right = pathLength(node->right);
        if (left == -1 || right == -1){
            return -1; 
        }
        else if (left == 0 && right == 0){
            return 1; 
        }
        else if (left != 0 && right != 0 && left != right){
            return -1; 
        }
        else {
            if(left > right){
                return 1 + left;
            }
            else {
                return 1 + right; 
            }
        }
    }
}


bool equalPaths(Node * root)
{
    if (root == nullptr){
        return true; 
    }
    
    if (pathLength(root) == -1){
        return false; 
    }
    else {
        return true; 
    }
}

