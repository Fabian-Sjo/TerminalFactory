#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define ROW 5
#define COL 5

typedef struct
{
    int parent_x;
    int parent_y;
    double f;
    double g;
    double h;
} Node;

typedef struct
{
    int row;
    int col;
} OpenNode;

int isValid(int row, int col)
{
    return row >= 0 && row < ROW &&
           col >= 0 && col < COL;
}

int isUnBlocked(int grid[][COL], int row, int col)
{
    return grid[row][col] == 1;
}

int isDestination(int row, int col,
                  int dest_row, int dest_col)
{
    return row == dest_row &&
           col == dest_col;
}

double calculateHValue(int row, int col,
                       int dest_row, int dest_col)
{
    double dx = row - dest_row;
    double dy = col - dest_col;

    return sqrt(dx * dx + dy * dy);
}

void tracePath(Node nodeDetails[][COL],
               int dest_row,
               int dest_col)
{
    int pathRow[ROW * COL];
    int pathCol[ROW * COL];
    int count = 0;

    int row = dest_row;
    int col = dest_col;

    while (!(nodeDetails[row][col].parent_x == row &&
             nodeDetails[row][col].parent_y == col))
    {
        pathRow[count] = row;
        pathCol[count] = col;
        count++;

        int parentRow = nodeDetails[row][col].parent_x;
        int parentCol = nodeDetails[row][col].parent_y;

        row = parentRow;
        col = parentCol;
    }

    pathRow[count] = row;
    pathCol[count] = col;
    count++;

    printf("\nPath:\n");

    for (int i = count - 1; i >= 0; i--)
    {
        printf("(%d,%d)", pathRow[i], pathCol[i]);

        if (i > 0)
            printf(" -> ");
    }

    printf("\n");
}

void aStarSearch(int grid[][COL],
                 int src_row,
                 int src_col,
                 int dest_row,
                 int dest_col)
{
    if (!isValid(src_row, src_col) ||
        !isValid(dest_row, dest_col))
    {
        printf("Invalid source or destination\n");
        return;
    }

    if (!isUnBlocked(grid, src_row, src_col) ||
        !isUnBlocked(grid, dest_row, dest_col))
    {
        printf("Source or destination blocked\n");
        return;
    }

    if (isDestination(src_row, src_col,
                      dest_row, dest_col))
    {
        printf("Already at destination\n");
        return;
    }

    int closedList[ROW][COL] = {0};
    Node nodeDetails[ROW][COL];

    for (int r = 0; r < ROW; r++)
    {
        for (int c = 0; c < COL; c++)
        {
            nodeDetails[r][c].f = DBL_MAX;
            nodeDetails[r][c].g = DBL_MAX;
            nodeDetails[r][c].h = DBL_MAX;
            nodeDetails[r][c].parent_x = -1;
            nodeDetails[r][c].parent_y = -1;
        }
    }

    nodeDetails[src_row][src_col].f = 0.0;
    nodeDetails[src_row][src_col].g = 0.0;
    nodeDetails[src_row][src_col].h = 0.0;
    nodeDetails[src_row][src_col].parent_x = src_row;
    nodeDetails[src_row][src_col].parent_y = src_col;

    OpenNode openList[ROW * COL];
    int openSize = 0;

    openList[openSize++] =
        (OpenNode){src_row, src_col};

    int rowOffset[8] =
        {-1, 0, 1, 0, -1, -1, 1, 1};

    int colOffset[8] =
        {0, -1, 0, 1, -1, 1, -1, 1};

    while (openSize > 0)
    {
        double minF = DBL_MAX;
        int minIndex = -1;

        for (int i = 0; i < openSize; i++)
        {
            int r = openList[i].row;
            int c = openList[i].col;

            if (nodeDetails[r][c].f < minF)
            {
                minF = nodeDetails[r][c].f;
                minIndex = i;
            }
        }

        OpenNode current =
            openList[minIndex];

        openList[minIndex] =
            openList[--openSize];

        int row = current.row;
        int col = current.col;

        closedList[row][col] = 1;

        printf("Checking (%d,%d)\n",
               row, col);

        for (int i = 0; i < 8; i++)
        {
            int newRow = row + rowOffset[i];
            int newCol = col + colOffset[i];

            if (!isValid(newRow, newCol))
                continue;

            if (isDestination(newRow,
                              newCol,
                              dest_row,
                              dest_col))
            {
                nodeDetails[newRow][newCol].parent_x = row;
                nodeDetails[newRow][newCol].parent_y = col;

                printf("Destination found\n");

                tracePath(nodeDetails,
                          dest_row,
                          dest_col);
                return;
            }

            if (closedList[newRow][newCol])
                continue;

            if (!isUnBlocked(grid,
                             newRow,
                             newCol))
                continue;

            double moveCost =
                (abs(rowOffset[i]) +
                 abs(colOffset[i]) == 2)
                    ? sqrt(2.0)
                    : 1.0;

            double gNew =
                nodeDetails[row][col].g +
                moveCost;

            double hNew =
                calculateHValue(newRow,
                                newCol,
                                dest_row,
                                dest_col);

            double fNew =
                gNew + hNew;

            if (fNew <
                nodeDetails[newRow][newCol].f)
            {
                nodeDetails[newRow][newCol].f = fNew;
                nodeDetails[newRow][newCol].g = gNew;
                nodeDetails[newRow][newCol].h = hNew;
                nodeDetails[newRow][newCol].parent_x = row;
                nodeDetails[newRow][newCol].parent_y = col;

                openList[openSize++] =
                    (OpenNode){newRow, newCol};
            }
        }
    }

    printf("Failed to find destination\n");
}

int main(void)
{
    int grid[ROW][COL] =
    {
        {1,0,1,1,1},
        {1,0,1,1,1},
        {1,0,1,1,1},
        {1,0,1,1,1},
        {1,1,1,1,1}
    };

    aStarSearch(grid,
                2, 2,
                0, 0);

    return 0;
}