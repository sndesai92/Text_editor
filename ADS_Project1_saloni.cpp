#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <cstdio>
#define BLOCKSIZE 256
using namespace std;

typedef int key_t;
typedef char object_t;

struct text_t{
    key_t key;
    text_t* left;
    text_t* right;
    text_t* parent;
    int height;
};

vector<text_t *> st; //Stack to Increase/Decrease Key
vector<text_t *> rb_st; //Stack for rebalancing


text_t* currentblock = NULL; //maintains a chunk of available nodes
int size_left; //number of nodes available

void increase_key(text_t* txt); //Increments the key of the node and its right subtree by 1.
void decrease_key(text_t* txt); //Decrements the key of the node and its right subtree by 1.
void rebalance();               //Makes the tree height balanced.
text_t* create_text_sequence(char * newline);  //Creates an object node of type text_t with the string value passed as a parameter.
void left_rotation(text_t* node);   //Rotates the tree to left from the current node.
void right_rotation(text_t* node);  //Rotates the tree to right from the current node.

//Returns a memory chunk of size text_t from the currentblock of available memory.
text_t* get_node()
{
    text_t* tmp_node;
    
    //If the current block is NULL or no memory is available, assign new block.
    if((currentblock == NULL) || (size_left == 0)){
      currentblock = (text_t*) malloc(BLOCKSIZE * sizeof(text_t));
      size_left = BLOCKSIZE;
    }
    
    tmp_node = currentblock++;
    size_left = size_left - 1;

    if(tmp_node != NULL)
        return tmp_node;
    else
        return NULL;

}

text_t* create_text_sequence(char * newline)
{

    object_t *tmp = (object_t *) malloc(strlen(newline)+1);
    if (tmp != NULL)
        strcpy(tmp, newline);
    return (text_t*)(tmp);
}

void left_rotation(text_t* node)
{
    text_t* tmp_node;
    key_t tmp_key;

    tmp_node = node->left;
    tmp_key = node->key;
    node->left = node->right;
    node->key = node->right->key;
    node->right = node->left->right;
    node->left->right = node->left->left;
    node->left->left = tmp_node;
    node->left->key = tmp_key;
}

void right_rotation(text_t* node)
{
    text_t* tmp_node;
    key_t tmp_key;

    tmp_node = node->right;
    tmp_key = node->key;
    node->right = node->left;
    node->key = node->left->key;
    node->left = node->right->left;
    node->right->left = node->right->right;
    node->right->right = tmp_node;
    node->right->key = tmp_key;
}

text_t * create_text(){

    text_t *tmp_node;
    //Get a node from the memory block.
    tmp_node = get_node();
    //Initialize the pointers to NULL
    tmp_node->left = NULL;
    tmp_node->right = NULL;
    tmp_node->parent = NULL;
    tmp_node->key = -1;
    tmp_node->height = -1;

    return tmp_node;
}


int length_text(text_t *txt){
    if(txt == NULL){
        return -1;
    }

    if(txt->left == NULL){
        return 0;
    }
    if(txt->right == NULL){
        return txt->key;
    }
    text_t *tmp_node;
    tmp_node = txt;
    while(tmp_node->right != NULL){
        tmp_node = tmp_node->right;
    }
    return tmp_node->key-1;
}


char * get_line( text_t *txt, int index){

    //No text to be returned, return NULL
    if(txt->left == NULL)
        return NULL;
    
    //If there is only one line or leaf node
    if(txt->right == NULL)
    {
        if(txt->key == index)
        {
            return (object_t*) txt->left;
        }
        else
            return NULL;
    }

    //Depending on the line_number/index go to the left or right subtree.
    if(index < txt->key)
    {
        return get_line(txt->left,index);
    }
    else
    {
        return get_line(txt->right,index);
    }
}


