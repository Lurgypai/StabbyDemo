#pragma once
#include "enet/enet.h"
#include "NetworkTypes.h"
#include "Pool.h"

#include <string>
#include <deque>


class Host {
public:
	Host();
	~Host();
	bool createClient(size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	bool createServer(int port, size_t peerCount, size_t channels = 0, enet_uint32 incomingBandwidth = 0, enet_uint32 outgoingBandwidth = 0);
	template<typename Packet>
	void sendPacket(ENetPeer * peer, size_t channel, Packet packet);
	void sendData(ENetPeer * peer, enet_uint8 channel, void * data, size_t dataSize);

	template<typename Packet>
	void sendPacket(NetworkId id, size_t channel, Packet packet);
	void sendData(NetworkId id, enet_uint8 channel, void * data, size_t dataSize);

	int service(ENetEvent *event, enet_uint32 timeout);
	NetworkId connect(const std::string & ip, int port, size_t channels);
	NetworkId addPeer(ENetPeer * peer);
	void removePeer(NetworkId peerId);
	void disconnect(NetworkId peerId);
	void resetConnection(NetworkId peerId);
private:
	ENetHost * host;
	size_t channelCount;
	Pool<ENetPeer *> peers;
	std::deque<NetworkId> freeIds;
};

template<typename Packet>
void Host::sendPacket(ENetPeer * peer, size_t channel, Packet packet) {
	packet.serialize();
	enet_peer_send(peer, channel, enet_packet_create(&packet, sizeof(Packet), ENET_PACKET_FLAG_RELIABLE));
}

template<typename Packet>
inline void Host::sendPacket(NetworkId id, size_t channel, Packet packet) {
	packet.serialize();
	enet_peer_send(peers[id], channel, enet_packet_create(&packet, sizeof(Packet), ENET_PACKET_FLAG_RELIABLE));
}
