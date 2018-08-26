/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AuthenticationPackets.h"
 #include <utility>
#include "HmacHash.h"
#include "Util.h"
#include "RSA.h"
#include "CharacterData.h"

bool WorldPackets::Auth::EarlyProcessClientPacket::ReadNoThrow()
{
    try
    {
        Read();
        return true;
    }
    catch (ByteBufferPositionException const& /*ex*/)
    {
    }

    return false;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Auth::VirtualRealmNameInfo const& virtualRealmInfo)
{
    data.WriteBit(virtualRealmInfo.IsLocal);
    data.WriteBit(virtualRealmInfo.IsInternalRealm);
    data.WriteBits(virtualRealmInfo.RealmNameActual.length(), 8);
    data.WriteBits(virtualRealmInfo.RealmNameNormalized.length(), 8);
    data.FlushBits();

    data.WriteString(virtualRealmInfo.RealmNameActual);
    data.WriteString(virtualRealmInfo.RealmNameNormalized);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Auth::VirtualRealmInfo const& virtualRealmInfo)
{
    data << uint32(virtualRealmInfo.RealmAddress);
    data << virtualRealmInfo.RealmNameInfo;

    return data;
}

void WorldPackets::Auth::Ping::Read()
{
    _worldPacket >> Serial;
    _worldPacket >> Latency;
}

WorldPacket const* WorldPackets::Auth::AuthChallenge::Write()
{
    _worldPacket.append(DosChallenge, 8);
    _worldPacket.append(Challenge.data(), Challenge.size());
    _worldPacket << uint8(DosZeroBits);

    return &_worldPacket;
}

void WorldPackets::Auth::AuthSession::Read()
{
    _worldPacket >> DosResponse;
    _worldPacket >> Build;
    _worldPacket >> BuildType;
    _worldPacket >> RegionID;
    _worldPacket >> BattlegroupID;
    _worldPacket >> RealmID;
    _worldPacket.read(LocalChallenge.data(), LocalChallenge.size());
    _worldPacket.read(Digest.data(), Digest.size());
    UseIPv6 = _worldPacket.ReadBit();

    auto realmJoinTicketSize = _worldPacket.read<uint32>();
    if (realmJoinTicketSize)
    {
        RealmJoinTicket.resize(std::min(realmJoinTicketSize, uint32(_worldPacket.size() - _worldPacket.rpos())));
        _worldPacket.read(reinterpret_cast<uint8*>(&RealmJoinTicket[0]), RealmJoinTicket.size());
    }
}

WorldPackets::Auth::AuthResponse::CharcterTemplateClass::CharcterTemplateClass(uint8 factionGroup, uint8 classID) : FactionGroup(factionGroup), ClassID(classID)
{
}

WorldPackets::Auth::AuthResponse::CharacterTemplateItemStruct::CharacterTemplateItemStruct(uint32 itemID, uint32 count, uint8 classID, uint8 factionGroup) : ItemID{ itemID }, Count{ count }, ClassID{ classID }, FactionGroup{ factionGroup }
{
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Auth::AuthWaitInfo const& waitInfo)
{
    data << uint32(waitInfo.WaitCount);
    data << uint32(waitInfo.WaitTime);
    data.WriteBit(waitInfo.HasFCM);
    data.FlushBits();

    return data;
}

WorldPackets::Auth::AuthResponse::AuthResponse() : ServerPacket(SMSG_AUTH_RESPONSE, 132) { }


