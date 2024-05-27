#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

#define NET_MASK "255.255.255.0"
#define FIRST_NO "0.0.0.1"
#define SOURCE_TOPOLOGY_FILE_NAME "./data/sample.csv"

void generateTopology();
std::vector<std::vector<int>> readMatrixFromCSV(const std::string& filename);
void ConfigureCommandLine(int argc, char* argv[]);
void SetupLogging();
NodeContainer CreateTopology();
NetDeviceContainer SetupDataLinkLayer(NodeContainer& nodes);
Ipv4InterfaceContainer SetupNetworkLayer(NodeContainer& nodes, NetDeviceContainer& devices);
void InstallApplications(NodeContainer& nodes, Ipv4InterfaceContainer& interfaces);

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    ConfigureCommandLine(argc, argv);
    SetupLogging();

    NodeContainer nodes = CreateTopology();
    NetDeviceContainer devices = SetupDataLinkLayer(nodes);
    Ipv4InterfaceContainer interfaces = SetupNetworkLayer(nodes, devices);
    InstallApplications(nodes, interfaces);

    Simulator::Run();
    Simulator::Destroy();
    generateTopology();

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
CreateTopology()
{
    NS_LOG_UNCOND("Creating Topology");
    NodeContainer nodes;
    nodes.Create(2);
    return nodes;
}

NetDeviceContainer
SetupDataLinkLayer(NodeContainer& nodes)
{
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices = pointToPoint.Install(nodes);
    return devices;
}

Ipv4InterfaceContainer
SetupNetworkLayer(NodeContainer& nodes, NetDeviceContainer& devices)
{
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    return interfaces;
}

void
InstallApplications(NodeContainer& nodes, Ipv4InterfaceContainer& interfaces)
{
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
}

void
generateTopology()
{
    std::string csvfilename = SOURCE_TOPOLOGY_FILE_NAME;
    std::vector<std::vector<int>> data = readMatrixFromCSV(csvfilename);

    for (size_t i = 0; i < data.size(); ++i)
    {
        for (size_t j = 0; j < data[i].size(); ++j)
        {
            if (data[i][j] == 1)
            {
                std::cout << "data[" << i << "][" << j << "] = " << data[i][j] << std::endl;
            }
        }
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
