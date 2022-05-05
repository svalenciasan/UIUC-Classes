/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    functionCalls.push_back("rotateLeft"); // Stores the rotation name (don't remove this)
    // your code here
    Node* y = t->right;
    t->right = y->left;
    y->left = t;
    
    //Update Height
    t->height = std::max(heightOrNeg1(t->left), heightOrNeg1(t->right)) + 1;
    t = y;
    y->height = std::max(heightOrNeg1(y->left), heightOrNeg1(y->right)) + 1;
}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    functionCalls.push_back("rotateLeftRight"); // Stores the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    functionCalls.push_back("rotateRight"); // Stores the rotation name (don't remove this)
    // your code here
    Node* y = t->left;
    t->left = y->right;
    y->right = t;
    
    //Update Height
    t->height = std::max(heightOrNeg1(t->left), heightOrNeg1(t->right)) + 1;
    t = y;
    y->height = std::max(heightOrNeg1(y->left), heightOrNeg1(y->right)) + 1;
}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    functionCalls.push_back("rotateRightLeft"); // Stores the rotation name (don't remove this)
    // your code here
    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    // your code here
    if (subtree == nullptr) {
        return;
    }

    int balance = heightOrNeg1(subtree->right) - heightOrNeg1(subtree->left);

    if (balance == -2) {
        int leftBalance = heightOrNeg1(subtree->left->right) - heightOrNeg1(subtree->left->left);
        if (leftBalance < 0) {
            rotateRight(subtree);
        } else {
            rotateLeftRight(subtree);
        }
    } else if (balance == 2){
        int rightBalance = heightOrNeg1(subtree->right->right) - heightOrNeg1(subtree->right->left);
        if (rightBalance > 0) {
            rotateLeft(subtree);
        } else {
            rotateRightLeft(subtree);
        }
    }

    //subtree->height = std::max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right));
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    // your code here
    if (subtree == nullptr) {
        subtree = new Node(key, value);
    } else if (key > subtree->key) {
        insert(subtree->right, key, value);
        rebalance(subtree);
    } else {
        insert(subtree->left, key, value);
        rebalance(subtree);
    }
    subtree->height = 1 + std::max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right));
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL) {
        return;
    }
    if (key < subtree->key) {
        // your code here
        remove(subtree->left, key);
    } else if (key > subtree->key) {
        // your code here
        remove(subtree->right, key);
    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            // your code here
            delete subtree;
            subtree = nullptr;
            return;
        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            // your code here
            // Node*& iop = subtree->left;
            // while(iop != nullptr) {
            //     iop = iop->right;
            // }
            // swap(subtree, iop);
            // delete iop;
            
            Node* iop = subtree->left;
            while (iop->right != NULL) {
                iop = iop->right;
            }
            swap(iop, subtree);
            remove(subtree->left, key);
        } else {
            /* one-child remove */
            // your code here
            if (subtree->left == nullptr) {
                Node* tmp = subtree;
                subtree = subtree->right;
                delete tmp;
                tmp = nullptr;
            } else {
                Node* tmp = subtree;
                subtree = subtree->left;
                delete tmp;
                tmp = nullptr;
            }
        }
        // your code here
        // rebalance(subtree);
    }
    subtree->height = 1 + std::max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right));
    rebalance(subtree);
}