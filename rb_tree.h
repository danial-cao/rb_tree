/*
 * rb_tree.h
 *
 *  Created on: 2016年9月22号
 *      Author: caohonghui
 */

#include <stddef.h>

#ifndef RB_TREE_H_
#define RB_TREE_H_


typedef struct rb_tree_node_s rb_tree_node_t;
/*如果a > b 返回值大于0，如果a < b 返回值小于0，a = b 则返回0。*/
typedef int (*rb_node_compare_t)(rb_tree_node_t* a, rb_tree_node_t* b);

struct rb_tree_node_s
{
	void *user_data;

	struct rb_tree_node_s *left_child;
	struct rb_tree_node_s *right_child;
	struct rb_tree_node_s *parent_node;
	int node_color; /* node color:1 (red),0 (black) */
};

typedef struct rb_tree_s
{
	struct rb_tree_node_s *rb_node_root; /* root of the tree */
	size_t node_size;
	rb_node_compare_t rb_node_compare;
} rb_tree_t;

enum
{
	GET_MAX, GET_MIN
};

int rb_tree_root_init(rb_tree_t *rb_tree, rb_node_compare_t rb_node_compare);
rb_tree_node_t *rb_tree_insert(rb_tree_t *rb_tree, rb_tree_node_t *node);
rb_tree_node_t *rb_tree_remove(rb_tree_t *rb_tree, rb_tree_node_t *node);
rb_tree_node_t *rb_tree_find(rb_tree_t *rb_tree, rb_tree_node_t *node);
rb_tree_node_t *rb_tree_nfind(rb_tree_t *rb_tree, rb_tree_node_t *node);

rb_tree_node_t *rb_tree_next(rb_tree_node_t *node);
rb_tree_node_t *rb_tree_prev(rb_tree_node_t *node);
rb_tree_node_t *rb_tree_min_or_max(rb_tree_t *rb_tree, int flag);

#define RB_MIN(rb_tree) rb_tree_min_or_max((rb_tree), GET_MIN)
#define RB_MAX(rb_tree) rb_tree_min_or_max((rb_tree), GET_MAX)

#define RB_FOREACH(x,rb_tree)	\
	for((x) = RB_MIN((rb_tree));	\
		(x) != NULL;	\
		(x) = rb_tree_next(x))

#define RB_FOREACH_FROM(x,y)	\
	for((x) = (y);	\
		((x) != NULL) && ((y) = rb_tree_next(x), (x) != NULL);	\
		(x) = (y))

#define RB_FOREACH_REVERSE(x,rb_tree)	\
	for((x) = RB_MAX((rb_tree));	\
		(x) != NULL;	\
		(x) = rb_tree_prev(x))

#define RB_FOREACH_REVERSE_FROM(x,y)	\
	for((x) = (y);	\
		((x) != NULL && ((y) = rb_tree_prev(x), (x) != NULL));	\
		(x) = (y))

#define RB_FOREACH_

#endif /* RB_TREE_H_ */
