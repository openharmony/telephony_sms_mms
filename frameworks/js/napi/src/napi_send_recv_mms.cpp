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
#include "napi_send_recv_mms.h"

#include "ability.h"
#include "napi_base_context.h"
#include "napi_mms_pdu.h"
#include "napi_mms_pdu_helper.h"
#include "sms_constants_utils.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
namespace {
const std::string SMS_PROFILE_URI = "datashare:///com.ohos.smsmmsability";
static const int32_t DEFAULT_REF_COUNT = 1;
const bool STORE_MMS_PDU_TO_FILE = false;
const int32_t ARGS_ONE = 1;
std::shared_ptr<DataShare::DataShareHelper> g_datashareHelper = nullptr;
constexpr static uint32_t WAIT_PDN_TOGGLE_TIME = 3000;
} // namespace
std::mutex NapiSendRecvMms::downloadCtx_;
std::mutex NapiSendRecvMms::countCtx_;
int32_t NapiSendRecvMms::reqCount_ = 0;
bool NapiSendRecvMms::waitFlag = false;

std::shared_ptr<OHOS::DataShare::DataShareHelper> GetDataShareHelper(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value argv[ARGS_ONE] = { 0 };
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = nullptr;
    bool isStageMode = false;
    napi_status status = OHOS::AbilityRuntime::IsStageContext(env, argv[0], isStageMode);
    if (status != napi_ok || !isStageMode) {
        auto ability = OHOS::AbilityRuntime::GetCurrentAbility(env);
        if (ability == nullptr) {
            TELEPHONY_LOGE("Failed to get native ability instance");
            return nullptr;
        }
        auto context = ability->GetContext();
        if (context == nullptr) {
            TELEPHONY_LOGE("Failed to get native context instance");
            return nullptr;
        }
        dataShareHelper = DataShare::DataShareHelper::Creator(context->GetToken(), SMS_PROFILE_URI);
    } else {
        auto context = OHOS::AbilityRuntime::GetStageModeContext(env, argv[0]);
        if (context == nullptr) {
            TELEPHONY_LOGE("Failed to get native stage context instance");
            return nullptr;
        }
        dataShareHelper = DataShare::DataShareHelper::Creator(context->GetToken(), SMS_PROFILE_URI);
    }
    return dataShareHelper;
}

bool GetMmsPduFromFile(const std::string &fileName, std::string &mmsPdu)
{
    char realPath[PATH_MAX] = { 0 };
    if (fileName.empty() || realpath(fileName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        return false;
    }

    FILE *pFile = fopen(realPath, "rb");
    if (pFile == nullptr) {
        TELEPHONY_LOGE("openFile Error");
        return false;
    }

    (void)fseek(pFile, 0, SEEK_END);
    long fileLen = ftell(pFile);
    if (fileLen <= 0 || fileLen > static_cast<long>(MMS_PDU_MAX_SIZE)) {
        (void)fclose(pFile);
        TELEPHONY_LOGE("fileLen Over Max Error");
        return false;
    }

    std::unique_ptr<char[]> pduBuffer = std::make_unique<char[]>(fileLen);
    if (!pduBuffer) {
        (void)fclose(pFile);
        TELEPHONY_LOGE("make unique pduBuffer nullptr Error");
        return false;
    }
    (void)fseek(pFile, 0, SEEK_SET);
    int32_t totolLength = static_cast<int32_t>(fread(pduBuffer.get(), 1, MMS_PDU_MAX_SIZE, pFile));
    TELEPHONY_LOGI("fread totolLength%{private}d", totolLength);

    long i = 0;
    while (i < fileLen) {
        mmsPdu += pduBuffer[i];
        i++;
    }
    (void)fclose(pFile);
    return true;
}

void StoreSendMmsPduToDataBase(NapiMmsPduHelper &helper) __attribute__((no_sanitize("cfi")))
{
    std::shared_ptr<NAPIMmsPdu> mmsPduObj = std::make_shared<NAPIMmsPdu>();
    if (mmsPduObj == nullptr) {
        TELEPHONY_LOGE("mmsPduObj nullptr");
        helper.NotifyAll();
        return;
    }
    std::string mmsPdu;
    if (!GetMmsPduFromFile(helper.GetPduFileName(), mmsPdu)) {
        TELEPHONY_LOGE("get mmsPdu fail");
        helper.NotifyAll();
        return;
    }
    mmsPduObj->InsertMmsPdu(helper, mmsPdu);
}

void NativeSendMms(napi_env env, void *data)
{
    auto asyncContext = static_cast<MmsContext *>(data);
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext nullptr");
        return;
    }
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        asyncContext->errorCode = TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
        return;
    }
    if (!STORE_MMS_PDU_TO_FILE) {
        std::string pduFileName = NapiUtil::ToUtf8(asyncContext->data);
        if (pduFileName.empty()) {
            asyncContext->errorCode = TELEPHONY_ERR_ARGUMENT_INVALID;
            asyncContext->resolved = false;
            TELEPHONY_LOGE("pduFileName empty");
            return;
        }
        if (g_datashareHelper == nullptr) {
            asyncContext->errorCode = TELEPHONY_ERR_LOCAL_PTR_NULL;
            asyncContext->resolved = false;
            TELEPHONY_LOGE("g_datashareHelper is nullptr");
            return;
        }
        NapiMmsPduHelper helper;
        helper.SetDataShareHelper(g_datashareHelper);
        helper.SetPduFileName(pduFileName);
        if (!helper.Run(StoreSendMmsPduToDataBase, helper)) {
            TELEPHONY_LOGE("StoreMmsPdu fail");
            asyncContext->errorCode = TELEPHONY_ERR_LOCAL_PTR_NULL;
            asyncContext->resolved = false;
            return;
        }
        asyncContext->data = NapiUtil::ToUtf16(helper.GetDbUrl());
    }
    asyncContext->errorCode =
        DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMms(asyncContext->slotId, asyncContext->mmsc,
            asyncContext->data, asyncContext->mmsConfig.userAgent, asyncContext->mmsConfig.userAgentProfile);
    if (asyncContext->errorCode == TELEPHONY_ERR_SUCCESS) {
        asyncContext->resolved = true;
    } else {
        asyncContext->resolved = false;
    }
    TELEPHONY_LOGI("NativeSendMms end resolved = %{public}d", asyncContext->resolved);
}

void SendMmsCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MmsContext *>(data);
    if (g_datashareHelper != nullptr) {
        g_datashareHelper->Release();
    }
    if (context == nullptr) {
        TELEPHONY_LOGE("SendMmsCallback context nullptr");
        return;
    }
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "sendMms", "ohos.permission.SEND_MESSAGES");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

bool MatchMmsParameters(napi_env env, napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case TWO_PARAMETERS: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, { napi_object, napi_object });
            break;
        }
        case THREE_PARAMETERS: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, { napi_object, napi_object, napi_function });
            break;
        }
        default: {
            break;
        }
    }
    if (typeMatch) {
        return NapiUtil::MatchObjectProperty(env, parameters[1],
            {
                { "slotId", napi_number },
                { "mmsc", napi_string },
                { "data", napi_string },
                { "mmsConfig", napi_object },
            });
    }
    return false;
}

static bool GetMmsValueLength(napi_env env, napi_value param)
{
    size_t len = 0;
    napi_status status = napi_get_value_string_utf8(env, param, nullptr, 0, &len);
    if (status != napi_ok) {
        TELEPHONY_LOGE("Get length failed");
        return false;
    }
    return (len > 0) && (len < BUFF_LENGTH);
}

static void GetMmsNameProperty(napi_env env, napi_value param, MmsContext &context)
{
    napi_value slotIdValue = NapiUtil::GetNamedProperty(env, param, "slotId");
    if (slotIdValue != nullptr) {
        napi_get_value_int32(env, slotIdValue, &(context.slotId));
    }
    napi_value mmscValue = NapiUtil::GetNamedProperty(env, param, "mmsc");
    if (mmscValue != nullptr && GetMmsValueLength(env, mmscValue)) {
        char strChars[NORMAL_STRING_SIZE] = { 0 };
        size_t strLength = 0;
        napi_get_value_string_utf8(env, mmscValue, strChars, BUFF_LENGTH, &strLength);
        std::string str8(strChars, strLength);
        context.mmsc = NapiUtil::ToUtf16(str8);
    }
    napi_value dataValue = NapiUtil::GetNamedProperty(env, param, "data");
    if (dataValue != nullptr && GetMmsValueLength(env, dataValue)) {
        char strChars[NORMAL_STRING_SIZE] = { 0 };
        size_t strLength = 0;
        napi_get_value_string_utf8(env, dataValue, strChars, BUFF_LENGTH, &strLength);
        std::string str8(strChars, strLength);
        context.data = NapiUtil::ToUtf16(str8);
    }
    napi_value configValue = NapiUtil::GetNamedProperty(env, param, "mmsConfig");
    if (configValue != nullptr) {
        napi_value uaValue = NapiUtil::GetNamedProperty(env, configValue, "userAgent");
        if (uaValue != nullptr && GetMmsValueLength(env, uaValue)) {
            char strChars[NORMAL_STRING_SIZE] = { 0 };
            size_t strLength = 0;
            napi_get_value_string_utf8(env, uaValue, strChars, BUFF_LENGTH, &strLength);
            std::string str8(strChars, strLength);
            context.mmsConfig.userAgent = NapiUtil::ToUtf16(str8);
        }
        napi_value uaprofValue = NapiUtil::GetNamedProperty(env, configValue, "userAgentProfile");
        if (uaprofValue != nullptr && GetMmsValueLength(env, uaprofValue)) {
            char strChars[NORMAL_STRING_SIZE] = { 0 };
            size_t strLength = 0;
            napi_get_value_string_utf8(env, uaprofValue, strChars, BUFF_LENGTH, &strLength);
            std::string str8(strChars, strLength);
            context.mmsConfig.userAgentProfile = NapiUtil::ToUtf16(str8);
        }
    }
}

