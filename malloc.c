/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author shashiupadhyay
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdarg.h>
#include <unistd.h>
#include "utility.h"

#define MINIMUM_EXTRA_SIZE sizeof(size_t)
#define MINIMUM_BLOCK_SIZE  sizeof(Tree) + MINIMUM_EXTRA_SIZE

char buffer[256 * 1024 * 1024];

typedef struct TreeNode {
    size_t size;
    void *address;
    struct TreeNode *left_node_size;
    struct TreeNode *right_node_size;
    struct TreeNode *left_node_address;
    struct TreeNode *right_node_address;
    struct TreeNode *parent_node_size;
    struct TreeNode *parent_node_address;
} Tree;

/*
 * variable to store nodes in tree based on size and address
 */
Tree *Tree_sort_by_size;
Tree *Tree_sort_by_address;

/*
 Functions prototypes
 */
/* Function to store nodes in Tree based on their size and address */
Tree *insert_TreeNode_SortOnSize_and_Address(Tree *root_node, Tree * node, int InsertOption) ;
/* Function to store nodes in Tree */
void insert_Node_in_BSTree(Tree *node);
/* Function to search nodes in tree */
Tree *search_ChildNode_of_Tree(Tree *node, int searchoption);
/* Function to delete leaf nodes from tree */
Tree *delete_Leaf_TreeNode(Tree *root_node, Tree *node, int searchoption);
/* Function to delete nodes in tree */
void delete_Node(Tree *node);
/* Function to search memory blocks in tree */
Tree *search_MemoryBlockNodes_in_Tree(size_t blocksize);
/* Function to split nodes from the tree on address basis */
Tree *splitnodes_On_Address_Basis(Tree * node, size_t blocksize);
/* function to return the address which is aligned to multiple of 8 */
size_t alignTo8(size_t blocksize);
/* function to allocate the memory blocks */
void *malloc(size_t blocksize);
/* function to re-allocate the memory blocks */
void *realloc(void *ptr, size_t size);
void *calloc(size_t newsize, size_t size);
/* function to free the memory blocks */
void free(void *free_ptr);
/* function to merge the memory blocks */
Tree *merge_MemoryBlocks(Tree *root_node, Tree *node);

size_t alignTo8(size_t blocksize) {
    if(blocksize % 8 == 0 ){
         blocksize = blocksize;
    }else{
        blocksize = blocksize + (8 - (blocksize % 8));
    }
    return blocksize;
}

void * malloc(size_t blocksize) {
    if (blocksize <= 0) {
        return NULL;
    }
    blocksize = alignTo8(blocksize);
    
    Tree *newtree_ptr = search_MemoryBlockNodes_in_Tree(blocksize);
    if (newtree_ptr != NULL) {
        Tree * splittedtree_ptr = splitnodes_On_Address_Basis(newtree_ptr, blocksize);
        return splittedtree_ptr->address;
    }

    size_t requested_size = blocksize + sizeof (Tree);
    Tree *new_block = (Tree *) sbrk(requested_size);
    new_block->size = blocksize;
    new_block->address = new_block + 1;
    return new_block->address;
}

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    Tree * newtree_ptr = (Tree *) (((char *) ptr) - sizeof (Tree));
    if (newtree_ptr->size >= size) {
        return ptr;
    }
    void *new_ptr;
    new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    memcpy(new_ptr, ptr, newtree_ptr->size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t newsize, size_t size) {
    size = newsize*size;
    size = alignTo8(size);
    void *calloc_ptr = malloc(size);
    memset(calloc_ptr, 0, size);
    return calloc_ptr;
}

void free(void *free_ptr) {
    if (free_ptr != 0) {
        Tree *tree_ptr = (Tree *) (((char *) free_ptr) - sizeof (Tree));
        tree_ptr->left_node_size =
        tree_ptr->right_node_size =
        tree_ptr->parent_node_size =
        tree_ptr->parent_node_address =
        tree_ptr->right_node_address =
        tree_ptr->left_node_address = NULL;
        tree_ptr = merge_MemoryBlocks(Tree_sort_by_address, tree_ptr);
        tree_ptr->left_node_size =
        tree_ptr->right_node_size =
        tree_ptr->parent_node_size =
        tree_ptr->parent_node_address =
        tree_ptr->right_node_address =
        tree_ptr->left_node_address = NULL;
        insert_Node_in_BSTree(tree_ptr);
    }
}

