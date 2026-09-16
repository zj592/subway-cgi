#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define MAX_STATIONS 200
#define MAX_NAME_LEN 50
#define INF INT_MAX

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
} Station;

typedef struct EdgeNode {
    int to;
    int line;
    int time;
    struct EdgeNode* next;
} EdgeNode;

typedef struct {
    EdgeNode* head;
} AdjList;

Station stations[MAX_STATIONS];
int stationCount = 0;
AdjList adjList[MAX_STATIONS];

// 函数声明
int findStationId(const char* name);
int findStationIndex(int id);
const char* findStationName(int id);
void loadStations(const char* filename);
void loadConnections(const char* filename);
void dijkstra(int startId, int endId, int* dist, int* prev, int* line);
void parseQueryString(const char* query, char* from, char* to);
void urlDecode(char* str);

// 从名称查找站点ID
int findStationId(const char* name) {
    if (!name || strlen(name) == 0) return -1;
    for (int i = 0; i < stationCount; i++) {
        if (strstr(stations[i].name, name) != NULL) {
            return stations[i].id;
        }
    }
    return -1;
}

// 从ID查找站点索引
int findStationIndex(int id) {
    for (int i = 0; i < stationCount; i++) {
        if (stations[i].id == id) {
            return i;
        }
    }
    return -1;
}

// 从ID查找站点名称
const char* findStationName(int id) {
    int idx = findStationIndex(id);
    if (idx != -1) {
        return stations[idx].name;
    }
    return "未知站点";
}

// 加载站点数据
void loadStations(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;

    char line[100];
    fgets(line, sizeof(line), file); // 跳过表头

    stationCount = 0;
    while (fgets(line, sizeof(line), file) && stationCount < MAX_STATIONS) {
        line[strcspn(line, "\n\r")] = '\0';
        if (strlen(line) == 0) continue;

        int id;
        char name[MAX_NAME_LEN];
        if (sscanf(line, "%d,%[^,]", &id, name) == 2) {
            stations[stationCount].id = id;
            strncpy(stations[stationCount].name, name, MAX_NAME_LEN - 1);
            stations[stationCount].name[MAX_NAME_LEN - 1] = '\0';
            stationCount++;
        }
    }
    fclose(file);
}

// 加载连接数据
void loadConnections(const char* filename) {
    for (int i = 0; i < MAX_STATIONS; i++) {
        adjList[i].head = NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) return;

    char line[100];
    fgets(line, sizeof(line), file); // 跳过表头

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n\r")] = '\0';
        if (strlen(line) == 0) continue;

        int from, to, lineNum, time;
        if (sscanf(line, "%d,%d,%d,%d", &from, &to, &lineNum, &time) == 4) {
            int fromIdx = findStationIndex(from);
            int toIdx = findStationIndex(to);

            if (fromIdx != -1 && toIdx != -1) {
                // 正向边
                EdgeNode* newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
                newNode->to = toIdx;
                newNode->line = lineNum;
                newNode->time = time;
                newNode->next = adjList[fromIdx].head;
                adjList[fromIdx].head = newNode;

                // 反向边
                newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
                newNode->to = fromIdx;
                newNode->line = lineNum;
                newNode->time = time;
                newNode->next = adjList[toIdx].head;
                adjList[toIdx].head = newNode;
            }
        }
    }
    fclose(file);
}

// Dijkstra算法
void dijkstra(int startId, int endId, int* dist, int* prev, int* line) {
    int startIdx = findStationIndex(startId);
    int endIdx = findStationIndex(endId);
    if (startIdx == -1 || endIdx == -1) return;

    int visited[MAX_STATIONS] = {0};
    for (int i = 0; i < MAX_STATIONS; i++) {
        dist[i] = INF;
        prev[i] = -1;
        line[i] = -1;
    }
    dist[startIdx] = 0;

    for (int i = 0; i < stationCount; i++) {
        int u = -1;
        int minDist = INF;
        for (int j = 0; j < stationCount; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1 || u == endIdx) break;
        visited[u] = 1;

        EdgeNode* p = adjList[u].head;
        while (p) {
            int v = p->to;
            if (!visited[v] && dist[u] != INF && dist[u] + p->time < dist[v]) {
                dist[v] = dist[u] + p->time;
                prev[v] = u;
                line[v] = p->line;
            }
            p = p->next;
        }
    }
}

// 解析查询参数
void parseQueryString(const char* query, char* from, char* to) {
    from[0] = '\0';
    to[0] = '\0';
    if (!query || strlen(query) == 0) return;

    char* queryCopy = strdup(query);
    if (!queryCopy) return;

    char* pairs[10];
    int pairCount = 0;
    char* current = queryCopy;
    
    while (current && *current != '\0' && pairCount < 10) {
        pairs[pairCount++] = current;
        while (*current != '\0' && *current != '&') current++;
        if (*current == '&') {
            *current = '\0';
            current++;
        }
    }

    for (int i = 0; i < pairCount; i++) {
        char* pair = pairs[i];
        if (!pair || *pair == '\0') continue;

        char* key = pair;
        char* value = strchr(pair, '=');
        if (value) {
            *value = '\0';
            value++;
        } else {
            value = "";
        }

        if (strcmp(key, "from") == 0) {
            strncpy(from, value, MAX_NAME_LEN - 1);
            from[MAX_NAME_LEN - 1] = '\0';
        } else if (strcmp(key, "to") == 0) {
            strncpy(to, value, MAX_NAME_LEN - 1);
            to[MAX_NAME_LEN - 1] = '\0';
        }
    }

    free(queryCopy);
}

