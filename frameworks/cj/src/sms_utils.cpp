/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "sms_utils.h"

#include "enum_convert_for_js.h"
#include "napi_util.h"
#include "sms_service_manager_client.h"
#include "state_registry_errors.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
char* MallocCString(const std::string& origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto len = origin.length() + 1;
    if (len > 10000) {  // 10000 is max string length
        return nullptr;
    }
    char* res = static_cast<char*>(malloc(sizeof(char) * len));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), len);
}

bool CreateCommonArgumentErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_ERR_ARGUMENT_MISMATCH:
        case TELEPHONY_ERR_ARGUMENT_INVALID:
        case TELEPHONY_ERR_ARGUMENT_NULL:
        case TELEPHONY_ERR_SLOTID_INVALID:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateCommonServiceErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_ERR_DESCRIPTOR_MISMATCH:
        case TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL:
        case TELEPHONY_ERR_WRITE_DATA_FAIL:
        case TELEPHONY_ERR_WRITE_REPLY_FAIL:
        case TELEPHONY_ERR_READ_DATA_FAIL:
        case TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL:
        case TELEPHONY_ERR_REGISTER_CALLBACK_FAIL:
        case TELEPHONY_ERR_CALLBACK_ALREADY_REGISTERED:
        case TELEPHONY_ERR_UNINIT:
        case TELEPHONY_ERR_UNREGISTER_CALLBACK_FAIL:
            error = JS_ERROR_TELEPHONY_SERVICE_ERROR;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateCommonSystemErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_ERR_FAIL:
        case TELEPHONY_ERR_MEMCPY_FAIL:
        case TELEPHONY_ERR_MEMSET_FAIL:
        case TELEPHONY_ERR_STRCPY_FAIL:
        case TELEPHONY_ERR_LOCAL_PTR_NULL:
        case TELEPHONY_ERR_SUBSCRIBE_BROADCAST_FAIL:
        case TELEPHONY_ERR_PUBLISH_BROADCAST_FAIL:
        case TELEPHONY_ERR_ADD_DEATH_RECIPIENT_FAIL:
        case TELEPHONY_ERR_STRTOINT_FAIL:
        case TELEPHONY_ERR_RIL_CMD_FAIL:
        case TELEPHONY_ERR_DATABASE_WRITE_FAIL:
        case TELEPHONY_ERR_DATABASE_READ_FAIL:
        case TELEPHONY_ERR_UNKNOWN_NETWORK_TYPE:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        case TELEPHONY_ERR_NO_SIM_CARD:
            error = JS_ERROR_TELEPHONY_NO_SIM_CARD;
            break;
        case TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API:
            error = JS_ERROR_ILLEGAL_USE_OF_SYSTEM_API;
            break;
        case TELEPHONY_ERR_AIRPLANE_MODE_ON:
            error = JS_ERROR_TELEPHONY_AIRPLANE_MODE_ON;
            break;
        case TELEPHONY_ERR_NETWORK_NOT_IN_SERVICE:
            error = JS_ERROR_TELEPHONY_NETWORK_NOT_IN_SERVICE;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateCommonErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < COMMON_ERR_OFFSET || errorCode >= CALL_ERR_OFFSET)) {
        return false;
    }
    if (CreateCommonArgumentErrorCodeForCj(errorCode, error) || CreateCommonServiceErrorCodeForCj(errorCode, error) ||
        CreateCommonSystemErrorCodeForCj(errorCode, error)) {
        return true;
    }
    return false;
}