Tree *merge_MemoryBlocks(Tree *root_node, Tree *node) {
    if (root_node == NULL) {
        return node;
    } else if (root_node == (Tree *) (node->address + node->size)) {
        Tree *newRoot = NULL;
        if (root_node == Tree_sort_by_address) {
            newRoot = Tree_sort_by_address;
        } else {
            newRoot = root_node->parent_node_address;
        }
        delete_Node(root_node);
        node->size = node->size + root_node->size + sizeof (Tree);
        return merge_MemoryBlocks(newRoot, node);
    } else if (node == (Tree*) (root_node->address + root_node->size)) {
        Tree *newRoot = NULL;
        if (root_node == Tree_sort_by_address) {
            newRoot = Tree_sort_by_address;
        } else {
            newRoot = root_node->parent_node_address;
        }
        delete_Node(root_node);
        root_node->size = root_node->size + node->size + sizeof (Tree);
        return merge_MemoryBlocks(newRoot, root_node);
    } else if (root_node > (Tree*) (node->address + node->size)) {
        return merge_MemoryBlocks(root_node->left_node_address, node);
    } else {
        return merge_MemoryBlocks(root_node->right_node_address, node);
    }
}

/*
 * Function to insert nodes in Tree
 * 0 for Sort on Size
 * 1 for Sort on Address
 */

void insert_Node_in_BSTree(Tree *node) {
    node->left_node_size =
    node->right_node_size =
    node->parent_node_size =
    node->parent_node_address =
    node->right_node_address =
    node->left_node_address = NULL;
    Tree_sort_by_size = insert_TreeNode_SortOnSize_and_Address(Tree_sort_by_size, node, 0);
    Tree_sort_by_address = insert_TreeNode_SortOnSize_and_Address(Tree_sort_by_address, node, 1);
}

Tree *insert_TreeNode_SortOnSize_and_Address(Tree *root_node, Tree * node, int InsertOption) {
    if (root_node == NULL) {
        return node;
    }

    if (InsertOption == 0) {
        if (node->size <= root_node->size) {
            if (root_node->left_node_size == NULL) {
                root_node->left_node_size = node;
                root_node->left_node_size->parent_node_size = root_node;
            } else {
                root_node->left_node_size =
                        insert_TreeNode_SortOnSize_and_Address(root_node->left_node_size, node, 0);
            }
        } else {
            if (root_node->right_node_size == NULL) {
                root_node->right_node_size = node;
                root_node->right_node_size->parent_node_size = root_node;
            } else {
                root_node->right_node_size =
                        insert_TreeNode_SortOnSize_and_Address(root_node->right_node_size, node, 0);
            }
        }
    } else if (InsertOption == 1) {
        if (node->address > root_node->address) {
            if (root_node->right_node_address != NULL) {
                root_node->right_node_address =
                        insert_TreeNode_SortOnSize_and_Address(root_node->right_node_address, node, 1);
            } else {
                root_node->right_node_address = node;
                root_node->right_node_address->parent_node_address = root_node;
            }
        } else {
            if (root_node->left_node_address != NULL) {
                root_node->left_node_address =
                        insert_TreeNode_SortOnSize_and_Address(root_node->left_node_address, node, 1);
            } else {
                root_node->left_node_address = node;
                root_node->left_node_address->parent_node_address = root_node;
            }
        }
    }
    return root_node;
}

Tree *search_ChildNode_of_Tree(Tree *node, int searchoption) {
    Tree *successornode = node;
    Tree *parentnode = node;

    if (!searchoption) {
        node = node->left_node_address;
        while (node != NULL) {
            successornode = node;
            node = node->right_node_address;
        }

        if (successornode->left_node_address == NULL && successornode != parentnode->left_node_address) {
            successornode->parent_node_address->right_node_address = NULL;
        } else if (successornode->left_node_address != NULL) {

            if (successornode != parentnode->left_node_address) {
                successornode->parent_node_address->right_node_address =
                        successornode->left_node_address;
            } else {
                successornode->parent_node_address->left_node_address =
                        successornode->left_node_address;
            }

            successornode->left_node_address->parent_node_address = successornode->parent_node_address;
        }

    } else {
        node = node->left_node_size;
        while (node != NULL) {
            successornode = node;
            node = node->right_node_size;
        }

        if (successornode->left_node_size != NULL) {
            if (successornode == parentnode->left_node_size) {
                successornode->parent_node_size->left_node_size = successornode->left_node_size;
            } else {
                successornode->parent_node_size->right_node_size = successornode->left_node_size;
            }
            successornode->left_node_size->parent_node_size = successornode->parent_node_size;
        } else if (successornode->left_node_size == NULL
                && successornode != parentnode->left_node_size) {
            successornode->parent_node_size->right_node_size = NULL;
        }
    }
    return successornode;
}