WorldPacket const* WorldPackets::Auth::AuthResponse::Write()
{
    _worldPacket << uint32(Result);
    _worldPacket.WriteBit(SuccessInfo.is_initialized());
    _worldPacket.WriteBit(WaitInfo.is_initialized());
    _worldPacket.FlushBits();

    if (SuccessInfo)
    {
        _worldPacket << uint32(SuccessInfo->VirtualRealmAddress);
        _worldPacket << uint32(SuccessInfo->VirtualRealms.size());
        _worldPacket << uint32(SuccessInfo->TimeRested);
        _worldPacket << uint8(SuccessInfo->ActiveExpansionLevel);
        _worldPacket << uint8(SuccessInfo->AccountExpansionLevel);
        _worldPacket << uint32(SuccessInfo->TimeSecondsUntilPCKick);
        _worldPacket << uint32(SuccessInfo->AvailableClasses->size());
        _worldPacket << uint32(SuccessInfo->Templates.size());
        _worldPacket << uint32(SuccessInfo->CurrencyID);
        _worldPacket << int32(SuccessInfo->Time);

        for (auto const& klass : *SuccessInfo->AvailableClasses)
        {
            _worldPacket << uint8(klass.first); /// the current class
            _worldPacket << uint8(klass.second); /// the required Expansion
        }

        _worldPacket.WriteBit(SuccessInfo->IsExpansionTrial);
        _worldPacket.WriteBit(SuccessInfo->ForceCharacterTemplate);
        _worldPacket.WriteBit(SuccessInfo->NumPlayersHorde.is_initialized());
        _worldPacket.WriteBit(SuccessInfo->NumPlayersAlliance.is_initialized());
        _worldPacket.FlushBits();

        _worldPacket << uint32(SuccessInfo->Billing.BillingPlan);
        _worldPacket << uint32(SuccessInfo->Billing.TimeRemain);
        _worldPacket.WriteBit(SuccessInfo->Billing.InGameRoom); // inGameRoom check in function checking which lua event to fire when remaining time is near end - BILLING_NAG_DIALOG vs IGR_BILLING_NAG_DIALOG
        _worldPacket.WriteBit(SuccessInfo->Billing.InGameRoom); // inGameRoom lua return from Script_GetBillingPlan
        _worldPacket.WriteBit(SuccessInfo->Billing.InGameRoom); // not used anywhere in the client
        _worldPacket.FlushBits();

        if (SuccessInfo->NumPlayersHorde)
            _worldPacket << uint16(*SuccessInfo->NumPlayersHorde);

        if (SuccessInfo->NumPlayersAlliance)
            _worldPacket << uint16(*SuccessInfo->NumPlayersAlliance);

        for (const auto& virtualRealm : SuccessInfo->VirtualRealms)
            _worldPacket << virtualRealm;

        for (auto const& templat : SuccessInfo->Templates)
        {
            _worldPacket << templat.TemplateSetID;
            _worldPacket << static_cast<uint32>(templat.Classes.size());
            for (auto const& templateClass : templat.Classes)
            {
                _worldPacket << templateClass.ClassID;
                _worldPacket << templateClass.FactionGroup;
            }

            _worldPacket.WriteBits(templat.Name.length(), 7);
            _worldPacket.WriteBits(templat.Description.length(), 10);
            _worldPacket.FlushBits();

            _worldPacket.WriteString(templat.Name);
            _worldPacket.WriteString(templat.Description);
        }
    }

    if (WaitInfo)
        _worldPacket << *WaitInfo;

    return &_worldPacket;
}


std::string const WorldPackets::Auth::ConnectTo::Haiku("An island of peace\nCorruption is brought ashore\nPandarens will rise\n\0\0", 71);

