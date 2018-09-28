

#include "eye_core.h"


static eye_inline void eye_rbtree_left_rotate(eye_rbtree_node_t **root,
    eye_rbtree_node_t *sentinel, eye_rbtree_node_t *node);
static eye_inline void eye_rbtree_right_rotate(eye_rbtree_node_t **root,
    eye_rbtree_node_t *sentinel, eye_rbtree_node_t *node);


void
eye_rbtree_insert(eye_rbtree_t *tree, eye_rbtree_node_t *node)
{
    eye_rbtree_node_t  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        eye_rbt_black(node);
        *root = node;

        return;
    }

    tree->insert(tree, node);

    /* re-balance tree */

    while (node != *root && eye_rbt_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (eye_rbt_is_red(temp)) {
                eye_rbt_black(node->parent);
                eye_rbt_black(temp);
                eye_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    eye_rbtree_left_rotate(root, sentinel, node);
                }

                eye_rbt_black(node->parent);
                eye_rbt_red(node->parent->parent);
                eye_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (eye_rbt_is_red(temp)) {
                eye_rbt_black(node->parent);
                eye_rbt_black(temp);
                eye_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    eye_rbtree_right_rotate(root, sentinel, node);
                }

                eye_rbt_black(node->parent);
                eye_rbt_red(node->parent->parent);
                eye_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    eye_rbt_black(*root);
}

void
eye_rbtree_delete(eye_rbtree_t *tree, eye_rbtree_node_t *node)
{
    eye_uint_t           red;
    eye_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;

    } else {
        subst = eye_rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        eye_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;

        return;
    }

    red = eye_rbt_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        eye_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != *root && eye_rbt_is_black(temp)) {

        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (eye_rbt_is_red(w)) {
                eye_rbt_black(w);
                eye_rbt_red(temp->parent);
                eye_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (eye_rbt_is_black(w->left) && eye_rbt_is_black(w->right)) {
                eye_rbt_red(w);
                temp = temp->parent;

            } else {
                if (eye_rbt_is_black(w->right)) {
                    eye_rbt_black(w->left);
                    eye_rbt_red(w);
                    eye_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                eye_rbt_copy_color(w, temp->parent);
                eye_rbt_black(temp->parent);
                eye_rbt_black(w->right);
                eye_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            w = temp->parent->left;

            if (eye_rbt_is_red(w)) {
                eye_rbt_black(w);
                eye_rbt_red(temp->parent);
                eye_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (eye_rbt_is_black(w->left) && eye_rbt_is_black(w->right)) {
                eye_rbt_red(w);
                temp = temp->parent;

            } else {
                if (eye_rbt_is_black(w->left)) {
                    eye_rbt_black(w->right);
                    eye_rbt_red(w);
                    eye_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                eye_rbt_copy_color(w, temp->parent);
                eye_rbt_black(temp->parent);
                eye_rbt_black(w->left);
                eye_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    eye_rbt_black(temp);
}


static inline void
eye_rbtree_left_rotate(eye_rbtree_node_t **root, eye_rbtree_node_t *sentinel,
    eye_rbtree_node_t *node)
{
    eye_rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static eye_inline void
eye_rbtree_right_rotate(eye_rbtree_node_t **root, eye_rbtree_node_t *sentinel,
    eye_rbtree_node_t *node)
{
    eye_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}


eye_rbtree_node_t *
eye_rbtree_next(eye_rbtree_t *tree, eye_rbtree_node_t *node)
{
    eye_rbtree_node_t  *root, *sentinel, *parent;

    sentinel = tree->sentinel;

    if (node->right != sentinel) {
        return eye_rbtree_min(node->right, sentinel);
    }

    root = tree->root;

    for ( ;; ) {
        parent = node->parent;

        if (node == root) {
            return NULL;
        }

        if (node == parent->left) {
            return parent;
        }

        node = parent;
    }
}