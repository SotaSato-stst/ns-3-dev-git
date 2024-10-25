#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

using namespace ns3;

#define NET_MASK "255.255.255.0"
#define FIRST_NO "0.0.0.1"
#define SOURCE_TOPOLOGY_FILE_PATH "./data/topology_data/adjacency_matrix/"
#define SOURCE_SENDER_SINKER_FILE_PATH "./data/topology_data/leaf_pairs/"
#define THROUGHPUT_FINE_PATH "./data/result/average_throughput/"
#define ASCII_FINE_PATH "./data/raw/ascii/"
#define SIM_START 00.10
#define SIM_STOP 10.10
#define CSV_SUFFIX ".csv"
#define ASCII_SUFFIX ".tr"

std::vector<std::vector<int>> readMatrixFromCSV(const std::string& filename);
void ConfigureCommandLine(int argc, char* argv[]);
void SetupLogging();
NodeContainer CreateNodes(int nodeNum);
void SetupIPLayer(NodeContainer& nodes,
                  std::vector<std::vector<int>> topologyAsMatrix,
                  std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
                  std::string asciiFIleName,
                  std::string bandWidth);

void InstallApplications(NodeContainer& nodes,
                         std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
                         std::vector<std::vector<int>> senderSinkerAsMatrix,
                         std::set<Ipv4Address>& sourceAddressSet,
                         std::set<Ipv4Address>& sinkAddressSet);
void installUdpEchoApplication(NodeContainer& nodes,
                               int sourceNodeIndex,
                               int sinkNodeIndex,
                               Ipv4Address sinkNodeAddress,
                               int sinkPort);
void installFTPApplication(NodeContainer& nodes,
                           int sourceNodeIndex,
                           int sinkNodeIndex,
                           Ipv4Address sinkNodeAddress,
                           int sinkPort);
int OutputFlowMonitor(Ptr<ns3::FlowMonitor> monitor,
                       Ptr<Ipv4FlowClassifier> flowmon,
                       std::set<Ipv4Address>& sourceAddressSet,
                       std::set<Ipv4Address>& sinkAddressSet);

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    // Parse command line
    std::string fileName = argv[1];
    float alpha = std::atof(argv[2]);
    float sinkSourceNum = std::atoi(argv[3]);
    std::string bandWidth = argv[4];

    std::string topologyFileName = SOURCE_TOPOLOGY_FILE_PATH + fileName + CSV_SUFFIX;
    std::string senderSinkerFileName = SOURCE_SENDER_SINKER_FILE_PATH + fileName+ CSV_SUFFIX;
    std::string throughputFileName = THROUGHPUT_FINE_PATH + fileName + CSV_SUFFIX;
    std::string asciiFileName = ASCII_FINE_PATH + fileName + ASCII_SUFFIX;

    std::vector<std::vector<int>> topologyAsMatrix = readMatrixFromCSV(topologyFileName);
    std::vector<std::vector<int>> senderSinkerAsMatrix = readMatrixFromCSV(senderSinkerFileName);
    std::unordered_map<std::int16_t, Ipv4Address> nodeAddressHashMap;

    int nodeNum = topologyAsMatrix.size();

    std::set<Ipv4Address> sourceAddressSet;
    std::set<Ipv4Address> sinkAddressSet;

    ConfigureCommandLine(argc, argv);
    SetupLogging();

    NodeContainer nodes = CreateNodes(nodeNum);
    SetupIPLayer(nodes, topologyAsMatrix, nodeAddressHashMap, asciiFileName, bandWidth);
    InstallApplications(nodes,
                        nodeAddressHashMap,
                        senderSinkerAsMatrix,
                        sourceAddressSet,
                        sinkAddressSet);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Setup FlowMonitor
    FlowMonitorHelper flowmon;
    flowmon.SetMonitorAttribute("JitterBinWidth", ns3::DoubleValue(0.001));
    flowmon.SetMonitorAttribute("DelayBinWidth", ns3::DoubleValue(0.001));
    flowmon.SetMonitorAttribute("PacketSizeBinWidth", ns3::DoubleValue(0.001));
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    NS_LOG_UNCOND("Start Simulation");
    Simulator::Stop(Seconds(SIM_STOP));
    Simulator::Run();
    Simulator::Destroy();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());

    float averageThroughput = OutputFlowMonitor(monitor, classifier, sourceAddressSet, sinkAddressSet);

    std::vector<float> numbers = {alpha,sinkSourceNum,  averageThroughput};
    std::ofstream file;
    file.open(throughputFileName, std::ios::out | std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << throughputFileName << std::endl;
        return 0;
    }

    // 数値をCSV形式で書き込む
    for (size_t i = 0; i < numbers.size(); ++i) {
        file << numbers[i];
        if (i < numbers.size() - 1) {
            file << ",";
        }
    }
    file << std::endl;

    file.close();

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
    // LogComponentEnable("TcpSocketBase", LOG_LEVEL_INFO);
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
             std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
             std::string asciiFileName,
             std::string bandWidth)
{
    NS_LOG_UNCOND("Setup IP");
    // Linkの設定
    std::vector<NetDeviceContainer> devices;
    PointToPointHelper pointToPoint;
    pointToPoint.SetQueue("ns3::DropTailQueue",
             "MaxSize", StringValue("10p"));
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(bandWidth));
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
                // std::cout << "topologyAsMatrix[" << i << "][" << j
                //           << "] = " << topologyAsMatrix[i][j] << std::endl;

                Ipv4InterfaceContainer interface = address.Assign(devices);
                // std::cout << "node[" << i << "]address:" << interface.GetAddress(0, 0) << std::endl;
                // std::cout << "node[" << j << "]address:" << interface.GetAddress(1, 0) << std::endl;
                nodeAddressHashMap[i] = interface.GetAddress(0, 0);
                nodeAddressHashMap[j] = interface.GetAddress(1, 0);
            }
        }
    }

    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream(asciiFileName));
}

