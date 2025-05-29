///// 黑盒测试
//
//#include <gtest/gtest.h>
//#include <fstream>
//#include "graph.h"  // 假设 Graph 类定义在此头文件中
//
//class BridgeWordsTest : public ::testing::Test {
//protected:
//    Graph graph;
//
//    // 从 EasyText.txt 构建图
//    void SetUp() override {
//        std::ifstream file("E:/Project/clion/SELab1/EasyTest.txt");
//        std::string line;
//        while (getline(file, line)) {
//            std::istringstream iss(line);
//            std::string prev, curr;
//            iss >> prev;
//            while (iss >> curr) {
//                graph.addEdge(prev, curr);  // 假设 addEdge 方法构建邻接表
//                prev = curr;
//            }
//        }
//    }
//};
//
//// 测试用例1: 有效输入，存在桥接词 "carefully"
//TEST_F(BridgeWordsTest, Case1_ScientistAnalyzed) {
//    auto bridges = graph.getBridgeWords("scientist", "analyzed");
//    ASSERT_EQ(bridges.size(), 1);
//    EXPECT_EQ(bridges[0], "carefully");  // 覆盖等价类 (1)(3)
//}
//
//// 测试用例2: 有效输入，无桥接词
//TEST_F(BridgeWordsTest, Case2_TheData) {
//    auto bridges = graph.getBridgeWords("the", "data");
//    EXPECT_TRUE(bridges.empty());  // 覆盖等价类 (1)(2)
//}
//
//// 测试用例3: word2不存在于图中
//TEST_F(BridgeWordsTest, Case3_DataUnknown) {
//    auto bridges = graph.getBridgeWords("data", "unknown");
//    EXPECT_TRUE(bridges.empty());  // 覆盖等价类 (7)
//}
//
//// 测试用例4: word1不存在于图中
//TEST_F(BridgeWordsTest, Case4_UnknownThe) {
//    auto bridges = graph.getBridgeWords("unknown", "the");
//    EXPECT_TRUE(bridges.empty());  // 覆盖等价类 (6)
//}
//
//// 测试用例5: 输入大小写混合，自动转小写后有效
//TEST_F(BridgeWordsTest, Case5_UpperCaseInput) {
//    auto bridges = graph.getBridgeWords("Scientist", "Analyzed");
//    ASSERT_EQ(bridges.size(), 1);
//    EXPECT_EQ(bridges[0], "carefully");  // 覆盖等价类 (1)(3)(5)
//}
//
//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

#include "gtest/gtest.h"
#include "graph.h"

using namespace std;

class BridgeWordsTest : public ::testing::Test {
protected:
    Graph g;

    void SetUp() override {
        // 构造测试图结构，覆盖全部等价类
        g.addEdge("A", "B");
        g.addEdge("B", "C");             // A → B → C （TC1、TC6）
        g.addEdge("X", "Y");
        g.addEdge("Y", "Z");
        g.addEdge("X", "W");
        g.addEdge("W", "Z");             // X → Y → Z 和 X → W → Z（TC2）
        g.addEdge("D", "E");
        g.addEdge("F", "G");             // 无桥接词（TC3）
    }
};

// TC1: 存在一个桥接词（EC3）
TEST_F(BridgeWordsTest, SingleBridgeWord) {
    vector<string> expected = {"b"};
    EXPECT_EQ(g.getBridgeWords("A", "C"), expected);
}

// TC2: 存在多个桥接词（EC4）
TEST_F(BridgeWordsTest, MultipleBridgeWords) {
//    vector<string> bridges = g.getBridgeWords("X", "Z");
//    EXPECT_THAT(bridges, ::testing::UnorderedElementsAre("y", "w"));
    vector<string> result = g.getBridgeWords("X", "Z");
    EXPECT_EQ(result.size(), 2);
    EXPECT_TRUE((result[0] == "y" && result[1] == "w") || (result[0] == "w" && result[1] == "y"));
}

// TC3: 无桥接词（EC5）
TEST_F(BridgeWordsTest, NoBridgeWordExists) {
    EXPECT_TRUE(g.getBridgeWords("D", "G").empty());
}

// TC4: word1 不存在（EC2）
TEST_F(BridgeWordsTest, Word1NotInGraph) {
    EXPECT_TRUE(g.getBridgeWords("Q", "A").empty());
}

// TC5: word2 不存在（EC2）
TEST_F(BridgeWordsTest, Word2NotInGraph) {
    EXPECT_TRUE(g.getBridgeWords("A", "R").empty());
}

// TC6: 大小写混合输入（EC7）
TEST_F(BridgeWordsTest, CaseInsensitiveInput) {
    vector<string> expected = {"b"};
    EXPECT_EQ(g.getBridgeWords("a", "C"), expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
