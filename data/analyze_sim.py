import networkx as nx
import matplotlib.pyplot as plt
import os
import numpy as np
# from . import analyze_eachnode_packet_process
import analyze_eachnode_packet_process


def execute(fileName, num_nodes):
    adjacent_matrix = np.genfromtxt(getAdjacencyMetrixPath(fileName), delimiter=',', dtype=float)
    plotNetworkWithPacketProcessAmount(fileName, adjacent_matrix, num_nodes)
    deleteFile(fileName)

def plotNetworkWithPacketProcessAmount(fileName, adjacent_matrix, num_nodes):
    node_counts = analyze_eachnode_packet_process.execute(getAsciiFIlePath(fileName), num_nodes)
    G = nx.from_numpy_array(adjacent_matrix)
    betweenness_centrality = nx.betweenness_centrality(G).values()
    os.makedirs(f"./output/networkplot/{fileName}", exist_ok=True)

    enqueue_counts = [node_counts[key][0] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, enqueue_counts, "enqueue_counts")
    dequeue_counts = [node_counts[key][1] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, dequeue_counts, "dequeue_counts")
    loss_counts = [node_counts[key][2] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, loss_counts, "loss_counts")
    receive_counts = [node_counts[key][3] for key in node_counts]
    plotNetworkWithWeigh(fileName, G, receive_counts, "receive_counts")
    plotNetworkWithWeigh(fileName, G, betweenness_centrality, "betweenness_centrality")

def plotNetworkWithWeigh(day, G, node_weights, node_weights_type):
    cmap = plt.get_cmap('cool')  # 例として'cool'カラーマップを使用

    # 各ノードの色を大きさに応じて設定する
    max_weight =max(node_weights)
    if max_weight == 0:
        max_weight = 1
    node_colors = [cmap(size / max_weight) for size in node_weights]
    plt.figure(figsize=(7, 10))
    np.random.seed(10)
    nx.draw(G, edge_color='gray', node_color=node_colors, node_size=40, with_labels=False)

    filename = f'{node_weights_type}.png'
    filepath = os.path.join(f"./output/networkplot/{day}", filename)
    plt.savefig(filepath)
    plt.close()

def deleteFile(fileName):
    os.remove(getAsciiFIlePath(fileName))


def getAsciiFIlePath(fileName):
    return './ascii/' + fileName + '.tr'

def getAdjacencyMetrixPath(fileName):
    return './adjacency_matrix/' + fileName + '.csv'

if __name__ == "__main__":
    execute('20240714180313_alpha=0_sourceSinkNum5-0', 100)
