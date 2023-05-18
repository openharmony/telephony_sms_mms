/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All rights reserved
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

#ifndef CDMA_SMS_PDU_CODEC_H
#define CDMA_SMS_PDU_CODEC_H

#include <vector>

#include "cdma_sms_types.h"

namespace OHOS {
namespace Telephony {
class CdmaSmsPduCodec {
public:
    CdmaSmsPduCodec() = default;
    virtual ~CdmaSmsPduCodec() = default;

    static bool CheckInvalidPDU(const std::vector<unsigned char> &pduStr);
    static int EncodeMsg(const struct SmsTransMsg &transMsg, unsigned char *pduStr, size_t pduStrLen);
    static int DecodeMsg(const unsigned char *pduStr, int pduLen, struct SmsTransMsg &transMsg);

private:
    static int EncodeP2PMsg(const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAckMsg(const struct SmsTransAckMsg &ackMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeCBMsg(const struct SmsTransBroadCastMsg &cbMsg, unsigned char *pduStr, size_t pduStrLen);

    static int EncodeTelesvcMsg(const struct SmsTeleSvcMsg &svcMsg, unsigned char *pduStr, size_t pduStrLen);

    static int EncodeTelesvcCancelMsg(
        const struct SmsTeleSvcCancel &cancelMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeTelesvcSubmitMsg(const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeTelesvcDeliverReportMsg(
        const struct SmsTeleSvcDeliverReport &dRMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAddress(const struct SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeSubAddress(const struct SmsTransSubAddr &address, unsigned char *pduStr, size_t pduStrLen);

    static int EncodeP2PMsgTeleServiceId(const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeP2PMsgServiceCategory(
        const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeP2PMsgReplyOption(const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen);

    static int EncodeCBMsgServiceCategory(
        const struct SmsTransBroadCastMsg &cbMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAckMsgCauseCode(
        const struct SmsTransAckMsg &ackMsg, unsigned char *pduStr, size_t pduStrLen, int &index);
    static int EncodeTelesvcSubmitMsgDeliveryTime(
        const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeTelesvcSubmitMsgPriorityIndicator(
        const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeTelesvcSubmitMsgPeplyOption(
        const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAddressTransParam(
        const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen, unsigned int &lenIndex);
    static int EncodeAddressModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAddressDigitModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAddressNumberModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeAddressOtherNumberModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeBearerUserDataParam(
        const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen, size_t &lenIndex);
    static int EncodeBearerUserDataEncodeType(
        const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen, int &remainBits);
    static int DecodeP2PMsgBearerReply(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg);
    static int DecodeP2PMsgBearerData(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg);
    static int DecodeCBMsgParamBearerData(const unsigned char *pduStr, int pduLen, struct SmsTransBroadCastMsg &cbMsg);

    static int DecodeP2PMsg(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg);
    static int DecodeCBMsg(const unsigned char *pduStr, int pduLen, struct SmsTransBroadCastMsg &cbMsg);
    static int DecodeAckMsg(const unsigned char *pduStr, int pduLen, struct SmsTransAckMsg &ackMsg);

    static void DecodeP2PTelesvcMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &svcMsg);
    static void DecodeP2PDeliveryAckMsg(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);
    static void DecodeP2PSubmitReportMsg(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static void DecodeP2PUserAckMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static void DecodeP2PReadAckMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static void DecodeP2PDeliverMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static void DecodeP2PSubmitMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static void DecodeCBBearerData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);

    static int DecodeP2PDeliverMsgBearerParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgOthersParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerMsgIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerTimeStamp(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerPeriodAbsolute(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerPeriodRelative(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerPriorityIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerPrivacyIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerReplyOption(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerNumberMesssages(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerAlertDelivery(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerLanguageIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerCallbackNumber(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerDisplayMode(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);
    static int DecodeP2PDeliverMsgBearerMessageIndex(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg);

    static void DecodeP2PEnhancedVmnParam(unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn);
    static void DecodeP2PEnhancedVmnAnDigitMode(
        unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn);
    static void DecodeP2PEnhancedVmnCliDigitMode(
        unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn);

    static int DecodeP2PSubmitMsgBearerParam(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgOthersParam(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamPeriodAbsolute(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamPeriodRelative(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamTimeAbsolute(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamTimeRelative(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamPriorityIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamPrivacyIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamReplyOption(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamMsgDelivery(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamLangIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamCallbackNumber(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);
    static int DecodeP2PSubmitMsgBearerParamDepositIndex(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg);

    static int DecodeP2PUserAckMsgBearerParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamResponseCode(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamTimeStamp(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);
    static int DecodeP2PUserAckMsgBearerParamDepositIndex(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck);

    static int DecodeP2PReadAckMsgBearerParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static int DecodeP2PReadAckMsgBearerParamIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static int DecodeP2PReadAckMsgBearerParamUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static int DecodeP2PReadAckMsgBearerParamTimeStamp(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static int DecodeP2PReadAckMsgBearerParamMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);
    static int DecodeP2PReadAckMsgBearerParamDepositIndex(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck);

    static int DecodeP2PSubmitReportMsgBearerParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static int DecodeP2PSubmitReportMsgBearerParamIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static int DecodeP2PSubmitReportMsgBearerUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static int DecodeP2PSubmitReportMsgBearerLanguageIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static int DecodeP2PSubmitReportMsgBearerParamMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);
    static int DecodeP2PSubmitReportMsgBearerParamTPFailCause(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport);

    static int DecodeP2PDeliveryAckMsgIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);
    static int DecodeP2PDeliveryAckMsgUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);
    static int DecodeP2PDeliveryAckMsgTimeStamp(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);
    static int DecodeP2PDeliveryAckMsgMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);
    static int DecodeP2PDeliveryAckMsgStatus(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg);

    static int DecodeCBBearerDataBearerParam(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamIdentifier(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamUserData(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamTimeStamp(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamPeriodAbsolute(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamPeriodRelative(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamPriorityIndicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamAlertDelivery(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamLang_indicator(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamCallbackNumber(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamDisplayMode(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);
    static int DecodeCBBearerDataBearerParamMultiEncoding(
        const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS);

    static void DecodeAddressDigitMode(unsigned char *tempStr, int tempLen, struct SmsTransAddr &transAddr);
    static void DecodeSubAddressSZData(unsigned char *tempStr, int tempLen, struct SmsTransSubAddr &subAddr);
    static int DecodeCMASType0TempData(unsigned char *tempStr, int tempLen, struct SmsTeleSvcCmasData &cmasData);
    static void DecodeUserDataEncodeType(unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData,
        bool headerInd, unsigned char numFields, unsigned char udhlBytes);
    static void DecodeUserDataEncodeASCIIType(unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData,
        bool headerInd, unsigned char numFields, unsigned char udhlBytes);
    static void DecodeUserDataEncodeGSM7Type(unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData,
        bool headerInd, unsigned char numFields, unsigned char udhlBytes);
    static void DecodeUserDataEncodeUnicodeType(unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData,
        bool headerInd, unsigned char numFields, unsigned char udhlBytes);
    static void DecodeUserDataEncodeDefaultType(
        unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData, bool headerInd);

    static int Encode7BitASCIIData(const struct SmsUserData &userData, unsigned char *dest, int &remainBits);
    static int Encode7BitGSMData(const struct SmsUserData &userData, unsigned char *dest, int &remainBits);
    static int EncodeUCS2Data(const struct SmsUserData &userData, unsigned char *dest, int &remainBits);

    static void DecodeUserData(
        unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData, bool headerInd);
    static void Decode7BitHeader(
        const unsigned char *pduStr, int pduLen, unsigned char udhlBytes, struct SmsUserData &userData);
    static void DecodeCMASData(unsigned char *pduStr, int pduLen, struct SmsTeleSvcCmasData &cmasData);
    static int DecodeCMASType0Data(unsigned char *pduStr, int pduLen, struct SmsTeleSvcCmasData &cmasData);

    static int DecodeTeleId(const unsigned char *pduStr, int pduLen, unsigned short &teleSvcId);
    static int DecodeSvcCtg(const unsigned char *pduStr, int pduLen, unsigned short &transSvcCtg);
    static int DecodeAddress(const unsigned char *pduStr, int pduLen, struct SmsTransAddr &transAddr);
    static int DecodeSubAddress(const unsigned char *pduStr, int pduLen, struct SmsTransSubAddr &subAddr);

    static int DecodeMsgId(const unsigned char *pduStr, int pduLen, struct SmsTransMsgId &smgId);
    static void DecodeCallBackNum(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcAddr &svcAddr);
    static int DecodeAbsTime(const unsigned char *pduStr, int pduLen, struct SmsTimeAbs &timeAbs);
    static enum SmsMessageType FindMsgType(const std::vector<unsigned char> &pduStr);
    static enum SmsEncodingType FindMsgEncodeType(const unsigned char value);
    static void DecodeP2PDeliverVmnAck(
        const unsigned char *pduStr, int pduLen, struct SmsEnhancedVmnAck &enhancedVmnAck);
    static void DecodeP2PEnhancedVmn(const unsigned char *pduStr, int pduLen, struct SmsEnhancedVmn &enhancedVmn);

    static unsigned char ConvertToBCD(const unsigned char val);
    static unsigned char DecodeDigitModeNumberType(const unsigned char val, bool isNumber);
    static unsigned char DecodeDigitModeNumberPlan(const unsigned char val);

    static int EncodeCbNumber(const SmsTeleSvcAddr &cbNumber, std::vector<unsigned char> &pdustr);
    static int EncodeAbsTime(const SmsTimeAbs &absTime, std::vector<unsigned char> &pdustr, size_t pduStrLen);
    static int EncodeMsgId(
        const SmsTransMsgId &msgId, const SmsMessageType &type, unsigned char *pduStr, size_t pduStrLen);
    static int EncodeBearerUserData(const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen);
    static void ShiftNBitForDecode(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit);
    static void ShiftRNBit(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit);
    static void ShiftNBit(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit);
};
} // namespace Telephony
} // namespace OHOS
#endif