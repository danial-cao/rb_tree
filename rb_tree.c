/*
 * rb_tree.c
 *
 *  Created on: 2016年9月22号
 *      Author: caohonghui
 */
#include <stddef.h>

#include "rb_tree.h"

#define RB_BLACK  0
#define RB_RED    1

#define RB_NODE_INIT(node) do{	\
	(node)->parent_node = (node)->left_child = (node)->right_child = NULL;	\
	(node)->node_color = RB_RED;	\
	}while(0)

#define RB_SWAP_NODE(A,B) do{ \
	(A)->left_child = (rb_tree_node_t*)((unsigned long)(A)->left_child ^ (unsigned long)(B)->left_child); \
	(A)->right_child = (rb_tree_node_t*)((unsigned long)(A)->right_child ^ (unsigned long)(B)->right_child); \
	(A)->parent_node = (rb_tree_node_t*)((unsigned long)(A)->parent_node ^ (unsigned long)(B)->parent_node); \
	(A)->node_color = (A)->node_color ^ (B)->node_color; \
	(B)->left_child = (rb_tree_node_t*)((unsigned long)(A)->left_child ^ (unsigned long)(B)->left_child); \
	(B)->right_child = (rb_tree_node_t*)((unsigned long)(A)->right_child ^ (unsigned long)(B)->right_child); \
	(B)->parent_node = (rb_tree_node_t*)((unsigned long)(A)->parent_node ^ (unsigned long)(B)->parent_node); \
	(B)->node_color = (A)->node_color ^ (B)->node_color; \
	(A)->left_child = (rb_tree_node_t*)((unsigned long)(A)->left_child ^ (unsigned long)(B)->left_child); \
	(A)->right_child = (rb_tree_node_t*)((unsigned long)(A)->right_child ^ (unsigned long)(B)->right_child); \
	(A)->parent_node = (rb_tree_node_t*)((unsigned long)(A)->parent_node ^ (unsigned long)(B)->parent_node); \
	(A)->node_color = (A)->node_color ^ (B)->node_color; \
	}while(0)

#define RB_ASSIGN_NODE_VALUE(A,B) do{ \
	(A)->left_child = (B)->left_child; \
	(A)->right_child = (B)->right_child; \
	(A)->parent_node = (B)->parent_node; \
	(A)->node_color = (B)->node_color; \
	}while(0)

static void _rb_tree_rotate_right(rb_tree_t *rb_tree, rb_tree_node_t *node);
static void _rb_tree_rotate_left(rb_tree_t *rb_tree, rb_tree_node_t *node);

/*插入节点后进行颜色调整*/
static void _rb_tree_insert_color(rb_tree_t *rb_tree, rb_tree_node_t *node);
/*删除节点后进行颜色调整*/
static void _rb_tree_move_color(rb_tree_t *rb_tree, rb_tree_node_t *parent, rb_tree_node_t *crr_node);

/*
 * 右旋：
 * 当在某个结点x上，做右旋操作时，我们假设它的左孩子y不是NULL，x可以为树内任意左孩子不是NULL的结点。
 * 右旋以x到y之间的链为“支轴”进行，它使结点y成为该孩子树的根，y的右孩子结点为x，而y的原右孩子则成为x的左孩子。
 * */
static void _rb_tree_rotate_right(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *l_child;

	l_child = node->left_child;
	if ((node->left_child = l_child->right_child) != NULL)
		l_child->right_child->parent_node = node;

	if ((l_child->parent_node = node->parent_node) != NULL)
	{
		if (node->parent_node->left_child == node)
			node->parent_node->left_child = l_child;
		else
			node->parent_node->right_child = l_child;
	}
	else
		rb_tree->rb_node_root = l_child;

	l_child->right_child = node;
	node->parent_node = l_child;

	return;
}

/*
 * 左旋：
 * 当在某个结点x上，做左旋操作时，我们假设它的右孩子y不是NULL，x可以为树内任意右孩子不是NULL的结点。
 * 左旋以x到y之间的链为“支轴”进行，它使结点y成为该孩子树的根，y的左孩子结点为x，而y的原左孩子则成为x的右孩子。
 * */
static void _rb_tree_rotate_left(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *r_child;

	r_child = node->right_child;
	if ((node->right_child = r_child->left_child) != NULL)
		r_child->left_child->parent_node = node;

	if ((r_child->parent_node = node->parent_node) != NULL)
	{
		if (node->parent_node->left_child == node)
			node->parent_node->left_child = r_child;
		else
			node->parent_node->right_child = r_child;
	}
	else
		rb_tree->rb_node_root = r_child;

	r_child->left_child = node;
	node->parent_node = r_child;

	return;
}

