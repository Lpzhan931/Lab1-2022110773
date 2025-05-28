//
// Created by 86178 on 2025/5/28.
//

#ifndef SELAB1_GRAPH_H
#define SELAB1_GRAPH_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cctype>
#include <random>
#include <thread>
#include <cmath>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>
#include <functional>

using namespace std;

vector<string> processFile(const string& filename);
vector<string> splitCommand(const string& s);
string toLower(const string& s);

class Graph {
private:
    unordered_map<string, unordered_map<string, int>> adjList;
    unordered_map<string, vector<string>> inEdges;
    unordered_map<string, int> outDegree;

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
        // 初始均分
        for (const auto& [node, _] : adjList) {
            pr[node] = 1.0 / N;
        }

        // 开始迭代
        for (int i = 0; i < iterations; ++i) {
            unordered_map<string, double> new_pr;
            // 求和无出边节点总PR值（均分给所有点）
            double dangling_sum = 0.0;
            for (const auto& [node, _] : adjList) {
                if (outDegree[node] == 0) {
                    dangling_sum += pr[node];
                }
            }

            // 每个点的新PR值由两部分：入边分来的、无出边节点分来的
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

    function<string()> inputProvider = []() {
        string input;
        getline(cin, input);
        return input;
    };

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
            if (visitedEdges.count(edge)) {
//                cout << "Last: " << next << "\n";
                break;
            }

            visitedEdges.insert(edge);

            path.push_back(next);
            current = next;

            cout << "Current path: ";
            for (const string& node : path) cout << node << " ";
            cout << "\nContinue? (Y/n): ";
//            string input;
//            getline(cin, input);
            string input = inputProvider();
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

void handleShortestPath(Graph& graph, const vector<string>& args);


#endif //SELAB1_GRAPH_H
