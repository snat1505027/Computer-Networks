#include <iostream>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define INF_COST 1000000
#define BUFFER_SIZE 1024
using namespace std;

int sockfd;
int clkRate;
int bytes_received;
char buffer[BUFFER_SIZE];
int global_flag = 0;
struct NodeStatus
{
    string adj;
    int cost;
    int connection;
    int rclkRate;
};

struct Entry
{
    string nextHop;
    int cost;
    string dest;
};

struct Router
{
  public:
    int ID;
    string IPAddress;
    map<pair<string, string>, int> routingTable;
    set<string> ipAddress;
    set<string> adjacentNodes;
    vector<NodeStatus> currentNodes;
    vector<Entry> vectorTable;
    int DVR_array[100][100];

    Router(string ipaddress)
    {
        IPAddress = ipaddress;
        ID = stoi(ipaddress.substr(ipaddress.find(".", 8) + 1, ipaddress.size() - 1));
        memset(DVR_array, INF_COST, sizeof(DVR_array));
        DVR_array[ID][ID] = 0;
    }

    void initVectorTable()
    {
        struct Entry entry;
        string dest = "192.168.0.";
        for (int i = 0; i < ipAddress.size(); i++)
        {
            char x = i + 1 + '0';
            string temp = dest + x;
            if (find(adjacentNodes.begin(), adjacentNodes.end(), temp) != adjacentNodes.end())
            {
                for (int j = 0; j < currentNodes.size(); j++)
                {
                    if (!currentNodes[j].adj.compare(temp))
                    {
                        entry.dest = temp;
                        entry.nextHop = temp;
                        entry.cost = currentNodes[j].cost;
                    }
                }
            }
            else if (!IPAddress.compare(temp))
            {
                entry.dest = temp;
                entry.nextHop = temp;
                entry.cost = 0;
            }
            else
            {
                entry.dest = temp;
                entry.nextHop = "\t-";
                entry.cost = INF_COST;
            }
            vectorTable.push_back(entry);
        }
    }

    void initialTable(string filename)
    {
        ifstream file(filename);
        string line = "";

        if (file.is_open())
        {
            //cout << "opened" << endl;
        }

        while (getline(file, line))
        {
            string buf;
            stringstream ss(line);
            struct NodeStatus cn;
            int i = 0, flag = 0;
            string src, dest, cost, hop;
            while (ss >> buf)
            {
                if (i == 0)
                {
                    src = buf;
                    if (!src.compare(IPAddress))
                    {
                        flag = 1;
                    }
                }
                if (i == 1)
                {
                    if (flag == 1)
                    {
                        hop = buf;
                        dest = buf;
                    }
                    else
                    {
                        dest = buf;
                        if (!dest.compare(IPAddress))
                        {
                            hop = src;
                            flag = 1;
                        }
                    }
                }
                if (i == 2)
                {
                    cost = buf;
                }
                i++;
            }
            if (flag)
            {
                adjacentNodes.insert(hop);
                int costInt = stoi(cost);
                routingTable[make_pair(hop, hop)] = costInt;
                int dest_ID = stoi(hop.substr(hop.find(".", 8) + 1, hop.size() - 1));
                DVR_array[ID][dest_ID] = costInt;

                cn.adj = hop;
                cn.connection = 1;
                cn.cost = costInt;
                cn.rclkRate = 0;
                currentNodes.push_back(cn);
            }

            ipAddress.insert(src);
            ipAddress.insert(dest);
        }

        for (set<string>::iterator it = ipAddress.begin(); it != ipAddress.end(); ++it)
        {
            if (!(*it).compare(IPAddress))
            {
                continue;
            }
            map<pair<string, string>, int>::iterator f = routingTable.find(make_pair((*it), (*it)));
            if (f == routingTable.end())
            {
                routingTable[make_pair((*it), "-")] = INF_COST;
            }
        }

        file.close();
        initVectorTable();
    }

