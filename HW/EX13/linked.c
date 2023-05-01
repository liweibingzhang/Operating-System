#include <stdio.h>
#include <stdlib.h>
// gcc -g -Wall -std=c99 linked.c -o linked -lpthread
//  Make a short name for a Node.
typedef struct Node Node;

// Node to build our list from.
struct Node
{
  // Value in our list
  int data;
  // Pointer to the next node.
  Node *next;
};
Node *getTail(Node *head)
{
  while (head->next)
  {
    head = head->next;
  }
  return head;
}

Node *quicksort(Node *head)
{
  if (!head || !head->next)
    return head;
  Node left, mid, right;
  left.next = mid.next = right.next = NULL;

  Node *ltail = &left;
  Node *mtail = &mid;
  Node *rtail = &right;

  int val = head->data;
  for (Node *p = head; p; p = p->next)
  {
    if (p->data < val)
    {
      ltail->next = p;
      ltail = ltail->next;
    }
    else if (p->data == val)
    {
      mtail->next = p;
      mtail = mtail->next;
    }
    else
    {
      rtail->next = p;
      rtail = rtail->next;
    }
  }

  ltail->next = NULL;
  mtail->next = NULL;
  rtail->next = NULL;

  left.next = quicksort(left.next);
  right.next = quicksort(right.next);

  getTail(&left)->next = mid.next;
  getTail(&left)->next = right.next;
  return left.next;
}
void push(Node **headRef, int key)
{
  Node *t = (Node *)malloc(sizeof(Node));
  t->data = key;
  t->next = (*headRef);
  (*headRef) = t;
}
void displist(Node *h) 
{
    while (h) {
        printf("%d\n", h->data);
        h = h->next;
    }
}
int main(int argc, char *argv[])
{
  int n = 10;
  if (argc >= 2)
    if (argc > 2 || sscanf(argv[1], "%d", &n) != 1 ||
        n < 0)
    {
      fprintf(stderr, "usage: array <n>\n");
      exit(EXIT_FAILURE);
    }

  // The tail pointer in List makes it easy to grow the list
  // front-to-back.
  Node *head = NULL;
  for (int i = 0; i < n; i++)
  {
    push(&head, rand());
  }
  head = quicksort(head);
  // Print the sorted items.
  displist(head);
  // Free memory for the list.

  return 0;
}
