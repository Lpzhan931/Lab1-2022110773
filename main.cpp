#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cctype>
#include <random>
#include <chrono>
#include <thread>
#include <functional>
#include <cmath>
#include <stack>
#include <utility>

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

class Graph {
private:
    unordered_map<string, unordered_map<string, int>> adjList;
    unordered_map<string, vector<string>> inEdges;
    unordered_map<string, int> outDegree;

//    struct pair_hash {
//        template <class T1, class T2>
//        size_t operator()(const pair<T1, T2>& p) const {
//            auto h1 = hash<T1>{}(p.first);
//            auto h2 = hash<T2>{}(p.second);
//            return h1 ^ h2;
//        }
//    };
    struct pair_hash {
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2>& p) const {
            size_t seed = 0;
            hash<T1> hasher1;
            hash<T2> hasher2;

            seed ^= hasher1(p.first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            seed ^= hasher2(p.second) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            return seed;
        }
    };

public:
    void buildGraph(const vector<string>& words) {
        if (words.size() < 2) return;
        for (size_t i = 0; i < words.size() - 1; ++i) {
            addEdge(words[i], words[i + 1]);
        }
    }

    void addEdge(const string& from, const string& to) {
        string f = toLower(from);
        string t = toLower(to);

        adjList[f];  // 若不存在，创建一个空邻接表
        adjList[t];

        adjList[f][t]++;
        inEdges[t].push_back(f);
        outDegree[f]++;
    }

    void display() const {
        for (const auto& [from, edges] : adjList) {
            cout << from << " -> ";
            bool first = true;
            for (const auto& [to, weight] : edges) {
                if (!first) cout << ", ";
                cout << to << "(" << weight << ")";
                first = false;
            }
            cout << endl;
        }
    }

    bool hasNode(const string& word) const {
//        return adjList.find(toLower(word)) != adjList.end();
        return adjList.count(toLower(word)) > 0;
    }

    vector<string> getBridgeWords(const string& word1, const string& word2) const {
        string w1 = toLower(word1);
        string w2 = toLower(word2);
        vector<string> bridges;

        auto it1 = adjList.find(w1);
        auto it2 = adjList.find(w2);
        if (it1 == adjList.end() || it2 == adjList.end()) {
            return bridges;
        }

        for (const auto& [word3, _] : it1->second) {
            const auto& edges = adjList.find(word3);
            if (edges != adjList.end() && edges->second.count(w2)) {
                bridges.push_back(word3);
            }
        }

        return bridges;
    }

    string generateNewText(const string& text) {
        vector<string> inputWords = processInputText(text);
        vector<string> newWords;

        for (size_t i = 0; i < inputWords.size(); ++i) {
            newWords.push_back(inputWords[i]);
            if (i < inputWords.size() - 1) {
                string current = inputWords[i];
                string next = inputWords[i + 1];
                vector<string> bridges = getBridgeWords(current, next);
                if (!bridges.empty()) {
                    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
                    uniform_int_distribution<int> dist(0, bridges.size() - 1);
                    newWords.push_back(bridges[dist(rng)]);
                }
            }
        }

        string result;
        for (const auto& word : newWords) {
            result += word + " ";
        }
        if (!result.empty()) result.pop_back();
        return result;
    }

    struct ShortestPathResult {
        unordered_map<string, int> distances;
        unordered_map<string, vector<string>> predecessors;
    };

    ShortestPathResult dijkstra(const string& start) {
        ShortestPathResult result;
        string s = toLower(start);

        for (const auto& [node, _] : adjList) {
            result.distances[node] = INT_MAX;
        }
        result.distances[s] = 0;

        using pii = pair<int, string>;
        priority_queue<pii, vector<pii>, greater<pii>> pq;
        pq.emplace(0, s);

        while (!pq.empty()) {
            auto [dist_u, u] = pq.top();
            pq.pop();

            if (dist_u > result.distances[u]) continue;

            if (adjList.find(u) == adjList.end()) continue;

            for (const auto& [v, weight] : adjList.at(u)) {
                int new_dist = dist_u + weight;
                if (new_dist < result.distances[v]) {
                    result.distances[v] = new_dist;
                    result.predecessors[v] = {u};
                    pq.emplace(new_dist, v);
                } else if (new_dist == result.distances[v]) {
                    result.predecessors[v].push_back(u);
                }
            }
        }

        return result;
    }

