#pragma once
#include "Pool.h"
#include <memory>
#include <vector>
#include <unordered_map>

#include "TypeData.h"
#include "PoolNotFoundException.h"

using pool_ptr = std::shared_ptr<IPool>;

class PoolMap {
public:
	template<typename U>
	void add(U&& u) {
		if (!pools.contains(TypeTag<U>::tag)) {
			pools.add(TypeTag<U>::tag, std::make_shared<Pool<U>>());
		}

		Pool<U>* pool = static_cast<Pool<U>*>(pools[TypeTag<U>::tag].get());
		pool->add(std::forward<U>(u));
	}

	template<typename U>
	void add(size_t pos, U&& u) {
		if (!pools.contains(TypeTag<U>::tag)) {
			pools.add(TypeTag<U>::tag, std::make_shared<Pool<U>>());
		}

		Pool<U>* pool = static_cast<Pool<U>*>(pools[TypeTag<U>::tag].get());
		pool->add(pos, std::forward<U>(u));
	}

	template<typename U>
	void add() {
		if (!pools.contains(TypeTag<U>::tag)) {
			pools.add(TypeTag<U>::tag, std::make_shared<Pool<U>>());
		}

		Pool<U>* pool = static_cast<Pool<U>*>(pools[TypeTag<U>::tag].get());
		pool->add();
	}

	template<typename T>
	Pool<T> & get() {
		if (pools.contains(TypeTag<T>::tag)) {
			return static_cast<Pool<T> &>(*(pools[TypeTag<T>::tag]));
		}
		else {
			throw PoolNotFoundException{ TypeTag<T>::tag };
		}
	}

	template<typename T>
	bool contains() {
		return pools.contains(TypeTag<T>::tag);
	}

	auto begin() {
		return pools.begin();
	}

	auto end() {
		return pools.end();
	}

private:
	Pool<pool_ptr> pools;
};