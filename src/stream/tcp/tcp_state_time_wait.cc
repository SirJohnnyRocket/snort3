//--------------------------------------------------------------------------
// Copyright (C) 2015-2016 Cisco and/or its affiliates. All rights reserved.
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

// tcp_state_time_wait.cc author davis mcpherson <davmcphe@cisco.com>
// Created on: Aug 5, 2015

#include <iostream>
using namespace std;

#include "tcp_module.h"
#include "tcp_tracker.h"
#include "tcp_session.h"
#include "tcp_normalizer.h"
#include "tcp_state_time_wait.h"

#ifdef UNIT_TEST
#include "catch/catch.hpp"
#endif

TcpStateTimeWait::TcpStateTimeWait(TcpStateMachine& tsm, TcpSession& session) :
    TcpStateHandler(TcpStreamTracker::TCP_TIME_WAIT, tsm), session(session)
{
}

TcpStateTimeWait::~TcpStateTimeWait()
{
}

bool TcpStateTimeWait::syn_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::syn_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::syn_ack_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::syn_ack_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::ack_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::ack_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::data_seg_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::data_seg_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::fin_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::fin_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::rst_sent(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}

bool TcpStateTimeWait::rst_recv(TcpSegmentDescriptor& tsd, TcpStreamTracker& tracker)
{
    TcpTracker& trk = static_cast< TcpTracker& >( tracker );

    return default_state_action(tsd, trk, __func__);
}
