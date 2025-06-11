#include "List.h"

void Append_Node(struct List_Node* head, struct List_Node* node)
{
	struct List_Node* ptr = head;
	// find the back node
	while (ptr->nxt != 0)
	{
		ptr = ptr->nxt;
	}
	ptr->nxt = node;
}

// 将当前节点移至链表末尾
void Deque_Enque_Node(struct List_Node* head, struct List_Node* node)
{
	// 0 -> 1 -> 2 -> 3
	if (node->nxt == 0)
	{
		return;
	}
	
	head->nxt = node->nxt;
	node->nxt = 0;
	
	struct List_Node* ptr = head;
	while (ptr->nxt != 0)
	{
		ptr = ptr->nxt;
	}
	ptr->nxt = node;
}

void Delete_Node(struct List_Node* head, struct List_Node* node)
{
	struct List_Node* ptr = head;
	while (ptr->nxt != 0)
	{
		// dummy -> 1 -> 2
		if (ptr->nxt == node)
		{
			ptr->nxt = ptr->nxt->nxt;
			return;
		}
		ptr = ptr->nxt;
	}
}