uint8 const WorldPackets::Auth::ConnectTo::PiDigits[130] =
{
    0x31, 0x41, 0x59, 0x26, 0x53, 0x58, 0x97, 0x93, 0x23, 0x84,
    0x62, 0x64, 0x33, 0x83, 0x27, 0x95, 0x02, 0x88, 0x41, 0x97,
    0x16, 0x93, 0x99, 0x37, 0x51, 0x05, 0x82, 0x09, 0x74, 0x94,
    0x45, 0x92, 0x30, 0x78, 0x16, 0x40, 0x62, 0x86, 0x20, 0x89,
    0x98, 0x62, 0x80, 0x34, 0x82, 0x53, 0x42, 0x11, 0x70, 0x67,
    0x98, 0x21, 0x48, 0x08, 0x65, 0x13, 0x28, 0x23, 0x06, 0x64,
    0x70, 0x93, 0x84, 0x46, 0x09, 0x55, 0x05, 0x82, 0x23, 0x17,
    0x25, 0x35, 0x94, 0x08, 0x12, 0x84, 0x81, 0x11, 0x74, 0x50,
    0x28, 0x41, 0x02, 0x70, 0x19, 0x38, 0x52, 0x11, 0x05, 0x55,
    0x96, 0x44, 0x62, 0x29, 0x48, 0x95, 0x49, 0x30, 0x38, 0x19,
    0x64, 0x42, 0x88, 0x10, 0x97, 0x56, 0x65, 0x93, 0x34, 0x46,
    0x12, 0x84, 0x75, 0x64, 0x82, 0x33, 0x78, 0x67, 0x83, 0x16,
    0x52, 0x71, 0x20, 0x19, 0x09, 0x14, 0x56, 0x48, 0x56, 0x69,
};

uint8 const P[] =
{
    0x7D, 0xBD, 0xB9, 0xE1, 0x2D, 0xAE, 0x42, 0x56, 0x6E, 0x2B, 0xE2, 0x89, 0xD9, 0xBB, 0x0C, 0x1F,
    0x67, 0x28, 0xC1, 0x4D, 0x91, 0x3C, 0xAD, 0x5F, 0xF0, 0x43, 0x86, 0x5C, 0x27, 0xDC, 0x58, 0xB3,
    0x0E, 0x75, 0x77, 0x78, 0x49, 0x35, 0xE7, 0xE7, 0xDF, 0xFD, 0x74, 0xAB, 0x4E, 0xFE, 0xD3, 0xAB,
    0x6B, 0x96, 0xF7, 0x89, 0xB2, 0x5A, 0x6A, 0x25, 0x03, 0x5A, 0x92, 0x1A, 0xF1, 0xFC, 0x05, 0x4E,
    0xCE, 0xDD, 0x37, 0xA4, 0x02, 0x53, 0x76, 0xCB, 0xC2, 0xD9, 0x63, 0xCB, 0x51, 0x94, 0xEC, 0x5C,
    0x39, 0xCC, 0xB2, 0x17, 0x0C, 0xA3, 0x43, 0x9A, 0xD0, 0x83, 0x27, 0x67, 0x52, 0x64, 0x37, 0x0E,
    0x38, 0xB7, 0x9B, 0xF4, 0x2D, 0xB8, 0x0F, 0x30, 0x72, 0xD3, 0x15, 0xF3, 0x2C, 0x39, 0x55, 0x72,
    0x2C, 0x55, 0x80, 0x63, 0xA0, 0xA1, 0x6F, 0x28, 0xF3, 0xF3, 0x5A, 0x6F, 0x68, 0x59, 0xB3, 0xF3
};

