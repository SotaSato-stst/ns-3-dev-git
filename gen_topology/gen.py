import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import csv
from . import calc
from . import preferentialAttachment
from . import generatesStatnet
import os
sourceSinkNum = 5
leaf_pairs_csv_file_path = './data/topology_data/leaf_pairs/'
adjacency_matrix_csv_file_path = './data/topology_data/adjacency_matrix/'
clustering_coefficient_csv_file_path = './data/topology_data/clustering_coefficient/'

# Parameters
# num_nodes = 100  # Total number of nodes in the graph
# M =  1           # Number of edges to attach from a new node to existing nodes
# alpha = 0  # Fraction of links that are updated
def execute(num_nodes = 100, M = 1, alpha = 0, sourceSinkNum = 5, fileName="sample.csv", isVisualizeGraph=False, type="WG"):
    # Generate the preferential attachment graph
    if type == "WG":
        G, adj_matrix = preferentialAttachment.generateGraph(num_nodes, M, alpha)
    elif type == "SN":
        G, adj_matrix = generatesStatnet.grow_statnet(num_nodes, num_nodes)

    # Find 3 pairs of leaf nodes
    leaf_pairs = calc.find_leaf_pairs(G, minimumDegree=M, num_pairs=sourceSinkNum)
    print(len(leaf_pairs))
    # Save the leaf pairs to a CSV file
    leaf_pairs_df = pd.DataFrame(leaf_pairs)
    leaf_pairs_df.to_csv(leaf_pairs_csv_file_path + fileName, index=False, header=False)

    # Print the path to the CSV file
    print(f"Leaf pairs saved to {leaf_pairs_csv_file_path + fileName}")

    # Save the adjacency matrix to a CSV file
    df = pd.DataFrame(adj_matrix)
    df.to_csv(adjacency_matrix_csv_file_path + fileName, index=False, header=False)

    # Print the path to the CSV file
    print(f"Adjacency matrix saved to {adjacency_matrix_csv_file_path + fileName}")

    # Save the clustering index to the CSV file
    network_clustering_coefficient = calc.calculate_clustering_coefficient(adj_matrix)
    # CSVファイルに書き込む
    with open(clustering_coefficient_csv_file_path + fileName, 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([num_nodes, alpha, network_clustering_coefficient])     # データ行

    # Optionally, visualize the graph
    if isVisualizeGraph:
        G = nx.from_numpy_array(adj_matrix)
        plt.figure(figsize=(7, 10))
        nx.draw(G, node_size=20, node_color='blue',
                edge_color='gray', with_labels=True)

        filename = f'topology_{alpha}.png'
        filepath = os.path.join("./data", filename)
        plt.savefig(filepath)
        plt.close()
    return adj_matrix

# Parameters
num_nodes = 100  # Total number of nodes in the graph
M =  1           # Number of edges to attach from a new node to existing nodes
alpha = 0  # Fraction of links that are updated




