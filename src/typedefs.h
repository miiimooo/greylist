/// $Id: typedefs.h 27 2009-09-15 19:37:26Z mimo $
#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "datasource.h"
#include "dbdatasource.h"

typedef TmplDataSource<DBDataSource> DBDataSourceType;

#include "core.h"
#include "greylist.h"
#include "tmplcore.h"

typedef TmplPolicy<Greylist> GreylistPolicy;

#include "whitelist.h"

typedef TmplPolicy<Whitelist> WhitelistPolicy;

#include "exactmatch.h"

typedef TmplMatcher<ExactMatch> ExactMatchMatcher;

#include "patternmatch.h" 

typedef TmplMatcher<PatternMatch> PatternMatchMatcher;

#include "allmatcher.h"

typedef TmplMatcher<AllMatch> AllMatchMatcher;

#include "cachedexactmatch.h"

typedef TmplMatcher<CachedExactMatch> CachedExactMatchMatcher;

#include "cachedpatternmatch.h"

typedef TmplMatcher<CachedPatternMatch> CachedPatternMatchMatcher;

#endif
