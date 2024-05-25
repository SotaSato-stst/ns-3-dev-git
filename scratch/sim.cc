/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    // アプリケーションにログをはかすことができる
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // データリンク層(L2)開始

    NS_LOG_UNCOND("Creating Topology");
    // NodeContainer = トポロジー生成
    NodeContainer nodes;
    nodes.Create(2);

    // PointToPointHelper = リンク属性決定
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // ネットワークデバイスをノードに装着
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // データリンク(L2)層終了

    // ネットワーク層(L3)開始

    // プロトコルスタックをデバイスに乗せる
    InternetStackHelper stack;
    stack.Install(nodes);

    // ネトワークアドレス決め
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer(9);

    // アプリケーションをノードに装着
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

    Simulator::Run();
    Simulator::Destroy();
}

// void
// generateTopology()
// {
//     std::string csvfilename = "data.csv";
//     std::vector<std::vector<int>> data = readCSV(csvfilename);
// }

// std::vector<std::vector<int>>
// readCSV(const std::string& filename)
// {
//     std::vector<std::vector<int>> data;
//     std::ifstream file(filename);
//     std::string line;

//     if (!file.is_open())
//     {
//         std::cerr << "Unable to open file " << filename << std::endl;
//         return data;
//     }

//     while (getline(file, line))
//     {
//         std::stringstream lineStream(line);
//         std::string cell;
//         std::vector<int> row;

//         while (getline(lineStream, cell, ','))
//         {
//             row.push_back(std::stoi(cell));
//         }

//         data.push_back(row);
//     }

//     file.close();
//     return data;
// }