/*
 * Function to delete nodes from Tree
 * 0 for deletion on Size basis
 * 1 for deletion on Address basis
 */
void delete_Node(Tree *node) {
    Tree_sort_by_size = delete_Leaf_TreeNode(Tree_sort_by_size, node, 1);
    Tree_sort_by_address = delete_Leaf_TreeNode(Tree_sort_by_address, node, 0);
}

Tree *delete_Leaf_TreeNode(Tree *root_node, Tree *node, int searchoption) {
    if (searchoption) {
        if (root_node == node) {
            if (node->left_node_size != NULL
                    && node->right_node_size != NULL) {
                Tree *successornode = search_ChildNode_of_Tree(node, searchoption);
                if (successornode != node->left_node_size) {
                    successornode->left_node_size = node->left_node_size;
                    if (successornode->left_node_size != NULL) {
                        successornode->left_node_size->parent_node_size = successornode;
                    }
                }
                successornode->right_node_size = node->right_node_size;
                if (successornode->right_node_size != NULL) {
                    successornode->right_node_size->parent_node_size = successornode;
                }
                return successornode;
            } else if (node->left_node_size != NULL) {
                return node->left_node_size;
            } else if (node->right_node_size != NULL) {
                return node->right_node_size;
            } else {
                return NULL;
            }
        } else if (root_node->size >= node->size) {
            root_node->left_node_size =
                    delete_Leaf_TreeNode(root_node->left_node_size, node, searchoption);
            if (root_node->left_node_size != NULL) {
                root_node->left_node_size->parent_node_size = root_node;
            }
        } else {
            root_node->right_node_size =
                    delete_Leaf_TreeNode(root_node->right_node_size, node, searchoption);
            if (root_node->right_node_size != NULL) {
                root_node->right_node_size->parent_node_size = root_node;
            }
        }
    } else {
        if (root_node == node) {
            if (node->left_node_address != NULL
                    && node->right_node_address != NULL) {
                Tree *successornode = search_ChildNode_of_Tree(node, searchoption);
                if (successornode != node->left_node_address) {
                    successornode->left_node_address = node->left_node_address;
                    if (successornode->left_node_address != NULL) {
                        successornode->left_node_address->parent_node_address = successornode;
                    }
                }
                successornode->right_node_address = node->right_node_address;
                if (successornode->right_node_address != NULL) {
                    successornode->right_node_address->parent_node_address = successornode;
                }
                return successornode;
            } else if (node->left_node_address != NULL) {
                return node->left_node_address;
            } else if (node->right_node_address != NULL) {
                return node->right_node_address;
            } else {
                return NULL;
            }
        } else if (root_node->address > node->address) {
            root_node->left_node_address =
                    delete_Leaf_TreeNode(root_node->left_node_address, node, searchoption);
            if (root_node->left_node_address != NULL) {
                root_node->left_node_address->parent_node_address = root_node;
            }
        } else {
            root_node->right_node_address =
                    delete_Leaf_TreeNode(root_node->right_node_address, node, searchoption);
            if (root_node->right_node_address != NULL) {
                root_node->right_node_address->parent_node_address = root_node;
            }
        }
    }
    return root_node;
}

Tree *search_MemoryBlockNodes_in_Tree(size_t blocksize) {
    Tree *node, *nodetobedeleted = NULL;
    for (node = Tree_sort_by_size; node != NULL;) {
        if (node->size == blocksize) {
            nodetobedeleted = node;
            break;
        } else if (node->size < blocksize) {
            node = node->right_node_size;
        } else {
            nodetobedeleted = node;
            node = node->left_node_size;
        }
    }
    if (nodetobedeleted != NULL) {
        delete_Node(nodetobedeleted);
    }
    return nodetobedeleted;
}

Tree *splitnodes_On_Address_Basis(Tree * node, size_t blocksize) {
    if (node->size >= (blocksize + MINIMUM_BLOCK_SIZE)) {
        size_t newcreatedBlock = ((size_t) node) + blocksize + sizeof (Tree);
        Tree *splittedTreeNode = (Tree *) newcreatedBlock;
        splittedTreeNode->size = node->size - (blocksize + sizeof (Tree));
        splittedTreeNode->address = (void*) newcreatedBlock + sizeof (Tree);
        splittedTreeNode->left_node_size = NULL;
        splittedTreeNode->right_node_size = NULL;
        splittedTreeNode->parent_node_size = NULL;
        splittedTreeNode->parent_node_address = NULL;
        splittedTreeNode->right_node_address = NULL;
        splittedTreeNode->right_node_size = NULL;
        insert_Node_in_BSTree(splittedTreeNode);
        node->size = blocksize;
    }
    return node;
}




