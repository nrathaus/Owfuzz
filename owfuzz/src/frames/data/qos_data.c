/**************************************************************************
 * Copyright (C) 2020-2021 by Hongjian Cao <haimohk@gmail.com>
 * *
 * This file is part of owfuzz.
 * *
 * Owfuzz is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * *
 * Owfuzz is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * *
 * You should have received a copy of the GNU General Public License
 * along with owfuzz.  If not, see <https://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "qos_data.h"
#include "common/wpa_common.h"
#include "crypto/aes.h"
#include "common/eapol_common.h"
#include "eap_common/eap_defs.h"

extern unsigned int seed;
extern fuzzing_option fuzzing_opt;

struct packet create_qos_data(struct ether_addr bssid, struct ether_addr smac, struct ether_addr dmac, char adhoc, struct packet *recv_pkt)
{
    struct packet data = {0};
    // struct ieee_hdr *hdr;
    struct ieee_hdr *hdr_new;
    struct llc_hdr *llc_h, llc;
    struct ieee802_1x_hdr *ieee8021x_hdr, ieee8021xdhr;
    struct eap_hdr *eaphdr, eap;
    struct ieee8021x_auth *wpa_auth;
    uint8_t eap_type = EAP_TYPE_NONE;
    struct wep_param wp = {0};
    char dsflag = 'a';
    int dlen;

    if (fuzzing_opt.fuzz_work_mode == FUZZ_WORK_MODE_AP)
        dsflag = 'f';
    else if (fuzzing_opt.fuzz_work_mode == FUZZ_WORK_MODE_STA)
        dsflag = 't';

    create_ieee_hdr(&data, IEEE80211_TYPE_QOSDATA, dsflag, 0x013A, dmac, smac, bssid, SE_NULLMAC, 0);
    hdr_new = (struct ieee_hdr *)data.data;

    if (recv_pkt)
    {
        // hdr = (struct ieee_hdr *) recv_pkt->data;

        if (fuzzing_opt.auth_type >= WPA_PSK_TKIP)
        {
            llc_h = (struct llc_hdr *)(recv_pkt->data + sizeof(struct ieee_hdr) + 2);
            if (llc_h->type == htons(0x888e) && llc_h->ssap == 0xaa && llc_h->dsap == 0xaa)
            {
                ieee8021x_hdr = (struct ieee802_1x_hdr *)(recv_pkt->data + sizeof(struct ieee_hdr) + 2 + sizeof(struct llc_hdr));
                if (ieee8021x_hdr->type == 0x03) // key; m2,m3,m4
                {
                    wpa_auth = (struct ieee8021x_auth *)(recv_pkt->data + sizeof(struct ieee_hdr) + 2 + sizeof(struct llc_hdr));
                    if (fuzzing_opt.wpa_s == WPA_4WAY_HANDSHAKE)
                    {
                        if (wpa_auth->version == 0x01 || wpa_auth->version == 0x02)
                        {
                            // fuzz_logger_log(FUZZ_LOG_INFO, "data 4way_handshake ......");
                            if (strcmp(fuzzing_opt.mode, AP_MODE) == 0)
                            {
                                print_interaction_status(bssid, dmac, smac, "M2", "M3");
                                create_eapol_m3(&data);
                            }
                            else if (strcmp(fuzzing_opt.mode, STA_MODE) == 0)
                            {
                                print_interaction_status(bssid, dmac, smac, "M3", "M4");
                                create_eapol_m4(&data);
                            }

                            fuzzing_opt.wpa_s = WPA_COMPLETED;
                        }
                    }
                    else if (fuzzing_opt.wpa_s == WPA_ASSOCIATED)
                    {
                        if (strcmp(fuzzing_opt.mode, STA_MODE) == 0)
                        {
                            print_interaction_status(bssid, dmac, smac, "M1", "M2");
                            create_eapol_m2(&data);
                        }
                    }
                    else
                    {
                        memset(data.data + data.len, random() % (0xff + 1), 128);
                        data.len += 128;
                    }
                }
                else if (ieee8021x_hdr->type == 0x00 || ieee8021x_hdr->type == 0x01 || ieee8021x_hdr->type == 0x02) // eap packet, start, logoff
                {
                    print_interaction_status(bssid, dmac, smac, "EAP", "EAP");

                    if (recv_pkt->len > (sizeof(struct ieee_hdr) + 2 + sizeof(struct llc_hdr) + sizeof(struct ieee802_1x_hdr)))
                    {
                        eaphdr = (struct eap_hdr *)(recv_pkt->data + sizeof(struct ieee_hdr) + 2 + sizeof(struct llc_hdr) + sizeof(struct ieee802_1x_hdr));
                        if (recv_pkt->len > (sizeof(struct ieee_hdr) + 2 + sizeof(struct llc_hdr) + sizeof(struct ieee802_1x_hdr) + sizeof(struct eap_hdr)))
                            eap_type = *(uint8_t *)((uint8_t *)eaphdr + 1);
                        else
                            eap_type = random() % 0xFF;
                    }
                    else
                    {
                        eaphdr = &eap;
                        eaphdr->identifier = random() % 0xFF;
                        eap_type = random() % 0xFF;
                    }

                    memcpy(data.data + data.len, llc_h, sizeof(struct llc_hdr));
                    data.len += sizeof(struct llc_hdr);

                    // ieee8021x_hdr->version = 0x00;
                    ieee8021x_hdr->type = 0x00;
                    if (0 == seed)
                    {
                        srandom(time(NULL));
                    }
                    ieee8021x_hdr->length = htons(random() % 512);
                    memcpy(data.data + data.len, ieee8021x_hdr, sizeof(struct ieee802_1x_hdr));
                    data.len += sizeof(struct ieee802_1x_hdr);

                    if (0 == seed)
                    {
                        srandom(time(NULL) + ieee8021x_hdr->length);
                    }
                    eaphdr->code = random() % 6 + 1;
                    // eaphdr->identifier = 0x00;
                    if (0 == seed)
                    {
                        srandom(time(NULL) + eaphdr->code);
                    }
                    eaphdr->length = htons(random() % 512);
                    memcpy(data.data + data.len, eaphdr, sizeof(struct eap_hdr));
                    data.len += sizeof(struct eap_hdr);

                    // type
                    data.data[data.len] = eap_type;
                    data.len += 1;
                    // type data
                    if (0 == seed)
                    {
                        srandom(time(NULL) + eap_type);
                    }
                    dlen = random() % 1024;
                    generate_random_data(data.data + data.len, dlen, VALUE_RANDOM);
                    data.len += dlen;

                    if (eaphdr->code == EAP_CODE_REQUEST)
                    {
                        switch (eap_type)
                        {
                        case EAP_TYPE_IDENTITY:
                            break;
                        case EAP_TYPE_NOTIFICATION:
                            break;
                        case EAP_TYPE_NAK:
                            break;
                        case EAP_TYPE_MD5:
                            break;
                        case EAP_TYPE_OTP:
                            break;
                        case EAP_TYPE_GTC:
                            break;
                        case EAP_TYPE_TLS:
                            break;
                        case EAP_TYPE_LEAP:
                            break;
                        case EAP_TYPE_SIM:
                            break;
                        case EAP_TYPE_TTLS:
                            break;
                        case EAP_TYPE_AKA:
                            break;
                        case EAP_TYPE_PEAP:
                            break;
                        case EAP_TYPE_MSCHAPV2:
                            break;
                        case EAP_TYPE_TLV:
                            break;
                        case EAP_TYPE_TNC:
                            break;
                        case EAP_TYPE_FAST:
                            break;
                        case EAP_TYPE_PAX:
                            break;
                        case EAP_TYPE_PSK:
                            break;
                        case EAP_TYPE_SAKE:
                            break;
                        case EAP_TYPE_IKEV2:
                            break;
                        case EAP_TYPE_AKA_PRIME:
                            break;
                        case EAP_TYPE_GPSK:
                            break;
                        case EAP_TYPE_PWD:
                            break;
                        case EAP_TYPE_EKE:
                            break;
                        case EAP_TYPE_TEAP:
                            break;
                        case EAP_TYPE_EXPANDED:
                            break;
                        }
                    }
                    else if (eaphdr->code == EAP_CODE_RESPONSE)
                    {
                    }
                    else if (eaphdr->code == EAP_CODE_SUCCESS)
                    {
                    }
                    else if (eaphdr->code == EAP_CODE_FAILURE)
                    {
                    }
                    else if (eaphdr->code == EAP_CODE_INITIATE)
                    {
                    }
                    else if (eaphdr->code == EAP_CODE_FINISH)
                    {
                    }
                }
            }
        }
        else if (fuzzing_opt.auth_type == SHARE_WEP)
        {
            memset(wp.init_vector, random() % (0xff + 1), 3);
            wp.key_index = 0;
            memcpy(data.data, &wp, sizeof(wp));
            data.len += sizeof(wp);

            hdr_new->flags |= 0x40;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            dlen = random() % 1024;
            generate_random_data(data.data + data.len, dlen, VALUE_RANDOM);
            data.len += dlen;
        }
        else if (fuzzing_opt.auth_type == OPEN_WEP)
        {
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            dlen = random() % 1024;
            generate_random_data(data.data + data.len, dlen, VALUE_RANDOM);
            data.len += dlen;
        }
        else
        {
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            dlen = random() % 1024;
            generate_random_data(data.data + data.len, dlen, VALUE_RANDOM);
            data.len += dlen;
        }
    }
    else
    {
        if (fuzzing_opt.wpa_s == WPA_4WAY_HANDSHAKE) // m1
        {

            if (strcmp(fuzzing_opt.mode, AP_MODE) == 0)
            {
                print_interaction_status(bssid, dmac, smac, "", "M1");
                create_eapol_m1(&data);
            }
        }
        else if (fuzzing_opt.wpa_s == WPA_EAP_HANDSHAKE)
        {
            print_interaction_status(bssid, dmac, smac, "", "EAP");

            llc.dsap = 0xaa;
            llc.ssap = 0xaa;
            llc.ctrl = 0x03;
            llc.oui[0] = 0x00;
            llc.oui[1] = 0x00;
            llc.oui[2] = 0x00;
            llc.type = htons(0x888e);
            memcpy(data.data + data.len, &llc, sizeof(struct llc_hdr));
            data.len += sizeof(struct llc_hdr);

            ieee8021xdhr.version = 0x02;
            ieee8021xdhr.type = 0x00;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            ieee8021xdhr.length = htons(5);
            memcpy(data.data + data.len, &ieee8021xdhr, sizeof(struct ieee802_1x_hdr));
            data.len += sizeof(struct ieee802_1x_hdr);

            eap.code = EAP_CODE_REQUEST;
            eap.identifier = random() % 0xFF;
            if (0 == seed)
            {
                srandom(time(NULL) + eap.identifier);
            }
            eap.length = htons(5);
            memcpy(data.data + data.len, &eap, sizeof(struct eap_hdr));
            data.len += sizeof(struct eap_hdr);

            data.data[data.len] = 1;
            data.len += 1;
        }
        else
        {
            if (fuzzing_opt.auth_type == SHARE_WEP)
            {
                memset(wp.init_vector, random() % (0xff + 1), 3);
                wp.key_index = 0;
                memcpy(data.data, &wp, sizeof(wp));
                data.len += sizeof(wp);

                hdr_new->flags |= 0x40;
            }
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            dlen = random() % 1024;
            generate_random_data(data.data + data.len, dlen, VALUE_RANDOM);
            data.len += dlen;
        }
    }
    return data;
}

void create_eapol_m1(struct packet *pkt)
{
    struct llc_hdr llc_h = {0};
    struct ieee8021x_auth wpa_auth = {0};

    if (fuzzing_opt.auth_type >= WPA_PSK_TKIP)
    {
        llc_h.dsap = 0xaa;
        llc_h.ssap = 0xaa;
        llc_h.ctrl = 0x03;
        llc_h.oui[0] = 0x00;
        llc_h.oui[1] = 0x00;
        llc_h.oui[2] = 0x00;
        llc_h.type = htons(0x888e);

        memcpy(pkt->data + pkt->len, &llc_h, sizeof(struct llc_hdr));
        pkt->len += sizeof(struct llc_hdr);

        generate_random_data(wpa_auth.nonce, sizeof(wpa_auth.nonce), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_iv, sizeof(wpa_auth.key_iv), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_rsc, sizeof(wpa_auth.key_rsc), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_id, sizeof(wpa_auth.key_id), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_mic, sizeof(wpa_auth.key_mic), VALUE_RANDOM);

        switch (fuzzing_opt.auth_type)
        {
        case WPA_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0089);
            wpa_auth.key_length = htons(32);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x008a);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x008a);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv_auth.key_length = htons(0);
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_TKIP:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x0089);
            wpa_auth.key_length = htons(32);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_AES:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x008a);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_TKIP_AES:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x008a);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA3:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x0088);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(22);
            break;
        default:
            break;
        }

        memcpy(pkt->data + pkt->len, &wpa_auth, sizeof(struct ieee8021x_auth));
        pkt->len += sizeof(struct ieee8021x_auth);
        pkt->len += ntohs(wpa_auth.wpa_length);
    }

    fuzz_logger_log(FUZZ_LOG_DEBUG, "WPA_4WAY_HANDSHAKE message 1 testing");
}

void create_eapol_m2(struct packet *pkt)
{
    struct llc_hdr llc_h = {0};
    struct ieee8021x_auth wpa_auth = {0};

    if (fuzzing_opt.auth_type >= WPA_PSK_TKIP)
    {
        llc_h.dsap = 0xaa;
        llc_h.ssap = 0xaa;
        llc_h.ctrl = 0x03;
        llc_h.oui[0] = 0x00;
        llc_h.oui[1] = 0x00;
        llc_h.oui[2] = 0x00;
        llc_h.type = htons(0x888e);

        memcpy(pkt->data + pkt->len, &llc_h, sizeof(struct llc_hdr));
        pkt->len += sizeof(struct llc_hdr);

        generate_random_data(wpa_auth.nonce, sizeof(wpa_auth.nonce), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_iv, sizeof(wpa_auth.key_iv), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_rsc, sizeof(wpa_auth.key_rsc), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_id, sizeof(wpa_auth.key_id), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_mic, sizeof(wpa_auth.key_mic), VALUE_RANDOM);

        switch (fuzzing_opt.auth_type)
        {
        case WPA_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0109);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(26);
            break;
        case WPA_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(26);
            break;
        case WPA2_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0109);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(24);
            break;
        case WPA2_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(24);
            break;
        case WPA2_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(0);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(24);
            break;
        case WPA3:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0108);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(28);
            break;
        default:
            break;
        }

        memcpy(pkt->data + pkt->len, &wpa_auth, sizeof(struct ieee8021x_auth));
        pkt->len += sizeof(struct ieee8021x_auth);
        pkt->len += ntohs(wpa_auth.wpa_length);
    }

    fuzz_logger_log(FUZZ_LOG_DEBUG, "WPA_4WAY_HANDSHAKE message 2 testing");
}

void create_eapol_m3(struct packet *pkt)
{
    struct llc_hdr llc_h = {0};
    struct ieee8021x_auth wpa_auth = {0};

    if (0 == seed)
    {
        srandom(time(NULL));
    }

    if (fuzzing_opt.auth_type >= WPA_PSK_TKIP)
    {
        llc_h.dsap = 0xaa;
        llc_h.ssap = 0xaa;
        llc_h.ctrl = 0x03;
        llc_h.oui[0] = 0x00;
        llc_h.oui[1] = 0x00;
        llc_h.oui[2] = 0x00;
        llc_h.type = htons(0x888e);

        memcpy(pkt->data + pkt->len, &llc_h, sizeof(struct llc_hdr));
        pkt->len += sizeof(struct llc_hdr);

        generate_random_data(wpa_auth.nonce, sizeof(wpa_auth.nonce), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_iv, sizeof(wpa_auth.key_iv), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_rsc, sizeof(wpa_auth.key_rsc), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_id, sizeof(wpa_auth.key_id), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_mic, sizeof(wpa_auth.key_mic), VALUE_RANDOM);

        switch (fuzzing_opt.auth_type)
        {
        case WPA_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x01c9);
            wpa_auth.key_length = htons(32);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(26);
            break;
        case WPA_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x01ca);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(26);
        case WPA_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x01ca);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(30);
            break;
        case WPA2_PSK_TKIP:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x13c9);
            wpa_auth.key_length = htons(32);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(62);
            break;
        case WPA2_PSK_AES:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x13ca);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(56);
            break;
        case WPA2_PSK_TKIP_AES:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x13ca);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(80);
            break;
        case WPA3:
            wpa_auth.version = 0x02; // 802.1X-2004
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL RSN Key
            wpa_auth.key_info = htons(0x13c8);
            wpa_auth.key_length = htons(16);
            wpa_auth.replay_counter = htonll(2);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(88);
            break;
        default:
            break;
        }

        memcpy(pkt->data + pkt->len, &wpa_auth, sizeof(struct ieee8021x_auth));
        pkt->len += sizeof(struct ieee8021x_auth);
        memset(pkt->data + pkt->len, 'A', ntohs(wpa_auth.wpa_length));
        pkt->len += ntohs(wpa_auth.wpa_length);
    }

    fuzz_logger_log(FUZZ_LOG_DEBUG, "WPA_4WAY_HANDSHAKE message 3 testing");
}

void create_eapol_m4(struct packet *pkt)
{
    struct llc_hdr llc_h = {0};
    struct ieee8021x_auth wpa_auth = {0};

    if (fuzzing_opt.auth_type >= WPA_PSK_TKIP)
    {
        llc_h.dsap = 0xaa;
        llc_h.ssap = 0xaa;
        llc_h.ctrl = 0x03;
        llc_h.oui[0] = 0x00;
        llc_h.oui[1] = 0x00;
        llc_h.oui[2] = 0x00;
        llc_h.type = htons(0x888e);

        memcpy(pkt->data + pkt->len, &llc_h, sizeof(struct llc_hdr));
        pkt->len += sizeof(struct llc_hdr);

        generate_random_data(wpa_auth.nonce, sizeof(wpa_auth.nonce), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_iv, sizeof(wpa_auth.key_iv), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_rsc, sizeof(wpa_auth.key_rsc), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_id, sizeof(wpa_auth.key_id), VALUE_RANDOM);
        generate_random_data(wpa_auth.key_mic, sizeof(wpa_auth.key_mic), VALUE_RANDOM);

        switch (fuzzing_opt.auth_type)
        {
        case WPA_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0109);
            wpa_auth.key_length = htons(32);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.a_length = htons(0); 1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0xfe; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x010a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_TKIP:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0309);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x030a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA2_PSK_TKIP_AES:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x030a);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(1);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        case WPA3:
            wpa_auth.version = 0x01; // 802.1X-2001
            wpa_auth.type = 0x03;
            if (0 == seed)
            {
                srandom(time(NULL));
            }
            wpa_auth.length = htons(random() % 1024);
            wpa_auth.descriptor = 0x02; // EAPOL WPA Key
            wpa_auth.key_info = htons(0x0308);
            wpa_auth.key_length = htons(0);
            wpa_auth.replay_counter = htonll(2);
            // wpa_auth.nonce
            // wpa_auth.key_iv
            // wpa_auth.key_rsc
            // wpa_auth.key_id
            // wpa_auth.key_mic
            wpa_auth.wpa_length = htons(0);
            break;
        default:
            break;
        }

        memcpy(pkt->data + pkt->len, &wpa_auth, sizeof(struct ieee8021x_auth));
        pkt->len += sizeof(struct ieee8021x_auth);
        pkt->len += ntohs(wpa_auth.wpa_length);
    }

    fuzz_logger_log(FUZZ_LOG_DEBUG, "WPA_4WAY_HANDSHAKE message 4 testing");
}