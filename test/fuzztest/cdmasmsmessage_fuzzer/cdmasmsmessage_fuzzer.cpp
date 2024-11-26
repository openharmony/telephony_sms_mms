/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "cdmasmsmessage_fuzzer.h"
#ifdef FUZZTEST
#define private public
#define protected public
#endif
#include "addsmstoken_fuzzer.h"
#include "cdma_sms_sub_parameter.h"
#include "cdma_sms_transport_message.h"
#include "cdma_sms_message.h"
#include "sms_service.h"
#include "securec.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static std::unique_ptr<CdmaSmsMessage> m_pCdmaSmsMessage;
constexpr int32_t SLEEP_TIME_SECONDS = 1;

bool IsServiceInited()
{
    if (!g_isInited) {
        m_pCdmaSmsMessage = std::make_unique<CdmaSmsMessage>();
        g_isInited = true;
    }
    return g_isInited;
}

void CreateMessageTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    m_pCdmaSmsMessage->GetProtocolId();
    m_pCdmaSmsMessage->IsReplaceMessage();
    m_pCdmaSmsMessage->IsCphsMwi();
    m_pCdmaSmsMessage->PduAnalysis(pdu);
    m_pCdmaSmsMessage->GetTransMsgType();
    m_pCdmaSmsMessage->GetTransTeleService();
    m_pCdmaSmsMessage->IsWapPushMsg();
    m_pCdmaSmsMessage->transMsg_ = std::make_unique<struct CdmaTransportMsg>();
    m_pCdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::BROADCAST;
    m_pCdmaSmsMessage->PduAnalysis(pdu);
    m_pCdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::ACK;
    m_pCdmaSmsMessage->PduAnalysis(pdu);
    m_pCdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::RESERVED;
    m_pCdmaSmsMessage->PduAnalysis(pdu);
    m_pCdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::P2P;
    m_pCdmaSmsMessage->IsWapPushMsg();
    m_pCdmaSmsMessage->GetTransMsgType();
    m_pCdmaSmsMessage->GetTransTeleService();
    m_pCdmaSmsMessage->PduAnalysis(pdu);
    m_pCdmaSmsMessage->IsWapPushMsg();
    m_pCdmaSmsMessage->GetSpecialSmsInd();
    m_pCdmaSmsMessage->IsStatusReport();
    m_pCdmaSmsMessage->GetDestPort();
    m_pCdmaSmsMessage->IsBroadcastMsg();
    m_pCdmaSmsMessage->GetCMASCategory();
    m_pCdmaSmsMessage->GetCMASResponseType();
    m_pCdmaSmsMessage->GetCMASSeverity();
    m_pCdmaSmsMessage->GetCMASUrgency();
    m_pCdmaSmsMessage->GetCMASCertainty();
    m_pCdmaSmsMessage->GetCMASMessageClass();
    m_pCdmaSmsMessage->IsCMAS();
    m_pCdmaSmsMessage->GetMessageId();
    m_pCdmaSmsMessage->GetFormat();
    m_pCdmaSmsMessage->GetLanguage();
    m_pCdmaSmsMessage->GetCbInfo();
    m_pCdmaSmsMessage->GetPriority();
    m_pCdmaSmsMessage->IsEmergencyMsg();
    m_pCdmaSmsMessage->GetServiceCategoty();
    m_pCdmaSmsMessage->GetGeoScope();
    m_pCdmaSmsMessage->GetReceTime();
}

void TestAnalysisP2pMsg(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    CdmaP2PMsg p2pMsg;
    p2pMsg.teleserviceId = static_cast<uint16_t>(size);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::SUBMIT;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVERY_ACK;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::USER_ACK;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::SUBMIT_REPORT;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVERY_ACK;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::RESERVED);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
}

void TestAnalysisDeliverMwi(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    CdmaP2PMsg p2pMsg;
    p2pMsg.teleserviceId = static_cast<uint16_t>(size);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    m_pCdmaSmsMessage->AnalsisDeliverMwi(p2pMsg);
    p2pMsg.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::VMN_95);
    p2pMsg.telesvcMsg.data.deliver.vmn.faxIncluded = true;
    m_pCdmaSmsMessage->AnalsisDeliverMwi(p2pMsg);
}


void TestAnalysisDeliverMsg(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    TeleserviceDeliver deliverMsg;
    deliverMsg.displayMode = SmsDisplayMode::IMMEDIATE;
    m_pCdmaSmsMessage->AnalsisDeliverMsg(deliverMsg);
    TeleserviceDeliverAck deliverAck;
    m_pCdmaSmsMessage->AnalsisDeliverAck(deliverAck);
    TeleserviceDeliverReport report;
    m_pCdmaSmsMessage->AnalsisSubmitReport(report);
    TeleserviceSubmit submit;
    m_pCdmaSmsMessage->AnalsisSubmitMsg(submit);
}

void TestAnalsisUserData(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    SmsTeleSvcUserData userData;
    userData.userData.headerCnt = 1;
    userData.userData.header[0].udhType = UserDataHeadType::UDH_CONCAT_8BIT;
    userData.encodeType = SmsEncodingType::GSM7BIT;
    m_pCdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::KOREAN;
    m_pCdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::LATIN;
    m_pCdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SHIFT_JIS;
    m_pCdmaSmsMessage->AnalsisUserData(userData);
    TeleserviceDeliver deliver;
    m_pCdmaSmsMessage->AnalsisCMASMsg(deliver);
}

void TestAnalysisCbMsg(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    CdmaBroadCastMsg cbMsg;
    m_pCdmaSmsMessage->AnalysisCbMsg(cbMsg);
    cbMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    m_pCdmaSmsMessage->AnalysisCbMsg(cbMsg);
    CdmaAckMsg ackMsg;
    m_pCdmaSmsMessage->AnalsisAckMsg(ackMsg);
}

void TestAnalsisHeader(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    m_pCdmaSmsMessage->CreateMessage(pdu);
    SmsUDH header;
    m_pCdmaSmsMessage->AddUserDataHeader(header);
    SmsTeleSvcUserData userData;
    m_pCdmaSmsMessage->AnalsisHeader(userData);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    CreateMessageTest(data, size);
    TestAnalysisP2pMsg(data, size);
    TestAnalysisDeliverMwi(data, size);
    TestAnalysisDeliverMsg(data, size);
    TestAnalsisUserData(data, size);
    TestAnalysisCbMsg(data, size);
    TestAnalsisHeader(data, size);
    sleep(SLEEP_TIME_SECONDS);
    DelayedSingleton<SmsService>::DestroyInstance();
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
