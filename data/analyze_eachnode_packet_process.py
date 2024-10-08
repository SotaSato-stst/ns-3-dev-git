from collections import defaultdict
import re

def execute(filename, NodeNum=100):
    """node_counts: [{nodeIndex: [enqueCount, dequeueCount, lossCount, receiveCount]}]"""

    node_counts = {key: [0, 0, 0, 0] for key in range(NodeNum)}
    with open(filename, 'r') as file:
        for line in file:
            nodeIndexStr = re.search(r'/NodeList/(\d+)/DeviceList/', line).group(1)
            nodeIndex = int(nodeIndexStr)

            if line.startswith('+'):
                node_counts[nodeIndex][0] += 1
                node_counts[nodeIndex][2] += 1

            if line.startswith('-'):
                node_counts[nodeIndex][1] += 1
                node_counts[nodeIndex][2] -= 1

            if line.startswith('d'):
                print("detect drop packet")

            if line.startswith('r'):
                node_counts[nodeIndex][3] += 1
    return node_counts


# if __name__ == "__main__":
#     execute('./ascii/20240714180313_alpha=0_sourceSinkNum5-0.tr')