bool CreateParameterErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;
    switch (errorCode) {
        case ERROR_SERVICE_UNAVAILABLE:
        case ERROR_NATIVE_API_EXECUTE_FAIL:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        case ERROR_PARAMETER_COUNTS_INVALID:
        case ERROR_PARAMETER_TYPE_INVALID:
            error = JS_ERROR_TELEPHONY_INVALID_INPUT_PARAMETER;
            break;
        case ERROR_SLOT_ID_INVALID:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

bool CreateCommonCallErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_CALL_ERR_NUMBER_OUT_OF_RANGE:
        case TELEPHONY_CALL_ERR_PHONE_NUMBER_EMPTY:
        case TELEPHONY_CALL_ERR_FORMAT_PHONE_NUMBER_FAILED:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        case TELEPHONY_CALL_ERR_PARAMETER_OUT_OF_RANGE:
        case TELEPHONY_CALL_ERR_INVALID_SLOT_ID:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        case TELEPHONY_CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT:
            error = JS_ERROR_TELEPHONY_CONFERENCE_EXCEED_LIMIT;
            break;
        case TELEPHONY_CALL_ERR_CONFERENCE_CALL_IS_NOT_ACTIVE:
            error = JS_ERROR_TELEPHONY_CONFERENCE_CALL_NOT_ACTIVE;
            break;
        case TELEPHONY_CALL_ERR_CALL_COUNTS_EXCEED_LIMIT:
            error = JS_ERROR_TELEPHONY_CALL_COUNTS_EXCEED_LIMIT;
            break;
        case TELEPHONY_CALL_ERR_CALL_IS_NOT_ACTIVATED:
        case TELEPHONY_CALL_ERR_ILLEGAL_CALL_OPERATION:
        case TELEPHONY_CALL_ERR_AUDIO_SETTING_MUTE_FAILED:
        case TELEPHONY_CALL_ERR_CALL_IS_NOT_ON_HOLDING:
        case TELEPHONY_CALL_ERR_PHONE_CALLS_TOO_FEW:
        case TELEPHONY_CALL_ERR_VIDEO_ILLEGAL_CALL_TYPE:
        case TELEPHONY_CALL_ERR_CONFERENCE_NOT_EXISTS:
        case TELEPHONY_CALL_ERR_CONFERENCE_SEPERATE_FAILED:
        case TELEPHONY_CALL_ERR_EMERGENCY_UNSUPPORT_CONFERENCEABLE:
        case TELEPHONY_CALL_ERR_VOLTE_NOT_SUPPORT:
        case TELEPHONY_CALL_ERR_VOLTE_PROVISIONING_DISABLED:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        case TELEPHONY_CALL_ERR_DIAL_IS_BUSY:
            error = JS_ERROR_TELEPHONY_DIAL_IS_BUSY;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateVideoCallErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_CALL_ERR_VIDEO_ILLEGAL_MEDIA_TYPE:
        case TELEPHONY_CALL_ERR_VIDEO_IN_PROGRESS:
        case TELEPHONY_CALL_ERR_VIDEO_ILLEAGAL_SCENARIO:
        case TELEPHONY_CALL_ERR_VIDEO_MODE_CHANGE_NOTIFY_FAILED:
        case TELEPHONY_CALL_ERR_VIDEO_NOT_SUPPORTED:
        case TELEPHONY_CALL_ERR_SETTING_AUDIO_DEVICE_FAILED:
        case TELEPHONY_CALL_ERR_VIDEO_INVALID_COORDINATES:
        case TELEPHONY_CALL_ERR_VIDEO_INVALID_ZOOM:
        case TELEPHONY_CALL_ERR_VIDEO_INVALID_ROTATION:
        case TELEPHONY_CALL_ERR_VIDEO_INVALID_CAMERA_ID:
        case TELEPHONY_CALL_ERR_INVALID_PATH:
        case TELEPHONY_CALL_ERR_CAMERA_NOT_TURNED_ON:
        case TELEPHONY_CALL_ERR_INVALID_DIAL_SCENE:
        case TELEPHONY_CALL_ERR_INVALID_VIDEO_STATE:
        case TELEPHONY_CALL_ERR_UNKNOW_DIAL_TYPE:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateSupplementServiceCallErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_CALL_ERR_INVALID_RESTRICTION_TYPE:
        case TELEPHONY_CALL_ERR_INVALID_RESTRICTION_MODE:
        case TELEPHONY_CALL_ERR_INVALID_TRANSFER_TYPE:
        case TELEPHONY_CALL_ERR_INVALID_TRANSFER_SETTING_TYPE:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        case TELEPHONY_CALL_ERR_FUNCTION_NOT_SUPPORTED:
            error = JS_ERROR_DEVICE_NOT_SUPPORT_THIS_API;
            break;
        case TELEPHONY_CALL_ERR_INVALID_TRANSFER_TIME:
        case TELEPHONY_CALL_ERR_NAPI_INTERFACE_FAILED:
        case TELEPHONY_CALL_ERR_CALLBACK_ALREADY_EXIST:
        case TELEPHONY_CALL_ERR_RESOURCE_UNAVAILABLE:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        case TELEPHONY_CALL_ERR_UT_NO_CONNECTION:
            error = JS_ERROR_CALL_UT_NO_CONNECTION;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateCallErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < CALL_ERR_OFFSET || errorCode >= CELLULAR_DATA_ERR_OFFSET)) {
        return false;
    }
    if (CreateCommonCallErrorCodeForCj(errorCode, error) || CreateVideoCallErrorCodeForCj(errorCode, error) ||
        CreateSupplementServiceCallErrorCodeForCj(errorCode, error)) {
        return true;
    }
    return false;
}

bool CreateDataErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < CELLULAR_DATA_ERR_OFFSET || errorCode >= SMS_MMS_ERR_OFFSET)) {
        return false;
    }
    bool flag = true;

    switch (errorCode) {
        case TELEPHONY_CELLULAR_DATA_INVALID_PARAM:
            error = JS_ERROR_CELLULAR_DATA_BASE_ERROR;
            break;
        default:
            flag = false;
            break;
    }
    return flag;
}

