#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#define RETURN_OK       0
#define RETURN_ERROR    -1

#define WIDTH   15
#define HEIGHT  15

#define OPEN_LIST_LEN   WIDTH*HEIGHT
#define CLOSE_LIST_LEN  WIDTH*HEIGHT

typedef enum tag_MAP_TYPE_E
{
    NORMAL = 0,
    NOACCESS
}MAP_TYPE_E;

typedef enum tag_DIRECT_E
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    DIRECT_BUTT
}DIRECT_E;


typedef struct tag_SET_S
{
    int x, y;
}SET_S;

SET_S data_direct[] = {
    { 0,-1 }, //UP
    { 0,1 },  //DOWN
    { -1,0 }, //LEFT
    { 1,0 }   //RIGHT
};  //与DIRECT_E保持一致

typedef struct tag_NODE_S
{
    SET_S set;
    int G, H, F;
    struct tag_NODE_S *fatherNode;
}NODE_S;

typedef struct tag_LIST_S
{
    int len;
    NODE_S array[OPEN_LIST_LEN];
}LIST_S;

typedef struct tag_MAP_S
{
    SET_S set;
    MAP_TYPE_E type;
}MAP_S;
MAP_S g_map[WIDTH][HEIGHT];

char data_map[WIDTH][HEIGHT] = {
    // 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //0
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //1
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //2
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //3
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //4
    { '0', 'X', '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //5
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //6
    { '0', 'X', 'X', '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //7
    { '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //8
    { '0', '0', '0', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //9
    { '0', 'X', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //10
    { '0', 'X', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //11
    { '0', 'X', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //12
    { '0', 'X', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //13
    { '0', 'X', 'X', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' },  //14
};

LIST_S g_openList = { 0 };
LIST_S g_closeList = { 0 };

int isSetVaild(SET_S* set)
{
    return (set->x < WIDTH && set->x >= 0 && set->y < HEIGHT && set->y >= 0);
}

void initMap()
{
    int x = 0;
    int y = 0;
    for (x = 0; x < WIDTH; x++)
    {
        for (y = 0; y < HEIGHT; y++)
        {
            g_map[x][y].set.x = x;
            g_map[x][y].set.y = y;

            switch (data_map[y][x])
            {
            case '0': g_map[x][y].type = NORMAL; break;
            case 'X': g_map[x][y].type = NOACCESS; break;
            default:
                break;
            }
        }
    }
}

int isSetEqual(SET_S* set_1, SET_S* set_2)
{
    return ((set_1->x == set_2->x) && (set_1->y == set_2->y));
}

int checkSetInList(LIST_S* openList, SET_S* dst)
{
    int i = 0;
    for (i = 0; i < openList->len; i++)
    {
        if (isSetEqual(dst, &openList->array[i].set))
        {
            return RETURN_OK;
        }
    }
    return RETURN_ERROR;
}

int calcCurrNodeHopeNormal(NODE_S* node, SET_S* dst)
{
    return (abs(node->set.x - dst->x) + abs(node->set.y - dst->y));
}

void addNode2List(LIST_S* list, NODE_S* node)
{
    memcpy(&list->array[list->len], node, sizeof(NODE_S));
    list->len++;
}

void delLastNodeFromOpenList(LIST_S* openList)
{
    openList->len--;
}

void addSet2List(LIST_S* openList, NODE_S* fatherNode, SET_S* currSet, SET_S* dst, int(*calcCurrHope)(NODE_S* , SET_S*))
{
    NODE_S srcNode = { 0 };

    srcNode.set = *currSet;

    if (NULL != fatherNode)
    {
        srcNode.G = fatherNode->G + 1;
    }

    srcNode.H = calcCurrHope(&srcNode, dst);
    srcNode.F = srcNode.G + srcNode.H;
    srcNode.fatherNode = fatherNode;

    addNode2List(openList, &srcNode);
}

NODE_S* getMinNodeFromOpenList(LIST_S* openList)
{
    NODE_S tmp = { 0 };
    int minIndex = 0;
    int minF = 0;
    int i = 0;

    minF = openList->array[openList->len - 1].F;

    /* 遍历openlist寻找最小F的下标 */
    for (i = 0; i < openList->len; i++)
    {
        NODE_S *currNode = &openList->array[i];
        if (currNode->F < minF)
        {
            minIndex = i;
            minF = currNode->F;
        }
    }

    /* 将最小的节点与当前最后的节点交换位置，方便删除 */
    tmp = openList->array[openList->len - 1];
    openList->array[openList->len - 1] = openList->array[minIndex];
    openList->array[minIndex] = tmp;

    return &openList->array[openList->len - 1];
}

void calcSetByDirect(SET_S* set, DIRECT_E direct)
{
    set->x += data_direct[direct].x;
    set->y += data_direct[direct].y;
}

int isSetCanPass(SET_S* set)
{
    return (g_map[set->x][set->y].type != NOACCESS);
}

NODE_S* findNodeBySetFromList(LIST_S* list, SET_S* set)
{
    int i = 0;
    for (i = 0; i < list->len; i++)
    {
        if (list->array[i].set.x == set->x
            && list->array[i].set.y == set->y)
        {
            return &list->array[i];
        }
    }

    return NULL;
}

void checkIfNeedUpdateOpenList(SET_S* set, NODE_S* currNode)
{
    NODE_S* listNode = NULL;

    listNode = findNodeBySetFromList(&g_openList, set);
    if (NULL == listNode)
    {
        return;
    }

    if (listNode->G > currNode->G++)
    {
        listNode->G = currNode->G++;
        listNode->fatherNode = currNode;
    }

    return;
}

void handleNearbyBlock(NODE_S* currNode, DIRECT_E direct, SET_S* dst)
{
    SET_S currSet = currNode->set;

    calcSetByDirect(&currSet, direct);
    if (!isSetVaild(&currSet))
    {
        return;
    }

    /* 如果当前坐标在openlist中，检查是否更新G值 */
    if (RETURN_OK == checkSetInList(&g_openList, &currSet))
    {
        checkIfNeedUpdateOpenList(&currSet, currNode);
        return;
    }
    /* 如果当前坐标在closelist中，检查是否需要移出 */
    if (RETURN_OK == checkSetInList(&g_closeList, &currSet))
    {
        //TODO
        return;
    }
    /* 既不在openlist也不在closelist，检查是否能加入openlist */
    if (isSetCanPass(&currSet))
    {
        addSet2List(&g_openList, currNode, &currSet, dst, calcCurrNodeHopeNormal);
        return;
    }
}

int aStar(SET_S* src, SET_S* dst)
{
    NODE_S *currNode = {0};
    int i = 0;

    /* 先将源点加入openlist */
    addSet2List(&g_openList, NULL, src, dst, calcCurrNodeHopeNormal);

    while (RETURN_OK != checkSetInList(&g_openList, dst))
    {
        /* openlist中无元素，说明没有可用路径 */
        if (0 == g_openList.len)
        {
            return RETURN_ERROR;
        }

        currNode = getMinNodeFromOpenList(&g_openList);

        /* 将当前节点从openlist取出，放置到closelist */
        delLastNodeFromOpenList(&g_openList);
        addNode2List(&g_closeList, currNode);

        /* 最终节点保存在closelist中，所以需要再次获取节点地址 */
        currNode = &g_closeList.array[g_closeList.len-1];

        for (i = 0; i < DIRECT_BUTT; i++)
        {
            handleNearbyBlock(currNode, (DIRECT_E)i, dst);
        }
    }

    return RETURN_OK;
}

void printMap()
{
    int i, j;

    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            printf("%c ", data_map[i][j]);
        }
        printf("\n");
    }
}

void printRode()
{
    NODE_S* currNode = NULL;
    int depth = 0;

    currNode = &g_openList.array[g_openList.len - 1];

    while (NULL != currNode)
    {
        printf("(%d,%d)<-", currNode->set.x, currNode->set.y);
        currNode = currNode->fatherNode;
        depth++;
    }
    printf("Begin\t\ttotal: (%d)step\n", depth);
}

int main()
{
    SET_S src;
    SET_S dst;


    initMap();

    printf("Input [srcX] [srcY] [dstX] [dstY]:\n");
    scanf_s("%d %d %d %d", &src.x, &src.y, &dst.x, &dst.y);

    if (!(isSetVaild(&src) && isSetVaild(&dst)))
    {
        printf("Input error!\n");
        return RETURN_ERROR;
    }

    printMap();
    aStar(&src, &dst);
    printRode();

    printf("Calc finish!\n");
    
    return RETURN_OK;
}