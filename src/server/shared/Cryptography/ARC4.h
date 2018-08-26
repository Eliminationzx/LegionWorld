/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#ifndef _AUTH_SARC4_H
#define _AUTH_SARC4_H

#include <openssl/evp.h>
#include "Define.h"

struct RC4_Context
{
    uint8 S[256] = {};
    uint8 x = 0;
    uint8 y = 0;
};

class ARC4
{
    public:
        ARC4(uint32 len);
        ARC4(uint8 *seed, uint32 len);
        ~ARC4();
        void Init(uint8 *seed);
        void UpdateData(int len, uint8 *data);

        static void rc4_init(RC4_Context * ctx, const uint8 * seed, int seedlen);
        static void rc4_process(RC4_Context * ctx, uint8 * data, int datalen);
    private:
        EVP_CIPHER_CTX m_ctx;
};

#endif