void append_line( text_t *txt, char * new_line)
{
    //Clear the stacks
    //st.clear();
    //rb_st.clear();
    
    //If tree is empty enter the first line.
    if(txt->left == NULL)
    {
        text_t *eof, *tmp;

        tmp = get_node();
        eof = get_node();

        tmp->left = (text_t*) create_text_sequence(new_line);
        tmp->right = NULL;
        tmp->parent = txt;
        tmp->key = 1;
        tmp->height = 0;


        eof->key = 2;
        eof->left = create_text_sequence("EOF");
        eof->right = NULL;
        eof->height = 0;
        eof->parent = txt;

        txt->left = tmp;
        txt->right = eof;
        txt->key = 2;
        txt->height = 1;

        return;
    }

    //Else if tree is not empty. Go to the rightmost node to append a new line.
    text_t *tmp_node = txt;
    while(tmp_node->right != NULL)
    {
        rb_st.push_back(tmp_node);
        tmp_node = tmp_node->right;
    }

    //After finding a place to create the new node, create a new node with the given text.
    text_t *old_leaf, *new_leaf;



    //Create a new node with the given data.
    new_leaf = get_node();
    new_leaf->left = (text_t*) create_text_sequence(new_line);
    new_leaf->right = NULL;
    new_leaf->parent = tmp_node;
    new_leaf->key = tmp_node->key;
    new_leaf->height = 0;

    //Create a new node and put the old node value there.
    old_leaf = get_node();
    old_leaf->left = tmp_node->left;
    old_leaf->right = NULL;
    old_leaf->parent = tmp_node;
    old_leaf->key = ++(tmp_node->key);
    old_leaf->height = 0;

    //Update the parent node with the new values.
    //Parent key takes on the higher value from its children.
    tmp_node->key = old_leaf->key;
    tmp_node->left = new_leaf;
    tmp_node->right = old_leaf;
    tmp_node->height = 1;
    tmp_node->left->height = 0;
    tmp_node->right->height = 0;

    //Check if the tree is balanced.
    //If not, rebalance the tree.

    rebalance();
    return;
}

char * set_line( text_t *txt, int index, char * new_line){
    if(txt->left == NULL)
        return NULL;    

    //If there is only one line or leaf node
    if(txt->right == NULL)
    {
        if(txt->key == index)
        {
            object_t* old = (object_t*) txt->left;
            txt->left = create_text_sequence(new_line);
            return old;
        }
        else
            return NULL;
    }

    if(index < txt->key)
    {
        return set_line(txt->left,index,new_line);
    }
    else
    {
        return set_line(txt->right,index,new_line);
    }

}


