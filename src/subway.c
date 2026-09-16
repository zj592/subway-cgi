#include "subway.h"

// 创建地铁网络
SubwayNetwork* create_subway_network() {
    SubwayNetwork* network = (SubwayNetwork*)malloc(sizeof(SubwayNetwork));
    if (network == NULL) {
        printf("内存分配失败！\n");
        exit(1);
    }
    
    network->station_count = 0;
    network->line_count = 0;
    
    // 初始化邻接表
    for (int i = 0; i < MAX_STATIONS; i++) {
        network->adj[i] = NULL;
    }
    
    return network;
}

// 加载站点数据
void load_stations(SubwayNetwork* network, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开站点文件: %s\n", filename);
        exit(1);
    }
    
    char line[256];
    // 跳过表头
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        Station station;
        char lines_str[256];
        
        // 格式: id,name,lines(逗号分隔)
        if (sscanf(line, "%d,%[^,],%[^\n]", &station.id, station.name, lines_str) != 3) {
            printf("站点数据格式错误: %s", line);
            continue;
        }
        
        // 解析线路信息
        station.line_count = 0;
        char* token = strtok(lines_str, ",");
        while (token != NULL && station.line_count < MAX_LINES) {
            station.lines[station.line_count++] = atoi(token);
            token = strtok(NULL, ",");
        }
        
        network->stations[network->station_count++] = station;
    }
    
    fclose(file);
    printf("成功加载 %d 个站点\n", network->station_count);
}

// 加载线路数据
void load_lines(SubwayNetwork* network, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开线路文件: %s\n", filename);
        exit(1);
    }
    
    char line[256];
    // 跳过表头
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        Line line_data;
        
        // 格式: id,name,color
        if (sscanf(line, "%d,%[^,],%[^\n]", &line_data.id, line_data.name, line_data.color) != 3) {
            printf("线路数据格式错误: %s", line);
            continue;
        }
        
        network->lines[network->line_count++] = line_data;
    }
    
    fclose(file);
    printf("成功加载 %d 条线路\n", network->line_count);
}

// 加载连接关系数据
void load_connections(SubwayNetwork* network, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开连接文件: %s\n", filename);
        exit(1);
    }
    
    char line[256];
    // 跳过表头
    fgets(line, sizeof(line), file);
    
    int connection_count = 0;
    while (fgets(line, sizeof(line), file)) {
        int from, to, line_id, time;
        
        // 格式: from,to,line,time
        if (sscanf(line, "%d,%d,%d,%d", &from, &to, &line_id, &time) != 4) {
            printf("连接数据格式错误: %s", line);
            continue;
        }
        
        // 添加双向连接
        add_edge(network, from, to, line_id, time);
        add_edge(network, to, from, line_id, time);
        connection_count++;
    }
    
    fclose(file);
    printf("成功加载 %d 条连接关系\n", connection_count);
}

// 添加边到邻接表
void add_edge(SubwayNetwork* network, int from, int to, int line, int time) {
    EdgeNode* node = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (node == NULL) {
        printf("内存分配失败！\n");
        exit(1);
    }
    
    node->to = to;
    node->line = line;
    node->time = time;
    node->next = network->adj[from];
    network->adj[from] = node;
}

// Dijkstra算法实现
void dijkstra(SubwayNetwork* network, int start, int end, DijkstraNode* nodes) {
    int visited[MAX_STATIONS] = {0};
    
    // 初始化节点
    for (int i = 0; i < MAX_STATIONS; i++) {
        nodes[i].distance = INF;
        nodes[i].prev = -1;
        nodes[i].line = -1;
    }
    
    nodes[start].distance = 0;
    
    for (int i = 0; i < network->station_count; i++) {
        // 找到未访问的距离最小的节点
        int u = -1;
        int min_dist = INF;
        for (int j = 0; j < MAX_STATIONS; j++) {
            if (!visited[j] && nodes[j].distance < min_dist) {
                min_dist = nodes[j].distance;
                u = j;
            }
        }
        
        if (u == -1 || u == end) break; // 所有可达节点都已处理或到达终点
        
        visited[u] = 1;
        
        // 遍历所有邻接节点
        EdgeNode* edge = network->adj[u];
        while (edge != NULL) {
            int v = edge->to;
            int new_dist = nodes[u].distance + edge->time;
            
            if (!visited[v] && new_dist < nodes[v].distance) {
                nodes[v].distance = new_dist;
                nodes[v].prev = u;
                nodes[v].line = edge->line;
            }
            
            edge = edge->next;
        }
    }
}