uint8 const Q[] =
{
    0x0B, 0x1A, 0x13, 0x07, 0x12, 0xEF, 0xDD, 0x97, 0x01, 0x9A, 0x21, 0x7D, 0xFA, 0xA3, 0xB7, 0xE2,
    0x39, 0x2E, 0x04, 0x92, 0x96, 0x45, 0x2A, 0xEB, 0x57, 0x03, 0xAC, 0xB1, 0x83, 0xCD, 0x25, 0x4F,
    0x2C, 0xA9, 0xA1, 0x54, 0x26, 0x54, 0xCF, 0xE6, 0x1B, 0x53, 0x51, 0x3A, 0xC1, 0x15, 0xF4, 0x17,
    0xBB, 0x17, 0x1F, 0x37, 0x66, 0x36, 0x1A, 0xD4, 0xB1, 0x5B, 0x49, 0xA8, 0xF1, 0x02, 0xB0, 0x42,
    0xA9, 0x66, 0xA0, 0xE2, 0x52, 0x2C, 0x8C, 0x89, 0xA2, 0xDD, 0xA6, 0xF1, 0xA3, 0xDF, 0xB6, 0x80,
    0x63, 0xB8, 0x10, 0xDA, 0xDE, 0x84, 0x56, 0xFA, 0xFB, 0x72, 0x65, 0x5E, 0xA3, 0x9C, 0x78, 0x65,
    0xD0, 0x73, 0x07, 0x34, 0x1D, 0xE1, 0x4D, 0x77, 0xE8, 0x00, 0x0F, 0x80, 0x1C, 0x5A, 0x21, 0x55,
    0x0A, 0x8C, 0xF4, 0x93, 0xF5, 0xF8, 0x40, 0xF2, 0x40, 0xEA, 0x52, 0x12, 0x40, 0xF0, 0xBF, 0xFA
};

uint8 const DP[] =
{
    0xE1, 0xA6, 0x22, 0xAB, 0xFF, 0x57, 0x83, 0x45, 0x3F, 0x93, 0x76, 0xC8, 0xFA, 0xD9, 0x17, 0xE1,
    0x49, 0x73, 0xC2, 0x13, 0x28, 0x0B, 0x1F, 0xE2, 0x9A, 0xF4, 0x7F, 0x7C, 0x37, 0x56, 0xA1, 0xDF,
    0x51, 0x97, 0x2F, 0x15, 0x10, 0x97, 0xCD, 0x2A, 0x40, 0x09, 0xFC, 0x0A, 0xC3, 0x3F, 0x88, 0x86,
    0xA9, 0x51, 0x13, 0xE1, 0x76, 0xCF, 0xA8, 0x37, 0x9A, 0x91, 0x3B, 0xD0, 0x70, 0xA1, 0xD7, 0x03,
    0x71, 0x59, 0x6C, 0xB3, 0x41, 0xB8, 0x32, 0x68, 0x56, 0xC8, 0xB8, 0xD1, 0xF9, 0x1D, 0x04, 0xC5,
    0x13, 0xB5, 0x8E, 0x57, 0x73, 0x02, 0x97, 0x7B, 0x33, 0x60, 0x68, 0xA9, 0xC2, 0x40, 0x96, 0x3C,
    0x57, 0x4E, 0x4F, 0xC0, 0xAB, 0x21, 0x5C, 0xBA, 0x7D, 0x65, 0xAA, 0x1B, 0xD6, 0x43, 0x06, 0xCE,
    0x3E, 0x0C, 0xB9, 0xB2, 0x82, 0xB0, 0xC9, 0x54, 0x59, 0x32, 0xC5, 0x88, 0x08, 0x9C, 0x9B, 0xBF
};

uint8 const DQ[] =
{
    0xE3, 0xB1, 0xED, 0x52, 0xEF, 0xE6, 0x88, 0x40, 0x50, 0x89, 0x4C, 0x99, 0xE5, 0xF7, 0xED, 0x03,
    0x1C, 0x54, 0x11, 0x24, 0x2F, 0x9D, 0xE8, 0xE6, 0x39, 0xFA, 0x19, 0xF4, 0x06, 0x55, 0x0B, 0x8B,
    0x95, 0xC8, 0xB1, 0xE2, 0x7C, 0x75, 0x3B, 0x2A, 0x40, 0xC3, 0xE7, 0xE0, 0x25, 0x18, 0xBF, 0xB5,
    0x03, 0x1B, 0x5A, 0x57, 0x92, 0x3C, 0x85, 0x7D, 0x7F, 0x43, 0x56, 0x1F, 0x1E, 0x80, 0xC3, 0xBA,
    0xF0, 0x53, 0xD7, 0x6A, 0xD0, 0xF2, 0xDD, 0x9C, 0xC6, 0x53, 0xE7, 0xB4, 0xD3, 0x9D, 0xAB, 0xBF,
    0xE0, 0x97, 0x50, 0x92, 0x23, 0xB9, 0xB7, 0xDC, 0xAA, 0xC4, 0x20, 0x93, 0x5A, 0xF5, 0xDE, 0x76,
    0x28, 0x93, 0x91, 0x44, 0x1E, 0x4C, 0x15, 0x2F, 0x7F, 0x45, 0x3C, 0x3B, 0x7D, 0x36, 0x3B, 0x24,
    0xC7, 0x8C, 0x65, 0x43, 0xAE, 0x65, 0x84, 0xBC, 0xF9, 0x76, 0x4E, 0x3C, 0x44, 0x05, 0xBC, 0xFA
};

