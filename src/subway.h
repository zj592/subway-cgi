#ifndef SUBWAY_H
#define SUBWAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// 定义最大常量
#define MAX_STATIONS 200       // 最大站点数量
#define MAX_LINES 20           // 最大线路数量
#define MAX_NAME_LEN 50        // 站点/线路名称最大长度
#define INF INT_MAX           

// 站点结构体
typedef struct {
    int id;                     // 站点ID
    char name[MAX_NAME_LEN];    // 站点名称
    int line_count;             // 经过的线路数量
    int lines[MAX_LINES];       // 经过的线路ID数组
} Station;

// 线路结构体
typedef struct {
    int id;                     // 线路ID
    char name[MAX_NAME_LEN];    // 线路名称
    char color[MAX_NAME_LEN];   // 线路颜色
} Line;

// 邻接表节点结构体 (表示站点之间的连接)
typedef struct EdgeNode {
    int to;                     // 目标站点ID
    int line;                   // 所属线路
    int time;                   // 所需时间(分钟)
    struct EdgeNode* next;      // 下一个节点
} EdgeNode;

// 地铁网络结构体
typedef struct {
    Station stations[MAX_STATIONS];  // 所有站点
    Line lines[MAX_LINES];           // 所有线路
    EdgeNode* adj[MAX_STATIONS];     // 邻接表
    int station_count;               // 站点数量
    int line_count;                  // 线路数量
} SubwayNetwork;

// Dijkstra算法中使用的节点信息
typedef struct {
    int distance;    // 距离起点的最短距离
    int prev;        // 前驱节点
    int line;        // 到达该节点的线路
} DijkstraNode;

// 函数声明
SubwayNetwork* create_subway_network();
void load_stations(SubwayNetwork* network, const char* filename);
void load_lines(SubwayNetwork* network, const char* filename);
void load_connections(SubwayNetwork* network, const char* filename);
void add_edge(SubwayNetwork* network, int from, int to, int line, int time);
void dijkstra(SubwayNetwork* network, int start, int end, DijkstraNode* nodes);
void print_path(SubwayNetwork* network, DijkstraNode* nodes, int start, int end);
int find_station_id(SubwayNetwork* network, const char* name);
void print_stations(SubwayNetwork* network);
void free_subway_network(SubwayNetwork* network);

#endif
