//
//  tldlist.c
//  Test Lab
//
//  Created by Chutmongkon Chuasaard on 06/10/2018.
//  Copyright © 2018 Chutmongkol Chuasaard. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tldlist.h"


/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */

// Adelson-Velskii Landis (AVL) tree

struct tldlist
{
    Date * begin;
    Date * end;
    
    // For Iteration
    TLDNode * head;
    TLDNode * tail;
    
    // For AVL Tree
    TLDNode * treehead;
    int lenght;
};

struct tldnode
{
    // For Iteration
    TLDNode * head;
    TLDNode * child;
    
    // For AVL Tree
    TLDNode * headtree;
    TLDNode * childleft;
    TLDNode * childright;
    
    char * tldname;
    
    int nodeheight;
    int count;
};

struct tlditerator
{
    TLDNode * currentNode;
};
TLDNode * check_for_uneven_node(TLDNode *node);
int tldlist_rotateright_at_node(TLDList *tld,TLDNode *node);
int tldlist_rotateleft_at_node(TLDList *tld,TLDNode *node);
int tldlist_selfbalance(TLDList *tld);
void resetHeight(TLDNode *node);

int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}


TLDList *tldlist_create(Date *begin, Date *end)
{
    TLDList * result = (TLDList *)malloc(sizeof(TLDList));
    result->begin = begin;
    result->end = end;
    result->treehead = NULL;
    result->lenght = 0;
    
    result->head = NULL;
    result->tail = NULL;
    return result;
}

char *extract_tld(char * hostname)
{
    char tldref [100];
    strcpy(tldref,hostname);
    
    char * token;
    char * tld = NULL;
    
    char * sep = ".";

    token = strtok(tldref, sep);
    while(token != NULL)
    {
        tld = token;
        token = strtok(NULL, sep);
    }
    char * result = malloc(10 * sizeof(char));
    strcpy(result, tld);
    return result;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld)
{
    //printf("Free TLDlist\n");
    TLDNode * node = tld->head;
    TLDNode * temp;
    while(node != NULL)
    {
        temp = node;
        node = node->child;
        free(temp->tldname);
        free(temp);
    }
    free(tld);
    //printf("Complete Free TLDlist\n");
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d)
{
    if(!(date_compare(d, tld->begin) > 0 && date_compare(d, tld->end) < 0))
    {
        return 0;
    }
    char * tldname = extract_tld(hostname);
    
    TLDNode * newnode = (TLDNode *)malloc(sizeof(TLDNode));
    
    newnode->child = NULL;
    newnode->childleft = NULL;
    newnode->childright = NULL;
    newnode->tldname = tldname;
    newnode->nodeheight = 0;
    newnode->count = 1;
    
    TLDNode * node = tld->treehead;
    TLDNode * beforenode = tld->treehead;
    if(tld->treehead == NULL)
    {
        tld->treehead = newnode;
        newnode->headtree = NULL;
    }
    else
    {
        while(node != NULL)
        {
            beforenode = node;
            if(strcmp(tldname, node->tldname) < 0)
            {
                if(node->childleft == NULL)
                {
                    node->childleft = newnode;
                    newnode->headtree = node;
                    resetHeight(newnode);
                    break;
                }
                node = node->childleft;
            }
            else if(strcmp(tldname, node->tldname) > 0)
            {
                if(node->childright == NULL)
                {
                    node->childright = newnode;
                    newnode->headtree = node;
                    resetHeight(newnode);
                    break;
                }
                node = node->childright;
            }
            else if(strcmp(tldname, node->tldname) == 0)
            {
                node->count += 1;
                free(newnode->tldname);
                free(newnode);
                
                tld->lenght += 1;
                return 1;
            }
        }
        //tldlist_selfbalance(tld);
    }
    
    if(tld->head == NULL)
    {
        tld->head = newnode;
        tld->tail = newnode;
        newnode->head = NULL;
    }
    else{
        tld->tail->child = newnode;
        newnode->head = tld->tail;
        tld->tail = newnode;
    }
    //printf("%s\n",tldname);
    tldlist_selfbalance(tld);
    tld->lenght += 1;
    return 1;
}

int tldlist_selfbalance(TLDList *tld)
{
    TLDNode * uneven_node = check_for_uneven_node(tld->treehead);
    if(check_for_uneven_node(tld->treehead) == NULL)
    {
        return 0;
    }
    // Left
    
    int left = -1;
    int right = -1;
    if(uneven_node->childleft != NULL)
    {
        left = uneven_node->childleft->nodeheight;
    }
    if(uneven_node->childright != NULL)
    {
        right = uneven_node->childright->nodeheight;
    }
    
    
    if(left > right)
    {
        int leftleft = -1;
        int leftright = -1;
        if(uneven_node->childleft->childleft != NULL)
        {
            leftleft = uneven_node->childleft->childleft->nodeheight;
        }
        if(uneven_node->childleft->childright != NULL)
        {
            leftright = uneven_node->childleft->childright->nodeheight;
        }
        
        // Left Left
        if(leftleft > leftright)
        {
            //printf("1\n");
            tldlist_rotateright_at_node(tld,uneven_node);
            //printf("Complete 1\n");
        }
        // Left Right
        else if(leftleft < leftright)
        {
            //printf("2\n");
            tldlist_rotateleft_at_node(tld,uneven_node->childleft);
            //printf("3\n");
            tldlist_rotateright_at_node(tld,uneven_node);
        }
    }
    // Right
    else if(left < right)
    {
        int rightleft = -1;
        int rightright = -1;
        if(uneven_node->childright->childleft != NULL)
        {
            rightleft = uneven_node->childright->childleft->nodeheight;
        }
        if(uneven_node->childright->childright != NULL)
        {
            rightright = uneven_node->childright->childright->nodeheight;
        }
        // Right Left
        if(rightleft > rightright)
        {
            //printf("4\n");
            tldlist_rotateright_at_node(tld,uneven_node->childright);
            //printf("5\n");
            tldlist_rotateleft_at_node(tld,uneven_node);
        }
        // Right Right
        else if(rightleft < rightright)
        {
            //printf("6\n");
            tldlist_rotateleft_at_node(tld,uneven_node);
        }
    }
    tldlist_selfbalance(tld);
    return 0;
}

