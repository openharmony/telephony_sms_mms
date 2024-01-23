/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gsm_sms_param_decode.h"

#include "gsm_pdu_hex_value.h"
#include "gsm_sms_common_utils.h"
#include "gsm_user_data_pdu.h"
#include "securec.h"
#include "telephony_log_wrapper.h"
#include "text_coder.h"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t SLIDE_DATA_STEP = 2;
static constexpr uint8_t BCD_TO_DIGITAL = 2;

bool GsmSmsParamDecode::DecodeAddressPdu(SmsReadBuffer &buffer, struct AddressNumber *resultNum)
{
    if (resultNum == nullptr) {
        TELEPHONY_LOGE("nullptr error");
        return false;
    }
    if (memset_s(resultNum->address, sizeof(resultNum->address), 0x00, sizeof(resultNum->address)) != EOK) {
        TELEPHONY_LOGE("memset_s error!");
        return false;
    }

    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte) || oneByte >= BCD_TO_DIGITAL * (MAX_ADDRESS_LEN + 1)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    uint8_t addrLen = oneByte;
    uint8_t bcdLen = 0;
    if (addrLen % SLIDE_DATA_STEP == 0) {
        bcdLen = addrLen / SLIDE_DATA_STEP;
    } else {
        bcdLen = addrLen / SLIDE_DATA_STEP + 1;
    }

    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    resultNum->ton = (oneByte & HEX_VALUE_70) >> HEX_VALUE_04;

    if (resultNum->ton == TYPE_ALPHA_NUMERIC) {
        return DecodeAddressAlphaNum(buffer, resultNum, bcdLen, addrLen);
    } else if (resultNum->ton == TYPE_INTERNATIONAL) {
        return DecodeAddressInternationalNum(buffer, resultNum, bcdLen);
    } else {
        return DecodeAddressDefaultNum(buffer, resultNum, bcdLen);
    }
}

bool GsmSmsParamDecode::DecodeAddressAlphaNum(
    SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen, uint8_t addrLen)
{
    uint8_t tmresultNum[MAX_ADDRESS_LEN] = { 0 };
    uint8_t tmplength = 0;
    GsmSmsCommonUtils utils;
    uint8_t dataLen = (addrLen * HEX_VALUE_04) / HEX_VALUE_07;
    if (!utils.Unpack7bitChar(buffer, dataLen, 0x00, tmresultNum, MAX_ADDRESS_LEN, tmplength)) {
        TELEPHONY_LOGE("unpack 7bit char error!");
        return false;
    }
    MsgLangInfo langInfo;
    langInfo.bSingleShift = false;
    langInfo.bLockingShift = false;
    TextCoder::Instance().Gsm7bitToUtf8(
        reinterpret_cast<uint8_t *>(resultNum->address), MAX_ADDRESS_LEN, tmresultNum, tmplength, langInfo);
    return true;
}

