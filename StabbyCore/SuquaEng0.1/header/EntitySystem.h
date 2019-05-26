#pragma once
#include <vector>
#include <unordered_map>
#include "PoolMap.h"


using EntityId = unsigned int;

//you may need to alter the inner workings of makeComps, and how components are stored
//but make sure that the implementation can change without changinging the interface

class EntitySystem {
public:

	template<typename T>
	static T* GetComp(EntityId id);

	template<typename T>
	static void MakeComps(unsigned int size, unsigned int* first);

	static void GenEntities(unsigned int num, unsigned int* idStore);
	
	template<typename T>
	static Pool<T> & GetPool();

	template<typename T>
	static bool Contains();

	template<typename T>
	static void FreeComps(unsigned int size, EntityId * first);
private:

	static EntityId entityIncrem;
	static PoolMap ComponentMaps;
};

template<typename T>
inline T * EntitySystem::GetComp(EntityId id) {
	for (auto& comp : ComponentMaps.get<T>()) {
		if (comp.getId() == id) {
			return &comp;
		}
	}
	return nullptr;
}

//TODO 
template<typename T>
inline void EntitySystem::MakeComps(unsigned int size, unsigned int * first) {

	for (int i = 0; i != size; i++) {
		ComponentMaps.add<T>(first[i]);
	}
}

template<typename T>
inline Pool<T> & EntitySystem::GetPool() {
	return ComponentMaps.get<T>();
}

template<typename T>
inline bool EntitySystem::Contains() {
	return ComponentMaps.contains<T>();
}

template<typename T>
inline void EntitySystem::FreeComps(unsigned int size, EntityId * first) {
	for (int i = 0; i != size; ++i) {
		for (auto iter = ComponentMaps.get<T>().beginResource(); iter != ComponentMaps.get<T>().endResource(); ++iter) {
			auto& comp = *iter;
			if (comp.val.getId() == first[i])
				comp.isFree = true;
		}
	}
}
