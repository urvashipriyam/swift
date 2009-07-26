#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h"

#include <boost/bind.hpp>

#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDBrowseQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDRegisterQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveHostnameQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

FakeDNSSDQuerier::FakeDNSSDQuerier() {
}

boost::shared_ptr<DNSSDBrowseQuery> FakeDNSSDQuerier::createBrowseQuery() {
	return boost::shared_ptr<DNSSDBrowseQuery>(new FakeDNSSDBrowseQuery(shared_from_this()));
}

boost::shared_ptr<DNSSDRegisterQuery> FakeDNSSDQuerier::createRegisterQuery(const String& name, int port, const LinkLocalServiceInfo& info) {
	return boost::shared_ptr<DNSSDRegisterQuery>(new FakeDNSSDRegisterQuery(name, port, info, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveServiceQuery> FakeDNSSDQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
	return boost::shared_ptr<DNSSDResolveServiceQuery>(new FakeDNSSDResolveServiceQuery(service, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveHostnameQuery> FakeDNSSDQuerier::createResolveHostnameQuery(const String& hostname, int interfaceIndex) {
	return boost::shared_ptr<DNSSDResolveHostnameQuery>(new FakeDNSSDResolveHostnameQuery(hostname, interfaceIndex, shared_from_this()));
}

void FakeDNSSDQuerier::addRunningQuery(boost::shared_ptr<FakeDNSSDQuery> query) {
	runningQueries.push_back(query);
	if (boost::shared_ptr<FakeDNSSDBrowseQuery> browseQuery = boost::dynamic_pointer_cast<FakeDNSSDBrowseQuery>(query)) {
		foreach(const DNSSDServiceID& service, services) {
			MainEventLoop::postEvent(boost::bind(boost::ref(browseQuery->onServiceAdded), service), shared_from_this());
		}
	}
	else if (boost::shared_ptr<FakeDNSSDResolveServiceQuery> resolveQuery = boost::dynamic_pointer_cast<FakeDNSSDResolveServiceQuery>(query)) {
		for(ServiceInfoMap::const_iterator i = serviceInfo.begin(); i != serviceInfo.end(); ++i) {
			if (i->first == resolveQuery->service) {
				MainEventLoop::postEvent(boost::bind(boost::ref(resolveQuery->onServiceResolved), i->second), shared_from_this());
			}
		}
	}
}

void FakeDNSSDQuerier::removeRunningQuery(boost::shared_ptr<FakeDNSSDQuery> query) {
	runningQueries.erase(std::remove(
		runningQueries.begin(), runningQueries.end(), query), runningQueries.end());
}

void FakeDNSSDQuerier::addService(const DNSSDServiceID& id) {
	services.insert(id);
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceAdded), id), shared_from_this());
	}
}

void FakeDNSSDQuerier::removeService(const DNSSDServiceID& id) {
	services.erase(id);
	serviceInfo.erase(id);
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceRemoved), id), shared_from_this());
	}
}

void FakeDNSSDQuerier::setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info) {
	std::pair<ServiceInfoMap::iterator, bool> r = serviceInfo.insert(std::make_pair(id, info));
	if (!r.second) {
		r.first->second = info;
	}
	foreach(const boost::shared_ptr<FakeDNSSDResolveServiceQuery>& query, getQueries<FakeDNSSDResolveServiceQuery>()) {
		if (query->service == id) {
			MainEventLoop::postEvent(boost::bind(boost::ref(query->onServiceResolved), info), shared_from_this());
		}
	}
}

void FakeDNSSDQuerier::setBrowseError() {
	foreach(const boost::shared_ptr<FakeDNSSDBrowseQuery>& query, getQueries<FakeDNSSDBrowseQuery>()) {
		MainEventLoop::postEvent(boost::ref(query->onError), shared_from_this());
	}
}

void FakeDNSSDQuerier::setRegisterError() {
	foreach(const boost::shared_ptr<FakeDNSSDRegisterQuery>& query, getQueries<FakeDNSSDRegisterQuery>()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(query->onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
	}
}

}
