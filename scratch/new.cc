#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PacketLossCountExample");

uint32_t packetsReceived = 0;
uint32_t packetsSent = 0;

void ReceivePacket(Ptr<Socket> socket) {
    while (socket->Recv()) {
        packetsReceived++;
    }
}

void SendPacket(Ptr<Socket> socket, uint32_t packetSize) {
    Ptr<Packet> packet = Create<Packet>(packetSize);
    socket->Send(packet);
    packetsSent++;
}

int main(int argc, char *argv[]) {
    // Set up logging
    LogComponentEnable("PacketLossCountExample", LOG_LEVEL_INFO);

    // Create two nodes
    NodeContainer nodes;
    nodes.Create(2);

    // Set up point-to-point link between nodes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install the devices and channels on the nodes
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Install the internet stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Create a socket to send packets from node 0
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> sendSocket = Socket::CreateSocket(nodes.Get(0), tid);
    InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
    sendSocket->Connect(remote);

    // Create a socket to receive packets at node 1
    Ptr<Socket> recvSocket = Socket::CreateSocket(nodes.Get(1), tid);
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 8080);
    recvSocket->Bind(local);
    recvSocket->SetRecvCallback(MakeCallback(&ReceivePacket));

    // Schedule sending of packets from node 0
    uint32_t packetSize = 1024; // Packet size in bytes
    uint32_t numPackets = 100;  // Number of packets to send
    Time interPacketInterval = MilliSeconds(50); // Interval between packets

    for (uint32_t i = 0; i < numPackets; ++i) {
        Simulator::Schedule(Seconds(i * interPacketInterval.GetSeconds()), &SendPacket, sendSocket, packetSize);
    }

    // Run the simulation
    Simulator::Stop(Seconds(numPackets * interPacketInterval.GetSeconds() + 1));
    Simulator::Run();

    // Calculate packet loss
    uint32_t packetLoss = packetsSent - packetsReceived;
    NS_LOG_INFO("Packets Sent: " << packetsSent);
    NS_LOG_INFO("Packets Received: " << packetsReceived);
    NS_LOG_INFO("Packet Loss: " << packetLoss);

    Simulator::Destroy();
    return 0;
}
