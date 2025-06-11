#ifndef __LIST_H
#define __LIST_H

struct List_Node
{
	void* container;
	struct List_Node* nxt;
};

void Append_Node(struct List_Node* head, struct List_Node* node);
void Deque_Enque_Node(struct List_Node* head, struct List_Node* node);
void Delete_Node(struct List_Node* head, struct List_Node* node);

#endif
