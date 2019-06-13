#include "stdafx.h"
#include "Host.h"

Host::Host() :
	host{NULL},
	channelCount{0}
{}

Host::~Host() {
	enet_host_destroy(host);
	for (auto iter = peers.beginResource(); iter != peers.endResource(); ++iter) {
		if (!iter->isFree) {
			enet_peer_reset(iter->val);
		}
	}
}

bool Host::createClient(size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	host = enet_host_create(NULL, peerCount, channels, incomingBandwidth, outgoingBandwidth);

	if (host != NULL) {
		channelCount = channels;
		peers.resize(peerCount);
		for (size_t id = 0; id != peerCount;) {
			freeIds.push_back(id++);
		}
	}

	return host != NULL;
}

bool Host::createServer(int port, size_t peerCount, size_t channels, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth) {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	host = enet_host_create(&address, peerCount, channels, incomingBandwidth, outgoingBandwidth);

	if (host != NULL) {
		channelCount = channels;
		peers.resize(peerCount);
		for (size_t id = 0; id != peerCount;) {
			freeIds.push_back(id++);
		}
	}

	return host != NULL;
}

void Host::sendData(ENetPeer * peer, enet_uint8 channel, void * data, size_t dataSize) {
	enet_peer_send(peer, channel, enet_packet_create(data, dataSize, ENET_PACKET_FLAG_RELIABLE));
}

void Host::sendData(NetworkId id, enet_uint8 channel, void * data, size_t dataSize) {
	enet_peer_send(peers[id], channel, enet_packet_create(data, dataSize, ENET_PACKET_FLAG_RELIABLE));
}

int Host::service(ENetEvent * event, enet_uint32 timeout) {
	return enet_host_service(host, event, timeout);
}

NetworkId Host::connect(const std::string & ip, int port, size_t channels) {
	if (!freeIds.empty()) {
		ENetAddress address;
		enet_address_set_host(&address, ip.c_str());
		address.port = port;

		NetworkId id = freeIds.front();
		freeIds.pop_front();
		peers.add(id, enet_host_connect(host, &address, channels, 0));
		return id;
	}
}

NetworkId Host::addPeer(ENetPeer * peer) {
	if (!freeIds.empty()) {
		NetworkId id = freeIds.front();
		freeIds.pop_front();
		peers.add(id, peer);
		return id;
	}
}

void Host::removePeer(NetworkId peerId) {
	freeIds.push_back(peerId);
	peers.free(peerId);
}

void Host::disconnect(NetworkId peerId) {
	enet_peer_disconnect(peers[peerId], 0);
	removePeer(peerId);
}

void Host::resetConnection(NetworkId peerId) {
	enet_peer_reset(peers[peerId]);
	removePeer(peerId);
}

//add full dosconnect support
//add proper quit packet sending to the server