    void printTableFromMap()
    {
        cout << "Destination  \tNext-hop  \tCost" << endl;
        cout << "--------------\t-------------\t-----" << endl;
        for (map<pair<string, string>, int>::iterator it = routingTable.begin(); it != routingTable.end(); ++it)
        {
            if (!(*it).first.second.compare("-"))
            {
                cout << (*it).first.first << "\t     -     \t" << (*it).second << endl;
            }
            else
            {
                cout << (*it).first.first << "\t" << (*it).first.second << "\t" << (*it).second << endl;
            }
        }
        cout << endl
             << endl;
        cout << "Printing DVR array" << endl;
        for (int i = 1; i <= routingTable.size() + 1; i++)
        {
            for (int j = 1; j <= routingTable.size() + 1; j++)
            {
                if (DVR_array[i][j] >= INF_COST)
                {
                    cout << "INF"
                         << " ";
                }
                else
                {
                    cout << DVR_array[i][j] << " ";
                }
            }
            cout << endl;
        }
    }

    void printTable()
    {
        cout << "\t------\t" << IPAddress << "\t------\t" << endl;
        cout << "Destination  \tNext Hop \tCost" << endl;
        cout << "-------------\t-------------\t-----" << endl;
        for (int i = 0; i < vectorTable.size(); i++)
        {
            if (!vectorTable[i].dest.compare(IPAddress))
                continue;
            cout << vectorTable[i].dest << "\t" << vectorTable[i].nextHop << "\t" << vectorTable[i].cost << endl;
        }
        cout << endl
             << endl;
        // cout << "Printing DVR array" << endl;
        // for (int i = 1; i <= routingTable.size() + 1; i++)
        // {
        //     for (int j = 1; j <= routingTable.size() + 1; j++)
        //     {
        //         if (DVR_array[i][j] >= INF_COST)
        //         {
        //             cout << "INF"
        //                  << " ";
        //         }
        //         else
        //         {
        //             cout << DVR_array[i][j] << " ";
        //         }
        //     }
        //     cout << endl;
        // }
    }

    void forwardMessage(string dest, int length, string msg)
    {
        string forwardPckt = "frwd:" + dest + ":" + to_string(length) + ":" + msg;
        string destination;

        // for (map<pair<string, string>, int>::iterator it = routingTable.begin(); it != routingTable.end(); ++it)
        // {
        //     if (!(*it).first.first.compare(dest))
        //     {
        //         destination = (*it).first.second;
        //         break;
        //     }
        // }
        for (vector<Entry>::iterator it = vectorTable.begin(); it != vectorTable.end(); ++it)
        {
            if (!(*it).dest.compare(dest))
            {
                destination = (*it).nextHop;
                break;
            }
        }
        struct sockaddr_in router_address;

        router_address.sin_family = AF_INET;
        router_address.sin_port = htons(4747);
        inet_pton(AF_INET, destination.c_str(), &router_address.sin_addr);

        int sent_bytes = sendto(sockfd, forwardPckt.c_str(), 1024, 0, (struct sockaddr *)&router_address, sizeof(sockaddr_in));
        cout << msg.c_str() << " packet forwarded to " << destination.c_str() << " (printed by " << IPAddress.c_str() << ")\n";
    }

    string returnIP(unsigned char *raw)
    {
        int ipSegment[4];
        //cout << "in make ip" << endl;
        for (int i = 0; i < 4; i++)
        {
            ipSegment[i] = raw[i];
            //cout << ipSegment[i] << endl;
        }
        string ip = to_string(ipSegment[0]) + "." + to_string(ipSegment[1]) + "." + to_string(ipSegment[2]) + "." + to_string(ipSegment[3]);
        return ip;
    }

    string generatePacketFromMap()
    {
        string packet = "newT" + IPAddress;
        for (map<pair<string, string>, int>::iterator it = routingTable.begin(); it != routingTable.end(); ++it)
        {
            packet.append("<");
            packet.append((*it).first.first);
            packet.append(";");
            packet.append((*it).first.second);
            packet.append(";");
            packet.append(to_string((*it).second));
        }
        return packet;
    }

    string generatePacketFromVector()
    {
        string packet = "newT" + IPAddress;
        for (int i = 0; i < vectorTable.size(); i++)
        {
            packet.append("<");
            packet.append(vectorTable[i].dest);
            packet.append(";");
            packet.append(vectorTable[i].nextHop);
            packet.append(";");
            packet.append(to_string(vectorTable[i].cost));
        }
        return packet;
    }

