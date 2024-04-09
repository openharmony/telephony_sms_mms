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

namespace OHOS {
namespace Telephony {
using namespace NetManagerStandard;
using namespace NetStack::HttpClient;
std::string METHOD_POST = "POST";
std::string METHOD_GET = "GET";
constexpr const char *SIMID_IDENT_PREFIX = "simId";
const bool STORE_MMS_PDU_TO_FILE = false;
constexpr static const int32_t WAIT_TIME_SECOND = 10 * 60;
constexpr static const unsigned int HTTP_TIME_MICRO_SECOND = WAIT_TIME_SECOND * 100;

MmsNetworkClient::MmsNetworkClient(int32_t slotId)
{
    slotId_ = slotId;
}

MmsNetworkClient::~MmsNetworkClient() {}

int32_t MmsNetworkClient::Execute(const std::string &method, const std::string &mmsc, std::string &data)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    if (METHOD_POST.compare(method) == 0) {
        ret = PostUrl(mmsc, data);
        responseData_ = "";
        return ret;
    } else if (METHOD_GET.compare(method) == 0) {
        ret = GetUrl(mmsc, data);
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

int32_t MmsNetworkClient::PostUrl(const std::string &mmsc, const std::string &fileName)
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
    ret = HttpRequest(METHOD_POST, mmsc, strBuf);
    if (ret != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("http fail error");
        return ret;
    }

    httpFinish_ = false;
    while (!httpFinish_) {
        TELEPHONY_LOGI("wait(), networkReady = false");
        if (clientCv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }
    if (!STORE_MMS_PDU_TO_FILE) {
        DeleteMmsPdu(fileName);
    }
    if (!httpSuccess_) {
        TELEPHONY_LOGI("send mms failed");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    } else {
        TELEPHONY_LOGI("send mms successed");
        return TELEPHONY_ERR_SUCCESS;
    }
}

int32_t MmsNetworkClient::HttpRequest(const std::string &method, const std::string &url, const std::string &data)
{
    HttpClientRequest httpReq;
    httpReq.SetURL(url);
    httpReq.SetHttpProxyType(HttpProxyType::USE_SPECIFIED);
    NetStack::HttpClient::HttpProxy httpProxy;
    int32_t ret = GetMmsApnPorxy(httpProxy);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("get mms apn error");
        return ret;
    }
    httpReq.SetHttpProxy(httpProxy);
    httpReq.SetConnectTimeout(HTTP_TIME_MICRO_SECOND);
    httpReq.SetTimeout(HTTP_TIME_MICRO_SECOND);
    if (method.compare(METHOD_POST) == 0) {
        httpReq.SetBody(data.c_str(), data.size());
        httpReq.SetMethod(HttpConstant::HTTP_METHOD_POST);
        httpReq.SetHeader("content-type", "application/vnd.wap.mms-message; charset=utf-8");
        httpReq.SetHeader("Accept", "application/vnd.wap.mms-message, application/vnd.wap.sic");
    } else {
        httpReq.SetMethod(HttpConstant::HTTP_METHOD_GET);
    }
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
    TELEPHONY_LOGI("before call [task->Start]");
    task->Start();
    TELEPHONY_LOGI("after call [task->Start]");
    return TELEPHONY_ERR_SUCCESS;
}

int32_t MmsNetworkClient::GetUrl(const std::string &mmsc, std::string &storeDirName)
{
    std::unique_lock<std::mutex> lck(clientCts_);
    std::string strData = "";
    if (HttpRequest(METHOD_GET, mmsc, strData) != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("http fail error");
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    }
    httpFinish_ = false;
    while (!httpFinish_) {
        TELEPHONY_LOGI("wait(), networkReady = false");
        if (clientCv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }
    TELEPHONY_LOGI("responseData_ len: %{public}d", static_cast<uint32_t>(responseData_.size()));
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
        clientCv_.notify_one();
    });
    task->OnCancel([this](const HttpClientRequest &request, const HttpClientResponse &response) {
        TELEPHONY_LOGI("OnCancel");
        httpFinish_ = true;
        clientCv_.notify_one();
    });
    task->OnFail(
        [this](const HttpClientRequest &request, const HttpClientResponse &response, const HttpClientError &error) {
            TELEPHONY_LOGE("OnFailed, responseCode:%{public}d", response.GetResponseCode());
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