// 查找站点ID
int find_station_id(SubwayNetwork* network, const char* name) {
    for (int i = 0; i < network->station_count; i++) {
        if (strcmp(network->stations[i].name, name) == 0) {
            return network->stations[i].id;
        }
    }
    return -1;
}

// 打印所有站点
void print_stations(SubwayNetwork* network) {
    printf("\n===== 深圳地铁站点列表 =====\n");
    for (int i = 0; i < network->station_count; i++) {
        printf("%d. %s (线路:", network->stations[i].id, network->stations[i].name);
        for (int j = 0; j < network->stations[i].line_count; j++) {
            printf(" %d", network->stations[i].lines[j]);
            if (j < network->stations[i].line_count - 1) {
                printf(",");
            }
        }
        printf(")\n");
    }
    printf("===========================\n");
}

// 打印路径和换乘信息
void print_path(SubwayNetwork* network, DijkstraNode* nodes, int start, int end) {
    if (nodes[end].distance == INF) {
        printf("无法找到从 %s 到 %s 的路径\n", 
               network->stations[start].name, 
               network->stations[end].name);
        return;
    }
    
    // 回溯路径
    int path[MAX_STATIONS];
    int path_lines[MAX_STATIONS];
    int length = 0;
    
    for (int at = end; at != -1; at = nodes[at].prev) {
        path[length] = at;
        path_lines[length] = nodes[at].line;
        length++;
    }
    
    // 反转路径
    for (int i = 0; i < length / 2; i++) {
        int temp = path[i];
        path[i] = path[length - 1 - i];
        path[length - 1 - i] = temp;
        
        temp = path_lines[i];
        path_lines[i] = path_lines[length - 1 - i];
        path_lines[length - 1 - i] = temp;
    }
    
    // 打印路径摘要
    printf("\n===== 出行路线 =====\n");
    printf("起点: %s\n", network->stations[start].name);
    printf("终点: %s\n", network->stations[end].name);
    printf("总时间: %d分钟\n", nodes[end].distance);
    
    // 分析换乘信息
    int transfer_count = 0;
    int current_line = path_lines[1]; // 第一条线路
    
    printf("\n详细路线:\n");
    printf("1. 从 %s 乘坐 ", network->stations[path[0]].name);
    
    // 查找线路名称
    char line_name[MAX_NAME_LEN];
    for (int i = 0; i < network->line_count; i++) {
        if (network->lines[i].id == current_line) {
            strcpy(line_name, network->lines[i].name);
            break;
        }
    }
    printf("%s 线\n", line_name);
    
    // 打印各站和换乘信息
    for (int i = 1; i < length; i++) {
        if (path_lines[i] != current_line && path_lines[i] != -1) {
            // 换乘
            transfer_count++;
            printf("   在 %s 换乘到 ", network->stations[path[i]].name);
            
            current_line = path_lines[i];
            for (int j = 0; j < network->line_count; j++) {
                if (network->lines[j].id == current_line) {
                    strcpy(line_name, network->lines[j].name);
                    break;
                }
            }
            printf("%s 线\n", line_name);
        } else if (i == length - 1) {
            // 终点
            printf("   到达终点 %s\n", network->stations[path[i]].name);
        }
    }
    
    printf("\n换乘次数: %d次\n", transfer_count);
    printf("====================\n");
}

// 释放地铁网络内存
void free_subway_network(SubwayNetwork* network) {
    if (network == NULL) return;
    
    // 释放邻接表
    for (int i = 0; i < MAX_STATIONS; i++) {
        EdgeNode* current = network->adj[i];
        while (current != NULL) {
            EdgeNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(network);
}
