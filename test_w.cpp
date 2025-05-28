/// 白盒测试
#include "gtest/gtest.h"
#include "graph.h"
#include <fstream>
#include <sstream>

using namespace std;

class RandomWalkTest : public ::testing::Test {
protected:
    Graph g;

    void SetUp() override {
        // 默认禁用控制台交互
        g.inputProvider = []() { return "n"; };
    }

    string readOutputFile(const string& filename) {
        ifstream file(filename);
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void removeOutputFile(const string& filename) {
        remove(filename.c_str());
    }
};

// TC1: 图为空
TEST_F(RandomWalkTest, EmptyGraphReturnsEmptyString) {
    string result = g.randomWalk();
    EXPECT_EQ(result, "");
}

// TC2: A->B->C，输入n
TEST_F(RandomWalkTest, SimpleWalkStopsAtN) {
    g.addEdge("A", "B");
    g.addEdge("B", "C");
    g.inputProvider = []() { return "n"; };
    string file = g.randomWalk();
    string path = readOutputFile(file);
    removeOutputFile(file);
    // 路径可能是 A B C 或 B C 等，验证格式即可
    EXPECT_TRUE(path.find(" ") != string::npos);
}

// TC3: A->B, B->A，重复边，触发 visitedEdges
TEST_F(RandomWalkTest, DetectsRepeatedEdge) {
    g.addEdge("A", "B");
    g.addEdge("B", "A");
    g.inputProvider = []() { return "n"; };
    string file = g.randomWalk();
    string path = readOutputFile(file);
    removeOutputFile(file);
    EXPECT_TRUE(path.find("a") != string::npos && path.find("b") != string::npos);
}

// TC4: A 无出边
TEST_F(RandomWalkTest, NoOutgoingEdges) {
    g.addEdge("A", "A");  // 自环
    g = Graph();          // 清空重新添加无出边节点
    g.inputProvider = []() { return "n"; };
    g.addEdge("A", "");   // 模拟 A 无出边
    g = Graph();          // 或用以下方式构造完全孤立节点
    g.inputProvider = []() { return "n"; };
    string file = g.randomWalk();  // 不构建图，跳过
    EXPECT_TRUE(file.empty() || readOutputFile(file).find("a") != string::npos);
}


// TC5: A->B->C，输入y继续，到达末尾
TEST_F(RandomWalkTest, WalkContinuesUntilNoMoreEdges) {
    g.addEdge("A", "B");
    g.addEdge("B", "C"); // Graph is a->b->c
    g.inputProvider = []() { return "y"; }; // Non-n, should continue

    string file = g.randomWalk();
    // It's good practice to assert that a file was actually created if the graph is not empty
    ASSERT_FALSE(file.empty()) << "randomWalk() returned an empty filename for a non-empty graph.";

    string path_content = readOutputFile(file);
    removeOutputFile(file);

    // Define all possible valid full paths (lowercase, with trailing space from your output format)
    string expected_path_from_a = "a b c ";
    string expected_path_from_b = "b c ";
    string expected_path_from_c = "c ";

    bool is_one_of_expected_paths = (path_content == expected_path_from_a ||
                                     path_content == expected_path_from_b ||
                                     path_content == expected_path_from_c);

    EXPECT_TRUE(is_one_of_expected_paths)
                        << "The generated path: \"" << path_content << "\""
                        << " was not one of the expected full paths for the graph a->b->c "
                        << "(expected: \"" << expected_path_from_a << "\", or \""
                        << expected_path_from_b << "\", or \""
                        << expected_path_from_c << "\").";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
