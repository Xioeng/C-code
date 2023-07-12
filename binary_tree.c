#include<stdio.h>
#include<time.h> 
#include<stdlib.h>
#include<assert.h>

/*Implementation of a binary tree which is built form data read form a .txt file
the array size is given by the first element of the .txt file
(In, Pre and Post)order walks are implemented and printed*/

// Reads and stores the file content stored at path_file into an array
int* read_file_into_array(char path_file[]){
    FILE* input_file = fopen(path_file, "r");

    int size = 0, element_index = 0;
    fscanf(input_file, "%d", &size);
    int* array = (int*)malloc(size * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);}
    rewind(input_file);     
    while(fscanf(input_file, "%d", &array[element_index])==1 && element_index < size){element_index++;}
    fclose(input_file);
    return array;
}

//Define the basic structure for a node
typedef struct node{
    int data;
    struct node *left;
    struct node *right;
}node;

node* create_node(int d){
    node* root = malloc(sizeof(node));
    root -> data = d;
    root -> left = NULL;
    root -> right = NULL;
    return root;
}

node* create_tree(int array[], int size){
    //Create an array of nodes
    node** node_array = (node**)malloc(size * sizeof(node*));
    for(int i = 0; i < size; i++){ node_array[i] =create_node(array[i]); }
    //Connect the tree nodes with their left and right children
    for(int i = 0; i < size; i++){
        if(2*i + 1< size){node_array[i] -> left = node_array[2*i + 1];}
        else{ node_array[i] -> left = NULL;}
        if(2*i + 2 < size){node_array[i] -> right = node_array[2*i + 2];}
        else{ node_array[i] -> right = NULL;}
    }
    return node_array[0];
}

void inorder(node* node){
    if (node !=NULL){
    inorder(node -> left);
    printf("%d ", node -> data);
    inorder(node -> right);}
}

void preorder(node* node){
    if (node !=NULL){
    printf("%d ", node -> data);
    preorder(node -> left);
    preorder(node -> right);}
}

void postorder(node* node){
    if (node !=NULL){
    postorder(node -> left);
    postorder(node -> right);
    printf("%d ", node -> data);}
}

void print_array(int array[], int size){
    assert(size > 0);
    printf("\nOriginal array of numbers\n");
    for (int i = 0; i < size; i++){printf("%d ", array[i]);}

}

void main(void){
    char file_name[] = "random_numbers.txt";
    int* array = read_file_into_array(file_name);
    print_array(array, array[0]);
    node* tree_root = create_tree(array, array[0]);

    printf("\nInorder walk\n");
    inorder(tree_root);
    printf("\nPreorder walk\n");
    preorder(tree_root);
    printf("\nPostorder walk\n");
    postorder(tree_root);
    printf("\n------------\n\n");
    free(array); free(tree_root); //Make sure that you free the space at the end
}