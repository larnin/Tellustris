#pragma once

#include <Nazara/Physics2D/Arbiter2D.hpp>

enum class ContactType
{
	Collision,
	Trigger
};

class ContactArbiter2D
{
public:
	ContactArbiter2D(Nz::Arbiter2D & arbiter, bool isFirst, ContactType type) : m_arbiter(arbiter), m_bIsFirst(isFirst), m_type(type) {}
	ContactArbiter2D(const ContactArbiter2D &) = delete;
	ContactArbiter2D & operator=(const ContactArbiter2D &) = delete;
	ContactArbiter2D(ContactArbiter2D &&) = default;
	ContactArbiter2D & operator=(ContactArbiter2D &&) = default;

	float computeTotalKinematicEnergy() const { return m_arbiter.ComputeTotalKinematicEnergy(); }
	Nz::Vector2f computeTotalImpulse() const { return m_arbiter.ComputeTotalImpulse(); }

	Nz::RigidBody2D* getThisBody() const { return m_bIsFirst ? m_arbiter.GetBodies().first : m_arbiter.GetBodies().second; }
	Nz::RigidBody2D* getOtherBody() const { return !m_bIsFirst ? m_arbiter.GetBodies().first : m_arbiter.GetBodies().second; }

	std::size_t getContactCount() const { return m_arbiter.GetContactCount(); }
	float getContactDepth(std::size_t i) const { return m_arbiter.GetContactDepth(i); }
	Nz::Vector2f getThisContactPoint(std::size_t i) const { return m_bIsFirst ? m_arbiter.GetContactPointA(i) : m_arbiter.GetContactPointB(i); }
	Nz::Vector2f getOtherContactPoint(std::size_t i) const { return !m_bIsFirst ? m_arbiter.GetContactPointA(i) : m_arbiter.GetContactPointB(i); }

	float getElasticity() const { return m_arbiter.GetElasticity(); }
	float getFriction() const { return m_arbiter.GetFriction(); }
	Nz::Vector2f getNormal() const { return m_arbiter.GetNormal(); }
	Nz::Vector2f getSurfaceVelocity() const { return m_arbiter.GetSurfaceVelocity(); }

	bool isFirstContact() const { return m_arbiter.IsFirstContact(); }
	bool isRemoval() const { return m_arbiter.IsRemoval(); }

	void setElasticity(float elasticity) { m_arbiter.SetElasticity(elasticity); }
	void setFriction(float friction) { m_arbiter.SetFriction(friction); }
	void setSurfaceVelocity(const Nz::Vector2f& surfaceVelocity) { m_arbiter.SetSurfaceVelocity(surfaceVelocity); }

	Nz::Arbiter2D & getNzArbiter() { return m_arbiter; }
	ContactType getContactType() const { return m_type; }

private:
	Nz::Arbiter2D& m_arbiter;
	bool m_bIsFirst;
	ContactType m_type;
};