// URL解码
void urlDecode(char* str) {
    if (!str || strlen(str) == 0) return;
    char* p = str;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '+') {
            *p++ = ' ';
        } else if (str[i] == '%' && i + 2 < len) {
            char hex[3] = {str[i+1], str[i+2], '\0'};
            *p++ = (char)strtol(hex, NULL, 16);
            i += 2;
        } else {
            *p++ = str[i];
        }
    }
    *p = '\0';
}

// 生成线路颜色样式（根据线路号生成不同颜色）
const char* getLineColor(int lineNum) {
    switch(lineNum % 8) { // 循环使用8种颜色
        case 1: return "#E63946"; // 红色
        case 2: return "#4CC9F0"; // 蓝色
        case 3: return "#10B981"; // 绿色
        case 4: return "#F59E0B"; // 黄色
        case 5: return "#8B5CF6"; // 紫色
        case 6: return "#EC4899"; // 粉色
        case 7: return "#6B7280"; // 灰色
        default: return "#064E3B"; // 深绿
    }
}

int main() {
    // 输出HTTP头和样式
    printf("Content-Type: text/html; charset=utf-8\n\n");
    printf("<style>\n");
    printf(".result-container { border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); padding: 20px; margin-top: 20px; background: white; }\n");
    printf(".route-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; padding-bottom: 10px; border-bottom: 1px solid #eee; }\n");
    printf(".route-title { font-size: 18px; font-weight: bold; color: #333; margin: 0; }\n");
    printf(".route-meta { color: #666; font-size: 14px; }\n");
    printf(".route-list { list-style: none; padding: 0; margin: 15px 0; }\n");
    printf(".route-list li { padding: 12px 0; position: relative; padding-left: 25px; }\n");
    printf(".route-list li::before { content: '→'; position: absolute; left: 0; color: #999; }\n");
    printf(".route-list li:first-child::before { content: '✓'; color: #10B981; }\n");
    printf(".station { padding: 3px 8px; border-radius: 4px; margin: 0 3px; }\n");
    printf(".transfer { font-weight: bold; color: #E63946; }\n");
    printf(".line-badge { display: inline-block; width: 24px; height: 24px; line-height: 24px; text-align: center; border-radius: 50%%; color: white; font-weight: bold; margin-right: 6px; }\n");
    printf(".error { color: #E63946; padding: 15px; background: #FEF2F2; border-radius: 6px; }\n");
    printf(".info { color: #4B5563; padding: 15px; background: #F3F4F6; border-radius: 6px; }\n");
    printf("</style>\n");

    // 加载数据
    loadStations("C:/Apache24/cgi-bin/stations.txt");
    loadConnections("C:/Apache24/cgi-bin/connections.txt");

    // 解析参数
    char fromName[MAX_NAME_LEN] = {0};
    char toName[MAX_NAME_LEN] = {0};
    char* query = getenv("QUERY_STRING");
    parseQueryString(query, fromName, toName);
    urlDecode(fromName);
    urlDecode(toName);

    // 执行查询并输出结果
    if (strlen(fromName) > 0 && strlen(toName) > 0) {
        int startId = findStationId(fromName);
        int endId = findStationId(toName);

        printf("<div class='result-container'>\n");
        
        if (startId == -1) {
            printf("<div class='error'>未找到起点站：%s</div>\n", fromName);
        } else if (endId == -1) {
            printf("<div class='error'>未找到终点站：%s</div>\n", toName);
        } else if (startId == endId) {
            printf("<div class='info'>起点和终点相同，无需乘车</div>\n");
        } else {
            int dist[MAX_STATIONS];
            int prev[MAX_STATIONS];
            int line[MAX_STATIONS];
            dijkstra(startId, endId, dist, prev, line);

            int endIdx = findStationIndex(endId);
            if (dist[endIdx] == INF) {
                printf("<div class='error'>未找到可用路线，请检查站点连接</div>\n");
            } else {
                // 回溯路径
                int path[MAX_STATIONS];
                int pathLines[MAX_STATIONS];
                int pathLen = 0;
                for (int v = endIdx; v != -1 && pathLen < MAX_STATIONS; v = prev[v]) {
                    path[pathLen] = v;
                    pathLines[pathLen] = line[v];
                    pathLen++;
                }

                // 输出结果头部
                printf("<div class='route-header'>\n");
                printf("<h3 class='route-title'>%s → %s</h3>\n", fromName, toName);
                printf("<div class='route-meta'>总耗时：%d分钟 | 共%d站</div>\n", dist[endIdx], pathLen - 1);
                printf("</div>\n");

                // 输出路线列表
                printf("<ul class='route-list'>\n");
                if (pathLen >= 2) {
                    int currentLine = pathLines[pathLen - 2];
                    const char* color = getLineColor(currentLine);
                    
                    printf("<li>\n");
                    printf("<span class='line-badge' style='background:%s'>%d</span>\n", color, currentLine);
                    printf("<span class='station'>%s</span>", stations[path[pathLen - 1]].name);
                    
                    for (int i = pathLen - 2; i > 0; i--) {
                        if (pathLines[i] != currentLine) {
                            currentLine = pathLines[i];
                            color = getLineColor(currentLine);
                            printf(" → <span class='transfer'>换乘</span> ");
                            printf("<span class='line-badge' style='background:%s'>%d</span>\n", color, currentLine);
                            printf("<span class='station'>%s</span>", stations[path[i]].name);
                        } else {
                            printf(" → <span class='station'>%s</span>", stations[path[i]].name);
                        }
                    }
                    printf(" → <span class='station'>%s</span>\n", stations[path[0]].name);
                    printf("</li>\n");
                }
                printf("</ul>\n");
            }
        }
        printf("</div>\n");
    } else if (strlen(fromName) > 0) {
        printf("<div class='info'>请输入终点站</div>\n");
    }

    return 0;
}