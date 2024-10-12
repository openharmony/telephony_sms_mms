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

#include "mms_network_client.h"

#include <curl/curl.h>
#include <curl/easy.h>

#include "core_manager_inner.h"
#include "http_client.h"
#include "http_client_constant.h"
#include "http_client_error.h"
#include "http_client_request.h"
#include "http_client_response.h"
#include "mms_apn_info.h"
#include "mms_persist_helper.h"
#include "net_conn_client.h"
#include "net_handle.h"
#include "net_link_info.h"
#include "securec.h"
#include "sms_constants_utils.h"
#include "telephony_common_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "string_utils.h"
#include "mms_codec_type.h"

namespace OHOS {
namespace Telephony {
using namespace NetManagerStandard;
using namespace NetStack::HttpClient;
std::string METHOD_POST = "POST";
std::string METHOD_GET = "GET";
constexpr const char *SIMID_IDENT_PREFIX = "simId";
const bool STORE_MMS_PDU_TO_FILE = false;
constexpr static const int32_t WAIT_TIME_SECOND = 10 * 60;
constexpr static const unsigned int HTTP_TIME_MICRO_SECOND = WAIT_TIME_SECOND * 1000;
constexpr static const uint8_t SEND_CONF_RESPONSE_STATUS_OK = 0x80;
constexpr static const uint32_t SEND_CONF_MAX_SIZE = 500;
constexpr static const uint8_t MAX_RETRY_TIMES = 3;
constexpr static const int32_t ONE_HUNDRED = 100;
constexpr static const int32_t VALID_RESPONSECODE_FIRST_NUM = 2;


MmsNetworkClient::MmsNetworkClient(int32_t slotId)
{
    slotId_ = slotId;
}

MmsNetworkClient::~MmsNetworkClient() {}

int32_t MmsNetworkClient::Execute(const std::string &method, const std::string &mmsc, std::string &data,
    const std::string &ua, const std::string &uaprof)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    if (METHOD_POST.compare(method) == 0) {
        ret = PostUrl(mmsc, data, ua, uaprof);
        std::unique_lock<std::mutex> lck(clientCts_);
        responseData_ = "";
        return ret;
    } else if (METHOD_GET.compare(method) == 0) {
        ret = GetUrl(mmsc, data, ua, uaprof);
        std::unique_lock<std::mutex> lck(clientCts_);
        responseData_ = "";
        return ret;
    }
    TELEPHONY_LOGI("mms http request fail");
    return ret;
}

int32_t MmsNetworkClient::GetMmscFromDb(const std::string &mmsc)
{
    std::shared_ptr<MmsApnInfo> mmsApnInfo = std::make_shared<MmsApnInfo>(slotId_);
    if (mmsApnInfo == nullptr) {
        TELEPHONY_LOGE("mmsApnInfo is nullptr");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }
    std::string mmscFromDataBase = mmsApnInfo->getMmscUrl();
    if (mmsc != mmscFromDataBase) {
        TELEPHONY_LOGE("mmsc is invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    return TELEPHONY_SUCCESS;
}

int32_t MmsNetworkClient::GetMmsDataBuf(std::string &strBuf, const std::string &fileName)
{
    if (STORE_MMS_PDU_TO_FILE) {
        if (!GetMmsPduFromFile(fileName, strBuf)) {
            TELEPHONY_LOGE("Get MmsPdu from file fail");
            return TELEPHONY_ERR_READ_DATA_FAIL;
        }
    } else {
        if (!GetMmsPduFromDataBase(fileName, strBuf)) {
            TELEPHONY_LOGE("Get MmsPdu from data base fail");
            return TELEPHONY_ERR_DATABASE_READ_FAIL;
        }
    }
    return TELEPHONY_SUCCESS;
}

int32_t MmsNetworkClient::GetMmsApnPorxy(NetStack::HttpClient::HttpProxy &httpProxy)
{
    std::shared_ptr<MmsApnInfo> mmsApnInfo = std::make_shared<MmsApnInfo>(slotId_);
    if (mmsApnInfo == nullptr) {
        TELEPHONY_LOGE("mmsApnInfo is nullptr");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }
    std::string proxy = mmsApnInfo->getMmsProxyAddressAndProxyPort();
    if (proxy.empty()) {
        TELEPHONY_LOGE("proxy empty");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }
    size_t locate = proxy.find(":");
    if (locate == 0 || locate == std::string::npos || static_cast<size_t>(locate + 1) == proxy.size()) {
        TELEPHONY_LOGE("mms apn error");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }

    httpProxy.host = proxy.substr(0, locate);
    std::string port = proxy.substr(locate + 1);
    if (!IsValidDecValue(port)) {
        TELEPHONY_LOGE("port not decimal");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }
    httpProxy.port = std::stoi(port);
    return TELEPHONY_SUCCESS;
}

int32_t MmsNetworkClient::PostUrl(const std::string &mmsc, const std::string &fileName, const std::string &ua,
    const std::string &uaprof)
{
    int32_t ret = GetMmscFromDb(mmsc);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("post request fail");
        return ret;
    }
    std::unique_lock<std::mutex> lck(clientCts_);
    std::string strBuf;
    ret = GetMmsDataBuf(strBuf, fileName);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("post request fail");
        return ret;
    }
    for (retryTimes_ = 0; retryTimes_ <= MAX_RETRY_TIMES; retryTimes_++) {
        ret = HttpRequest(METHOD_POST, mmsc, strBuf, ua, uaprof);
        if (ret != TELEPHONY_ERR_SUCCESS) {
            TELEPHONY_LOGE("http fail error");
            return ret;
        }
        httpFinish_ = false;
        httpSuccess_ = false;
        while (!httpFinish_) {
            TELEPHONY_LOGI("wait(), networkReady = false");
            if (clientCv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
                TELEPHONY_LOGE("wait networkready timeout");
                return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
            }
        }
        if (!httpSuccess_ || responseCode_ / ONE_HUNDRED != VALID_RESPONSECODE_FIRST_NUM) {
            TELEPHONY_LOGE("http post task is not success, task responseCode is %{public}d", responseCode_);
            responseData_ = "";
            responseCode_ = 0;
            continue;
        }
        if (!CheckSendConf()) {
            TELEPHONY_LOGE("send mms failed due to send-conf decode fail");
            responseData_ = "";
            continue;
        }
        break;
    }
    if (!STORE_MMS_PDU_TO_FILE) {
        DeleteMmsPdu(fileName);
    }
    if (retryTimes_ > MAX_RETRY_TIMES) {
        TELEPHONY_LOGE("send mms retry times over 3, send mms failed");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }
    return TELEPHONY_ERR_SUCCESS;
}

bool MmsNetworkClient::CheckSendConf()
{
    uint32_t length = responseData_.size();
    if (length > SEND_CONF_MAX_SIZE || length == 0) {
        TELEPHONY_LOGE("send mms response length invalid");
        return true;
    }
    std::unique_ptr<char[]> sendMmsResponse = std::make_unique<char[]>(length);
    if (memset_s(sendMmsResponse.get(), length, 0x00, length) != EOK) {
        TELEPHONY_LOGE("memset_s error");
        return true;
    }
    if (memcpy_s(sendMmsResponse.get(), length, &responseData_[0], length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error");
        return true;
    }
    MmsDecodeBuffer sendMmsResponseBuffer;
    if (!sendMmsResponseBuffer.WriteDataBuffer(std::move(sendMmsResponse), length)) {
        TELEPHONY_LOGE("write buffer error");
        return true;
    }
    if (!mmsHeader_.DecodeMmsHeader(sendMmsResponseBuffer)) {
        TELEPHONY_LOGE("decode send mms response error");
        return true;
    }
    uint8_t value = 0;
    if (!mmsHeader_.GetOctetValue(MmsFieldCode::MMS_RESPONSE_STATUS, value)) {
        TELEPHONY_LOGE("get response status error");
        return true;
    }
    if (value != SEND_CONF_RESPONSE_STATUS_OK) {
        TELEPHONY_LOGE("sendconf response status is not OK, the value is %{public}02X", value);
        return false;
    }
    return true;
}

void MmsNetworkClient::GetCoverUrl(std::string str)
{
    if (str.size() == 0) {
        TELEPHONY_LOGI("url is empty");
        return;
    }
    int32_t stride = 2;
    for (uint8_t i = 0; i < str.size(); i = i + stride) {
        str[i] = '*';
    }
    TELEPHONY_LOGI("decode result is: %{public}s", str.c_str());
}

int32_t MmsNetworkClient::HttpRequest(const std::string &method, const std::string &url, const std::string &data,
    const std::string &ua, const std::string &uaprof)
{
    HttpClientRequest httpReq;
    httpReq.SetURL(url);
    NetStack::HttpClient::HttpProxy httpProxy;
    int32_t ret = GetMmsApnPorxy(httpProxy);
    if (ret == TELEPHONY_SUCCESS) {
        httpReq.SetHttpProxyType(HttpProxyType::USE_SPECIFIED);
        httpReq.SetHttpProxy(httpProxy);
    } else {
        TELEPHONY_LOGE("get mms apn error");
    }
    httpReq.SetConnectTimeout(HTTP_TIME_MICRO_SECOND);
    httpReq.SetTimeout(HTTP_TIME_MICRO_SECOND);
    if (method.compare(METHOD_POST) == 0) {
        httpReq.SetBody(data.c_str(), data.size());
        httpReq.SetMethod(HttpConstant::HTTP_METHOD_POST);
        httpReq.SetHeader("content-type", "application/vnd.wap.mms-message; charset=utf-8");
        httpReq.SetHeader("Accept", "*/*, application/vnd.wap.mms-message, application/vnd.wap.sic");
    } else {
        httpReq.SetMethod(HttpConstant::HTTP_METHOD_GET);
    }
    httpReq.SetHeader("User-Agent", ua);
    httpReq.SetHeader("x-wap-profile", uaprof);
    HttpSession &session = HttpSession::GetInstance();
    auto task = session.CreateTask(httpReq);
    if (task == nullptr || task->GetCurlHandle() == nullptr) {
        TELEPHONY_LOGE("task nullptr error");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }

    if (GetIfaceName().empty()) {
        TELEPHONY_LOGE("Ifacename empty");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }
    CURLcode errCode = CURLE_OK;
    errCode = curl_easy_setopt(task->GetCurlHandle(), CURLOPT_INTERFACE, GetIfaceName().c_str());
    if (errCode != CURLE_OK) {
        TELEPHONY_LOGE("CURLOPT_INTERFACE failed errCode:%{public}d", errCode);
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }
    HttpCallBack(task);
    task->Start();
    return TELEPHONY_ERR_SUCCESS;
}

int32_t MmsNetworkClient::GetUrl(const std::string &mmsc, std::string &storeDirName, const std::string &ua,
    const std::string &uaprof)
{
    std::unique_lock<std::mutex> lck(clientCts_);
    for (retryTimes_ = 0; retryTimes_ <= MAX_RETRY_TIMES; retryTimes_++) {
        std::string strData = "";
        if (HttpRequest(METHOD_GET, mmsc, strData, ua, uaprof) != TELEPHONY_ERR_SUCCESS) {
            TELEPHONY_LOGE("http fail error");
            return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
        }
        httpFinish_ = false;
        httpSuccess_ = false;
        while (!httpFinish_) {
            TELEPHONY_LOGI("wait(), networkReady = false");
            if (clientCv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
                TELEPHONY_LOGE("wait networkready timeout");
                return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
            }
        }
        if (!httpSuccess_) {
            TELEPHONY_LOGE("http get task is not success");
            responseData_ = "";
            responseCode_ = 0;
            continue;
        }
        if (responseCode_ / ONE_HUNDRED != VALID_RESPONSECODE_FIRST_NUM) {
            TELEPHONY_LOGE("get task responseCode is not success:%{public}d", responseCode_);
            responseData_ = "";
            responseCode_ = 0;
            continue;
        }
        break;
    }
    if (retryTimes_ > MAX_RETRY_TIMES) {
        TELEPHONY_LOGE("download mms retry times over 3, download mms failed");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }
    TELEPHONY_LOGI("responseData_ len: %{public}d", static_cast<uint32_t>(responseData_.size()));
    if (responseData_.size() == 0) {
        GetCoverUrl(mmsc);
    }
    return UpdateMmsPduToStorage(storeDirName);
}

int32_t MmsNetworkClient::UpdateMmsPduToStorage(std::string &storeDirName)
{
    uint32_t len = responseData_.size();
    if (len > MMS_PDU_MAX_SIZE || len == 0) {
        TELEPHONY_LOGE("MMS pdu length invalid");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (STORE_MMS_PDU_TO_FILE) {
        std::unique_ptr<char[]> resultResponse = std::make_unique<char[]>(len);
        if (memset_s(resultResponse.get(), len, 0x00, len) != EOK) {
            TELEPHONY_LOGE("memset_s error");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (memcpy_s(resultResponse.get(), len, &responseData_[0], len) != EOK) {
            TELEPHONY_LOGE("memcpy_s error");
            return TELEPHONY_ERR_MEMCPY_FAIL;
        }
        if (!WriteBufferToFile(std::move(resultResponse), len, storeDirName)) {
            TELEPHONY_LOGE("write to file error");
            return TELEPHONY_ERR_WRITE_DATA_FAIL;
        }
        return TELEPHONY_ERR_SUCCESS;
    } else {
        std::shared_ptr<MmsPersistHelper> mmsPduObj = std::make_shared<MmsPersistHelper>();
        if (mmsPduObj == nullptr) {
            TELEPHONY_LOGE("mmsPduObj nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        bool ret = mmsPduObj->InsertMmsPdu(responseData_, storeDirName);
        TELEPHONY_LOGI("ret:%{public}d, length:%{public}d", ret, len);
        return ret ? TELEPHONY_ERR_SUCCESS : TELEPHONY_ERR_FAIL;
    }
}

bool MmsNetworkClient::GetMmsPduFromFile(const std::string &fileName, std::string &strBuf)
{
    FILE *pFile = nullptr;
    char realPath[PATH_MAX] = { 0 };
    if (fileName.empty() || realpath(fileName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        return false;
    }

    pFile = fopen(realPath, "rb");
    if (pFile == nullptr) {
        TELEPHONY_LOGE("openFile Error");
        return false;
    }

    (void)fseek(pFile, 0, SEEK_END);
    long fileLen = ftell(pFile);
    if (fileLen <= 0 || fileLen > static_cast<long>(MMS_PDU_MAX_SIZE)) {
        (void)fclose(pFile);
        TELEPHONY_LOGE("Mms Over Long Error");
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
    TELEPHONY_LOGI("sendMms totolLength:%{public}d", totolLength);
    (void)fclose(pFile);

    long i = 0;
    while (i < fileLen) {
        strBuf += pduBuffer[i];
        i++;
    }
    return true;
}

bool MmsNetworkClient::GetMmsPduFromDataBase(const std::string &dbUrl, std::string &strBuf)
{
    if (dbUrl.empty()) {
        TELEPHONY_LOGE("dbUrl is empty");
        return false;
    }
    std::shared_ptr<MmsPersistHelper> mmsPdu = std::make_shared<MmsPersistHelper>();
    if (mmsPdu == nullptr) {
        TELEPHONY_LOGE("mmsPdu nullptr");
        return false;
    }
    strBuf = mmsPdu->GetMmsPdu(dbUrl);
    if (strBuf.empty()) {
        TELEPHONY_LOGE("strBuf is empty");
        return false;
    }
    return true;
}

void MmsNetworkClient::DeleteMmsPdu(const std::string &dbUrl)
{
    std::shared_ptr<MmsPersistHelper> mmsPdu = std::make_shared<MmsPersistHelper>();
    if (mmsPdu == nullptr) {
        TELEPHONY_LOGE("mmsPdu is nullptr");
        return;
    }
    mmsPdu->DeleteMmsPdu(dbUrl);
}

std::string MmsNetworkClient::GetIfaceName()
{
    int32_t simId = CoreManagerInner::GetInstance().GetSimId(slotId_);
    std::list<int32_t> netIdList;
    int32_t ret =
        NetConnClient::GetInstance().GetNetIdByIdentifier(SIMID_IDENT_PREFIX + std::to_string(simId), netIdList);
    std::string ifaceName;
    if (ret != NETMANAGER_SUCCESS) {
        TELEPHONY_LOGE("get netIdList by identifier fail, ret = %{public}d", ret);
        return ifaceName;
    }
    std::list<sptr<NetHandle>> netList;
    int32_t result = NetConnClient::GetInstance().GetAllNets(netList);
    if (result != NETMANAGER_SUCCESS) {
        TELEPHONY_LOGE("get all nets fail, ret = %{public}d", result);
        return ifaceName;
    }
    for (sptr<NetHandle> netHandle : netList) {
        for (auto netId : netIdList) {
            if (netId != netHandle->GetNetId()) {
                continue;
            }
            NetAllCapabilities capabilities;
            NetConnClient::GetInstance().GetNetCapabilities(*netHandle, capabilities);
            auto search = capabilities.netCaps_.find(NetCap::NET_CAPABILITY_MMS);
            if (search == capabilities.netCaps_.end()) {
                continue;
            }
            NetLinkInfo info;
            NetConnClient::GetInstance().GetConnectionProperties(*netHandle, info);
            ifaceName = info.ifaceName_;
            TELEPHONY_LOGI("data is connected ifaceName = %{public}s", ifaceName.c_str());
            return ifaceName;
        }
    }
    TELEPHONY_LOGI("slot = %{public}d data is not connected for this slot", slotId_);
    return ifaceName;
}

bool MmsNetworkClient::WriteBufferToFile(
    const std::unique_ptr<char[]> &buff, uint32_t len, std::string &strPathName) const
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
    FILE *pFile = nullptr;
    pFile = fopen(realPath, "wb");
    if (!pFile) {
        TELEPHONY_LOGE("open file fail");
        return false;
    }
    uint32_t fileLen = fwrite(buff.get(), len, 1, pFile);
    if (fileLen == 0) {
        TELEPHONY_LOGI("write mms buffer to file error");
        (void)fclose(pFile);
        return false;
    }
    (void)fclose(pFile);
    return true;
}

void MmsNetworkClient::HttpCallBack(std::shared_ptr<HttpClientTask> task)
{
    task->OnSuccess([task, this](const HttpClientRequest &request, const HttpClientResponse &response) {
        TELEPHONY_LOGI("OnSuccess");
        httpFinish_ = true;
        httpSuccess_ = true;
        responseCode_ = response.GetResponseCode();
        clientCv_.notify_one();
    });
    task->OnCancel([this](const HttpClientRequest &request, const HttpClientResponse &response) {
        TELEPHONY_LOGI("OnCancel, responseCode:%{public}d", response.GetResponseCode());
        httpFinish_ = true;
        clientCv_.notify_one();
    });
    task->OnFail(
        [this](const HttpClientRequest &request, const HttpClientResponse &response, const HttpClientError &error) {
            TELEPHONY_LOGE("OnFailed, errorCode:%{public}d", error.GetErrorCode());
            httpFinish_ = true;
            clientCv_.notify_one();
        });
    task->OnDataReceive([this](const HttpClientRequest &request, const uint8_t *data, size_t length) {
        if (data == nullptr || length == 0) {
            return;
        }
        responseData_.insert(responseData_.size(), reinterpret_cast<const char *>(data), length);
    });
    task->OnProgress(
        [](const HttpClientRequest &request, u_long dltotal, u_long dlnow, u_long ultotal, u_long ulnow) {});
}
} // namespace Telephony
} // namespace OHOS
