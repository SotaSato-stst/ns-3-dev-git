#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

#define NET_MASK "255.255.255.0"
#define FIRST_NO "0.0.0.1"
#define SOURCE_TOPOLOGY_FILE_NAME "./data/topology_sample.csv"
#define SOURCE_SENDER_SINKER_FILE_NAME "./data/sender_sinker_sample.csv"

std::vector<std::vector<int>> readMatrixFromCSV(const std::string& filename);
void ConfigureCommandLine(int argc, char* argv[]);
void SetupLogging();
NodeContainer CreateNodes(int nodeNum);
std::vector<NetDeviceContainer> SetupDataLinkLayer(NodeContainer& nodes,
                                                   std::vector<std::vector<int>> topologyAsMatrix);
std::vector<Ipv4InterfaceContainer> SetupNetworkLayer(NodeContainer& nodes,
                                                      std::vector<NetDeviceContainer>& devices);
void InstallApplications(NodeContainer& nodes,
                         std::vector<Ipv4InterfaceContainer>& interfaces,
                         std::vector<std::vector<int>> senderSinkerAsMatrix);

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    std::string topologyFileName = SOURCE_TOPOLOGY_FILE_NAME;
    std::string senderSinkerFileName = SOURCE_SENDER_SINKER_FILE_NAME;
    std::vector<std::vector<int>> topologyAsMatrix = readMatrixFromCSV(topologyFileName);
    std::vector<std::vector<int>> senderSinkerAsMatrix = readMatrixFromCSV(senderSinkerFileName);
    int nodeNum = topologyAsMatrix.size();

    ConfigureCommandLine(argc, argv);
    SetupLogging();

    NodeContainer nodes = CreateNodes(nodeNum);
    std::vector<ns3::NetDeviceContainer> devices = SetupDataLinkLayer(nodes, topologyAsMatrix);
    std::vector<ns3::Ipv4InterfaceContainer> interfaces = SetupNetworkLayer(nodes, devices);
    InstallApplications(nodes, interfaces, senderSinkerAsMatrix);

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

// 隣接行列(topologyAsMatrix)を元にリンクを貼る。リンクを貼ったノードのペアはdeviceとして返却される。
std::vector<NetDeviceContainer>
SetupDataLinkLayer(NodeContainer& nodes, std::vector<std::vector<int>> topologyAsMatrix)
{
    std::vector<NetDeviceContainer> devices;
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    for (size_t i = 0; i < topologyAsMatrix.size(); ++i)
    {
        for (size_t j = 0; j < topologyAsMatrix[i].size(); ++j)
        {
            if (topologyAsMatrix[i][j] == 1)
            {
                NetDeviceContainer device = pointToPoint.Install(nodes.Get(i), nodes.Get(j));
                std::cout << "topologyAsMatrix[" << i << "][" << j
                          << "] = " << topologyAsMatrix[i][j] << std::endl;
                devices.push_back(device);
            }
        }
    }
    return devices;
}

// デバイスそれぞれにIPを振り分ける。10.1.1.1からアドレス部分がインクリメントされる。IPがそれぞれ振られたnodeのペア(device)はinterfaceとして返却される。
std::vector<Ipv4InterfaceContainer>
SetupNetworkLayer(NodeContainer& nodes, std::vector<NetDeviceContainer>& devices)
{
    std::vector<Ipv4InterfaceContainer> interfaces;
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    for (int i = 0; i < devices.size(); i++)
    {
        Ipv4InterfaceContainer interface = address.Assign(devices[i]);
        std::cout << "address: " << interface.GetAddress(0, 0) << "," << interface.GetAddress(1, 0)
                  << std::endl;
        interfaces.push_back(interface);
    }
    return interfaces;
}

void
InstallApplications(NodeContainer& nodes,
                    std::vector<Ipv4InterfaceContainer>& interfaces,
                    std::vector<std::vector<int>> senderSinkerAsMatrix)
{
    UdpEchoServerHelper echoServer(20);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces[1].GetAddress(1), 20);
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
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