bool CreateNetworkSearchErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < CORE_SERVICE_NETWORK_SEARCH_ERR_OFFSET || errorCode >= CORE_SERVICE_CORE_ERR_OFFSET)) {
        return false;
    }
    bool flag = true;
    switch (errorCode) {
        case CORE_SERVICE_SEND_CALLBACK_FAILED:
        case CORE_SERVICE_RADIO_PROTOCOL_TECH_UNKNOWN:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

bool CreateVcardErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    bool flag = true;
    switch (errorCode) {
        case TELEPHONY_ERR_VCARD_FILE_INVALID:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

bool CreateSimErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < CORE_SERVICE_SIM_ERR_OFFSET || errorCode >= CORE_SERVICE_NETWORK_SEARCH_ERR_OFFSET)) {
        return false;
    }
    bool flag = true;
    switch (errorCode) {
        case CORE_SERVICE_SIM_CARD_IS_NOT_ACTIVE:
            error = JS_ERROR_SIM_CARD_IS_NOT_ACTIVE;
            break;
        case CORE_ERR_SIM_CARD_LOAD_FAILED:
        case CORE_ERR_SIM_CARD_UPDATE_FAILED:
            error = JS_ERROR_SIM_CARD_OPERATION_ERROR;
            break;
        case CORE_ERR_OPERATOR_KEY_NOT_EXIT:
        case CORE_ERR_OPERATOR_CONF_NOT_EXIT:
            error = JS_ERROR_OPERATOR_CONFIG_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

bool CreateSmsErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < SMS_MMS_ERR_OFFSET || errorCode >= STATE_REGISTRY_ERR_OFFSET)) {
        return false;
    }
    bool flag = true;
    switch (errorCode) {
        case TELEPHONY_SMS_MMS_DECODE_DATA_EMPTY:
        case TELEPHONY_SMS_MMS_UNKNOWN_SIM_MESSAGE_STATUS:
        case TELEPHONY_SMS_MMS_MESSAGE_LENGTH_OUT_OF_RANGE:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

bool CreateObserverErrorCodeForCj(int32_t errorCode, int32_t& error)
{
    if ((errorCode < STATE_REGISTRY_ERR_OFFSET || errorCode >= NET_MANAGER_ERR_OFFSET)) {
        return false;
    }
    bool flag = true;
    switch (errorCode) {
        case TELEPHONY_STATE_REGISTRY_SLODID_ERROR:
            error = JS_ERROR_TELEPHONY_ARGUMENT_ERROR;
            break;
        case TELEPHONY_STATE_REGISTRY_PERMISSION_DENIED:
            error = JS_ERROR_TELEPHONY_PERMISSION_DENIED;
            break;
        case TELEPHONY_STATE_REGISTRY_DATA_NOT_EXIST:
        case TELEPHONY_STATE_UNREGISTRY_DATA_NOT_EXIST:
        case TELEPHONY_STATE_REGISTRY_DATA_EXIST:
        case TELEPHONY_STATE_REGISTRY_NOT_IMPLEMENTED:
            error = JS_ERROR_TELEPHONY_SYSTEM_ERROR;
            break;
        default:
            flag = false;
            break;
    }

    return flag;
}

int32_t ConverErrorCodeForCj(int32_t errorCode)
{
    int32_t error = 0;
    if (errorCode == TELEPHONY_ERR_SUCCESS) {
        return JS_ERROR_TELEPHONY_SUCCESS;
    }
    if (CreateParameterErrorCodeForCj(errorCode, error)) {
        return error;
    }
    if (!CreateCommonErrorCodeForCj(errorCode, error) && !CreateCallErrorCodeForCj(errorCode, error) &&
        !CreateDataErrorCodeForCj(errorCode, error) && !CreateNetworkSearchErrorCodeForCj(errorCode, error) &&
        !CreateVcardErrorCodeForCj(errorCode, error) && !CreateSimErrorCodeForCj(errorCode, error) &&
        !CreateSmsErrorCodeForCj(errorCode, error) && !CreateObserverErrorCodeForCj(errorCode, error)) {
        error = JS_ERROR_TELEPHONY_UNKNOW_ERROR;
    }
    return error;
}

int32_t ConverErrorCodeWithPermissionForCj(int32_t errorCode)
{
    if (errorCode == TELEPHONY_ERR_PERMISSION_ERR) {
        return JS_ERROR_TELEPHONY_PERMISSION_DENIED;
    }
    return ConverErrorCodeForCj(errorCode);
}
} // namespace Telephony
} // namespace OHOS
