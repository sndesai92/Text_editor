#include <stdlib.h>
#include <string.h>
#include <iostream>
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


/*struct text_t{
    std::vector<char*> _text;
};*/


text_t* currentblock = NULL;
int size_left;
text_t* free_list = NULL;
int line_number = 0;

text_t* get_node()
{
	text_t* tmp_node;
	if(free_list != NULL)
	{
		tmp_node = free_list;
		free_list = free_list->right;
	}
	else
	{
		if((currentblock == NULL) || (size_left == 0))
		{
			currentblock = (text_t*) malloc(BLOCKSIZE * sizeof(text_t));
			size_left = BLOCKSIZE;
		}
		tmp_node = currentblock++;
		size_left = size_left - 1;
	}
	
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
   	tmp_node = get_node();
   	tmp_node->left = NULL;
	tmp_node->right = NULL;
	tmp_node->parent = NULL;
   	tmp_node->key = -1;
	tmp_node->height = -1;
	
    return tmp_node;
}


int length_text(text_t *txt){
    //if (txt == NULL) return -1;
    //return txt->_text.size();
	
	return -1;
}


char * get_line( text_t *txt, int index){
	
		//No text to be returned, return NULL
		if(txt->left == NULL)
			return NULL;
	
		//If there is only one line or leaf node
		if(txt->right == NULL)
		{
			if(txt->key == index)
				return (object_t*) txt->left;
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
	
    //return txt->_text[index-1];
	//return NULL;
}

void append_line( text_t *txt, char * new_line)
{
	//If tree is empty enter the first line.
	if(txt->left == NULL)
	{	
		txt->left = (text_t*) create_text_sequence(new_line);
		txt->right = NULL;
		txt->parent = NULL;
		txt->key = ++line_number;
		txt->height = 0;
		return;
	}
	
	//Else if tree is not empty. Go to the rightmost node to append a new line.
	text_t *tmp_node = txt;
	while(tmp_node->right != NULL)
	{
		tmp_node = tmp_node->right;
	}
	
	//After finding a place to create the new node, create a new node with the given text.
	text_t *old_leaf, *new_leaf;
	
	//Create a new node and put the old node value there.
	old_leaf = get_node();
	old_leaf->left = tmp_node->left;
	old_leaf->right = NULL;
	old_leaf->parent = tmp_node;
	old_leaf->key = tmp_node->key;
	old_leaf->height = 0;
	
	//Create a new node with the given data.
	new_leaf = get_node();
	new_leaf->left = (text_t*) create_text_sequence(new_line);
	new_leaf->right = NULL;
	new_leaf->parent = tmp_node;
	new_leaf->key = ++line_number;
	new_leaf->height = 0;
	
	//Update the parent node with the new values.
	//Parent key takes on the higher value from its children.
	tmp_node->key = new_leaf->key;
	tmp_node->left = old_leaf;
	tmp_node->right = new_leaf;
	tmp_node->height = 0;
	tmp_node->left->height = 0;
	tmp_node->right->height = 0;
	
	//Check if the tree is balanced.
	//If not, rebalance the tree.
	int finished = 0;
	while((!finished) && (tmp_node != NULL))
	{
		int old_height,tmp_height;
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
	
	return;
    //txt->_text.push_back(new_line);
}


char * set_line( text_t *txt, int index, char * new_line){
    //char *old = txt->_text[index-1];
    //txt->_text[index-1] = new_line;
	char *old = NULL;
    return old;
}


void insert_line( text_t *txt, int index, char* new_line){
    //txt->_text.insert(txt->_text.begin() + index - 1, new_line);
}

char * delete_line( text_t *txt, int index)
{
    //char *old = txt->_text[index-1];
    //txt->_text.erase(txt->_text.begin()+index-1);
    //return old;
	char *old = NULL;
	return old;
}

void display_text(text_t* txt)
{
	if(txt->right == NULL)
	{
		cout<<"The key is "<<txt->key<<endl;
		cout<<"This is  "<<(object_t *)txt->left<<endl;
		return;
	}
	
	cout<<endl<<"The current key of root is "<<txt->key<<endl;
	cout<<"The height of current node is "<<txt->height<<endl;
	cout<<"Go the left subtree"<<endl;
	display_text(txt->left);
	
	cout<<"Go to the right subtree"<<endl;
	display_text(txt->right);
	
}

int main()
{
	//Creating a new text.
	text_t* text = create_text();

	
	//Appending Line 1
	append_line(text,"Line 1");
		
	//Appending Line 2
	append_line(text,"Line 2");
	
	//Appending Line 3
	append_line(text,"Line 3");
	
	//Appending Line 4
	append_line(text,"Line 4");
	
	//Appending Line 5
	append_line(text,"Line 5");
	
	cout<<endl<<"Display the text"<<endl;
	display_text(text);
	
	cout<<"Display text ended"<<endl;
	
	cout<<endl<<"Get line 1"<<endl;
	char* str = get_line(text,1);
	
	cout<<"Returned line at index 1 is "<<str<<endl;
	
	cout<<endl<<"Get line 2"<<endl;
	str = get_line(text,2);
	
	cout<<"Returned line at index 2 is "<<str<<endl;
	
	cout<<endl<<"Get line 3"<<endl;
	str = get_line(text,3);
	
	cout<<"Returned line at index 3 is "<<str<<endl;
	
	cout<<endl<<"Get line 4"<<endl;
	str = get_line(text,4);
	
	cout<<"Returned line at index 4 is "<<str<<endl;
	
	cout<<endl<<"Get line 5"<<endl;
	str = get_line(text,5);
	
	cout<<"Returned line at index 5 is "<<str<<endl;
	
	return 0;
}
