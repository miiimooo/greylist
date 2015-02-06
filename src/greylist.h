/// $Id: greylist.h 53 2009-10-11 21:34:25Z mimo $
/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
*/

#ifndef GREYLIST_H
#define GREYLIST_H

#include <iostream>
#include <memory>
#include <sstream>
#include <boost/smart_ptr.hpp>
#include "defensive.h"
#include "triplet.h"
#include "datasource.h"
#include "core.h"

class Greylist
{
public:
  typedef enum {
    reverse,
    weak,
    normal,
    init
  }ModeType;
  ~Greylist() { tracer << "gl destroyed" << std::endl; }
  Greylist(boost::shared_ptr<DataSource> ds, ModeType m, bool v, unsigned t, unsigned w) 
    : _ds(ds), _mode(m), _verbose(v), _timeout(t), _weakbytes(w) { }
  Core::ActionType decide(Triplet& triplet, Core::Extra& extra) {
    unsigned long long id;
    unsigned long count, uts;
    if (!findTriplet(triplet, id, count, uts)) {
      insertTriplet(triplet);
      extra.setPrefix("new");
      return Core::defer;
    }
    unsigned long lNow = getNow();
    unsigned long lTimeDiff = 0;
    
    if(uts < lNow) {  // not in future
      lTimeDiff = lNow - uts;
    }
    if( (count > 0) || (lTimeDiff > _timeout) ) {
      incrementTriplet(id, lNow);
      extra.set("ok", count, lTimeDiff);
      return Core::dunno;
    }
    extra.set("wait", count, lTimeDiff);
    return Core::defer;
  }
    /** 
      grab a Triplet
      check against white/grey/black-list modules
      look up Triplet in database
      update/insert into database
      return result
      optional: terminate thread (for resource leaks after x runs)
      yield (or semaphore? somehow waiting for a new Triplet)
      **/
protected:
  bool findTriplet(Triplet &triplet, unsigned long long& id, unsigned long& count,
		    unsigned long& uts) {
//     std::auto_ptr<DataSource::BaseQuery> query(_ds->createQuery());
    boost::shared_ptr<DataSource::BaseQuery> query(_ds->createQuery());
//     query->setTable("triplet");
    query->addReturnField("id")->addReturnField("count")->addReturnField("uts");
    query->addWhere("sender", triplet.getSender(), "=");
    query->addWhere("recipient", triplet.getRecipient(), "=");
    ModeType mode = _mode;
    if ((mode == reverse) && (triplet.getShortClientName().empty())) { 
      mode = weak;  //fallback
    }
    unsigned parts = IPAddress::default_length;
    switch(mode) {
      case reverse:
	query->addWhere("client_address", triplet.getShortClientName(), "=");
	break;
      case weak:
	parts = _weakbytes;
      case normal:
	for(int i=0, j=64; i < parts; i++, j>>=1) {
	  query->addWhere("ip" + Utils::StrmConvert(j), 
			   triplet.getClientAddressNumericIdx(i), "=");
	}
	break;
    }
    query->lookup();
    // TODO return num, vals here
//     std::cout << query->numRows() << std::endl;
    if (query->numRows() == 0) {
      return false;
    }
    query->fetchRow();
    id = query->getBigNumeric(1);
    count = query->getNumeric(2);
    uts = query->getNumeric(3);
//     std::cout << id << ", " << count << ", " << uts << std::endl;
    //TODO maybe some db maintenance if numRows>1
    return true;
  }
  void insertTriplet(Triplet &triplet) {
    unsigned long lNow = getNow();
    boost::shared_ptr<DataSource::BaseQuery> query(_ds->createQuery());
//     query->setTable("triplet");
    //TODO add if(triplet.getReversed)
    query->addString("client_address", triplet.getShortClientName());
    query->addString("sender", triplet.getSender());
    query->addString("recipient", triplet.getRecipient());
    query->addNumeric("ip64", triplet.getClientAddressNumericIdx(0));
    query->addNumeric("ip32", triplet.getClientAddressNumericIdx(1));
    query->addNumeric("ip16", triplet.getClientAddressNumericIdx(2));
    query->addNumeric("ip8", triplet.getClientAddressNumericIdx(3));
    query->addNumeric("count", 0ul);
    query->addNumeric("uts", lNow);
    query->insert();
  }
  void incrementTriplet(unsigned long long id, unsigned long lNow) {
    boost::shared_ptr<DataSource::BaseQuery> query(_ds->createQuery());
//     query->setTable("triplet");
    query->addIncrement("count", 1);
    query->addSet("uts", lNow);
    query->addWhere("id", id, "="); /* always last */
    query->update();
  }
  unsigned long getNow() {
    time_t ltime;
    time(&ltime); //get ultime (unix timestamp) in ltime
    unsigned long lNow = ltime;
    return lNow;
  }
  
private:
  boost::shared_ptr<DataSource> _ds;
  ModeType _mode;
  bool _verbose;
  unsigned _timeout;
  unsigned _weakbytes;
//   static _fields;
};

#endif // GREYLIST_H