uint8 const InverseQ[] =
{
    0x63, 0xC1, 0x14, 0x2B, 0x57, 0x0B, 0x8A, 0x3C, 0x27, 0xDB, 0x96, 0x82, 0x27, 0xEB, 0xF6, 0x45,
    0x6D, 0x07, 0x50, 0xE8, 0x4A, 0xD4, 0xB6, 0x7A, 0x3C, 0x8B, 0x4D, 0x65, 0xF0, 0x50, 0x70, 0x84,
    0x71, 0x2B, 0xC6, 0x6D, 0x28, 0x2D, 0x76, 0x38, 0x73, 0x93, 0xDB, 0x44, 0xD7, 0xC0, 0x7F, 0xD9,
    0x57, 0x18, 0x28, 0x57, 0xF1, 0x13, 0x38, 0xA4, 0x91, 0x67, 0x1E, 0x13, 0x73, 0x55, 0xFC, 0x7B,
    0xAF, 0x50, 0xFA, 0xFD, 0x16, 0x12, 0x6F, 0xA4, 0x95, 0x15, 0x9C, 0x07, 0x18, 0xA6, 0x46, 0xFD,
    0xB3, 0xCF, 0xA5, 0x0E, 0x05, 0x30, 0xEC, 0x2C, 0xCD, 0x62, 0xDD, 0x6F, 0xB1, 0xFE, 0x6C, 0x05,
    0x2F, 0x11, 0xA6, 0xA0, 0x98, 0xAC, 0x9B, 0x15, 0xF0, 0x04, 0xC4, 0x7B, 0x79, 0xAA, 0x51, 0x25,
    0x2A, 0x84, 0x73, 0xE6, 0x77, 0x47, 0xA3, 0xEB, 0xCF, 0x6D, 0xC8, 0x96, 0x3A, 0x1B, 0x02, 0x52
};

WorldPackets::Auth::ConnectTo::ConnectTo() : ServerPacket(SMSG_CONNECT_TO, 8 + 4 + 256 + 1)
{
    HexStrToByteArray("F41DCB2D728CF3337A4FF338FA89DB01BBBE9C3B65E9DA96268687353E48B94C", Payload.PanamaKey);
    Payload.Adler32 = 0xA0A66C10;
}