void
InstallApplications(NodeContainer& nodes,
                    std::unordered_map<std::int16_t, Ipv4Address>& nodeAddressHashMap,
                    std::vector<std::vector<int>> senderSinkerAsMatrix,
                    std::set<Ipv4Address>& sourceAddressSet,
                    std::set<Ipv4Address>& sinkAddressSet)
{
    NS_LOG_UNCOND("Install Application");

    for (int i = 0; i < senderSinkerAsMatrix.size(); i++)
    {
        int sourceNodeIndex = senderSinkerAsMatrix[i][0];
        int sinkNodeIndex = senderSinkerAsMatrix[i][1];
        Ipv4Address sinkNodeAddress = nodeAddressHashMap[sinkNodeIndex];
        Ipv4Address sourceNodeAddress = nodeAddressHashMap[sourceNodeIndex];

        // 同じノードに複数のserverをinstallする可能性があり、port番号が被らないように、loopのindexをport番号とする。
        // 49152-65535 は動的に振り分けられるものとなっているので、こちらの範囲のportを用いる
        int sinkPort = i + 49152;

        installFTPApplication(nodes, sourceNodeIndex, sinkNodeIndex, sinkNodeAddress, sinkPort);

        // std::cout << "node[" << sourceNodeIndex << "] address" << sourceNodeAddress
        //           << ", send to send node[" << sinkNodeIndex << "], address " << sinkNodeAddress
        //           << std::endl;

        sinkAddressSet.insert(sinkNodeAddress);
        sourceAddressSet.insert(sourceNodeAddress);
    }
}

void
installUdpEchoApplication(NodeContainer& nodes,
                          int sourceNodeIndex,
                          int sinkNodeIndex,
                          Ipv4Address sinkNodeAddress,
                          int sinkPort)
{
    // sink serverの準備
    UdpEchoServerHelper echoServer(sinkPort);
    ApplicationContainer sinkApp = echoServer.Install(nodes.Get(sinkNodeIndex));
    sinkApp.Start(Seconds(SIM_START + 0.10));
    sinkApp.Stop(Seconds(SIM_STOP - 0.10));

    // source serverの準備。
    // 宛先などをセットアップ
    UdpEchoClientHelper echoClient(sinkNodeAddress, sinkPort);
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer sourceApp = echoClient.Install(nodes.Get(sourceNodeIndex));
    sourceApp.Start(Seconds(SIM_START + 0.10));
    sourceApp.Stop(Seconds(SIM_STOP - 0.10));
}

void
installFTPApplication(NodeContainer& nodes,
                      int sourceNodeIndex,
                      int sinkNodeIndex,
                      Ipv4Address sinkNodeAddress,
                      int sinkPort)
{
    // Source側の設定
    AddressValue remoteAddress(InetSocketAddress(sinkNodeAddress, sinkPort));
    BulkSendHelper ftp("ns3::TcpSocketFactory", Address());
    ftp.SetAttribute("Remote", remoteAddress);
    ftp.SetAttribute("MaxBytes", UintegerValue(500 * 1024 * 1024));
    ApplicationContainer sourceApp = ftp.Install(nodes.Get(sourceNodeIndex));
    sourceApp.Start(Seconds(SIM_START + 0.10));
    sourceApp.Stop(Seconds(SIM_STOP - 0.10));

    // Sink側の設定
    Address sinkAddress(InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkAddress);
    ApplicationContainer sinkApp = sinkHelper.Install(nodes.Get(sinkNodeIndex));
    sinkApp.Start(Seconds(SIM_START + 0.10));
    sinkApp.Stop(Seconds(SIM_STOP - 0.10));
}

int
OutputFlowMonitor(Ptr<ns3::FlowMonitor> monitor,
                  Ptr<Ipv4FlowClassifier> classifier,
                  std::set<Ipv4Address>& sourceAddressSet,
                  std::set<Ipv4Address>& sinkAddressSet)
{
    monitor->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    // std::cout << "----------------------------------\n";

    std::vector<uint64_t> values;
    uint64_t sum = 0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);

        // client側 -> server側の通信のみcaptureする
        if (sourceAddressSet.find(t.sourceAddress) != sourceAddressSet.end() &&
            sinkAddressSet.find(t.destinationAddress) != sinkAddressSet.end())
        {
            // std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> "
            //           << t.destinationAddress << ")\n";
            uint64_t troughPut = i->second.rxBytes * 8.0 /
                                 (i->second.timeLastRxPacket.GetSeconds() -
                                  i->second.timeFirstTxPacket.GetSeconds()) /
                                 1024;
            // std::cout << "TroughPut: " << troughPut << " Kbps\n";
            // std::cout << "----------------------------------\n";
            values.push_back(troughPut);
            sum += troughPut;
        }
    }

    // 標準出力へoutput
    int average = sum / values.size();
    std::cout << "Average value: " << average << std::endl;

    return average;
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
