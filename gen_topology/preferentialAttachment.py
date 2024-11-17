import numpy as np
import networkx as nx

def generateGraph(num_nodes, M, alpha):
    G = nx.complete_graph(M + 1)

    for new_node in range(M + 1, num_nodes):
        NodeNum = len(G.nodes())
        degrees = np.array([G.degree(n) for n in G.nodes()])

        # 確率式: https://arxiv.org/pdf/cond-mat/0011442
        probs = (degrees + (alpha * M)) / ((1 + alpha) * M * NodeNum)
        # 正規化
        total_probs = np.sum(probs)
        # print(total_probs)
        probs = probs / total_probs

        existing_nodes = np.random.choice(
            G.nodes(), size=M, replace=False, p=probs)

        for existing_node in existing_nodes:
            G.add_edge(new_node, existing_node)

    adj_matrix = nx.to_numpy_array(G)

    # シミュレーションように上三角部分の1を対象部分に移し、下三角行列にする
    for i in range(adj_matrix.shape[0]):
        for j in range(i + 1, adj_matrix.shape[1]):
            if adj_matrix[i, j] == 1:
                adj_matrix[i, j] = 0
                adj_matrix[j, i] = 1
    return G, adj_matrix
