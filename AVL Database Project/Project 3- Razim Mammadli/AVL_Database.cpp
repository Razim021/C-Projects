//Razim Mammadli
//U95773780
/*In this project, I implemented a rudimentary Database Management System (https:// en.wikipedia.org/wiki/Database) 
to facilitate INSERT, UPDATE, SEARCH, and DELETE functionalities (utilizing an underlying AVL tree structure).
*/
// AVL_Database.cpp
#include "AVL_Database.hpp"
#include <algorithm>
#include <cmath>

Record::Record(const std::string& k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr), nodeCount(0), searchComparisonCount(0) {}

int AVLTree::height(AVLNode* node) {
    return node ? node->height : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }
}

int AVLTree::getBalance(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}


//This function finds the leftmost (minimum) node in the tree, used in delete operation.
AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    // Loop down to find the leftmost leaf
    while (current && current->left) {
        current = current->left;
    }
    return current;
}
// Rotates the tree right at the given node to maintain AVL balance
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // we perform rotation.
    x->right = y;
    y->left = T2;

    // and after, update heights
    updateHeight(y);
    updateHeight(x);

    // finally, return new root
    return x;
}

//This function performs a left rotation on the tree to maintain AVL balance.
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    //at first, we perform rotation.
    y->left = x;
    x->right = T2;

    // and update heights here.
    updateHeight(x);
    updateHeight(y);

    // at the end, return new root of subtree.
    return y;
}

// This is the recursive helper function to insert a new record into the AVL tree.
AVLNode* AVLTree::insertHelper(AVLNode* node, Record* record) {
    if (!node) {
        nodeCount++;
        return new AVLNode(record); // Our base case is to insert at a leaf,
    }

    // Recursive case is traverse left or right depending on the record's value
    if (record->value < node->record->value) {
        node->left = insertHelper(node->left, record);
    } else if (record->value > node->record->value) {
        node->right = insertHelper(node->right, record);
    } else {
        return node; // Duplicate values are not allowed.
    }

    // After insertion, we update the height and balance the tree.
    updateHeight(node);
    int balance = getBalance(node);

    // we balance the tree if needed (4 cases). 
    if (balance > 1 && record->value < node->left->record->value)
        return rotateRight(node); // Left heavy, rotate right
    if (balance < -1 && record->value > node->right->record->value)
        return rotateLeft(node); // Right heavy, rotate left
    if (balance > 1 && record->value > node->left->record->value) {
        node->left = rotateLeft(node->left);
        return rotateRight(node); // Left-right case, rotate left then right
    }
    if (balance < -1 && record->value < node->right->record->value) {
        node->right = rotateRight(node->right); // Right-left case, rotate right then left
        return rotateLeft(node);
    }

    return node; // at the end we return the (possibly new) root of the subtree. 
}

// Public method to insert a record into the AVL tree
void AVLTree::insert(Record* record) {
    root = insertHelper(root, record);
}


// This is recursive helper function to delete a node with the given key and value
AVLNode* AVLTree::deleteHelper(AVLNode* node, const std::string& key, int value) {
    if (!node) return node; // Base case: node not found
    // Here, traversing left or right based on the value to find the node to delete
    if (value < node->record->value) {
        node->left = deleteHelper(node->left, key, value);
    } else if (value > node->record->value) {
        node->right = deleteHelper(node->right, key, value);
    } else {
        // Now we found node with the value, should delete it.
        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node; // If node has no children, just delete it.
                node = nullptr;
            } else {
                *node = *temp; // If node has one child, replace node with child.
            }
            delete temp->record;
            delete temp;
            nodeCount--; //Decrease node count
        } else {
            // if node is with two children: we get the inorder successor (min value node).
            AVLNode* temp = minValueNode(node->right);
            node->record = temp->record; // Copy the inorder successor's record.
            node->right = deleteHelper(node->right, temp->record->key, temp->record->value);
        }
    }
    //If the node is null, return it.
    if (!node) return node;
    
    // We update the height and balance the tree after deletion.
    updateHeight(node);
    int balance = getBalance(node);

    //Balance the tree if needed (4 cases)
    if (balance > 1 && getBalance(node->left) >= 0)
        return rotateRight(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
        return rotateLeft(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Public method to delete a node with the given key and value.
void AVLTree::deleteNode(const std::string& key, int value) {
    root = deleteHelper(root, key, value);
}


// This one is recursive helper function to search for a node with a given key and value.
AVLNode* AVLTree::searchHelper(AVLNode* node, const std::string& key, int value) const {
    if (!node) return nullptr; //bas case, if node not found.

    searchComparisonCount++; // Increment comparison count for each node visited.
    if (value == node->record->value && key == node->record->key)
        return node; // Record found

    // Traverse left or right based on the value to find the node
    if (value < node->record->value)
        return searchHelper(node->left, key, value);

    return searchHelper(node->right, key, value);
}

// Public method to search for a record with a given key and value
Record* AVLTree::search(const std::string& key, int value) {
    searchComparisonCount = 0; // Reset search comparison count
    AVLNode* result = searchHelper(root, key, value); // Search recursively
    return result ? result->record : new Record("", 0); // Return record if found, else empty record
}


// IndexedDatabase Implementation
void IndexedDatabase::insert(Record* record) {
    index.insert(record);
}

Record* IndexedDatabase::search(const std::string& key, int value) {
    return index.search(key, value);
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);
}



/* RangeQuery Hints
1. Base: if (!node) return
2. Key Traversal Logic:
   - If value >= start: check left subtree
   - If start <= value <= end: add to result
   - If value <= end: check right subtree
*/


// this is recursive helper function for range query to find records within the given range. 
void IndexedDatabase::rangeQueryHelper(AVLNode* node, int start, int end, std::vector<Record*>& result) const {
    if (!node) return; // Base case is when node is null.

    // If the value of the current node is within the range, we will add it to the result. 
    if (node->record->value >= start && node->record->value <= end)
        result.push_back(node->record);

    // If the value is greater than the start, we explore the left subtree.
    if (node->record->value > start)
        rangeQueryHelper(node->left, start, end, result);

    // If the value is less than the end, we explore the right subtree.
    if (node->record->value < end)
        rangeQueryHelper(node->right, start, end, result);
}
// Public method for range query to get records within the given range
std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end){
    std::vector<Record*> result;
    rangeQueryHelper(index.root, start, end, result); // Perform range query
    return result;
}

void IndexedDatabase::clearHelper(AVLNode* node) {
    if (!node) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node->record;
    delete node;
}

void IndexedDatabase::clearDatabase() {
    clearHelper(index.root);
    index.root = nullptr;
}

int IndexedDatabase::calculateHeight(AVLNode* node) const {
    if (!node) return 0;
    return 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));
}

int IndexedDatabase::getTreeHeight() const {
    return calculateHeight(index.root);
}

int IndexedDatabase::getSearchComparisons(const std::string& key, int value) {
    search(key, value);
    return index.getLastSearchComparisons();
}