/*插入节点后进行颜色调整*/
static void _rb_tree_insert_color(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *parent, *grandpa, *uncle;
	/*父节点若为黑色不存在颜色冲突，满足红黑树性质*/
	while ((parent = node->parent_node) != NULL && parent->node_color != RB_BLACK)
	{
		grandpa = parent->parent_node;
		if (grandpa->left_child == parent)
		{
			uncle = grandpa->right_child;
			if (NULL != uncle && uncle->node_color == RB_RED)/*叔节点是红色，将冲突向上移*/
			{
				grandpa->node_color = RB_RED;
				uncle->node_color = parent->node_color = RB_BLACK;
				node = grandpa;
			}
			else
			{
				/*node是右孩子节点*/
				if (node == parent->right_child)
				{
					/*以父节点为中心节点左旋转*/
					_rb_tree_rotate_left(rb_tree, parent);
					node = parent;
					parent = node->parent_node;
				}
				grandpa->node_color = RB_RED;
				parent->node_color = RB_BLACK;
				/*以祖父节点为中心节点右旋转*/
				_rb_tree_rotate_right(rb_tree, grandpa);
			}
		}
		else
		{
			uncle = grandpa->left_child;
			if (NULL != uncle && uncle->node_color == RB_RED)/*叔节点是红色，将冲突向上移*/
			{
				grandpa->node_color = RB_RED;
				uncle->node_color = parent->node_color = RB_BLACK;
				node = grandpa;
			}
			else
			{
				if (node == parent->left_child)
				{
					/*以父节点为中心节点右旋转*/
					_rb_tree_rotate_right(rb_tree, parent);
					node = parent;
					parent = node->parent_node;
				}
				grandpa->node_color = RB_RED;
				parent->node_color = RB_BLACK;
				/*以祖父节点为中心节点左旋转*/
				_rb_tree_rotate_left(rb_tree, grandpa);
			}
		}
	}

	rb_tree->rb_node_root->node_color = RB_BLACK;

	return;
}

/*删除节点后进行颜色调整*/
static void _rb_tree_move_color(rb_tree_t *rb_tree, rb_tree_node_t *parent, rb_tree_node_t *crr_node)
{
	rb_tree_node_t *brother;

	while (((NULL == crr_node) || RB_BLACK == crr_node->node_color) && rb_tree->rb_node_root != crr_node)
	{
		if (parent->left_child == crr_node)/*左孩子*/
		{
			brother = parent->right_child;
			/*红兄：转为黑兄红父情况*/
			if (RB_RED == brother->node_color)
			{
				brother->node_color = RB_BLACK;
				parent->node_color = RB_RED;
				_rb_tree_rotate_left(rb_tree, parent);
				brother = parent->right_child;
			}

			/*黑兄双黑侄儿*/
			if ((NULL == brother->left_child || RB_BLACK == brother->left_child->node_color)
					&& (NULL == brother->right_child || RB_BLACK == brother->right_child->node_color))
			{
				brother->node_color = RB_RED;
				crr_node = parent;
				parent = crr_node->parent_node; /*将检查点向上移，以父节点进行调整*/
			}
			else
			{
				/*左侄儿为红色节点：转为右侄儿为红色节点的情况*/
				if (NULL == brother->right_child || RB_BLACK == brother->right_child)
				{
					brother->left_child->node_color = RB_BLACK;
					brother->node_color = RB_RED;
					_rb_tree_rotate_right(rb_tree, brother);
					brother = parent->right_child;
				}
				/*右侄儿为红色节点*/
				brother->node_color = parent->node_color;
				brother->right_child->node_color = RB_BLACK;
				parent->node_color = RB_BLACK;
				_rb_tree_rotate_left(rb_tree, parent);
				crr_node = rb_tree->rb_node_root;
				break;
			}
		}
		else
		{
			brother = parent->left_child;
			/*红兄：转为黑兄红父情况*/
			if (RB_RED == brother->node_color)
			{
				brother->node_color = RB_BLACK;
				parent->node_color = RB_RED;
				_rb_tree_rotate_right(rb_tree, parent);
				brother = parent->left_child;
			}
			/*黑兄双黑侄儿*/
			if ((NULL == brother->left_child || RB_BLACK == brother->left_child->node_color)
					&& (NULL == brother->right_child || RB_BLACK == brother->right_child->node_color))
			{
				brother->node_color = RB_RED;
				crr_node = parent;
				parent = crr_node->parent_node;/*将检查点向上移，以父节点进行调整*/
			}
			else
			{
				/*右侄儿为红色节点：转为左侄儿为红色节点的情况*/
				if (NULL == brother->left_child || RB_BLACK == brother->left_child)
				{
					brother->right_child->node_color = RB_BLACK;
					brother->node_color = RB_RED;
					_rb_tree_rotate_left(rb_tree, brother);
					brother = parent->left_child;
				}
				/*左侄儿为红色*/
				brother->node_color = parent->node_color;
				brother->left_child->node_color = RB_BLACK;
				parent->node_color = RB_BLACK;
				_rb_tree_rotate_right(rb_tree, parent);
				crr_node = rb_tree->rb_node_root;
				break;
			}
		}
	}

	if (crr_node)
		crr_node->node_color = RB_BLACK;

	return;
}