bool GsmSmsParamDecode::DecodeAddressInternationalNum(
    SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen)
{
    buffer.MoveForward(1);
    uint8_t oneByte = 0;
    uint8_t index = 0;
    uint8_t bcdArr[MAX_ADDRESS_LEN + 1] = { 0 };
    while (index < bcdLen && index < MAX_ADDRESS_LEN) {
        if (!buffer.ReadByte(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        bcdArr[index++] = oneByte;
    }

    GsmSmsCommonUtils utils;
    std::string addrNum;
    if (!utils.BcdToDigit(bcdArr, bcdLen, addrNum, SMS_MAX_ADDRESS_LEN)) {
        TELEPHONY_LOGE("BcdToDigit fail!");
        return false;
    }
    for (uint8_t i = 0; i < addrNum.size() && i < MAX_ADDRESS_LEN; i++) {
        resultNum->address[i + 1] = addrNum[i];
    }

    if (resultNum->address[1] != '\0') {
        resultNum->address[0] = '+';
    }
    return true;
}

bool GsmSmsParamDecode::DecodeAddressDefaultNum(SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen)
{
    uint8_t index = 0;
    buffer.MoveForward(1);
    uint8_t oneByte = 0;
    uint8_t bcdArr[MAX_ADDRESS_LEN + 1] = { 0 };
    while (index < bcdLen && index < MAX_ADDRESS_LEN) {
        if (!buffer.ReadByte(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        bcdArr[index++] = oneByte;
    }
    GsmSmsCommonUtils utils;
    std::string addrNum;
    if (!utils.BcdToDigit(bcdArr, bcdLen, addrNum, SMS_MAX_ADDRESS_LEN)) {
        TELEPHONY_LOGE("BcdToDigit fail!");
        return false;
    }
    for (uint8_t i = 0; i < addrNum.size() && i < MAX_ADDRESS_LEN; i++) {
        resultNum->address[i] = addrNum[i];
    }
    return true;
}

void GsmSmsParamDecode::DecodeSmscPdu(uint8_t *srcAddr, uint8_t addrLen, enum TypeOfNum ton, std::string &desAddr)
{
    if (srcAddr == nullptr || addrLen == 0) {
        TELEPHONY_LOGE("smsc data error.");
        return;
    }

    GsmSmsCommonUtils utils;
    if (ton == TYPE_INTERNATIONAL) {
        desAddr[0] = '+';
        if (!utils.BcdToDigit(srcAddr, addrLen, desAddr, SMS_MAX_ADDRESS_LEN)) {
            TELEPHONY_LOGE("BcdToDigit fail!");
            return;
        }
    } else {
        if (!utils.BcdToDigit(srcAddr, addrLen, desAddr, SMS_MAX_ADDRESS_LEN)) {
            TELEPHONY_LOGE("BcdToDigit fail!");
            return;
        }
    }
}

uint8_t GsmSmsParamDecode::DecodeSmscPdu(const uint8_t *pTpdu, uint8_t pduLen, struct AddressNumber &desAddrObj)
{
    if (pTpdu == nullptr || pduLen == 0) {
        TELEPHONY_LOGE("nullptr error.");
        return 0;
    }
    if (memset_s(desAddrObj.address, sizeof(desAddrObj.address), 0x00, sizeof(desAddrObj.address)) != EOK) {
        TELEPHONY_LOGE("memset_s error!");
        return 0;
    }

    uint8_t offset = 0;
    uint8_t addrLen = pTpdu[offset++];
    if (addrLen == 0) {
        TELEPHONY_LOGI("smsc is 00.");
        return offset;
    }
    if (addrLen >= pduLen || offset >= pduLen) {
        TELEPHONY_LOGI("smsc absent.");
        return 0;
    }
    desAddrObj.ton = (pTpdu[offset] & HEX_VALUE_70) >> HEX_VALUE_04;
    desAddrObj.npi = pTpdu[offset++] & HEX_VALUE_0F;

    GsmSmsCommonUtils utils;
    if (desAddrObj.ton == TYPE_INTERNATIONAL) {
        if (addrLen > SMS_MAX_ADDRESS_LEN || (offset + addrLen) >= pduLen) {
            TELEPHONY_LOGE("addrLen invalid.");
            return 0;
        }
        std::string addrNum;
        utils.BcdToDigit(&(pTpdu[offset]), addrLen, addrNum, SMS_MAX_ADDRESS_LEN);
        for (uint8_t i = 0; i < addrNum.size() && i < MAX_ADDRESS_LEN; i++) {
            desAddrObj.address[i + 1] = addrNum[i];
        }
        if (desAddrObj.address[1] != '\0') {
            desAddrObj.address[0] = '+';
        }
    } else {
        if (addrLen > SMS_MAX_ADDRESS_LEN || (offset + addrLen) >= pduLen) {
            TELEPHONY_LOGE("addrLen invalid.");
            return 0;
        }
        std::string addrNum;
        utils.BcdToDigit(&(pTpdu[offset]), addrLen, addrNum, SMS_MAX_ADDRESS_LEN);
        for (uint8_t i = 0; i < addrNum.size() && i < MAX_ADDRESS_LEN; i++) {
            desAddrObj.address[i] = addrNum[i];
        }
    }

    offset += (addrLen - 1);
    return offset;
}

bool GsmSmsParamDecode::DecodeDcsPdu(SmsReadBuffer &buffer, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs nullptr.");
        return false;
    }
    uint8_t dcs = 0;
    if (!buffer.ReadByte(dcs)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    smsDcs->bMWI = false;
    smsDcs->bIndActive = false;
    smsDcs->indType = SMS_OTHER_INDICATOR;
    if (((dcs & HEX_VALUE_C0) >> HEX_VALUE_06) == 0) {
        DecodeDcsGeneralGroupPdu(dcs, smsDcs);
    } else if (((dcs & HEX_VALUE_F0) >> HEX_VALUE_04) == HEX_VALUE_0F) {
        DecodeDcsClassGroupPdu(dcs, smsDcs);
    } else if (((dcs & HEX_VALUE_C0) >> HEX_VALUE_06) == HEX_VALUE_01) {
        DecodeDcsDeleteGroupPdu(dcs, smsDcs);
    } else if (((dcs & HEX_VALUE_F0) >> HEX_VALUE_04) == HEX_VALUE_0C) {
        DecodeDcsDiscardGroupPdu(dcs, smsDcs);
    } else if (((dcs & HEX_VALUE_F0) >> HEX_VALUE_04) == HEX_VALUE_0D) {
        DecodeDcsStoreGsmGroupPdu(dcs, smsDcs);
    } else if (((dcs & HEX_VALUE_F0) >> HEX_VALUE_04) == HEX_VALUE_0E) {
        DecodeDcsStoreUCS2GroupPdu(dcs, smsDcs);
    } else {
        DecodeDcsUnknownGroupPdu(dcs, smsDcs);
    }
    return true;
}

void GsmSmsParamDecode::DecodeDcsGeneralGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is nullptr.");
        return;
    }

    smsDcs->codingGroup = CODING_GENERAL_GROUP;
    smsDcs->bCompressed = (dcs & HEX_VALUE_20) >> HEX_VALUE_05;
    smsDcs->codingScheme = GetMsgCodingScheme((dcs & HEX_VALUE_0C) >> HEX_VALUE_02);
    if (((dcs & HEX_VALUE_10) >> HEX_VALUE_04) == 0) {
        smsDcs->msgClass = SMS_CLASS_UNKNOWN;
    } else {
        smsDcs->msgClass = GetMsgClass(dcs & HEX_VALUE_03);
    }
}

void GsmSmsParamDecode::DecodeDcsClassGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is nullptr.");
        return;
    }

    smsDcs->codingGroup = SMS_CLASS_GROUP;
    smsDcs->bCompressed = false;
    smsDcs->codingScheme = GetMsgCodingScheme((dcs & HEX_VALUE_0C) >> HEX_VALUE_02);
    smsDcs->msgClass = GetMsgClass(dcs & HEX_VALUE_03);
}

