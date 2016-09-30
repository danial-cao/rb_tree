/*
 * main.c
 *
 *  Created on: 2016年9月22号
 *      Author: caohonghui
 */

#include <stdio.h>
#include <string.h>

#include "rb_tree.h"

typedef struct person
{
	int age;
	char name[48];
} person_t;

int compare(rb_tree_node_t *a, rb_tree_node_t *b)
{
	int ret;
	ret = (ret = ((person_t*) a->user_data)->age - ((person_t*) b->user_data)->age) != 0 ? ret : (a - b);
	return ret;
}

void show_rb_tree_incr(rb_tree_t *rb_tree)
{
	rb_tree_node_t *tmp;
	RB_FOREACH(tmp,rb_tree)
	{
		printf("name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}
}

void show_rb_tree_incr_from(rb_tree_node_t *a, rb_tree_node_t *b)
{
	RB_FOREACH_FROM(a,b)
	{
		printf("name:%s,age:%d\n", ((person_t*) a->user_data)->name, ((person_t*) a->user_data)->age);
	}
}

void show_rb_tree_decr(rb_tree_t *rb_tree)
{
	rb_tree_node_t *tmp;
	RB_FOREACH_REVERSE(tmp,rb_tree)
	{
		printf("name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}
}

void show_rb_tree_decr_from(rb_tree_node_t *a, rb_tree_node_t *b)
{
	RB_FOREACH_REVERSE_FROM(a,b)
	{
		printf("name:%s,age:%d\n", ((person_t*) a->user_data)->name, ((person_t*) a->user_data)->age);
	}
}

void show_rb_tree(rb_tree_t *rb_tree)
{
	rb_tree_node_t *tmp;
	printf("show rb tree\n");
	RB_FOREACH(tmp,rb_tree)
	{
		if (NULL == tmp->parent_node)
			printf("root");
		else if (tmp->parent_node->left_child == tmp)
			printf("is left child,parent's age is[%d]", ((person_t*) tmp->parent_node->user_data)->age);
		else
			printf("is right child,parent's age is[%d]", ((person_t*) tmp->parent_node->user_data)->age);

		printf("node name:%s,age:%d,color is:%d\n", ((person_t*) tmp->user_data)->name,
				((person_t*) tmp->user_data)->age, tmp->node_color);
	}
}

int main()
{
	printf("beginning of rb_tree test...");
	rb_tree_t rb_tree;

	rb_tree_root_init(&rb_tree, compare);
	printf("rb tree node number:%d\n", rb_tree.node_size);

	person_t person[100];
	rb_tree_node_t nodes[100];
	rb_tree_node_t *tmp;

	int i;
	for (i = 0; i < 100; i++)
	{
		person[i].age = i;
		sprintf(person[i].name, "chh_%d", i);

		nodes[i].user_data = &person[i];

		rb_tree_insert(&rb_tree, &nodes[i]);
	}
	printf("rb tree node number:%d\n", rb_tree.node_size);
	show_rb_tree(&rb_tree);

	tmp = rb_tree_find(&rb_tree, &nodes[9]);
	if (NULL != tmp)
	{
		printf("find name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}

	for (i = 3; i < 97; i++)
	{
		rb_tree_remove(&rb_tree, &nodes[i]);
	}

	tmp = rb_tree_nfind(&rb_tree, &nodes[9]);
	if (NULL != tmp)
	{
		printf("nfind name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}

	tmp = RB_MIN(&rb_tree);
	if (NULL != tmp)
	{
		printf("the min node:name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}
	tmp = RB_MAX(&rb_tree);
	if (NULL != tmp)
	{
		printf("the max node:name:%s,age:%d\n", ((person_t*) tmp->user_data)->name, ((person_t*) tmp->user_data)->age);
	}
//	printf("rb tree node number:%d\n", rb_tree.node_size);
//	printf("%d\n", compare(RB_MIN(&rb_tree), RB_MAX(&rb_tree)));
//
//	show_rb_tree_incr(&rb_tree);
//	show_rb_tree_decr(&rb_tree);
//
//	show_rb_tree_incr_from(tmp, &nodes[4]);
//	show_rb_tree_decr_from(tmp, &nodes[4]);

	show_rb_tree(&rb_tree);

	return 0;
}
