// priorityqueue.h
//
// TODO:  write this header comment
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <set>

using namespace std;

template<typename T>
class priorityqueue {
 private:
    struct NODE {
        int priority;  // used to build BST
        T value;  // stored data for the p-queue
        bool dup;  // marked true when there are duplicate priorities
        NODE* parent;  // links back to parent
        NODE* link;  // links to linked list of NODES with duplicate priorities
        NODE* left;  // links to left child
        NODE* right;  // links to right child
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of elements in the pqueue
    NODE* curr;  // pointer to next item in pqueue (see begin and next)
    // private helper function for toString() method
    void inOrder(NODE* n, stringstream& ss) {
        if (n == nullptr) return;
        // first go left
        inOrder(n->left, ss);
        // get data of node
        ss << n->priority << " value: " << n->value << endl;
        // check for duplicates
        if (n->dup) {
            NODE* c = n->link;
            while (c != nullptr) {
                // add each duplicate node to the stream
                ss << c->priority << " value: " << c->value << endl;
                c = c->link;
            }
        }
        // go right
        inOrder(n->right, ss);
    }
    // private helper function for clear() method
    void postOrderDelete(NODE* n) {
        if (n == nullptr) {
            return;
        }
        // go through left subtrees
        postOrderDelete(n->left);
        // go through right subtrees
        postOrderDelete(n->right);
        // deal with node
        // first check if node has duplicates
        if (n->dup) {
            NODE* c = n->link;
            NODE* next = c;
            // delete each duplicate
            while (c != nullptr) {
                next = c->link;
                delete c;
                c = next;
            }
        }
        // delete the node
        delete n;
    }
    // private helper function for operator= method
    void preOrderCopy(NODE* n) {
        if (n == nullptr) {
            return;
        }
        // copy node
        enqueue(n->value, n->priority);
        // check for duplicates
        if (n->dup) {
            NODE* c = n->link;
            while (c != nullptr) {
                enqueue(c->value, c->priority);
                c = c->link;
            }
        }
        // go through left subtree
        preOrderCopy(n->left);
        // go through right subtree
        preOrderCopy(n->right);
    }
    bool equal(NODE* cur, NODE* otherCur) const {
        if (cur == nullptr && otherCur == nullptr) {
            // both empty
            return true;
        } else if (cur == nullptr) {
            // one of the trees are empty
            return false;
        } else if (otherCur == nullptr) {
            // one of the trees are empty
            return false;
        }
        // if there are duplicates, compare each node in the linked lists
        if (cur->dup && otherCur->dup) {
            NODE* c = cur;
            NODE* oC = otherCur;
            while (c != nullptr) {
                if (c->value != oC->value) {
                    return false;
                }
                c = c->link;
                oC = oC->link;
            }
        } else if (cur->dup || otherCur->dup) {
            return false;
        }
        return (cur->value == otherCur->value && equal(cur->left, otherCur->left) && equal(cur->right, otherCur->right));
    }
public:

    //
    // default constructor:
    //
    // Creates an empty priority queue.
    // O(1)
    //
    priorityqueue() {
        root = nullptr;
        size = 0;
        curr = nullptr;
    }
    //
    // operator=
    //
    // Clears "this" tree and then makes a copy of the "other" tree.
    // Sets all member variables appropriately.
    // O(n), where n is total number of nodes in custom BST
    //
    priorityqueue& operator=(const priorityqueue& other) {
        // check if assigning to ourself:
        if (this == &other) {
            return *this;
        }
        // free memory in this priorityqueue
        this->clear();
        // copy values from other
        this->root = nullptr;
        this->size = 0;
        this->curr = nullptr;
        // return if tree is empty
        if (other.root == nullptr) {
            return *this;
        }
        // use preorder traversal to make a copy of the tree
        this->preOrderCopy(other.root);
        return *this;
    }
    //
    // clear:
    //
    // Frees the memory associated with the priority queue but is public.
    // O(n), where n is total number of nodes in custom BST
    //
    void clear() {
        // using post order traversal
        NODE* n = root;
        // call post order function
        postOrderDelete(n);
        // set size to 0, root and curr to nullptr
        size = 0;
        root = nullptr;
        curr = nullptr;
    }
    //
    // destructor:
    //
    // Frees the memory associated with the priority queue.
    // O(n), where n is total number of nodes in custom BST
    //
    ~priorityqueue() {
        clear();
    }
    //
    // enqueue:
    //
    // Inserts the value into the custom BST in the correct location based on
    // priority.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    void enqueue(T value, int priority) {
        // check if tree is empty
        if (size == 0) {
            // set newNode to be root
            NODE* newNode = new NODE;
            newNode->value = value;
            newNode->priority = priority;
            newNode->dup = false;
            newNode->parent = nullptr;
            newNode->link = nullptr;
            newNode->left = nullptr;
            newNode->right = nullptr;
            root = newNode;
            // increase size
            size++;
        } else {
            // BST is not empty, insert by priority
            NODE* prev = nullptr;
            NODE* cur = root;
            // traverse through tree by priority
            while (cur != nullptr) {
                // if priority is same, there is a duplicate
                if (priority == cur->priority) {
                    // make sure the first node in the list is marked as duplicate
                    cur->dup = true;
                    // if there is an existing linked list, we must traverse to the end to link a newNode
                    NODE* p = nullptr;
                    NODE* c = cur;
                    // traverse to end of list if list already exists
                    while (c != nullptr) {
                        p = c;
                        c = c->link;
                    }
                    // create a newNode
                    NODE* newNode = new NODE;
                    newNode->priority = priority;
                    newNode->value = value;
                    newNode->dup = true;
                    newNode->parent = p;
                    newNode->link = nullptr;
                    newNode->left = nullptr;
                    newNode->right = nullptr;
                    // point previous node to newNode
                    p->link = newNode;
                    // update size
                    size++;
                    // end function
                    return;
                }
                // go left
                if (priority < cur->priority) {
                    prev = cur;
                    cur = cur->left;
                } else {
                    // go right
                    prev = cur;
                    cur = cur->right;
                }
            }
            // if newNode is not duplicate, insert where we fell out of tree
            NODE* newNode = new NODE;
            newNode->priority = priority;
            newNode->value = value;
            newNode->dup = false;
            newNode->parent = prev;
            newNode->link = nullptr;
            newNode->left = nullptr;
            newNode->right = nullptr;
            // check whether newNode is the left or right subtree of prev
            if (priority < prev->priority) {
                prev->left = newNode;
            } else {
                prev->right = newNode;
            }
            // update Size
            size++;
        }
    }
    //
    // dequeue:
    //
    // returns the value of the next element in the priority queue and removes
    // the element from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T dequeue() {
        T valueOut;
        NODE* prev = nullptr;
        NODE* c = root; 
        NODE* parent = nullptr;
        NODE* r = nullptr;
        NODE* next = nullptr;
        if (c == nullptr) {
            // tree is empty
            return {};
        }
        // traverse to leftmost leaf in tree
        while (c != nullptr) {
            prev = c;
            c = c->left;
        }
        c = prev;
        valueOut = c->value;
        // check if duplicates exist
        if (c->dup) {
            if (c->link == nullptr) {
                if (c->parent != nullptr) {
                    // get the parent and right subtree of current node
                    parent = c->parent;
                    r = c->right;
                    // delete node
                    delete c;
                    size--;
                    // rewire
                    if (r == nullptr) {
                        parent->left = nullptr;
                    } else {
                        parent->left = r;
                        r->parent = parent;
                    }
                } else {
                    parent = nullptr;
                    r = c->right;
                    // delete node
                    delete c; 
                    size--; 
                    if (r == nullptr) {
                        // no other nodes left in tree
                        parent = nullptr;
                        root = nullptr;
                        size = 0;
                        curr = nullptr;
                    } else {
                        root = r;
                        r->parent = parent;
                    }
                }
            } else {
                // get parent, right subtree, and the next node in the linked list
                parent = c->parent;
                r = c->right;
                next = c->link;
                // delete current node;
                delete c;
                size--;
                // rewire
                if (parent == nullptr) {
                    next->parent = nullptr;
                    root = next;
                } else {
                    next->parent = parent;
                    parent->left = next;
                }
                if (r == nullptr) {
                    next->right = nullptr;
                } else {
                    next->right = r;
                    r->parent = next;
                }
            }
        } else {
            if (c->parent != nullptr) {
                // get the parent and right subtree of current node
                parent = c->parent;
                r = c->right;
                // delete node
                delete c;
                size--;
                // rewire
                if (r == nullptr) {
                    parent->left = nullptr;
                } else {
                    parent->left = r;
                    r->parent = parent;
                }
            } else {
                parent = nullptr;
                r = c->right;
                // delete node
                delete c; 
                size--;
                if (r == nullptr) {
                    // no other nodes left in tree
                    parent = nullptr;
                    root = nullptr;
                    size = 0;
                    curr = nullptr;
                } else {
                    root = r;
                    r->parent = parent;
                }
            }
        }
        return valueOut;
    }
    //
    // Size:
    //
    // Returns the # of elements in the priority queue, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;
    }
    //
    // begin
    //
    // Resets internal state for an inorder traversal.  After the
    // call to begin(), the internal state denotes the first inorder
    // node; this ensure that first call to next() function returns
    // the first inorder node value.
    //
    // O(logn), where n is number of unique nodes in tree
    //
    // Example usage:
    //    pq.begin();
    //    while (tree.next(value, priority)) {
    //      cout << priority << " value: " << value << endl;
    //    }
    //    cout << priority << " value: " << value << endl;
    void begin() {
        NODE* prev = nullptr;
        NODE* c = root; 
        // traverse to leftmost leaf in tree
        while (c != nullptr) {
            prev = c;
            c = c->left;
        }
        // set private member variable curr to prev
        curr = prev;
    }
    //
    // next
    //
    // Uses the internal state to return the next inorder priority, and
    // then advances the internal state in anticipation of future
    // calls.  If a value/priority are in fact returned (via the reference
    // parameter), true is also returned.
    //
    // False is returned when the internal state has reached null,
    // meaning no more values/priorities are available.  This is the end of the
    // inorder traversal.
    //
    // O(?) - hard to say.  But approximately O(logn + m).  Definitely not O(n).
    //
    // Example usage:
    //    pq.begin();
    //    while (tree.next(value, priority)) {
    //      cout << priority << " value: " << value << endl;
    //    }
    //    cout << priority << " value: " << value << endl;
    //
    bool next(T& value, int &priority) {
        NODE* c = curr; 
        NODE* child = nullptr;
        // special case when next() is called when the whole tree has already been traversed in order
        if (c == nullptr) {
            // there is no more values/priorities to be given
            priority = -999;
            return false;
        }
        value = curr->value;
        priority = curr->priority;
        // check for duplicates
        if (c->dup) {
            if (c->link != nullptr) {
                c = c->link;
                curr = c;
                return true;
            } else {
                // end of list has been reached
                while (c->priority == (c->parent)->priority) {
                    if ((c->parent)->parent == nullptr) {
                        c = c->parent;
                        break;
                    }
                    c = c->parent;
                }
            }
        }
        
        // first condition: right subtree is not empty
        if (c->right != nullptr) {
            NODE* prev = nullptr;
            c = c->right;
            // traverse to leftmost leaf in tree
            while (c != nullptr) {
                prev = c;
                c = c->left;
            }
            // update curr
            curr = prev; 
            return true;
        } else {
            // second condition: right subtree is empty
            child = c;
            c = c->parent;
            while (true) {
                if (c == nullptr) {
                    curr = nullptr;
                    return false;
                }
                if (c->left == child) {
                    curr = c;
                    return true;
                }
                child = c;
                c = c->parent;
            }
        }

    }
    //
    // toString:
    //
    // Returns a string of the entire priority queue, in order.  Format:
    // "1 value: Ben
    //  2 value: Jen
    //  2 value: Sven
    //  3 value: Gwen"
    //
    string toString() {
        string result = "";
        stringstream ss;
        // check if priorityqueue is empty
        if (root == nullptr) return result; 
        // call inOrder
        inOrder(root, ss);
        // convert stringstream to string and assign to result
        result = ss.str();
        return result;
    }
    //
    // peek:
    //
    // returns the value of the next element in the priority queue but does not
    // remove the item from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T peek() {
        NODE* prev = nullptr;
        NODE* c = root;
        T valueOut;
        if (c == nullptr) {
            // tree is empty
            return {};
        }
        // traverse to leftmost leaf in tree
        while (c != nullptr) {
            prev = c;
            c = c->left;
        }
        c = prev;
        valueOut = c->value;
        return valueOut;
    }
    
    //
    // ==operator
    //
    // Returns true if this priority queue as the priority queue passed in as
    // other.  Otherwise returns false.
    // O(n), where n is total number of nodes in custom BST
    //
    bool operator==(const priorityqueue& other) const {
        // return result of private helper function
        return equal(this->root, other.root);
    }
    
    //
    // getRoot - Do not edit/change!
    //
    // Used for testing the BST.
    // return the root node for testing.
    //
    void* getRoot() {
        return root;
    }
};