void insert_line( text_t *txt, int index, char* new_line){
    //Clear the stacks
    st.clear();
    rb_st.clear();

    //If no Text then return NULL
    if(txt == NULL){
        return;
    }

    //If Empty text then append_line which will append at the first line only.
    if(txt->left == NULL){
        append_line(txt, new_line);
        return;
    }

    //If index is greater than the length of text then append the line
    int length = length_text(txt);
    if (index>length){
        append_line(txt, new_line);
        return;
    }

    //Find the given index
    text_t *tmp_node = txt;
    while (tmp_node->right != NULL){
        rb_st.push_back(tmp_node); //For Rebalancing
        if(index < tmp_node->key) {
            tmp_node->key += 1;
            st.push_back(tmp_node->right); //For updating the keys
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }

    text_t *old_leaf, *new_leaf;

    //Create a new node for the "new_line"
    new_leaf = get_node();
    new_leaf->left = (text_t*) create_text_sequence(new_line);
    new_leaf->right = NULL;
    new_leaf->parent = tmp_node;
    new_leaf->key = tmp_node->key;
    new_leaf->height = 0;


    //Get the node of the old line
    old_leaf = get_node();
    old_leaf->left = tmp_node->left;
    old_leaf->right = NULL;
    old_leaf->parent = tmp_node;
    old_leaf->key = ++(tmp_node->key);
    old_leaf->height = 0;

    //Split the old node, add the "new_line" node to left and "old line" node to right
    tmp_node->key = old_leaf->key;
    tmp_node->left = new_leaf;
    tmp_node->right = old_leaf;
    tmp_node->height = 0;
    tmp_node->left->height = 0;
    tmp_node->right->height = 0;


    //Update line numbers
    while(st.size() != 0){
        text_t *t = st[st.size()-1];
        st.pop_back();
        increase_key(t);
    }

    //Rebalance
    rebalance();

}

char * delete_line( text_t *txt, int index)
{
    //Clear the stacks
    st.clear();
    rb_st.clear();

    char * ret_str; //For return string
    
    //If Empty Text then return NULL
    if(txt->left == NULL){
        return "NULL";
    }
    
    if(txt->right == NULL && txt->key == index){
        ret_str = ret_str = (char *) txt->left;
        return ret_str;
    }

    text_t *tmp_node = txt;
    text_t *upper_node, *other_node;

    //Get the index Line
    while (tmp_node->right != NULL){
        rb_st.push_back(tmp_node);
        upper_node = tmp_node;
        if(index < tmp_node->key) {
            tmp_node->key -= 1;
            st.push_back(tmp_node->right);
            other_node = tmp_node->right;
            tmp_node = tmp_node->left;
        } else {
            other_node = tmp_node->left;
            tmp_node = tmp_node->right;
        }
    }
    
    //Update the keys
    while(st.size() != 0){
        text_t *t = st[st.size()-1];
        st.pop_back();
        decrease_key(t);
    }

    //Update deleteed node
    if (index == tmp_node->key){
        ret_str = (char *) tmp_node->left;

        if (upper_node->left == tmp_node){
            upper_node->key = other_node->key;
        }else{
            upper_node->key = other_node->key;
        }


        upper_node->height = other_node->height;
        upper_node->left = other_node->left;
        upper_node->right = other_node->right;
    } else {
        ret_str = "NULL";
    }


    rb_st.pop_back();
    
    
    //Rebalance
    rebalance();

    return ret_str;
}

void increase_key(text_t* txt)
{
    if(txt->right == NULL)
    {
        txt->key++;
        return;
    }

    txt->key++;
    increase_key(txt->left);
    increase_key(txt->right);
}

void decrease_key(text_t* txt)
{
    if(txt->right == NULL)
    {
        txt->key--;
        return;
    }

    txt->key--;
    decrease_key(txt->left);
    decrease_key(txt->right);

}


//As described in the book  
void rebalance(){
    int finished = 0;
    text_t *tmp_node;
    while((!finished) && rb_st.size()!=0)
    {
        int old_height,tmp_height;
        tmp_node = rb_st[rb_st.size() -1];
        rb_st.pop_back();
        old_height = tmp_node->height;

        if((tmp_node->left->height - tmp_node->right->height) == 2)
        {

            if((tmp_node->left->left->height - tmp_node->right->height) == 1)
            {
                right_rotation(tmp_node);
                tmp_node->right->height = tmp_node->right->left->height + 1;
                tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {
                left_rotation(tmp_node->left);
                right_rotation(tmp_node);
                tmp_height = tmp_node->left->left->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        }
        else if ((tmp_node->right->height - tmp_node->left->height) == 2)
        {
            if((tmp_node->right->right->height - tmp_node->left->height) == 1)
            {
                left_rotation(tmp_node);
                tmp_node->left->height = tmp_node->left->right->height + 1;
                tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {
                right_rotation(tmp_node->right);
                left_rotation(tmp_node);
                tmp_height = tmp_node->right->right->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        }
        else
        {
            if(tmp_node->left->height > tmp_node->right->height)
            {
                tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {
                tmp_node->height = tmp_node->right->height + 1;
            }


        }

        if(tmp_node->height == old_height)
            finished = 1;

        tmp_node = tmp_node->parent;
    }


    rb_st.clear();
}

