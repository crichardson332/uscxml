/**
 *  @file
 *  @author     2012-2016 Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *  @copyright  Simplified BSD
 *
 *  @cond
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 *  @endcond
 */

#ifndef FASTMICROSTEP_H_065FE1F7
#define FASTMICROSTEP_H_065FE1F7

//#define USCXML_VERBOSE 1

#include "uscxml/Common.h"
#include "uscxml/util/DOM.h" // X

#include <vector>
#include <map>
#include <set>
#include "MicroStepImpl.h"

#include <boost/dynamic_bitset.hpp>

//#undef WITH_CACHE_FILES

#ifdef _WIN32
#define BITSET_BLOCKTYPE size_t
#else
#define BITSET_BLOCKTYPE
#endif

namespace uscxml {

/**
 * @ingroup microstep
 * @ingroup impl
 *
 * MicroStep implementation backed by indexed bit-arrays.
 */
class FastMicroStep : public MicroStepImpl {
public:
	FastMicroStep(MicroStepCallbacks* callbacks);
	virtual ~FastMicroStep();
	virtual std::shared_ptr<MicroStepImpl> create(MicroStepCallbacks* callbacks);

	std::string getName() {
		return "fast";
	}

	virtual InterpreterState step(size_t blockMs);
	virtual void reset();
	virtual bool isInState(const std::string& stateId);
    virtual std::list<std::string> getActiveStates();
	virtual std::list<XERCESC_NS::DOMElement*> getConfiguration();
	void markAsCancelled();

	virtual void deserialize(const Data& encodedState);
	virtual Data serialize();

protected:
	FastMicroStep() {}; // only for factory

	class Transition {
	public:
		Transition(uint32_t postFixOrder) : postFixOrder(postFixOrder) {}
		const uint32_t postFixOrder; // making these const increases performance somewhat

		XERCESC_NS::DOMElement* element = NULL;
		boost::dynamic_bitset<BITSET_BLOCKTYPE> conflicts;

		uint32_t source = 0;
		boost::dynamic_bitset<BITSET_BLOCKTYPE> target;

		XERCESC_NS::DOMElement* onTrans = NULL;

		std::string event;
		std::string cond;

		unsigned char type = 0;

	};

	class State {
	public:
		State(uint32_t documentOrder) : documentOrder(documentOrder) {}
		const uint32_t documentOrder;

		XERCESC_NS::DOMElement* element = NULL;
		boost::dynamic_bitset<BITSET_BLOCKTYPE> completion;
		boost::dynamic_bitset<BITSET_BLOCKTYPE> children;
		boost::dynamic_bitset<BITSET_BLOCKTYPE> ancestors;
		uint32_t parent = 0;

		std::list<XERCESC_NS::DOMElement*> data;
		std::list<XERCESC_NS::DOMElement*> invoke;
		std::list<XERCESC_NS::DOMElement*> onEntry;
		std::list<XERCESC_NS::DOMElement*> onExit;
		XERCESC_NS::DOMElement* doneData = NULL;
		std::string name;

		unsigned char type = 0;
	};

	virtual void init(XERCESC_NS::DOMElement* scxml);

	std::list<XERCESC_NS::DOMElement*> getCompletion(const XERCESC_NS::DOMElement* state);

	unsigned char _flags;
	std::map<std::string, int> _stateIds;

	std::vector<State*> _states;
	std::vector<Transition*> _transitions;
	std::list<XERCESC_NS::DOMElement*> _globalScripts;

	boost::dynamic_bitset<BITSET_BLOCKTYPE> _configuration;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _invocations;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _history;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _initializedData;

	std::set<boost::dynamic_bitset<BITSET_BLOCKTYPE> > _microstepConfigurations;

	Binding _binding;
	XERCESC_NS::DOMElement* _scxml;
	X _xmlPrefix;
	X _xmlNS;

	bool _isInitialized;
	bool _isCancelled;
	Event _event; // we do not care about the event's representation

private:
	std::list<XERCESC_NS::DOMElement*> getHistoryCompletion(const XERCESC_NS::DOMElement* state);
	void resortStates(XERCESC_NS::DOMElement* node, const X& xmlPrefix);

	std::string toBase64(const boost::dynamic_bitset<BITSET_BLOCKTYPE>& bitset);
	boost::dynamic_bitset<BITSET_BLOCKTYPE> fromBase64(const std::string& encoded);

	boost::dynamic_bitset<BITSET_BLOCKTYPE> _exitSet;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _entrySet;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _targetSet;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _tmpStates;

	// per transition exit set, kept in a vector for cache locality (~25% faster than transition[i]->exitSet)
	std::vector<std::pair<uint32_t, uint32_t> > _exitSets;

	boost::dynamic_bitset<BITSET_BLOCKTYPE> _conflicts;
	boost::dynamic_bitset<BITSET_BLOCKTYPE> _transSet;

#ifdef USCXML_VERBOSE
	void printStateNames(const boost::dynamic_bitset<BITSET_BLOCKTYPE>& bitset);
#endif

	uint32_t getTransitionDomain(const Transition* transition);
	std::pair<uint32_t, uint32_t> getExitSet(const Transition* transition);
	std::map<uint32_t, std::pair<uint32_t, uint32_t> > _exitSetCache;

	friend class Factory;
};

}

#endif /* end of include guard: FASTMICROSTEP_H_065FE1F7 */
