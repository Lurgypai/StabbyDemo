#pragma once
#include "EntitySystem.h"
#include "PositionComponent.h"
#include "AABB.h"
#include "CombatComponent.h"
#include "NetworkTypes.h"

class ZombieLC : public CombatComponent {
public:
	enum class State : uint8_t {
		walking,
		attacking,
		charging,
		stunned,
		dead,
		idle
	};

	struct ZombieState {

		bool operator==(const ZombieState & other) {
			return facing == other.facing && health == other.health && relaxFrame == other.relaxFrame && stunFrame == other.stunFrame &&
				chargeFrame == other.chargeFrame && attackChanged == other.attackChanged && state ==
				other.state && pos == other.pos && vel == other.vel;
		}

		bool operator!=(const ZombieState & other) {
			return !(*this == other);
		}

		int32_t facing;
		int32_t health;
		uint32_t relaxFrame;
		uint32_t stunFrame;
		uint32_t chargeFrame;
		uint32_t deathFrame;
		uint32_t idleFrame;
		bool attackChanged;
		State state;

		Vec2f pos;
		Vec2f vel;

		Time_t when;
	};

	ZombieLC(EntityId id = 0);

	template<typename T>
	void searchForTarget();
	void runLogic();

	virtual void damage(int i) override;
	virtual void die() override;
	virtual void onAttackLand() override;
	virtual AABB * getActiveHitbox() override;
	virtual int getActiveDamage() override;
	//checks if the attack changed, and resets it if it did.
	virtual bool readAttackChange() override;
	virtual const AABB * getHurtboxes(int * size) const override;
	virtual void updateHurtboxes() override;

	bool isStunned() const;
	void setState(const ZombieState & state);
	ZombieState getState();

	//temporary variable to keep track of which zombie is which
	int onlineId;
protected:
	const static int WIDTH = 4;
	const static int HEIGHT = 20;

	ZombieState zombieState;
	float moveSpeed;
	int relaxFramMax;
	int stunFrameMax;
	int chargeFrameMax;
	int deathFrameMax;
	int idleFrameMax;

	AABB hitbox;
	AABB hurtbox;

	EntityId targetId;
};

template<typename T>
inline void ZombieLC::searchForTarget() {
	if (targetId == 0) {
		if (EntitySystem::Contains<T>()) {
			PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

			EntityId closestId = EntitySystem::GetPool<T>().front().getId();
			Vec2f closestPos = EntitySystem::GetComp<PositionComponent>(closestId)->pos;

			for (auto& component : EntitySystem::GetPool<T>()) {
				PositionComponent * otherPos = EntitySystem::GetComp<PositionComponent>(component.getId());
				if (position->pos.distance(otherPos->pos) < position->pos.distance(closestPos)) {
					closestId = component.getId();
					closestPos = otherPos->pos;
				}
			}

			targetId = closestId;
		}
	}
}
