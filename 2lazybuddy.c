/* Algorithm as given in William Stalling's book on OS */

#include <stdio.h>
#include <stdlib.h>

/* size is in powers of two
 * e.g. size = 9 means 2^9
 * loc_free = 1 means the memory block is locally free
 * loc_free = glob_free = 0 means the block is occupied
 */
struct node {
    int size;
    int loc_free;
    int glob_free;
    struct node *next;
};

#define FREE 0
#define ALLOC 1
#define LOCFREE 1
#define GLOBFREE 2
#define ANYFREE 3
#define ALLOCATED 0

int L[10], G[10], A[10], D[10], N[10];
struct node *prev = NULL, *head;
struct node *node1, *node2, *tmp;
struct node *nodetmp, *nodesplit;

/* Fetch a new block of required size */
struct node *getnode(int i)
{
    tmp = (struct node *)malloc(sizeof(struct node));
    tmp->size = i;
    tmp->loc_free = 1;
    tmp->glob_free = 0;
    tmp->next = NULL;
    return tmp;
}

/* to print the vector */
void printvec(int arr[])
{
    int k = 0;
    while (k < 10) {
        printf("%d ", arr[k]);
        k++;
    }
    puts("");
}

/* Split a free block in two */
struct node *split(struct node *nodespl, int i, int j)
{
    
    if (j > i) {
        node1 = getnode(j - 1);
        node2 = getnode(j - 1);
        N[j]--;
        N[j - 1] += 2;
        if (nodespl->loc_free == 1)
            L[j]--;
        else
            G[j]--;
        L[j - 1] += 2;
        if (prev == NULL) {
            node1->next = node2;
            node2->next = head->next;
            head = node1;
        } else {
            prev->next = node1;
            node1->next = node2;
            node2->next = nodespl->next;
        }
        free(nodespl);
        
        split(node1, i, j - 1);
    }
    return nodespl;
}

/* Search for the required node nearest to head */
struct node *search(struct node *srchnode, int i, int which)
{
    tmp = srchnode;
    prev = NULL;
    switch (which) {
    case LOCFREE:
        while (tmp->next != NULL) {
            if (tmp->loc_free == 1 && tmp->size == i) {
                return tmp;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        break;
    case GLOBFREE:
        while (tmp->next != NULL) {
            if (tmp->glob_free == 1 && tmp->size == i) {
                return tmp;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        break;
    case ANYFREE:
        while (tmp->next != NULL) {
            if ((tmp->loc_free == 1 || tmp->glob_free == 1)
                && tmp->size == i) {
                return tmp;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        break;
    case ALLOCATED:
        while (tmp->next != NULL) {
            if ((tmp->loc_free == 0 && tmp->glob_free == 0)
                && tmp->size == i) {
                return tmp;
            }
            prev = tmp;
            tmp = tmp->next;
        }
    default:
        break;
    }
    return tmp;
}

/* Coalesce two adjacent globally free nodes of the same size */
void coalesce(struct node *tmpnode)
{
    if (tmpnode->next && tmpnode->next->size == tmpnode->size && tmpnode->glob_free && tmpnode->next->glob_free) {
        G[tmpnode->size] -= 2;
        tmp = getnode(tmpnode->size + 1);
        tmp->loc_free = 0;
        tmp->glob_free = 1;
        G[tmpnode->size + 1]++;
        if (prev)
            prev->next = tmp;
        else
            head = tmp;
        tmp->next = tmpnode->next->next;
        free(tmpnode->next);
        free(tmpnode);
    }
}

int main()
{
    int i, j, reqsize, req, freesize;    // reqsize is the requested size for allocation
    N[9] = 1;
    L[9] = 1;
    
    head = getnode(9);
    puts("req: 1 - ALLOC, 0 - FREE, 9 - EXIT");
    scanf("%d", &req);
    while (req != 9) {
        if (req == ALLOC) {
            puts("reqsize");
            scanf("%d", &reqsize);
            i = reqsize;
            if (L[i] > 0) {
                nodetmp = search(head, i, LOCFREE);
                nodetmp->loc_free = 0;
                D[i] += 2;
                A[i]++;
                L[i]--;
            } else if (G[i] > 0) {
                nodetmp = search(head, i, GLOBFREE);
                nodetmp->glob_free = 0;
                D[i]++;
                A[i]++;
                G[i]--;
            } else {
                j = i;
                while (L[j] == 0 && G[j] == 0) {
                    j++;
                }
                nodetmp = search(head, j, ANYFREE);
                nodesplit = split(nodetmp, i, j);
                nodesplit = search(head, i, ANYFREE);
                nodesplit->loc_free = nodesplit->glob_free = 0;
                A[i]++;
                L[i]--;
                D[i] = (A[i] > L[i]) ? A[i] - L[i] : 0;
            }
            
            nodetmp = head;
            while (nodetmp) {    // print the list of free and allocated blocks
                printf("%d %d %d\n", nodetmp->size, nodetmp->loc_free, nodetmp->glob_free);
                nodetmp = nodetmp->next;
            }
            printvec(L);    // and print all the arrays
            printvec(G);
            printvec(A);
            printvec(D);
        } else {
            printf("enter size to be freed\n");
            scanf("%d", &freesize);
            if (A[freesize] == 0)
                break;
            i = freesize;
            if (D[i] >= 2) {
                nodetmp = search(head, i, ALLOCATED);
                nodetmp->loc_free = 1;
                L[i]++;
                A[i]--;
                D[i] -= 2;
            } else if (D[i] == 1) {
                nodetmp = search(head, i, ALLOCATED);
                nodetmp->glob_free = 1;
                G[i]++;
                A[i]--;
                coalesce(nodetmp);
                D[i] = 0;
            } else {
                nodetmp = search(head, i, ALLOCATED);
                nodetmp->glob_free = 1;
                G[i]++;
                A[i]--;
                coalesce(nodetmp);
                nodetmp = search(head, i, LOCFREE);
                if (nodetmp) {
                    nodetmp->loc_free = 0;
                    nodetmp->glob_free = 1;
                    G[i]++;
                    L[i]--;
                    coalesce(nodetmp);
                }
                D[i] = 0;
            }
            nodetmp = head;
            while (nodetmp) {
                printf("%d %d %d\n", nodetmp->size, nodetmp->loc_free, nodetmp->glob_free);
                nodetmp = nodetmp->next;
            }
            printvec(L);
            printvec(G);
            printvec(A);
            printvec(D);
        }
        puts("req: 1 - ALLOC, 0 - FREE, 9 - EXIT");
        scanf("%d", &req);
    }
    return 0;
}
