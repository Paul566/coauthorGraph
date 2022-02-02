#include <stack>
#include "FreeStylePartitioner.h"

FreeStylePartitioner::FreeStylePartitioner(const std::string &adjList_filename, const std::string &ErdosIndex_filename)
        : Analyser(adjList_filename, ErdosIndex_filename) {
    shrunkGraph initialGraph;
    for (int i = 0; i < size; i++) {
        std::vector<std::pair<int, int>> myvector;
        for (int to: adjList[i])
            myvector.emplace_back(to, 1);
        initialGraph.adjList.emplace_back(myvector);
        initialGraph.getBack.emplace_back(1, i);
    }
    initialGraph.weights = std::vector<int>(size, 1);
    initialGraph.size = size;
    tower.push_back(std::make_shared<shrunkGraph>(initialGraph));
}

void FreeStylePartitioner::hideLeaves() {
    std::vector<int> newWeights = tower.back()->weights;
    std::vector<int> newIndices;
    std::vector<int> degs;
    int newSize = tower.back()->size;
    std::vector<std::vector<int>> insides; // insides[i] is a list of vertices moved inside the i-th vertex
    insides.reserve(tower.back()->size);
    for (int i = 0; i < tower.back()->size; i++) {
        insides.emplace_back(1, i);
        degs.push_back(tower.back()->adjList[i].size());
    }

    for (int i = 0; i < tower.back()->size; i++) {
        int cur = i;
        while (degs[cur] == 1) {
            newSize--;
            int parent;
            for (auto &to: tower.back()->adjList[cur]) {
                if (newWeights[to.first]) {
                    parent = to.first;
                    break;
                }
            }
            newWeights[cur] = 0;
            newWeights[parent] += newWeights[cur];
            degs[parent]--;
            degs[cur] = 0;
            insides[parent].insert(insides[parent].end(), insides[cur].begin(), insides[cur].end());
            cur = parent;
        }
    }

    int cnt = 0;
    for (int i = 0; i < tower.back()->size; i++) {
        if (newWeights[i] == 0)
            newIndices.push_back(-1);
        else {
            newIndices.push_back(cnt);
            cnt++;
        }
    }

    shrunkGraph newGraph;
    for (int i = 0; i < tower.back()->size; i++) {
        if (newWeights[i] == 0)
            continue;
        std::vector<std::pair<int, int>> myvector;
        for (std::pair<int, int> to: tower.back()->adjList[i]) {
            if (newWeights[to.first] != 0)
                myvector.emplace_back(newIndices[to.first], to.second);
        }
        newGraph.adjList.push_back(myvector);
        newGraph.getBack.push_back(insides[i]);
        newGraph.weights.push_back(newWeights[i]);
    }
    newGraph.size = newSize;
    tower.push_back(std::make_shared<shrunkGraph>(newGraph));
}

int FreeStylePartitioner::getNumberOfLeaves() {
    int ans = 0;
    for (auto l: tower.back()->adjList) {
        if (l.size() == 1)
            ans++;
    }
    return ans;
}

std::vector<std::pair<int, int>> FreeStylePartitioner::findBridges() {
    std::vector<bool> visited(tower.back()->size, false);
    std::vector<int> discover(tower.back()->size);
    std::vector<int> low(tower.back()->size);
    std::vector<int> parent(tower.back()->size, -1);
    std::vector<std::pair<int, int>> ans;
    dfsForBridges(0, 0, visited, discover, low, parent, ans);
    return ans;
}

void FreeStylePartitioner::dfsForBridges(int time, int u, std::vector<bool> &visited, std::vector<int> &discover,
                                         std::vector<int> &low, std::vector<int> &parent,
                                         std::vector<std::pair<int, int>> &bridges) {
    visited[u] = true;
    discover[u] = time;
    low[u] = time;
    for (auto to: tower.back()->adjList[u]) {
        if (!visited[to.first]) {
            parent[to.first] = u;
            dfsForBridges(time + 1, to.first, visited, discover, low, parent, bridges);
            low[u] = std::min(low[u], low[to.first]);
            if (low[to.first] > discover[u])
                bridges.emplace_back(u, to.first);
        } else {
            if (to.first != parent[u])
                low[u] = std::min(low[u], discover[to.first]);
        }
    }
}

void FreeStylePartitioner::dfsForCutVertices(int time, int u, std::vector<bool> &visited, std::vector<int> &discover,
                                             std::vector<int> &low, int parent,
                                             std::vector<int> &cutVertices) {
    int children = 0;
    visited[u] = true;
    discover[u] = time;
    low[u] = time;
    for (auto to: tower.back()->adjList[u]) {
        if (!visited[to.first]) {
            children++;
            dfsForCutVertices(time + 1, to.first, visited, discover, low, u, cutVertices);
            low[u] = std::min(low[u], low[to.first]);
            if (parent != -1 && low[to.first] >= discover[u])
                cutVertices.push_back(u);
        } else {
            if (to.first != parent)
                low[u] = std::min(low[u], discover[to.first]);
        }
    }
    if ((parent == -1) && (children > 1))
        cutVertices.push_back(u);
}

std::vector<int> FreeStylePartitioner::findCutVertices() {
    std::vector<bool> visited(tower.back()->size, false);
    std::vector<int> discover(tower.back()->size);
    std::vector<int> lowest(tower.back()->size);
    std::vector<int> parents(tower.back()->size);
    std::vector<int> ans;
    std::stack<int> dfsStack;
    std::vector<int> dfsWay;
    dfsStack.push(0);
    std::unordered_set<int> children0;
    for (auto to : tower.back()->adjList[0])
        children0.insert(to.first);
    discover[0] = 0;
    lowest[0] = 0;
    visited[0] = true;
    parents[0] = -1;
    int time = 0;
    while(!dfsStack.empty()) {
        int cur = dfsStack.top();
        dfsStack.pop();
        visited[cur] = true;
        dfsWay.push_back(cur);
        discover[cur] = time;
        lowest[cur] = time;
        time++;
        for (auto to: tower.back()->adjList[cur]) {
            if (!visited[to.first]) {
                dfsStack.push(to.first);
                if (cur) {
                    children0.erase(to.first);
                }
            }
        }
    }
    for (int i = tower.back()->size-1; i > 0; i--){
        for (auto to: tower.back()->adjList[dfsWay[i]]) {
            if (discover[to.first] != discover[dfsWay[i]] - 1)
                lowest[dfsWay[i]] = std::min(lowest[dfsWay[i]], lowest[to.first]);
        }
        if ((discover[dfsWay[i]] == lowest[dfsWay[i]]) && (tower.back()->adjList[dfsWay[i]]).size() > 1)
            ans.push_back(dfsWay[i]);
    }
    if (children0.size() > 1)
        ans.push_back(0);
    return ans;
}
