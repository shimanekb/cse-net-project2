#include <iostream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Topology CSE Networks Simulation");

int main(int argc, char *argv[])
{
  // Nodes
  NodeContainer na;
  na.Create(1);
  NodeContainer nb;
  nb.Create(1);
  NodeContainer nc;
  nc.Create(1);
  NodeContainer nd;
  nd.Create(1);
  NodeContainer ne;
  ne.Create(1);

  // Connect Nodes
  NodeContainer naNe;
  naNe.Add(na.Get(0));
  naNe.Add(ne.Get(0));
  NodeContainer nbNd;
  nbNd.Add(nb.Get(0));
  nbNd.Add(nd.Get(0));
  NodeContainer ncNd;
  ncNd.Add(nc.Get(0));
  ncNd.Add(nd.Get(0));
  NodeContainer ndNe;
  ndNe.Add(nd.Get(0));
  ndNe.Add(ne.Get(0));

  // Configure Connections
  PointToPointHelper pointToPointNaNe;
  pointToPointNaNe.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNaNe.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNaNe;
  p2pDevicesNaNe = pointToPointNaNe.Install(naNe);

  PointToPointHelper pointToPointNbNd;
  pointToPointNbNd.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNbNd.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNbNd;
  p2pDevicesNbNd = pointToPointNbNd.Install(nbNd);

  PointToPointHelper pointToPointNcNd;
  pointToPointNcNd.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNcNd.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNcNd;
  p2pDevicesNcNd = pointToPointNcNd.Install(ncNd);

  PointToPointHelper pointToPointNdNe;
  pointToPointNdNe.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPointNdNe.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevicesNdNe;
  p2pDevicesNdNe = pointToPointNdNe.Install(ndNe);

  std::cout << "Hello WorldDDDD";
  return 0;  
}