namespace
{
    std::string RSAPrivateKey = R"(-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA7rPc1NPDtFRRzmZbyzK48PeSU8YZ8gyFL4omqXpFn2DE683q
f41Z2FeyYHsJTJtouMft7x6ADeZrN1tTkOsYEw1/Q2SD2pjmrMIwooKlxsvH+4af
n6kCagNJxTj7wMhVzMDOJZG+hc/R0TfOzIPS6jCAB3uAn51EVCIpvoba20jFqfkT
NpUjdvEO3IQNlAISqJfzOxTuqm+YBSdOH6Ngpana2BffM8viE1SLGLDKubuIZAbf
dabXYQC7sFoOetR3CE0V4hCDsASqnot3qQaJXQhdD7gua8HLZM9uXNtPWGUIUfsN
SBpvtj0fC93+Gx3wv7Ana/WOvMdAAf+nC4DWXwIDAQABAoIBACKa5q/gB2Y0Nyvi
APrDXrZoXclRVd+WWxSaRaKaPE+vuryovI9DUbwgcpa0H5QAj70CFwdsd4oMVozO
6519x56zfTiq8MaXFhIDkQNuR1Q7pMFdMfT2jogJ8/7olO7M3EtzxC8EIwfJKhTX
r15M2h3jbBwplmsNZKOB1GVvrXjOm1KtOZ4CTTM0WrPaLVDT9ax8pykjmFw16vGP
j/R5Dky9VpabtfZOu/AEW259XDEiQgTrB4Eg+S4GJjHqAzPZBmMy/xhlDK4oMXef
qXScfD4w0RxuuCFr6lxLPZz0S35BK1kIWmIkuv+9eQuI4Hr1CyVwch4fkfvrp84x
8tvAFnkCgYEA87NZaG9a8/Mob6GgY4BVLHJVOSzzFdNyMA+4LfSbtzgON2RSZyeD
0JpDowwXssw5XOyUUctj2cLLdlMCpDfdzk4F/PEakloDJWpason3lmur0/5Oq3T9
3+fnNUl4d3UOs1jcJ1yGQ/BfrTyRTcEoZx8Mu9mJ4ituVkKuLeG5vX0CgYEA+r/w
QBJS6kDyQPj1k/SMClUhWhyADwDod03hHTQHc9BleJyjXmVy+/pWhN7aELhjgLbf
o/Gm3aKJjCxS4qBmqUKwAvGoSVux1Bo2ZjcfF7sX9BXBOlFTG+bPVCZUoaksTyXN
g7GsA1frKkWWkgQuOeK3o/p9IZoBl93vEgcTGgsCgYEAv5ucCIjFMllUybCCsrkM
Ps4GQ9YbqmV9ulwhq8BPTlc8lkDCqWhgM3uXAnNXjrUTxQQd+dG4yFZoMrhBs2xZ
cQPXoXDQO5GaN6jPduETUamGiD/DCvwJQCrNlxAVL5dR36FWN3x/9JriHwsoE8Jz
SeEX2frIdpM/RYNX/6sipuECgYEA+rwFRDxOdvm8hGWuQ2WMxyQ7Nn07PEV/LxVM
HkSRkyh23vVakyDEqty3uSOSUJfgv6ud07TnU8ac3fLQatdT8LrDgB4fVkN/fYU8
kldaGwO1vxgl4OfDQCo7dXzisciViwtVBvQZ+jnm6J0vJBFUHAPt9+WZTIlQQIjm
71LtseMCgYBSAhs6lshtz+ujR3fmc4QqJVGqeXvEBPAVm6yYoKYRLwVs/rFv3WLN
LOwwBQ6lz7P9RqYYB5wVlaRvEhb9+lCve/xVcxMeZ5GkOBPxVygYV9l/wNdE25Nz
OHYtKG3GK3GEcFDwZU2LPHq21EroUAdtRfbrJ4KW2yc8igtXKxTBYw==
-----END RSA PRIVATE KEY-----
)";

std::unique_ptr<Trinity::Crypto::RSA> ConnectToRSA;

uint8 const WherePacketHmac[] =
{
    0x2C, 0x1F, 0x1D, 0x80, 0xC3, 0x8C, 0x23, 0x64, 0xDA, 0x90, 0xCA, 0x8E, 0x2C, 0xFC, 0x0C, 0xCE,
    0x09, 0xD3, 0x62, 0xF9, 0xF3, 0x8B, 0xBE, 0x9F, 0x19, 0xEF, 0x58, 0xA1, 0x1C, 0x34, 0x14, 0x41,
    0x3F, 0x23, 0xFD, 0xD3, 0xE8, 0x14, 0xEC, 0x2A, 0xFD, 0x4F, 0x95, 0xBA, 0x30, 0x7E, 0x56, 0x5D,
    0x83, 0x95, 0x81, 0x69, 0xB0, 0x5A, 0xB4, 0x9D, 0xA8, 0x55, 0xFF, 0xFC, 0xEE, 0x58, 0x0A, 0x2F
};
}