    const char *readBytes(sockaddr_in &remoteAddress)
    {
        socklen_t addrlen;
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&remoteAddress, &addrlen);
        if (bytes_received < BUFFER_SIZE)
        {
            buffer[bytes_received] = NULL;
        }
        return buffer;
    }

    string readString(sockaddr_in &remoteAddress)
    {
        readBytes(remoteAddress);
        return bytes_received > 0 ? string(buffer, bytes_received) : "";
    }

    string extractIP(string recv, int i, int j)
    {
        unsigned char *src = new unsigned char[5];
        string temp = recv.substr(i, j);

        for (int i = 0; i < 4; i++)
        {
            src[i] = temp[i];
        }
        return returnIP(src);
    }

    int getLength(string recv, int start, int end){
        unsigned char *c1 = new unsigned char[3];
        string msgLength = recv.substr(start, end);
        int length = 0;
        c1[0] = msgLength[0];
        c1[1] = msgLength[1];
        int x0, x1;
        x0 = c1[0];
        x1 = c1[1] * 256;
        length = x1 + x0;
        return length;
    }

    string extractMsg(string recv, int start, int end)
    {
        int length = getLength(recv,start,end);
        char msg[length + 1];
        for (int i = 0; i < length; i++)
        {
            msg[i] = buffer[14 + i];
        }
        msg[length] = '\0';
        return string(msg);
    }

    vector<string> forwardedMsg(string recv)
    {
        //cout<<"\t\t\t???????????????????  "<<recv<<endl<<endl;

        vector<string> msg;
        stringstream str(recv);
        string temp;

        while (getline(str, temp, ':'))
        {
            msg.push_back(temp);
            //cout<<"\t\t"<<temp<<endl<<endl;
        }
        string len = msg[2];
        int ilen = stoi(len);
        msg[3].resize(ilen);
        return msg;
    }

    map<pair<string, string>, int> extractRoutingTable(string packet)
    {
        map<pair<string, string>, int> newRoutingTable;
        stringstream str(packet);
        string token;
        vector<string> entries;
        while (getline(str, token, '<'))
        {
            entries.push_back(token);
        }
        //struct RoutingTableEntry rte;
        for (int i = 0; i < entries.size(); i++)
        {
            //cout << entries[i] << endl;
            stringstream temp(entries[i]);
            string tok[3];
            int k = 0;
            while (getline(temp, tok[k], ';'))
            {
                k++;
            }
            newRoutingTable[make_pair(tok[0], tok[1])] = stoi(tok[2]);
            //cout << "dest : " << tok[0] << "  next : " << tok[1] << "  cost : " << tok[2] << endl;
        }
        return newRoutingTable;
    }

    vector<Entry> extractVectorTableFromNeighbor(string packet)
    {
        vector<Entry> newRoutingTable;

        stringstream str(packet);
        string token;
        vector<string> entries;
        while (getline(str, token, '<'))
        {
            entries.push_back(token);
        }
        //struct RoutingTableEntry rte;
        for (int i = 0; i < entries.size(); i++)
        {
            //cout << entries[i] << endl;
            stringstream temp(entries[i]);
            string tok[3];
            int k = 0;
            while (getline(temp, tok[k], ';'))
            {
                k++;
            }
            struct Entry entry;
            entry.cost = stoi(tok[2]);
            entry.dest = tok[0];
            entry.nextHop = tok[1];
            newRoutingTable.push_back(entry);
            //cout << "dest : " << tok[0] << "  next : " << tok[1] << "  cost : " << tok[2] << endl;
        }
        return newRoutingTable;
    }

    void updateRoutingTableForNeighbor(vector<Entry> newRoutingTable, string ip)
    {
        cout << "\n\n\t*****************Got from neighbours*************" << endl
             << endl;
        //printTable();

        int tempCost;
        for (int i = 0; i < ipAddress.size(); i++)
        {
            for (int j = 0; j < currentNodes.size(); j++)
            {
                if (!ip.compare(currentNodes[j].adj))
                {
                    tempCost = currentNodes[j].cost + newRoutingTable[i].cost;
                    if (!ip.compare(vectorTable[i].nextHop) || (tempCost < vectorTable[i].cost && IPAddress.compare(newRoutingTable[i].nextHop) != 0))
                    {
                        if (vectorTable[i].cost != tempCost)
                        {
                            //cout << "******changed********" << endl<< endl;
                            vectorTable[i].nextHop = ip;
                            vectorTable[i].cost = tempCost;
                            global_flag = 1;
                        }
                        break;
                    }
                }
            }
        }
        if (global_flag)
        {
            printTable();
        }
        global_flag = 0;
    }
    void updateNeighbor(string ip)
    {
        int i;
        for (i = 0; i < currentNodes.size(); i++)
        {
            if (!currentNodes[i].adj.compare(ip))
            {
                currentNodes[i].connection = 1;
                currentNodes[i].rclkRate = clkRate;
                break;
            }
        }
        //cout << "receiver : " << IPAddress << " sender : " << i << " recv clk : " << currentNodes[i].rclkRate << endl;
    }

    bool checkValidity(string adj, int i, string str)
    {
        int flag = 0;
        //!adj.compare(vectorTable[i].dest) || !isNeighbor(routingTable[i].destination)
        for (int i = 0; i < currentNodes.size(); i++)
        {
            if (!str.compare(currentNodes[i].adj))
            {
                flag = 1;
                break;
            }
        }
        return (!adj.compare(vectorTable[i].dest) || flag);
    }

    bool isAdjacent(string ip)
    {
        for (int i = 0; i < currentNodes.size(); i++)
        {
            if (!ip.compare(currentNodes[i].adj))
                return true;
        }
        return false;
    }

    void updateForFailure(int i, string hop, int cost)
    {
        vectorTable[i].nextHop = hop;
        vectorTable[i].cost = cost;
        global_flag = 1;
    }

    void LinkFailure(string adj)
    {
        for (int i = 0; i < vectorTable.size(); i++)
        {
            if (!adj.compare(vectorTable[i].nextHop))
            {
                if (checkValidity(adj, i, vectorTable[i].dest))
                {
                    updateForFailure(i, "\t-", INF_COST);
                }
                else if (isAdjacent(vectorTable[i].dest))
                {
                    int idx;
                    for (int k = 0; k < currentNodes.size(); k++)
                    {
                        if (!vectorTable[i].dest.compare(currentNodes[k].adj))
                        {
                            idx = k;
                            break;
                        }
                    }
                    updateForFailure(i, vectorTable[i].dest, currentNodes[idx].cost);
                }
            }
        }
        if (global_flag)
        {
            printTable();
        }
        global_flag = 0;
    }

    void exchangeTable()
    {
        clkRate++;
        string packet = generatePacketFromVector();
        //cout<<packet<<endl;

        for (set<string>::iterator it = adjacentNodes.begin(); it != adjacentNodes.end(); ++it)
        {
            struct sockaddr_in router_address;

            router_address.sin_family = AF_INET;
            router_address.sin_port = htons(4747);
            inet_pton(AF_INET, (*it).c_str(), &router_address.sin_addr);

            int sent_bytes = sendto(sockfd, packet.c_str(), 1024, 0, (struct sockaddr *)&router_address, sizeof(sockaddr_in));
            if (sent_bytes != -1)
            {
                cout << "routing table : " << IPAddress << " sent to : " << (*it) << endl;
            }
        }

        for (int i = 0; i < currentNodes.size(); i++)
        {
            if (clkRate - currentNodes[i].rclkRate > 3 && currentNodes[i].connection == 1)
            {
                cout << "----- link down with : " << currentNodes[i].adj << " -----" << endl;
                currentNodes[i].connection = -1;
                LinkFailure(currentNodes[i].adj);
            }
        }
    }

    void CostChange(string adj,int oldCost, int newCost)
    {
        for(int i = 0; i<ipAddress.size(); i++)
        {
            if(!adj.compare(vectorTable[i].nextHop))
            {
                if(!adj.compare(vectorTable[i].dest))
                {
                    vectorTable[i].cost = newCost;
                }
                else
                {
                    vectorTable[i].cost = vectorTable[i].cost - oldCost + newCost;
                }
                global_flag = 1;
            }
            else if(!adj.compare(vectorTable[i].dest) && vectorTable[i].cost>newCost)
            {
                updateForFailure(i,adj,newCost);
            }
        }
        if(global_flag){
            printTable();
        }
            
        global_flag = 0;
    }

    void updateCost(string recv)
    {
        string src = extractIP(recv, 4, 4);
        string dest = extractIP(recv, 8, 4);

        int newCost = getLength(recv,12,2);
        string nbr;
        int oldCost;
        for (int i = 0; i < currentNodes.size(); i++)
        {
            if (!src.compare(currentNodes[i].adj))
            {
                oldCost = currentNodes[i].cost;
                currentNodes[i].cost = newCost;
                nbr = src;
            }
            else if (!dest.compare(currentNodes[i].adj))
            {
                oldCost = currentNodes[i].cost;
                currentNodes[i].cost = newCost;
                nbr = dest;
            }
        }
        CostChange(nbr, oldCost, newCost);
    }

    void communicate()
    {
        struct sockaddr_in router_address;
        socklen_t addrlen;

        while (true)
        {
            string recv = readString(router_address);

            if (recv.empty())
            {
                continue;
            }

            string header = recv.substr(0, 4);
            if (!header.compare("show"))
            {
                cout << endl
                     << endl;
                printTable();
                cout << endl;
            }
            else if (!header.compare("clk "))
            {
                //cout << "Adjacent nodes size: " << adjacentNodes.size() << endl;
                exchangeTable();
            }
            else if (!header.compare("send"))
            {
                string src = extractIP(recv, 4, 4);
                string dest = extractIP(recv, 8, 4);

                string message = extractMsg(recv, 12, 2);

                if (!dest.compare(IPAddress))
                {
                    cout << "\n\t\tForwarded Packet" << endl;
                    cout << message << " packet reached destination (printed by " << dest << ")\n";
                    cout << endl;
                }
                else
                {
                    cout << "\n\t\tForwarded Packet" << endl;
                    forwardMessage(dest, message.length(), message);
                    cout << endl;
                }
            }
            else if (!header.compare("frwd"))
            {

                vector<string> fmsgs = forwardedMsg(recv);

                if (!fmsgs[1].compare(IPAddress))
                {
                    cout << "\n\t\tReceived Packet" << endl;
                    cout << fmsgs[3] << " packet reached destination (printed by " << fmsgs[1] << ")\n";
                    cout << endl;
                }
                else
                {
                    forwardMessage(fmsgs[1], stoi(fmsgs[2]), fmsgs[3]);
                }
                fmsgs.clear();
            }
            else if (!header.compare("cost"))
            {
                updateCost(recv);
            }
            else if (!header.compare("newT"))
            {
                string ipAdd = recv.substr(4, 11);
                updateNeighbor(ipAdd);

                int length = recv.length() - 15;
                char pckt[length];
                for (int i = 0; i < length; i++)
                {
                    pckt[i] = buffer[16 + i];
                }
                string packet(pckt);
                vector<Entry> newTable = extractVectorTableFromNeighbor(packet);
                updateRoutingTableForNeighbor(newTable, ipAdd);
            }
        }
    }
};

int main(int argc, char *argv[])
{
    string filename = "";

    if (argc != 3)
    {
        cout << "./router <ip address> <topology-file>" << endl;
        return 0;
    }

    filename.append(argv[2]);
    //cout << filename << endl;

    Router router(argv[1]);

    router.initialTable(filename);
    router.printTable();

    int bind_flag;
    struct sockaddr_in client_address;

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(4747);
    inet_pton(AF_INET, argv[1], &client_address.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind_flag = bind(sockfd, (struct sockaddr *)&client_address, sizeof(sockaddr_in));

    if (!bind_flag)
    {
        cout << "Connection successful!!" << endl;
    }
    else
    {
        cout << "Connection failed!!!" << endl;
    }

    cout << "--------------------------------------" << endl;

    router.communicate();

    return 0;
}