    vector<vector<string>> getAllShortestPaths(const string& start, const string& end, const ShortestPathResult& result) {
        vector<vector<string>> paths;
        if (result.distances.find(end) == result.distances.end() || result.distances.at(end) == INT_MAX) {
            return paths;
        }

        vector<string> path;
        getAllPaths(start, end, result.predecessors, path, paths);
        for (auto& p : paths) {
            reverse(p.begin(), p.end());
        }
        return paths;
    }

    void calculatePageRank(double damping = 0.85, int iterations = 100) {
        unordered_map<string, double> pr;
        int N = adjList.size();
        for (const auto& [node, _] : adjList) {
            pr[node] = 1.0 / N;
        }

        for (int i = 0; i < iterations; ++i) {
            unordered_map<string, double> new_pr;
            double dangling_sum = 0.0;
            for (const auto& [node, _] : adjList) {
                if (outDegree[node] == 0) {
                    dangling_sum += pr[node];
                }
            }

            for (const auto& [node, _] : adjList) {
                double sum = 0.0;
                for (const string& in_node : inEdges[node]) {
                    sum += pr[in_node] / outDegree[in_node];
                }
                new_pr[node] = (1 - damping) / N + damping * (sum + dangling_sum / N);
            }

            pr = new_pr;
        }

        vector<pair<string, double>> sorted(pr.begin(), pr.end());
        sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.second > b.second; });

        cout << "PageRank Results:\n";
        for (const auto& [node, score] : sorted) {
            cout << node << ": " << score << "\n";
        }
    }

    string randomWalk() {
        if (adjList.empty()) return "";

        vector<string> nodes;
        for (const auto& [node, _] : adjList) nodes.push_back(node);
        static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> dist(0, nodes.size() - 1);
        string current = nodes[dist(rng)];

        unordered_set<pair<string, string>, pair_hash> visitedEdges;
        vector<string> path = {current};

        while (true) {
            if (adjList[current].empty()) break;

            vector<string> outNodes;
            vector<int> weights;
            int total = 0;
            for (const auto& [to, w] : adjList[current]) {
                outNodes.push_back(to);
                weights.push_back(w);
                total += w;
            }

            uniform_int_distribution<int> edge_dist(0, total - 1);
            int r = edge_dist(rng);
            int sum = 0;
            string next;
            for (size_t i = 0; i < outNodes.size(); ++i) {
                sum += weights[i];
                if (r < sum) {
                    next = outNodes[i];
                    break;
                }
            }

            pair<string, string> edge = {current, next};
            if (visitedEdges.count(edge)) break;
            visitedEdges.insert(edge);

            path.push_back(next);
            current = next;

            cout << "Current path: ";
            for (const string& node : path) cout << node << " ";
            cout << "\nContinue? (Y/n): ";
            string input;
            getline(cin, input);
            if (!input.empty() && tolower(input[0]) == 'n') break;
        }

        ofstream out("random_walk.txt");
        for (const string& node : path) {
            out << node << " ";
        }
        out.close();
        return "random_walk.txt";
    }

private:
    vector<string> processInputText(const string& text) {
        string processed;
        for (char c : text) {
            if (isalpha(c)) {
                processed += tolower(c);
            } else {
                processed += ' ';
            }
        }
        vector<string> words;
        stringstream ss(processed);
        string word;
        while (ss >> word) {
            words.push_back(word);
        }
        return words;
    }

    void getAllPaths(const string& start, const string& current,
                     const unordered_map<string, vector<string>>& predecessors,
                     vector<string>& path, vector<vector<string>>& paths) {
        path.push_back(current);
        if (current == start) {
            paths.push_back(path);
        } else {
            for (const string& pred : predecessors.at(current)) {
                getAllPaths(start, pred, predecessors, path, paths);
            }
        }
        path.pop_back();
    }
};

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

int main(int argc, char* argv[]) {
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

    // Master 分支上的修改（R3）
    // B1 分支上的修改（第1个文件）
    // C4 上的修改（第1个文件）
    
}