napi_value NapiSendRecvMms::SendMms(napi_env env, napi_callback_info info)
{
    size_t parameterCount = THREE_PARAMETERS;
    napi_value parameters[THREE_PARAMETERS] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchMmsParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<MmsContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("MmsContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (!STORE_MMS_PDU_TO_FILE) {
        g_datashareHelper = GetDataShareHelper(env, info);
    }
    GetMmsNameProperty(env, parameters[1], *context);
    if (parameterCount == THREE_PARAMETERS) {
        napi_create_reference(env, parameters[PARAMETERS_INDEX_TWO], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(env, context, "SendMms", NativeSendMms, SendMmsCallback);
    return result;
}

bool WriteBufferToFile(const std::unique_ptr<char[]> &buff, uint32_t len, const std::string &strPathName)
{
    if (buff == nullptr) {
        TELEPHONY_LOGE("buff nullptr");
        return false;
    }

    char realPath[PATH_MAX] = { 0 };
    if (strPathName.empty() || realpath(strPathName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        return false;
    }

    FILE *pFile = fopen(realPath, "wb");
    if (pFile == nullptr) {
        TELEPHONY_LOGE("openFile Error");
        return false;
    }
    uint32_t fileLen = fwrite(buff.get(), len, 1, pFile);
    (void)fclose(pFile);
    if (fileLen > 0) {
        TELEPHONY_LOGI("write mms buffer to file success");
        return true;
    } else {
        TELEPHONY_LOGI("write mms buffer to file error");
        return false;
    }
}

bool StoreMmsPduToFile(const std::string &fileName, const std::string &mmsPdu)
{
    uint32_t len = static_cast<uint32_t>(mmsPdu.size());
    if (len > MMS_PDU_MAX_SIZE || len == 0) {
        TELEPHONY_LOGE("MMS pdu length invalid");
        return false;
    }

    std::unique_ptr<char[]> resultResponse = std::make_unique<char[]>(len);
    if (memset_s(resultResponse.get(), len, 0x00, len) != EOK) {
        TELEPHONY_LOGE("memset_s err");
        return false;
    }
    if (memcpy_s(resultResponse.get(), len, &mmsPdu[0], len) != EOK) {
        TELEPHONY_LOGE("memcpy_s error");
        return false;
    }

    TELEPHONY_LOGI("len:%{public}d", len);
    if (!WriteBufferToFile(std::move(resultResponse), len, fileName)) {
        TELEPHONY_LOGE("write to file error");
        return false;
    }
    return true;
}

void GetMmsPduFromDataBase(NapiMmsPduHelper &helper) __attribute__((no_sanitize("cfi")))
{
    NAPIMmsPdu mmsPduObj;
    std::string mmsPdu = mmsPduObj.GetMmsPdu(helper);
    if (mmsPdu.empty()) {
        TELEPHONY_LOGE("from dataBase empty");
        return;
    }

    mmsPduObj.DeleteMmsPdu(helper);
    if (!StoreMmsPduToFile(helper.GetStoreFileName(), mmsPdu)) {
        TELEPHONY_LOGE("store mmsPdu fail");
    }
    helper.NotifyAll();
}

static bool DownloadExceptionCase(
    MmsContext &context, std::shared_ptr<OHOS::DataShare::DataShareHelper> g_datashareHelper)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        context.errorCode = TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
        context.resolved = false;
        return false;
    }
    if (g_datashareHelper == nullptr) {
        TELEPHONY_LOGE("g_datashareHelper is nullptr");
        context.errorCode = TELEPHONY_ERR_LOCAL_PTR_NULL;
        context.resolved = false;
        return false;
    }
    std::string fileName = NapiUtil::ToUtf8(context.data);
    char realPath[PATH_MAX] = { 0 };
    if (fileName.empty() || realpath(fileName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        context.errorCode = TELEPHONY_ERR_ARGUMENT_INVALID;
        context.resolved = false;
        return false;
    }
    FILE *pFile = fopen(realPath, "wb");
    if (pFile == nullptr) {
        TELEPHONY_LOGE("openFile Error");
        context.errorCode = TELEPHONY_ERR_ARGUMENT_INVALID;
        context.resolved = false;
        return false;
    }
    (void)fclose(pFile);
    return true;
}

void UpdateReqCount()
{
    std::unique_lock<std::mutex> lck(NapiSendRecvMms::countCtx_);
    NapiSendRecvMms::reqCount_++;
    TELEPHONY_LOGI("reqCount_:%{public}d", NapiSendRecvMms::reqCount_);
}

void DecreaseReqCount()
{
    NapiSendRecvMms::reqCount_--;
    if (NapiSendRecvMms::reqCount_ > 0) {
        NapiSendRecvMms::waitFlag = true;
    } else {
        NapiSendRecvMms::waitFlag = false;
    }
}

void NativeDownloadMms(napi_env env, void *data)
{
    auto asyncContext = static_cast<MmsContext *>(data);
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("asyncContext nullptr");
        return;
    }
    if (!DownloadExceptionCase(*asyncContext, g_datashareHelper)) {
        TELEPHONY_LOGE("Exception case");
        return;
    }

    TELEPHONY_LOGI("native download mms");
    UpdateReqCount();
    std::unique_lock<std::mutex> lck(NapiSendRecvMms::downloadCtx_);
    if (NapiSendRecvMms::waitFlag) {
        TELEPHONY_LOGI("down multiple mms at once wait");
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_PDN_TOGGLE_TIME));
    }
    std::u16string dbUrls;
    asyncContext->errorCode =
        DelayedSingleton<SmsServiceManagerClient>::GetInstance()->DownloadMms(asyncContext->slotId, asyncContext->mmsc,
            dbUrls, asyncContext->mmsConfig.userAgent, asyncContext->mmsConfig.userAgentProfile);
    TELEPHONY_LOGI("NativeDownloadMms dbUrls:%{public}s", NapiUtil::ToUtf8(dbUrls).c_str());
    if (asyncContext->errorCode == TELEPHONY_ERR_SUCCESS) {
        asyncContext->resolved = true;
        if (!STORE_MMS_PDU_TO_FILE) {
            NapiMmsPduHelper helper;
            helper.SetDataShareHelper(g_datashareHelper);
            helper.SetDbUrl(NapiUtil::ToUtf8(dbUrls));
            helper.SetStoreFileName(NapiUtil::ToUtf8(asyncContext->data));
            if (!helper.Run(GetMmsPduFromDataBase, helper)) {
                TELEPHONY_LOGE("StoreMmsPdu fail");
                asyncContext->errorCode = TELEPHONY_ERR_LOCAL_PTR_NULL;
                asyncContext->resolved = false;
                return;
            }
        }
    } else {
        asyncContext->resolved = false;
    }
    DecreaseReqCount();
    TELEPHONY_LOGI("NativeDownloadMms end resolved = %{public}d", asyncContext->resolved);
}

void DownloadMmsCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MmsContext *>(data);
    if (g_datashareHelper != nullptr && !NapiSendRecvMms::waitFlag) {
        g_datashareHelper->Release();
    }
    if (context == nullptr) {
        TELEPHONY_LOGE("SendMmsCallback context nullptr");
        return;
    }
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "downloadMms", "ohos.permission.RECEIVE_MMS");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

napi_value NapiSendRecvMms::DownloadMms(napi_env env, napi_callback_info info)
{
    size_t parameterCount = THREE_PARAMETERS;
    napi_value parameters[THREE_PARAMETERS] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchMmsParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("DownloadMms parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<MmsContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("DownloadMms MmsContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (!STORE_MMS_PDU_TO_FILE) {
        g_datashareHelper = GetDataShareHelper(env, info);
    }
    GetMmsNameProperty(env, parameters[1], *context);
    if (parameterCount == THREE_PARAMETERS) {
        napi_create_reference(env, parameters[PARAMETERS_INDEX_TWO], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(env, context, "DownloadMms", NativeDownloadMms, DownloadMmsCallback);
    return result;
}
} // namespace Telephony
} // namespace OHOS
