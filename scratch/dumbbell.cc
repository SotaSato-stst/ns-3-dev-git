#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DumbbellTopologyFTPExample");

int main(int argc, char *argv[]) {
    CommandLine cmd;
    cmd.Parse(argc, argv);

    NodeContainer leftNodes;
    leftNodes.Create(2);  // 左側のホストノード

    NodeContainer rightNodes;
    rightNodes.Create(2);  // 右側のホストノード

    NodeContainer routers;
    routers.Create(2);  // 中央のルータノード

    // 左側のホストと左ルータを接続
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Gbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));

    NetDeviceContainer leftDevices1, leftDevices2;
    leftDevices1 = pointToPoint.Install(leftNodes.Get(0), routers.Get(0));
    leftDevices2 = pointToPoint.Install(leftNodes.Get(1), routers.Get(0));

    // 右側のホストと右ルータを接続
    NetDeviceContainer rightDevices1, rightDevices2;
    rightDevices1 = pointToPoint.Install(rightNodes.Get(0), routers.Get(1));
    rightDevices2 = pointToPoint.Install(rightNodes.Get(1), routers.Get(1));

    // ルータ間を接続
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer routerDevices;
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("sample.tr"));
    routerDevices = pointToPoint.Install(routers.Get(0), routers.Get(1));

    // パケットロスを設定
    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(0.01));  // パケットロス率 1%
    routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
// 送信パケットのコールバック
    Ptr<PacketLossCounter> packetLossCounter = CreateObject<PacketLossCounter> ();

    routerDevices.Get (0)->TraceConnectWithoutContext ("PhyTxDrop", MakeCallback (&PacketLossCounter::IncLost, packetLossCounter));
    routerDevices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&PacketLossCounter::IncLost, packetLossCounter));
    // インターネットプロトコルのインストール
    InternetStackHelper stack;
    stack.Install(leftNodes);
    stack.Install(rightNodes);
    stack.Install(routers);

    // IPアドレスの割り当て
    Ipv4AddressHelper address;

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

    // FTPアプリケーションの設定
    uint16_t port = 21;  // FTPのポート番号

    // パケットシンク（サーバ側）
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
                                      InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = packetSinkHelper.Install(rightNodes.Get(0));
    sinkApps.Start(Seconds(1.0));
    sinkApps.Stop(Seconds(10.0));

    // バルクセンド（クライアント側）
    BulkSendHelper bulkSendHelper("ns3::TcpSocketFactory",
                                  InetSocketAddress(rightInterfaces1.GetAddress(0), port));
    bulkSendHelper.SetAttribute("MaxBytes", UintegerValue(0));  // 無制限
    ApplicationContainer clientApps = bulkSendHelper.Install(leftNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // パケットロスの結果を出力
    // パケットロスカウンタの設定
    std::cout << "Packet Lost: " << packetLossCounter->GetLost () << std::endl;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
