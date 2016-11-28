
File Name: malloc.c

Below is the brief of the functions created.
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




