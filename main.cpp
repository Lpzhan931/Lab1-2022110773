
#include "graph.h"

using namespace std;

// 辅助函数：将字符串转换为小写
string toLower(const string& s) {
    string result;
    for (char c : s) {
        result += tolower(c);
    }
    return result;
}

// 分割命令参数
vector<string> splitCommand(const string& s) {
    vector<string> args;
    stringstream ss(s);
    string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }
    return args;
}

// 处理文本文件
vector<string> processFile(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        string processed;
        for (char c : line) {
            if (isalpha(c)) {
                processed += tolower(c);
            } else {
                processed += ' ';
            }
        }
        stringstream ss(processed);
        string word;
        while (ss >> word) {
            words.push_back(word);
        }
    }
    return words;
}


void handleShortestPath(Graph& graph, const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Usage: shortestpath <word1> [word2]\n";
        return;
    }

    string word1 = toLower(args[1]);
    if (!graph.hasNode(word1)) {
        cout << word1 << " not in graph!\n";
        return;
    }

    if (args.size() == 2) {
        auto result = graph.dijkstra(word1);
        for (const auto& [node, dist] : result.distances) {
            if (node == word1) continue;
            cout << "To " << node << ": ";
            if (dist == INT_MAX) {
                cout << "Unreachable\n";
            } else {
                auto paths = graph.getAllShortestPaths(word1, node, result);
                cout << "Distance " << dist << " | Paths: ";
                for (const auto& path : paths) {
                    cout << "[";
                    for (size_t i = 0; i < path.size(); ++i) {
                        if (i > 0) cout << "->";
                        cout << path[i];
                    }
                    cout << "] ";
                }
                cout << "\n";
            }
        }
    } else {
        string word2 = toLower(args[2]);
        if (!graph.hasNode(word2)) {
            cout << word2 << " not in graph!\n";
            return;
        }

        auto result = graph.dijkstra(word1);
        auto paths = graph.getAllShortestPaths(word1, word2, result);
        if (paths.empty()) {
            cout << "No path between " << word1 << " and " << word2 << "\n";
        } else {
            cout << "Shortest paths (" << result.distances[word2] << "):\n";
            for (const auto& path : paths) {
                for (size_t i = 0; i < path.size(); ++i) {
                    if (i > 0) cout << "->";
                    cout << path[i];
                }
                cout << "\n";
            }
        }
    }
}

int main111(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    vector<string> words = processFile(argv[1]);
    Graph graph;
    graph.buildGraph(words);

    cout << "Generated Graph:\n";
    graph.display();

    string command;
    while (true) {
        cout << "\n> ";
        getline(cin, command);
        vector<string> args = splitCommand(command);
        if (args.empty()) continue;

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "bridge" && args.size() == 3) {
            auto bridges = graph.getBridgeWords(args[1], args[2]);
            if (bridges.empty()) {
                if (!graph.hasNode(args[1]) || !graph.hasNode(args[2])) {
                    cout << "No " << (!graph.hasNode(args[1]) ? args[1] : args[2]) << " in graph!\n";
                } else {
                    cout << "No bridge words between " << args[1] << " and " << args[2] << "\n";
                }
            } else {
                cout << "Bridge words: ";
                for (size_t i = 0; i < bridges.size(); ++i) {
                    if (i > 0) cout << (i == bridges.size()-1 ? " and " : ", ");
                    cout << bridges[i];
                }
                cout << "\n";
            }
        } else if (args[0] == "generate") {
            cout << "Enter text: ";
            string text;
            getline(cin, text);
            cout << "New text: " << graph.generateNewText(text) << "\n";
        } else if (args[0] == "shortestpath") {
            handleShortestPath(graph, args);
        } else if (args[0] == "pagerank") {
            graph.calculatePageRank();
        } else if (args[0] == "randomwalk") {
            string filename = graph.randomWalk();
            cout << "Random walk saved to " << filename << "\n";
        } else {
            cout << "Unknown command\n";
        }
    }

    return 0;

}




//
//int add(int a, int b) {
//    return a + b;
//}
//
//TEST(add, zero) {
//    EXPECT_EQ(0, add(0, 0)) << "yes";
//}
//
//TEST(add, positive_number) {
//    EXPECT_EQ(3, add(1, 2));
//}
//
//TEST(add, negative_number) {
//    EXPECT_EQ(-3, add(-1, -2));
//}
//
//int main() {
//    ::testing::InitGoogleTest();
//    return RUN_ALL_TESTS();
//}