TLDNode * check_for_uneven_node(TLDNode *node)
{
    if(node == NULL)
    {
        return NULL;
    }
    int childrightHeight;
    int childleftHeight;
    
    if(node->childleft != NULL)
    {
        childleftHeight = node->childleft->nodeheight;
    }
    else
    {
        childleftHeight = -1;
    }
    
    if(node->childright != NULL)
    {
        childrightHeight = node->childright->nodeheight;
    }
    else
    {
        childrightHeight = -1;
    }
    
    if(childleftHeight == -1 && childrightHeight == -1)
    {
        //printf("Gotcha0\n");
        return NULL;
    }
    else if(childleftHeight == -1 && childrightHeight > 1)
    {
        //printf("Gotcha1\n");
        return node;
    }
    else if(childrightHeight == -1 && childleftHeight > 1)
    {
        //printf("Gotcha2\n");
        return node;
    }
    else if(childleftHeight - childrightHeight > 1)
    {
        //printf("Gotcha3\n");
        return node;
    }
    else
    {
        TLDNode * temp1 = check_for_uneven_node(node->childleft);
        TLDNode * temp2 = check_for_uneven_node(node->childright);
        if(temp1 == NULL )
        {
            return temp2;
        }
        return temp1;
    }
}

void resetHeight(TLDNode *node)
{
    if(node == NULL || node->headtree == NULL)
    {
        return;
    }
    int targetheight = node->nodeheight + 1;
    TLDNode *uppernode = node->headtree;
    if(targetheight >= uppernode->nodeheight)
    {
        uppernode->nodeheight = targetheight;
        //printf("Reset height of node %s to %d.\n",uppernode->tldname,uppernode->nodeheight);
        resetHeight(node->headtree);
    }
}

//  ^>>>V
//  ^   V
//  ^<<<V
int tldlist_rotateright_at_node(TLDList *tld,TLDNode *node)
{
    TLDNode *temp = NULL;
    TLDNode *prevhead = node->headtree;
    temp = node->childleft;
    node->childleft = node->childleft->childright;
    
    if(node->childleft != NULL)
    {
        node->childleft->headtree = node;
    }
    temp->childright = node;
    node->headtree = temp;
    temp->headtree = prevhead;
    if(prevhead != NULL)
    {
        if(prevhead->childleft == node)
        {
            prevhead->childleft = temp;
        }
        else
        {
            prevhead->childright = temp;
        }
    }
    else
    {
        if(temp->childright == tld->treehead){
            tld->treehead = temp;
        }
    }
    
    // Reset Height
    //printf("Check\n.");
    int leftheight = -1;
    int rightheight = -1;
    if(node->childright != NULL) { rightheight = node->childright->nodeheight; }
    if(node->childleft != NULL) { leftheight = node->childleft->nodeheight; }
    node->nodeheight = max(leftheight,rightheight) + 1;
    
    leftheight = -1;
    rightheight = -1;
    if(temp->childright != NULL) { rightheight = temp->childright->nodeheight; }
    if(temp->childleft != NULL) { leftheight = temp->childleft->nodeheight; }
    temp->nodeheight = max(leftheight,rightheight) + 1;
    return 0;
}

//  V<<<^
//  V   ^
//  V>>>^
int tldlist_rotateleft_at_node(TLDList *tld,TLDNode *node)
{
    TLDNode *temp = NULL;
    TLDNode *prevhead = node->headtree;
    
    temp = node->childright;
    node->childright = node->childright->childleft;
    if(node->childright != NULL)
    {
        node->childright->headtree = node;
    }
    
    temp->childleft = node;
    node->headtree = temp;
    temp->headtree = prevhead;
    
    if(prevhead != NULL)
    {
        if(prevhead->childleft == node)
        {
            prevhead->childleft = temp;
        }
        else
        {
            prevhead->childright = temp;
        }
    }
    else
    {
        if(temp->childleft == tld->treehead){
            tld->treehead = temp;
        }
    }
    
    // Reset Height
    // Reset Height
    //printf("Check\n.");
    int leftheight = -1;
    int rightheight = -1;
    if(node->childright != NULL) { rightheight = node->childright->nodeheight; }
    if(node->childleft != NULL) { leftheight = node->childleft->nodeheight; }
    node->nodeheight = max(leftheight,rightheight) + 1;
    
    leftheight = -1;
    rightheight = -1;
    if(temp->childright != NULL) { rightheight = temp->childright->nodeheight; }
    if(temp->childleft != NULL) { leftheight = temp->childleft->nodeheight; }
    temp->nodeheight = max(leftheight,rightheight) + 1;
    return 0;
}

/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld)
{
    return tld->lenght;
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld)
{
    TLDIterator * result = (TLDIterator *)malloc(sizeof(TLDIterator));
    result->currentNode = tld->head;
    return result;
}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter)
{
    TLDNode * temp = iter->currentNode;
    if(iter->currentNode != NULL)
    {
        iter->currentNode = iter->currentNode->child;
    }
    return temp;
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter)
{
    if(iter != NULL)
    {
        iter->currentNode = NULL;
        free(iter);
    }
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node)
{
    return node->tldname;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node)
{
    return node->count;
}

