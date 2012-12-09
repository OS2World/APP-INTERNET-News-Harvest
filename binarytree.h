#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef UNIX
#include <new.h>
#endif



// Non-intrusive binary tree class
//
// add(T *) - add an object into the tree
// subtract(T *) - remove an object from the tree
//                 uses search based on comparison method
// find(T *) - search for an object whose comparison with
//             the given parameter == 0
// find(T) - as above

template<class T>class BinaryTree
{
public:
    BinaryTree()
    {
        root    = NULL;
        current = NULL;
        red_black = 0;
    }

    ~BinaryTree()
    {
         while (!isEmpty())
        {
            T *f = subtract(root->object);
            delete f;
        }
    }

    int isEmpty()
    {
        return root==NULL;
    }

    int isRedBlack()
    {
        return red_black == 1;
    }

    T *add(T *f)
    {
        int branch = 1;

    //    printf("adding "); f->print(); printf(" \n");

        if (root == NULL)
        {
            root = new BinaryTreeFork(f);
            current = root;
        }
        else
        {
            BinaryTreeFork *ptr,*last,*addition;
            ptr = root;
            while (ptr != NULL)
            {
                last = ptr;
                if (T::compare(*f,*(ptr->object)) < 0)
                {
                    ptr = ptr->left_fork;
                    branch = 0;
                }
                else
                {
                    ptr = ptr->right_fork;
                    branch = 1;
                }
            }
            // if here ptr == NULL && last != NULL
            // last is last node passed through
            addition = new BinaryTreeFork(f);
            addition->parent = last;
            // but which branch did ptr exit on?!
            if (branch == 0)
            {
                // left
                last->left_fork = addition;
            }
            else
            {
                // right
                last->right_fork = addition;
            }

            current = addition;
        }
        return f;
    }

    T *subtract(T *f)
    {
        BinaryTreeFork  *ptr;
        BinaryTreeFork  *lefty;
        BinaryTreeFork **parent_arm;
        T *returner = NULL;

        ptr = findFork(f);
        if (ptr)
        {
            returner = ptr->object;

            // did we find the root?
            if (root == ptr)
            {
                if (ptr->left_fork == NULL && ptr->right_fork == NULL)
                {
                    root = NULL;
                    current = NULL;
                }
                else if (ptr->left_fork == NULL && ptr->right_fork != NULL)
                {
                    root = ptr->right_fork;
                    root->parent = NULL;
                    current = root;
                }
                else if (ptr->left_fork != NULL && ptr->right_fork == NULL)
                {
                    root = ptr->left_fork;
                    root->parent = NULL;
                    current = root;
                }
                else
                {
                    root = ptr->right_fork;
                    root->parent = NULL;
                    lefty = findLeftMostNode(root);
                    lefty->left_fork = ptr->left_fork;
                    lefty->left_fork->parent = lefty;
                }
            }
            else  // no
            {
                // is ptr root of a L-subtree or R-subtree
                if (ptr->parent->left_fork == ptr)
                    parent_arm = &(ptr->parent->left_fork);
                else
                    parent_arm = &(ptr->parent->right_fork);

                // must repair the tree structure
                // we can have a root & 0, 1 or 2 subtrees
                if (ptr->left_fork == NULL && ptr->right_fork == NULL)
                {
                    *parent_arm = NULL;  // 0 subtrees - LEAF
                }
                else if (ptr->left_fork == NULL && ptr->right_fork != NULL)
                {
                    // 1 subtree on the right
                    *parent_arm = ptr->right_fork;
                    ptr->right_fork->parent = ptr->parent;
                }
                else if (ptr->left_fork != NULL && ptr->right_fork == NULL)
                {
                    // 1 subtree on the left
                    *parent_arm = ptr->left_fork;
                    ptr->left_fork->parent = ptr->parent;
                }
                else
                {
                    // 2 subtrees!
                    // use the right one, and hang the left subtree
                    // on the very left of the right one.
                    *parent_arm = ptr->right_fork;
                    ptr->right_fork->parent = ptr->parent;

                    lefty = findLeftMostNode(ptr->right_fork);
                    lefty->left_fork = ptr->left_fork;
                    lefty->left_fork->parent = lefty;
                }
            }
            delete ptr;
        }
    //    else
    //    {
    //        // the search failed
    //        printf("Failed to find node!\n");
    //    }

        return returner;
    }

    T *find(T f)
    {
        return find(&f);
    }

    T *find(T *f)
    {
        BinaryTreeFork *ptr = findFork(f);
        if (ptr)
            return ptr->object;
        else
            return NULL;
    }


    //
    // Perform an inorder traversal, performing func(&node)
    // on each node
    //
    void traverse(void(func)(T *f))
    {
       if (root != NULL)
           doTraversal(root,func);
    }

private:
    // class T *MUST* define a static comparison method:
    //
    // static int T::compare(T t1,T t2)
    // returns 0 if t1 == t2
    // returns < 0 if t1 < t2
    // returns > 0 otherwise
    //

    class BinaryTreeFork
    {
    public:
        BinaryTreeFork() { nullify();  object=NULL; }
        BinaryTreeFork(T *f) { nullify(); object = f; }

        void nullify() { parent=left_fork=right_fork = NULL; }
        BinaryTreeFork *left_fork;
        BinaryTreeFork *right_fork;
        BinaryTreeFork *parent;
        T *object;
    };



    BinaryTreeFork *root;
    BinaryTreeFork *current;
    BinaryTreeFork *findFork(T *f)
    {
        BinaryTreeFork *ptr;
        int delta;

        ptr = root;
        while (ptr != NULL)
        {
            delta = T::compare(*f,*(ptr->object));
            if (delta == 0)
                break;
            else if (delta < 0)
                ptr = ptr->left_fork;
            else
                ptr = ptr->right_fork;
        }

        return ptr;
    }

    BinaryTreeFork *findLeftMostNode(BinaryTreeFork *fork)
    {
        if (fork != NULL)
        {
            while (fork->left_fork != NULL)
            {
                fork = fork->left_fork;
            }
        }
        return fork;
    }


    void doTraversal(BinaryTreeFork *root,void(func)(T *f))
    {
        if (root->left_fork != NULL)
            doTraversal(root->left_fork,func);

        (func)(root->object);

        if (root->right_fork != NULL)
            doTraversal(root->right_fork,func);
    }

    int red_black;



};

#endif //#ifndef __BINARYTREE_H__
