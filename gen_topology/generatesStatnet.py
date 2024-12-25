import numpy as np
import networkx as nx
import matplotlib.pyplot as plt

def grow_statnet(num_nodes, num_links):
    M = 1
    alpha = 1
    G = nx.Graph()
    G.add_nodes_from(range(num_nodes))

    # Sequentially add links
    for node in range(num_nodes):
        degrees = np.array([G.degree(n) for n in G.nodes()])
        # Choose two distinct nodes with probabilities proportional to their degrees
        print("Degrees: ", degrees)
        probs = (degrees + (alpha * M)) / ((1 + alpha) * M * num_nodes)
        total_probs = np.sum(probs)
        probs = probs / total_probs

        existing_node = node

        while existing_node == node or G.has_edge(node, existing_node):
            existing_node = np.random.choice(G.nodes(), size=1, replace=False,p=probs)[0]

        G.add_edge(node, existing_node)

    adj_matrix = nx.to_numpy_array(G)
    # シミュレーションように上三角部分の1を対象部分に移し、下三角行列にする
    for i in range(adj_matrix.shape[0]):
        for j in range(i + 1, adj_matrix.shape[1]):
            if adj_matrix[i, j] == 1:
                adj_matrix[i, j] = 0
                adj_matrix[j, i] = 1
    return G, adj_matrix

# Parameters for the network
num_nodes = 200  # Number of nodes
num_links = 200  # Total number of links to add

# Generate the network
adj_matrix, graph = grow_statnet(num_nodes, num_links)

# Display the adjacency matrix
print("Adjacency Matrix:")
print(adj_matrix)

# Visualize the network
plt.figure(figsize=(10, 8))
nx.draw(graph, node_size=10, with_labels=False)
plt.title("Statnet with Fixed Nodes and Sequential Links")
plt.show()
