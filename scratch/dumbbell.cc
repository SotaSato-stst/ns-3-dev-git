#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

using namespace ns3;
#define STOP_SIM 20.0


NS_LOG_COMPONENT_DEFINE("DumbbellTopologyFTPExample");

int main(int argc, char *argv[]) {
    // LogComponentEnable("TcpSocketBase", LOG_LEVEL_INFO);

    CommandLine cmd;
    cmd.Parse(argc, argv);

// -----------------node の初期化-------------------
    NodeContainer leftNodes;
    leftNodes.Create(2);  // 左側のホストノード

    NodeContainer rightNodes;
    rightNodes.Create(2);  // 右側のホストノード

    NodeContainer routers;
    routers.Create(2);  // 中央のルータノード

// -----------------IPの設定-------------------
    // 左側のホストと左ルータを接続
    PointToPointHelper hostRouterP2P;
    hostRouterP2P.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    hostRouterP2P.SetChannelAttribute("Delay", StringValue("1ms"));

    NetDeviceContainer leftDevices1, leftDevices2;
    leftDevices1 = hostRouterP2P.Install(leftNodes.Get(0), routers.Get(0));
    leftDevices2 = hostRouterP2P.Install(leftNodes.Get(1), routers.Get(0));

    // 右側のホストと右ルータを接続
    NetDeviceContainer rightDevices1, rightDevices2;
    rightDevices1 = hostRouterP2P.Install(rightNodes.Get(0), routers.Get(1));
    rightDevices2 = hostRouterP2P.Install(rightNodes.Get(1), routers.Get(1));

    // ルータ間を接続
    PointToPointHelper routersP2P;
    routersP2P.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    routersP2P.SetChannelAttribute("Delay", StringValue("1ms"));

    NetDeviceContainer routerDevices;
    routerDevices = routersP2P.Install(routers.Get(0), routers.Get(1));

    AsciiTraceHelper ascii;
    hostRouterP2P.EnableAsciiAll(ascii.CreateFileStream("host.tr"));
    routersP2P.EnableAsciiAll(ascii.CreateFileStream("router.tr"));
    // パケットロスを設定
    // Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    // em->SetAttribute("ErrorRate", DoubleValue(0.01));  // パケットロス率 1%
    // routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
// 送信パケットのコールバック
    // Ptr<PacketLossCounter> packetLossCounter = CreateObject<PacketLossCounter> ();

    // routerDevices.Get (0)->TraceConnectWithoutContext ("PhyTxDrop", MakeCallback (&PacketLossCounter::IncLost, packetLossCounter));
    // routerDevices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&PacketLossCounter::IncLost, packetLossCounter));
    // インターネットプロトコルのインストール
    InternetStackHelper stack;
    stack.Install(leftNodes);
    stack.Install(rightNodes);
    stack.Install(routers);

    // IPアドレスの割り当て
    Ipv4AddressHelper address;

//-----------------IPアドレスの振り分け--------------------------------

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer leftInterfaces1 = address.Assign(leftDevices1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer leftInterfaces2 = address.Assign(leftDevices2);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer rightInterfaces1 = address.Assign(rightDevices1);

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer rightInterfaces2 = address.Assign(rightDevices2);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer routerInterfaces = address.Assign(routerDevices);


// ------------Applicationの設定-------------------------------
    // FTPアプリケーションの設定
    uint16_t port = 21;  // FTPのポート番号

    // パケットシンク（サーバ側）
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
                                      InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps1 = packetSinkHelper.Install(rightNodes.Get(0));
    ApplicationContainer sinkApps2 = packetSinkHelper.Install(rightNodes.Get(1));
    sinkApps1.Start(Seconds(1.0));
    sinkApps1.Stop(Seconds(STOP_SIM));
    sinkApps2.Start(Seconds(1.0));
    sinkApps2.Stop(Seconds(STOP_SIM));

    // バルクセンド（クライアント側）
    BulkSendHelper bulkSendHelper1("ns3::TcpSocketFactory",
                                  InetSocketAddress(rightInterfaces1.GetAddress(0), port));
    bulkSendHelper1.SetAttribute("MaxBytes", UintegerValue(5 * 1024 * 1024));  // 無制限
    ApplicationContainer clientApps1 = bulkSendHelper1.Install(leftNodes.Get(0));
    clientApps1.Start(Seconds(1.0));
    clientApps1.Stop(Seconds(STOP_SIM));

    BulkSendHelper bulkSendHelper2("ns3::TcpSocketFactory",
                                  InetSocketAddress(rightInterfaces2.GetAddress(0), port));
    bulkSendHelper2.SetAttribute("MaxBytes", UintegerValue(5 * 1024 * 1024));  // 無制限
    ApplicationContainer clientApps2 = bulkSendHelper2.Install(leftNodes.Get(1));
    clientApps2.Start(Seconds(1.0));
    clientApps2.Stop(Seconds(STOP_SIM));

//  -------------------flow monitorの設定-----------------
    FlowMonitorHelper flowmon;
    flowmon.SetMonitorAttribute("JitterBinWidth", ns3::DoubleValue(0.001));
    flowmon.SetMonitorAttribute("DelayBinWidth", ns3::DoubleValue(0.001));
    flowmon.SetMonitorAttribute("PacketSizeBinWidth", ns3::DoubleValue(0.001));
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    // パケットロスの結果を出力
    // パケットロスカウンタの設定
    // std::cout << "Packet Lost: " << packetLossCounter->GetLost () << std::endl;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(STOP_SIM));
    Simulator::Run();
    Simulator::Destroy();

//  ---------------------データ処理----------------------------
//  ************flow monitor****************
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    monitor->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
        i != stats.end();
        ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::cout << "Source: " << t.sourceAddress << " bps\n";
        uint64_t troughPut = i->second.rxBytes * 8.0 /
                                 (i->second.timeLastRxPacket.GetSeconds() -
                                  i->second.timeFirstTxPacket.GetSeconds()) /
                                 1024;
        std::cout << "TroughPut: " << troughPut << " Kbps\n";
        std::cout << "LostPacket: " << i->second.lostPackets << " bps\n";
    }
    return 0;
}
