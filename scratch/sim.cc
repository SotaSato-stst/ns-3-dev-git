#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

#define NET_MASK "255.255.255.0"
#define FIRST_NO "0.0.0.1"
#define SOURCE_TOPOLOGY_FILE_NAME "./data/adjacency_matrix.csv"
#define SOURCE_SENDER_SINKER_FILE_NAME "./data/leaf_pairs.csv"

std::vector<std::vector<int>> readMatrixFromCSV(const std::string& filename);
void ConfigureCommandLine(int argc, char* argv[]);
void SetupLogging();
NodeContainer CreateNodes(int nodeNum);
void SetupIPLayer(NodeContainer& nodes,
                  std::vector<std::vector<int>> topologyAsMatrix,
                  std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap);
void InstallApplications(NodeContainer& nodes,
                         std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
                         std::vector<std::vector<int>> senderSinkerAsMatrix);

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    std::string topologyFileName = SOURCE_TOPOLOGY_FILE_NAME;
    std::string senderSinkerFileName = SOURCE_SENDER_SINKER_FILE_NAME;
    std::vector<std::vector<int>> topologyAsMatrix = readMatrixFromCSV(topologyFileName);
    std::vector<std::vector<int>> senderSinkerAsMatrix = readMatrixFromCSV(senderSinkerFileName);
    std::unordered_map<std::int16_t, Ipv4Address> nodeAddressHashMap;

    int nodeNum = topologyAsMatrix.size();

    ConfigureCommandLine(argc, argv);
    SetupLogging();

    NodeContainer nodes = CreateNodes(nodeNum);
    SetupIPLayer(nodes, topologyAsMatrix, nodeAddressHashMap);
    InstallApplications(nodes, nodeAddressHashMap, senderSinkerAsMatrix);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

void
ConfigureCommandLine(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);
}

void
SetupLogging()
{
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
}

NodeContainer
CreateNodes(int nodeNum)
{
    NS_LOG_UNCOND("Creating Topology");
    NodeContainer nodes;
    nodes.Create(nodeNum);
    return nodes;
}

// 隣接行列(topologyAsMatrix)を元にリンクを貼り、それぞれのノードにIPを割り振る。ノードのアドレスはnodeAddressHashMapに格納される。(ノードのindexからaddressを取り出せる)
void
SetupIPLayer(NodeContainer& nodes,
             std::vector<std::vector<int>> topologyAsMatrix,
             std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap)
{
    NS_LOG_UNCOND("Setup IP");
    // Linkの設定
    std::vector<NetDeviceContainer> devices;
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // IPの設定
    std::vector<Ipv4InterfaceContainer> interfaces;
    InternetStackHelper stack;
    stack.Install(nodes);

    // リンクとIPをinstallする
    for (size_t i = 0; i < topologyAsMatrix.size(); ++i)
    {
        for (size_t j = 0; j < topologyAsMatrix[i].size(); ++j)
        {
            if (topologyAsMatrix[i][j] == 1)
            {
                Ipv4AddressHelper address;
                // ネットワーク部をそれぞれのデバイスごとに振り分ける。
                std::string ipAddress = "10.1." + std::to_string(i + 1) + ".0";
                const char* ipAddressStr = ipAddress.c_str();
                Ipv4Address base(ipAddressStr);
                address.SetBase(base, "255.255.255.0");

                NetDeviceContainer devices = pointToPoint.Install(nodes.Get(i), nodes.Get(j));
                std::cout << "topologyAsMatrix[" << i << "][" << j
                          << "] = " << topologyAsMatrix[i][j] << std::endl;

                Ipv4InterfaceContainer interface = address.Assign(devices);
                std::cout << "node[" << i << "]address:" << interface.GetAddress(0, 0) << std::endl;
                std::cout << "node[" << j << "]address:" << interface.GetAddress(1, 0) << std::endl;
                nodeAddressHashMap[i] = interface.GetAddress(0, 0);
                nodeAddressHashMap[j] = interface.GetAddress(1, 0);
            }
        }
    }
}

void
InstallApplications(NodeContainer& nodes,
                    std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
                    std::vector<std::vector<int>> senderSinkerAsMatrix)
{
    NS_LOG_UNCOND("Install Application");
    for (int i = 0; i < senderSinkerAsMatrix.size(); i++)
    {
        int sourceNodeIndex = senderSinkerAsMatrix[i][0];
        int sinkNodeIndex = senderSinkerAsMatrix[i][1];

        // 同じノードに複数のserverをinstallする可能性があり、port番号が被らないように、loopのindexをport番号とする。
        // 49152-65535 は動的に振り分けられるものとなっているので、こちらの範囲のportを用いる
        int portNum = i + 49152;

        // sink serverの準備
        UdpEchoServerHelper echoServer(portNum);
        ApplicationContainer serverApps = echoServer.Install(nodes.Get(sinkNodeIndex));
        serverApps.Start(Seconds(1.0));
        serverApps.Stop(Seconds(10.0));

        // source serverの準備。
        // 宛先などをセットアップ
        UdpEchoClientHelper echoClient(nodeAddressHashMap[sinkNodeIndex], portNum);
        echoClient.SetAttribute("MaxPackets", UintegerValue(2));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        // install
        ApplicationContainer clientApps = echoClient.Install(nodes.Get(sourceNodeIndex));
        clientApps.Start(Seconds(2.0));
        clientApps.Stop(Seconds(10.0));

        std::cout << "node[" << sourceNodeIndex << "] address"
                  << nodeAddressHashMap[sourceNodeIndex] << ", send to send node[" << sinkNodeIndex
                  << "], address " << nodeAddressHashMap[sinkNodeIndex] << std::endl;
    }
}

std::vector<std::vector<int>>
readMatrixFromCSV(const std::string& filename)
{
    std::vector<std::vector<int>> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Unable to open file " << filename << std::endl;
        return data;
    }

    while (getline(file, line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<int> row;

        while (getline(lineStream, cell, ','))
        {
            row.push_back(std::stoi(cell));
        }

        data.push_back(row);
    }

    file.close();
    return data;
}
