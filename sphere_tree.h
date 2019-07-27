#pragma once
#include "gl/glew.h"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <iostream>
#include <stack>

// ********************************
// 空间Sphere Tree，用于加速求交
// 现在还没实现完GPU版本
// 之后可以用在黑洞周围的粒子显示上
// ********************************

using namespace std;
using namespace glm;

float length2(vec3 v) {
    return dot(v, v);
}

struct LeafSphereInfo {
    int particle_type;
    int __no_use;
};
static_assert(sizeof(LeafSphereInfo) == 8, "size");


struct Sphere {
    vec3 position;
    float radius = -1;
    bool contains(vec3 pos) {
        return radius * radius >= length2(pos - position);
    }
    Sphere() = default;
    Sphere(vec3 p, float r) : position(p), radius(r) {};
};

struct SphereTreeNode {
    Sphere sphere;
    vector<SphereTreeNode*> children;
    LeafSphereInfo info;
    int leaf_index = -1;
    //SphereTreeNode* left = nullptr;
    //SphereTreeNode* right = nullptr;
    bool leaf() { return children.size() == 0; }
    int height() {
        int h = -1;
        for (SphereTreeNode* child : children) {
            h = max(h, child->height());
        }
        return h + 1;
    }
    SphereTreeNode(Sphere sp) : sphere(sp) {};
};

struct GPUSphereTreeNode {
    vec3 position;
    float radius;
    int on_hit; // next. -1 = particle
    int on_miss; // miss. -1 = end
};
static_assert(sizeof(GPUSphereTreeNode) == 24, "size");


class GPUSphereTree {
    int _n_nodes;
    GPUSphereTreeNode* _nodes;
    int _n_info;
    LeafSphereInfo* _info;

public:
    GPUSphereTree(int n_nodes, GPUSphereTreeNode* nodes, int n_info, LeafSphereInfo* info) : _n_nodes(n_nodes), _nodes(nodes), _n_info(n_info), _info(info) {

    }
    ~GPUSphereTree() {
        if (_nodes) delete _nodes;
        if (_info) delete _info;
    }
};

class SphereTree {
    vector<Sphere> _spheres;
    SphereTreeNode* _root = nullptr;
    
    float find_coverage_radius(vec3 center, const vector<Sphere>& spheres) {
        int i_max_dist;
        float max_dist = -1;
        int i = 0;
        for (auto& sp : spheres) {
            float dist = length(sp.position - center) + sp.radius;
            if (dist > max_dist) {
                i_max_dist = i;
                max_dist = dist;
            }
            i++;
        }
        return max_dist;
    }

    int pick_farthest(vec3 center, const vector<Sphere>& spheres) {
        int i_max_dist;
        float max_dist2 = -1;
        int i = 0;
        for (auto& sp : spheres) {
            float dist2 = length2(sp.position - center);
            if (dist2 > max_dist2) {
                i_max_dist = i;
                max_dist2 = dist2;
            }
            i++;
        }
        return i_max_dist;
    }

    int count_nodes(SphereTreeNode* node) {
        int c = 1;
        for (auto& child : node->children) {
            c += count_nodes(child);
        }
        return c;
    }

    SphereTreeNode* construct_node(vector<Sphere> spheres) {
        if (spheres.size() == 0)  return nullptr;
        if (spheres.size() == 1)  return new SphereTreeNode(spheres[0]);

        // binary partition the spheres
        vector<Sphere> left;
        vector<Sphere> right;
        left.reserve(spheres.size() / 2);
        right.reserve(spheres.size() / 2);

        // pick a random sphere
        int i1 = rand() % spheres.size();
        vec3 center1 = spheres[i1].position;

        // pick a farthest sphere
        int i2 = pick_farthest(center1, spheres);
        vec3 center2 = spheres[i2].position;

        // classify the spheres to left or right
        for (auto& sp : spheres) {
            float l1 = length(sp.position - center1);
            float l2 = length(sp.position - center2);
            if (l1 < l2) {
                left.push_back(sp);
            }
            else {
                right.push_back(sp);
            }
        }

        // find center position
        vec3 sum_pos = vec3();
        for (auto& sp : spheres) {
            sum_pos += sp.position;
        }
        vec3 avg_pos = sum_pos / float(spheres.size());
        float cover = find_coverage_radius(avg_pos, spheres);

        auto node = new SphereTreeNode(Sphere(avg_pos, cover));
        if (left.size() == spheres.size() || right.size() == spheres.size()) {
            for (auto& sp : spheres) {
                node->children.push_back(new SphereTreeNode(sp));
            }
        }
        else {
            node->children.push_back(construct_node(move(left)));
            node->children.push_back(construct_node(move(right)));
        }
        return node;
    }

public:
    SphereTree(vector<Sphere> spheres) {
        _spheres = move(spheres);
        _root = construct_node(_spheres);
    };

    SphereTree(const SphereTree&) = delete;
    SphereTree(SphereTree&&) = delete;

    int height() {
        return _root->height();
    }

    SphereTreeNode* query_first(vec3 position, int& amt) {
        amt = 0;
        stack<SphereTreeNode*> stk;
        stk.push(_root);
        while (!stk.empty()) {
            auto current = stk.top();
            stk.pop();
            if (current->sphere.contains(position)) {
                amt++;
                if (current->leaf()) return current;
                for (int i = current->children.size() - 1; i >= 0; i--) {
                    stk.push(current->children[i]);
                }
            }
        }
        return nullptr;
    }

    GPUSphereTree generate_gpu_tree() {
        auto* leaf_info = new LeafSphereInfo[_spheres.size()];
        auto* nodes = new GPUSphereTreeNode[count_nodes(_root)];

        stack<SphereTreeNode*> stk;
        stk.push(_root);
        int i_node = 0;
        int i_leaf = 0;
        while (!stk.empty()) {
            auto current = stk.top();
            stk.pop();
            
            i_node++;
        }

        GPUSphereTree gt(i_node, nodes, _spheres.size(), leaf_info);
        return gt;
    };
};

// 测试程序
int main2() {
    // vec3 v = vec3();
    vector<Sphere> spheres;
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            spheres.push_back(Sphere(vec3(i, j, 0), 1));
        }
    }


    //spheres.push_back(Sphere(vec3(0, 0, 0), 1));
    //spheres.push_back(Sphere(vec3(0, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(2, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(3, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(4, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(4, 2, 0), .5));
    //spheres.push_back(Sphere(vec3(4, 1, 0), .5));
    //spheres.push_back(Sphere(vec3(8, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(9, 0, 0), .5));
    //spheres.push_back(Sphere(vec3(9, 1, 0), .5));
    //spheres.push_back(Sphere(vec3(8, 2, 0), .5));

    SphereTree tree(move(spheres));
    int h = tree.height();
    int amt = 0;
    auto result = tree.query_first(vec3(21.3, 28, 0), amt);

    return 1;
}