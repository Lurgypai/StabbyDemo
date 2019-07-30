#include "stdafx.h"
#include "CombatComponent.h"

CombatComponent::CombatComponent(EntityId id_) : id{id_}
{}

EntityId CombatComponent::getId() const {
	return id;
}