int rb_tree_root_init(rb_tree_t *rb_tree, rb_node_compare_t rb_node_compare)
{
	if (NULL == rb_tree || NULL == rb_node_compare)
		return -1;
	rb_tree->rb_node_root = NULL;
	rb_tree->node_size = 0;
	rb_tree->rb_node_compare = rb_node_compare;
	return 0;
}

/*二叉查找树性质：左孩子节点小于父节点，右孩子节点大于父节点*/
rb_tree_node_t *rb_tree_insert(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *tmp = NULL;
	rb_tree_node_t *parent = NULL;
	int comp;

	tmp = rb_tree->rb_node_root;
	while (NULL != tmp)
	{
		parent = tmp;
		comp = rb_tree->rb_node_compare(parent, node);
		if (comp > 0)
			tmp = tmp->left_child;
		else if (comp < 0)
			tmp = tmp->right_child;
		else
			return tmp;
	}

	RB_NODE_INIT(node);
	node->parent_node = parent;
	if (NULL != parent)
	{
		if (comp > 0)
			parent->left_child = node;
		else
			parent->right_child = node;
	}
	else
	{
		rb_tree->rb_node_root = node;
	}

	/*新加节点置为红色，所以各结点路径长度没有变化，只需要调整颜色即可*/
	_rb_tree_insert_color(rb_tree, node);
	rb_tree->node_size++;

	return NULL;
}

/*
 * 删除节点：node节点必须保证是rb_tree树中关联存在的点，否则出现未知错误
 * */
rb_tree_node_t *rb_tree_remove(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *child, *parent, *old;
	int color;

	old = node;
	if (NULL == node->left_child)
		child = node->right_child;
	else if (NULL == node->right_child)
		child = node->left_child;
	else /*删除节点有两孩子节点*/
	{
#if 0
		/*方法一*/
		rb_tree_node_t *tmp;
		node = node->right_child;
		while ((tmp = node->left_child) != NULL) /*取右孩子最左节点,可能就是右孩子*/
			node = tmp;

		/*将old与node节点交换，转为删除节点old只有一个孩子节点或者无孩子节点的情况*/
		if (NULL == old->parent_node)
			rb_tree->rb_node_root = node;
		else if (old->parent_node->left_child == old)
			old->parent_node->left_child = node;
		else
			old->parent_node->right_child = node;
		old->left_child->parent_node = node;
		old->right_child->parent_node = node;

		/*node节点为old的孩子节点*/
		if (node->parent_node->right_child == node)
			old->right_child = old;

		if (node->parent_node->right_child == node)
			node->parent_node->right_child = old;
		else if (node->parent_node->left_child == node)
			node->parent_node->left_child = old;
		if (NULL != node->left_child)
			node->left_child->parent_node = old;
		if (NULL != node->right_child)
			node->right_child->parent_node = old;

		RB_SWAP_NODE(old, node);

		return rb_tree_remove(rb_tree, old);
#else
		/*方法二*/
		/*取右孩子最左节点(可能就是右孩子本身),替换要移除的节点*/
		rb_tree_node_t *tmp;
		node = node->right_child;
		while ((tmp = node->left_child) != NULL)
			node = tmp; /*找到移动替换的节点，也就是真正删除的节点*/

		child = node->right_child; /*node节点肯定没有左孩子，可能有右孩子*/
		parent = node->parent_node;
		color = node->node_color;

		if (NULL != child)
			child->parent_node = parent;
		if (NULL != parent)
		{
			if (node == parent->right_child)
			parent->right_child = child;
			else
			parent->left_child = child;
		}
		else
		{ /*此处不会发生*/
			rb_tree->rb_node_root = child;
		}
		/*如果真正删除的节点是old的孩子节点*/
		if (node->parent_node == old)
			parent = node;

		/*node替换old节点*/
		RB_ASSIGN_NODE_VALUE(node, old);
		if (NULL != old->parent_node)
		{
			if (old->parent_node->left_child == old)
				old->parent_node->left_child = node;
			else
				old->parent_node->right_child = node;
		}
		else
		{
			rb_tree->rb_node_root = node;
		}
		/* old有两孩子节点，我们是取右孩子的最左孩子替代old节点，
		 * 如果最左节点就是右孩子，则右孩子已经被删除，所以此处
		 * 要判断右孩子是否为空*/
		old->left_child->parent_node = node;
		if (NULL != old->right_child)
			old->right_child->parent_node = node;
		goto color;
#endif
	}

	parent = node->parent_node;
	color = node->node_color;
	/*直接删除node节点*/
	if (NULL != child)
		child->parent_node = parent;
	if (NULL != parent)
	{
		if (parent->left_child == node)
			parent->left_child = child;
		else
			parent->right_child = child;
	}
	else
	{
		rb_tree->rb_node_root = child;
	}

	color: if (color == RB_BLACK) /*删除节点为红色不影响红黑树性质*/
		_rb_tree_move_color(rb_tree, parent, child);
	rb_tree->node_size--;
	RB_NODE_INIT(old);

	return old;
}

