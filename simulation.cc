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
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pointToPoint.SetChannelAttribute("Delay", TimeValue(MilliSeconds(.5)));
  NetDeviceContainer p2pDevices;
  p2pDevices= pointToPoint.Install(naNe);

  std::cout << "Hello WorldDDDD";
  return 0;  
}