void GsmSmsParamDecode::DecodeDcsDeleteGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is nullptr.");
        return;
    }

    smsDcs->codingGroup = CODING_DELETION_GROUP;
    smsDcs->bCompressed = false;
    smsDcs->msgClass = SMS_CLASS_UNKNOWN;
}

void GsmSmsParamDecode::DecodeDcsDiscardGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is null.");
        return;
    }

    smsDcs->codingGroup = CODING_DISCARD_GROUP;
    GetMwiType(dcs, *smsDcs);
}

void GsmSmsParamDecode::DecodeDcsStoreGsmGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is null.");
        return;
    }

    smsDcs->codingGroup = CODING_STORE_GROUP;
    smsDcs->codingScheme = DATA_CODING_7BIT;
    GetMwiType(dcs, *smsDcs);
}

void GsmSmsParamDecode::DecodeDcsStoreUCS2GroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is null.");
        return;
    }

    smsDcs->codingGroup = CODING_STORE_GROUP;
    smsDcs->codingScheme = DATA_CODING_UCS2;
    GetMwiType(dcs, *smsDcs);
}

void GsmSmsParamDecode::DecodeDcsUnknownGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs)
{
    if (smsDcs == nullptr) {
        TELEPHONY_LOGE("smsDcs is null.");
        return;
    }

    smsDcs->codingGroup = CODING_UNKNOWN_GROUP;
    smsDcs->bCompressed = (dcs & HEX_VALUE_20) >> HEX_VALUE_05;
    smsDcs->codingScheme = GetMsgCodingScheme((dcs & HEX_VALUE_0C) >> HEX_VALUE_02);
    smsDcs->msgClass = SMS_CLASS_UNKNOWN;
}

enum SmsMessageClass GsmSmsParamDecode::GetMsgClass(uint8_t dcs)
{
    return (enum SmsMessageClass)(dcs & HEX_VALUE_03);
}

enum DataCodingScheme GsmSmsParamDecode::GetMsgCodingScheme(uint8_t dcs)
{
    return (enum DataCodingScheme)(dcs & HEX_VALUE_03);
}

enum SmsIndicatorType GsmSmsParamDecode::GetMsgIndicatorType(const uint8_t dcs)
{
    return (enum SmsIndicatorType)(dcs & HEX_VALUE_03);
}

void GsmSmsParamDecode::GetMwiType(const uint8_t dcs, struct SmsDcs &smsDcs)
{
    smsDcs.bCompressed = false;
    smsDcs.msgClass = SMS_CLASS_UNKNOWN;
    smsDcs.bMWI = true;
    smsDcs.bIndActive = (((dcs & HEX_VALUE_08) >> HEX_VALUE_03) == HEX_VALUE_01) ? true : false;
    smsDcs.indType = GetMsgIndicatorType(dcs & HEX_VALUE_03);
}

bool GsmSmsParamDecode::DecodeTimePdu(SmsReadBuffer &buffer, struct SmsTimeStamp *timeStamp)
{
    if (timeStamp == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    /* decode in ABSOLUTE time type. */
    timeStamp->format = SMS_TIME_ABSOLUTE;

    uint8_t pickByte = 0;
    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    timeStamp->time.absolute.year =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_0A + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);

    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.month =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_0A + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);
    return DecodeTimePduPartData(buffer, timeStamp);
}

bool GsmSmsParamDecode::DecodeTimePduPartData(SmsReadBuffer &buffer, struct SmsTimeStamp *timeStamp)
{
    uint8_t pickByte = 0;
    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.day =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_0A + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);

    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.hour =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_0A + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);

    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.minute =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_0A + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);
    return DecodeTimePduData(buffer, timeStamp);
}

bool GsmSmsParamDecode::DecodeTimePduData(SmsReadBuffer &buffer, struct SmsTimeStamp *timeStamp)
{
    if (timeStamp == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    uint8_t pickByte = 0;
    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.second =
        (pickByte & HEX_VALUE_0F) * HEX_VALUE_10 + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);

    if (!buffer.PickOneByte(pickByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    timeStamp->time.absolute.timeZone =
        (pickByte & HEX_VALUE_07) * HEX_VALUE_10 + ((oneByte & HEX_VALUE_F0) >> HEX_VALUE_04);

    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte & HEX_VALUE_08) {
        timeStamp->time.absolute.timeZone *= (-1);
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
