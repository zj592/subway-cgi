#include "subway.h"

// 显示主菜单
void show_menu() {
    printf("\n===== 深圳地铁查询系统 =====\n");
    printf("1. 查询路线\n");
    printf("2. 查看所有站点\n");
    printf("3. 退出系统\n");
    printf("请选择操作 (1-3): ");
}

// 获取用户输入的站点名称
void get_station_name(char* name, const char* prompt) {
    printf("%s: ", prompt);
    fflush(stdin);
    fgets(name, MAX_NAME_LEN, stdin);
    // 移除换行符
    name[strcspn(name, "\n")] = '\0';
}

int main() {
    // 创建地铁网络
    SubwayNetwork* network = create_subway_network();
    
    // 加载数据
    printf("正在加载地铁数据...\n");
    load_lines(network, "lines.txt");
    load_stations(network, "stations.txt");
    load_connections(network, "connections.txt");
    
    printf("数据加载完成，欢迎使用深圳地铁查询系统！\n");
    
    int choice;
    do {
        show_menu();
        scanf("%d", &choice);
        getchar(); // 吸收换行符
        
        switch (choice) {
            case 1: {
                char start_name[MAX_NAME_LEN];
                char end_name[MAX_NAME_LEN];
                
                get_station_name(start_name, "请输入起点站名称");
                get_station_name(end_name, "请输入终点站名称");
                
                int start_id = find_station_id(network, start_name);
                int end_id = find_station_id(network, end_name);
                
                if (start_id == -1) {
                    printf("未找到站点: %s\n", start_name);
                    break;
                }
                
                if (end_id == -1) {
                    printf("未找到站点: %s\n", end_name);
                    break;
                }
                
                if (start_id == end_id) {
                    printf("起点和终点相同，请重新输入！\n");
                    break;
                }
                
                // 执行Dijkstra算法
                DijkstraNode nodes[MAX_STATIONS];
                dijkstra(network, start_id, end_id, nodes);
                
                // 打印路径
                print_path(network, nodes, start_id, end_id);
                break;
            }
            
            case 2:
                print_stations(network);
                break;
            
            case 3:
                printf("感谢使用深圳地铁查询系统，再见！\n");
                break;
            
            default:
                printf("无效的选择，请重新输入！\n");
        }
    } while (choice != 3);
    
    // 释放资源
    free_subway_network(network);
    
    return 0;
}