bool WorldPackets::Auth::ConnectTo::InitializeEncryption()
{
    auto rsa = Trinity::make_unique<Trinity::Crypto::RSA>();
    if (!rsa->LoadFromString(RSAPrivateKey, Trinity::Crypto::RSA::PrivateKey{}))
        return false;

    ConnectToRSA = std::move(rsa);
    return true;
}


WorldPacket const* WorldPackets::Auth::ConnectTo::Write()
{
    HmacSha1 hmacHash(64, WherePacketHmac);
    hmacHash.UpdateData(Payload.Where.data(), 16);
    hmacHash.UpdateData(reinterpret_cast<uint8* const>(&Payload.Type), 1);
    hmacHash.UpdateData(reinterpret_cast<uint8* const>(&Payload.Port), 2);
    hmacHash.UpdateData((uint8* const)Haiku.c_str(), 71);
    hmacHash.UpdateData(Payload.PanamaKey, 32);
    hmacHash.UpdateData(PiDigits, 108);
    hmacHash.UpdateData(&Payload.XorMagic, 1);
    hmacHash.Finalize();

    ByteBuffer payload;
    payload << uint32(Payload.Adler32);
    payload << uint8(Payload.Type);
    payload.append(Payload.Where.data(), 16);
    payload << uint16(Payload.Port);
    payload.append(Haiku.data(), 71);
    payload.append(Payload.PanamaKey, 32);
    payload.append(PiDigits, 108);
    payload << uint8(Payload.XorMagic);
    payload.append(hmacHash.GetDigest(), hmacHash.GetLength());

    BigNumber bnData;
    bnData.SetBinary(payload.contents(), payload.size());

    BigNumber p;
    BigNumber q;
    BigNumber dmp1;
    BigNumber dmq1;
    BigNumber iqmp;

    p.SetBinary(P, 128);
    q.SetBinary(Q, 128);
    dmp1.SetBinary(DP, 128);
    dmq1.SetBinary(DQ, 128);
    iqmp.SetBinary(InverseQ, 128);

    BigNumber m1 = (bnData % p).ModExp(dmp1, p);
    BigNumber m2 = (bnData % q).ModExp(dmq1, q);
    BigNumber h = (iqmp * (m1 - m2)) % p;
    // Be sure to use the positive remainder
    if (h.IsNegative())
        h += p;

    BigNumber m = m2 + h * q;

    _worldPacket << uint64(Key);
    _worldPacket << uint32(Serial);
    _worldPacket.append(m.AsByteArray(256).get(), 256);
    _worldPacket << uint8(Con);

    return &_worldPacket;
}

void WorldPackets::Auth::AuthContinuedSession::Read()
{
    _worldPacket >> DosResponse;
    _worldPacket >> Key;
    _worldPacket.read(LocalChallenge.data(), LocalChallenge.size());
    _worldPacket.read(Digest.data(), Digest.size());
}

void WorldPackets::Auth::LogDisconnect::Read()
{
    _worldPacket >> Reason;
}

void WorldPackets::Auth::ConnectToFailed::Read()
{
    Serial = _worldPacket.read<ConnectToSerial>();
    _worldPacket >> Con;
}

WorldPacket const* WorldPackets::Auth::WaitQueueUpdate::Write()
{
    _worldPacket << WaitInfo;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Auth::BanReason::Write()
{
    _worldPacket << Reason;

    return &_worldPacket;
}

void WorldPackets::Auth::WardenData::Read()
{
    if (auto size = _worldPacket.read<uint32>())
    {
        Data.resize(size);
        _worldPacket.read(Data.contents(), size);
    }
}

