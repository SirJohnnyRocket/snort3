//--------------------------------------------------------------------------
// Copyright (C) 2014-2020 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2006-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// host_attributes.cc  author davis mcpherson <davmcphe@cisco.com>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "host_attributes.h"

#include "hash/lru_cache_shared.h"
#include "main/shell.h"
#include "main/snort_config.h"
#include "main/thread.h"

using namespace snort;

static const PegInfo host_attribute_pegs[] =
{
    { CountType::MAX, "total_hosts", "maximum number of entries in the host attribute table" },
    { CountType::SUM, "hosts_pruned", "number of LRU hosts pruned due to configured resource limits" },
    { CountType::SUM, "dynamic_host_adds", "number of host additions after initial host file load" },
    { CountType::SUM, "dynamic_service_adds", "number of service additions after initial host file load" },
    { CountType::SUM, "dynamic_service_updates", "number of service updates after initial host file load" },
    { CountType::SUM, "service_list_overflows", "number of service additions that failed due to configured resource limits" },
    { CountType::END, nullptr, nullptr }
};

template<typename Key, typename Value, typename Hash>
class HostLruSharedCache : public LruCacheShared<Key, Value, Hash>
{
public:
    HostLruSharedCache(const size_t initial_size) : LruCacheShared<Key, Value, Hash>(initial_size)
    { }
};

typedef HostLruSharedCache<snort::SfIp, HostAttributesDescriptor, HostAttributesCacheKey> HostAttributesSharedCache;

static THREAD_LOCAL HostAttributesSharedCache* active_cache = nullptr;
static HostAttributesSharedCache* swap_cache = nullptr;
static HostAttributesSharedCache* next_cache = nullptr;
static HostAttributesSharedCache* old_cache = nullptr;
static THREAD_LOCAL HostAttributeStats host_attribute_stats;

bool HostAttributesDescriptor::update_service
    (uint16_t port, uint16_t protocol, SnortProtocolId snort_protocol_id, bool& updated)
{
    std::lock_guard<std::mutex> lck(host_attributes_lock);

    for ( auto& s : services)
    {
        if ( s.ipproto == protocol && (uint16_t)s.port == port )
        {
            s.snort_protocol_id = snort_protocol_id;
            updated = true;
            return true;
        }
    }

    // service not found, add it
    if ( services.size() < SnortConfig::get_conf()->get_max_services_per_host() )
    {
        updated = false;
        services.emplace_back(HostServiceDescriptor(port, protocol, snort_protocol_id));
        return true;
    }

    return false;
}

SnortProtocolId HostAttributesDescriptor::get_snort_protocol_id(int ipprotocol, uint16_t port) const
{
    std::lock_guard<std::mutex> lck(host_attributes_lock);

    for ( auto& s : services )
    {
        if ( (s.ipproto == ipprotocol) && (s.port == port) )
            return s.snort_protocol_id;
    }

    return UNKNOWN_PROTOCOL_ID;
}

bool HostAttributesManager::load_hosts_file(snort::SnortConfig* sc, const char* fname)
{
    delete next_cache;
    next_cache = new HostAttributesSharedCache(sc->max_attribute_hosts);

    Shell sh(fname);
    if ( sh.configure(sc, false, true) )
        return true;

    // loading of host file failed...
    delete next_cache;
    next_cache = nullptr;
    return false;
}

bool HostAttributesManager::add_host(HostAttributesEntry host, snort::SnortConfig* sc)
{
    if ( !next_cache )
        next_cache = new HostAttributesSharedCache(sc->max_attribute_hosts);

    return next_cache->find_else_insert(host->get_ip_addr(), host, true);
}

bool HostAttributesManager::activate()
{
    old_cache = active_cache;
    active_cache = next_cache;
    swap_cache = next_cache;
    next_cache = nullptr;

    return ( active_cache != old_cache ) ? true : false;
}

void HostAttributesManager::initialize()
{ active_cache = swap_cache; }

void HostAttributesManager::swap_cleanup()
{ delete old_cache; }

void HostAttributesManager::term()
{ delete active_cache; }

HostAttributesEntry HostAttributesManager::find_host(const snort::SfIp& host_ip)
{
    if ( active_cache )
        return active_cache->find(host_ip);

    return nullptr;
}

void HostAttributesManager::update_service(const snort::SfIp& host_ip, uint16_t port, uint16_t protocol, SnortProtocolId snort_protocol_id)
{
    if ( active_cache )
    {
        bool created = false;
        HostAttributesEntry host = active_cache->find_else_create(host_ip, &created);
        if ( host )
        {
            if ( created )
            {
                host->set_ip_addr(host_ip);
                host_attribute_stats.dynamic_host_adds++;
            }

            bool updated = false;
            if ( host->update_service(port, protocol, snort_protocol_id, updated) )
            {
                if ( updated )
                    host_attribute_stats.dynamic_service_updates++;
                else
                    host_attribute_stats.dynamic_service_adds++;
            }
            else
                host_attribute_stats.service_list_overflows++;
        }
    }
}

int32_t HostAttributesManager::get_num_host_entries()
{
    if ( active_cache )
        return active_cache->size();

    return -1;
}

const PegInfo* HostAttributesManager::get_pegs()
{ return (const PegInfo*)&host_attribute_pegs; }

PegCount* HostAttributesManager::get_peg_counts()
{
    if ( active_cache )
    {
        LruCacheSharedStats* cache_stats = (LruCacheSharedStats*) active_cache->get_counts();
        host_attribute_stats.hosts_pruned = cache_stats->alloc_prunes;
        host_attribute_stats.total_hosts = active_cache->size();
    }

    return (PegCount*)&host_attribute_stats;
}