/*查找节点node，找到则返回节点指针，否则返回NULL*/
rb_tree_node_t *rb_tree_find(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *tmp;
	int comp;

	tmp = rb_tree->rb_node_root;
	while (NULL != tmp)
	{
		comp = rb_tree->rb_node_compare(tmp, node);
		if (comp < 0)
			tmp = tmp->right_child;
		else if (comp > 0)
			tmp = tmp->left_child;
		else
			return tmp;
	}

	return NULL;
}

/*查找树中第一个大于或等于node的节点*/
rb_tree_node_t *rb_tree_nfind(rb_tree_t *rb_tree, rb_tree_node_t *node)
{
	rb_tree_node_t *tmp;
	rb_tree_node_t *res;
	int comp;

	tmp = rb_tree->rb_node_root;
	while (NULL != tmp)
	{
		comp = rb_tree->rb_node_compare(tmp, node);
		if (comp > 0)
		{
			res = tmp;
			tmp = tmp->left_child;
		}
		else if (comp < 0)
			tmp = tmp->right_child;
		else
			return tmp;
	}

	return res;
}

/*返回下一大于值节点，递增排序*/
rb_tree_node_t *rb_tree_next(rb_tree_node_t *node)
{
	if (NULL != node->right_child)
	{
		node = node->right_child;
		while (NULL != node->left_child)
			node = node->left_child;
	}
	else
	{
		if (NULL != node->parent_node && (node == node->parent_node->left_child))
			node = node->parent_node;
		else
		{
			while (NULL != node->parent_node && (node == node->parent_node->right_child))
				node = node->parent_node;
			node = node->parent_node;
		}
	}

	return node;
}

/*返回下一小于值节点，递减排序*/
rb_tree_node_t *rb_tree_prev(rb_tree_node_t *node)
{
	if (NULL != node->left_child)
	{
		node = node->left_child;
		while (NULL != node->right_child)
			node = node->right_child;
	}
	else
	{
		if (NULL != node->parent_node && (node == node->parent_node->right_child))
			node = node->parent_node;
		else
		{
			while (NULL != node->parent_node && (node == node->parent_node->left_child))
				node = node->parent_node;
			node = node->parent_node;
		}
	}
	return node;
}

/*查找树中最大或最小节点*/
rb_tree_node_t *rb_tree_min_or_max(rb_tree_t *rb_tree, int flag)
{
	rb_tree_node_t *tmp, *parent;

	parent = tmp = rb_tree->rb_node_root;
	while (NULL != tmp)
	{
		parent = tmp;
		if (GET_MIN == flag)
			tmp = tmp->left_child;
		else
			tmp = tmp->right_child;
	}

	return parent;
}
