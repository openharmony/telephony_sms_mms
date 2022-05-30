/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SMS_WAP_PUSH_CONTENT_TYPE_H
#define SMS_WAP_PUSH_CONTENT_TYPE_H

#include <map>

#include "sms_wap_push_buffer.h"

namespace OHOS {
namespace Telephony {
enum class WapPushContentTypes {
    X_X                                          = 0x00,
    TEXT_X                                       = 0x01,
    TEXT_HTML                                    = 0x02,
    TEXT_PLAIN                                   = 0x03,
    TEXT_X_HDML                                  = 0x04,
    TEXT_X_TTML                                  = 0x05,
    TEXT_X_VCALENDAR                             = 0x06,
    TEXT_X_VCARD                                 = 0x07,
    TEXT_VND_WAP_WML                             = 0x08,
    TEXT_VND_WAP_WMLSCRIPT                       = 0x09,
    TEXT_VND_WAP_WTA_EVENT                       = 0x0A,
    MULTIPART_X                                  = 0x0B,
    MULTIPART_MIXED                              = 0x0C,
    MULTIPART_FORM_DATA                          = 0x0D,
    MULTIPART_BYTERANTES                         = 0x0E,
    MULTIPART_ALTERNATIVE                        = 0x0F,
    APPLICATION_X                                = 0x10,
    APPLICATION_JAVA_VM                          = 0x11,
    APPLICATION_X_WWW_FORM_URLENCODED            = 0x12,
    APPLICATION_X_HDMLC                          = 0x13,
    APPLICATION_VND_WAP_WMLC                     = 0x14,
    APPLICATION_VND_WAP_WMLSCRIPTC               = 0x15,
    APPLICATION_VND_WAP_WTA_EVENTC               = 0x16,
    APPLICATION_VND_WAP_UAPROF                   = 0x17,
    APPLICATION_VND_WAP_WTLS_CA_CERTIFICATE      = 0x18,
    APPLICATION_VND_WAP_WTLS_USER_CERTIFICATE    = 0x19,
    APPLICATION_X_X509_CA_CERT                   = 0x1A,
    APPLICATION_X_X509_USER_CERT                 = 0x1B,
    IMAGE_X                                      = 0x1C,
    IMAGE_GIF                                    = 0x1D,
    IMAGE_JPEG                                   = 0x1E,
    IMAGE_TIFF                                   = 0x1F,
    IMAGE_PNG                                    = 0x20,
    IMAGE_VND_WAP_WBMP                           = 0x21,
    APPLICATION_VND_WAP_MULTIPART_X              = 0x22,
    APPLICATION_VND_WAP_MULTIPART_MIXED          = 0x23,
    APPLICATION_VND_WAP_MULTIPART_FORM_DATA      = 0x24,
    APPLICATION_VND_WAP_MULTIPART_BYTERANGES     = 0x25,
    APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE    = 0x26,
    APPLICATION_XML                              = 0x27,
    TEXT_XML                                     = 0x28,
    APPLICATION_VND_WAP_WBXML                    = 0x29,
    APPLICATION_X_X968_CROSS_CERT                = 0x2A,
    APPLICATION_X_X968_CA_CERT                   = 0x2B,
    APPLICATION_X_X968_USER_CERT                 = 0x2C,
    TEXT_VND_WAP_SI                              = 0x2D,
    APPLICATION_VND_WAP_SIC                      = 0x2E,
    TEXT_VND_WAP_SL                              = 0x2F,
    APPLICATION_VND_WAP_SLC                      = 0x30,
    TEXT_VND_WAP_CO                              = 0x31,
    APPLICATION_VND_WAP_COC                      = 0x32,
    APPLICATION_VND_WAP_MULTIPART_RELATED        = 0x33,
    APPLICATION_VND_WAP_SIA                      = 0x34,
    TEXT_VND_WAP_CONNECTIVITY_XML                = 0x35,
    APPLICATION_VND_WAP_CONNECTIVITY_WBXML       = 0x36,
    APPLICATION_PKCS7_MIME                       = 0x37,
    APPLICATION_VND_WAP_HASHED_CERTIFICATE       = 0x38,
    APPLICATION_VND_WAP_SIGNED_CERTIFICATE       = 0x39,
    APPLICATION_VND_WAP_CERT_RESPONSE            = 0x3A,
    APPLICATION_XHTML_XML                        = 0x3B,
    APPLICATION_WML_XML                          = 0x3C,
    TEXT_CSS                                     = 0x3D,
    APPLICATION_VND_WAP_MMS_MESSAGE              = 0x3E,
    APPLICATION_VND_WAP_ROLLOVER_CERTIFICATE     = 0x3F,
    APPLICATION_VND_WAP_LOCC_WBXML               = 0x40,
    APPLICATION_VND_WAP_LOC_XML                  = 0x41,
    APPLICATION_VND_SYNCML_DM_WBXML              = 0x42,
    APPLICATION_VND_SYNCML_DM_XML                = 0x43,
    APPLICATION_VND_SYNCML_NOTIFICATION          = 0x44,
    APPLICATION_VND_WAP_XHTML_XML                = 0x45,
    APPLICATION_VND_WV_CSP_CIR                   = 0x46,
    APPLICATION_VND_OMA_DD_XML                   = 0x47,
    APPLICATION_VND_OMA_DRM_MESSAGE              = 0x48,
    APPLICATION_VND_OMA_DRM_CONTENT              = 0x49,
    APPLICATION_VND_OMA_DRM_RIGHTS_XML           = 0x4A,
    APPLICATION_VND_OMA_DRM_RIGHTS_WBXML         = 0x4B,
    APPLICATION_VND_WV_CSP_XML                   = 0x4C,
    APPLICATION_VND_WV_CSP_WBXML                 = 0x4D,
    APPLICATION_VND_SYNCML_DS_NOTIFICATION       = 0x4E,
    AUDIO_X                                      = 0x4F,
    VIDEO_X                                      = 0x50,
    APPLICATION_VND_OMA_DD2_XML                  = 0x51,
    APPLICATION_MIKEY                            = 0x52,
    APPLICATION_VND_OMA_DCD                      = 0x53,
    APPLICATION_VND_OMA_DCDC                     = 0x54,
    CONTENT_TYPE_UNDOWN                          = 0xFF
};

struct WapKeyValue {
    WapPushContentTypes key;
    const char *value;
};

static const WapKeyValue wapContentNames[] = {
    { WapPushContentTypes::X_X,                                        "*/*" },
    { WapPushContentTypes::TEXT_X,                                     "text/*" },
    { WapPushContentTypes::TEXT_HTML,                                  "text/html" },
    { WapPushContentTypes::TEXT_PLAIN,                                 "text/plain" },
    { WapPushContentTypes::TEXT_X_HDML,                                "text/x-hdml" },
    { WapPushContentTypes::TEXT_X_TTML,                                "text/x-ttml" },
    { WapPushContentTypes::TEXT_X_VCALENDAR,                           "text/x-vCalendar" },
    { WapPushContentTypes::TEXT_X_VCARD,                               "text/x-vCard" },
    { WapPushContentTypes::TEXT_VND_WAP_WML,                           "text/vnd.wap.wml" },
    { WapPushContentTypes::TEXT_VND_WAP_WMLSCRIPT,                     "text/vnd.wap.wmlscript" },
    { WapPushContentTypes::TEXT_VND_WAP_WTA_EVENT,                     "text/vnd.wap.wta-event" },
    { WapPushContentTypes::MULTIPART_X,                                "multipart/*" },
    { WapPushContentTypes::MULTIPART_MIXED,                            "multipart/mixed" },
    { WapPushContentTypes::MULTIPART_FORM_DATA,                        "multipart/form-data" },
    { WapPushContentTypes::MULTIPART_BYTERANTES,                       "multipart/byterantes" },
    { WapPushContentTypes::MULTIPART_ALTERNATIVE,                      "multipart/alternative" },
    { WapPushContentTypes::APPLICATION_X,                              "application/*" },
    { WapPushContentTypes::APPLICATION_JAVA_VM,                        "application/java-vm" },
    { WapPushContentTypes::APPLICATION_X_WWW_FORM_URLENCODED,          "application/x-www-form-urlencoded" },
    { WapPushContentTypes::APPLICATION_X_HDMLC,                        "application/x-hdmlc" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WMLC,                   "application/vnd.wap.wmlc" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WMLSCRIPTC,             "application/vnd.wap.wmlscriptc" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WTA_EVENTC,             "application/vnd.wap.wta-eventc" },
    { WapPushContentTypes::APPLICATION_VND_WAP_UAPROF,                 "application/vnd.wap.uaprof" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WTLS_CA_CERTIFICATE,    "application/vnd.wap.wtls-ca-certificate" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WTLS_USER_CERTIFICATE,  "application/vnd.wap.wtls-user-certificate" },
    { WapPushContentTypes::APPLICATION_X_X509_CA_CERT,                 "application/x-x509-ca-cert" },
    { WapPushContentTypes::APPLICATION_X_X509_USER_CERT,               "application/x-x509-user-cert" },
    { WapPushContentTypes::IMAGE_X,                                    "image/*" },
    { WapPushContentTypes::IMAGE_GIF,                                  "image/gif" },
    { WapPushContentTypes::IMAGE_JPEG,                                 "image/jpeg" },
    { WapPushContentTypes::IMAGE_TIFF,                                 "image/tiff" },
    { WapPushContentTypes::IMAGE_PNG,                                  "image/png" },
    { WapPushContentTypes::IMAGE_VND_WAP_WBMP,                         "image/vnd.wap.wbmp" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_X,            "application/vnd.wap.multipart.*" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_MIXED,        "application/vnd.wap.multipart.mixed" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_FORM_DATA,    "application/vnd.wap.multipart.form-data" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_BYTERANGES,   "application/vnd.wap.multipart.byteranges" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE,  "application/vnd.wap.multipart.alternative" },
    { WapPushContentTypes::APPLICATION_XML,                            "application/xml" },
    { WapPushContentTypes::TEXT_XML,                                   "text/xml" },
    { WapPushContentTypes::APPLICATION_VND_WAP_WBXML,                  "application/vnd.wap.wbxml" },
    { WapPushContentTypes::APPLICATION_X_X968_CROSS_CERT,              "application/x-x968-cross-cert" },
    { WapPushContentTypes::APPLICATION_X_X968_CA_CERT,                 "application/x-x968-ca-cert" },
    { WapPushContentTypes::APPLICATION_X_X968_USER_CERT,               "application/x-x968-user-cert" },
    { WapPushContentTypes::TEXT_VND_WAP_SI,                            "text/vnd.wap.si" },
    { WapPushContentTypes::APPLICATION_VND_WAP_SIC,                    "application/vnd.wap.sic" },
    { WapPushContentTypes::TEXT_VND_WAP_SL,                            "text/vnd.wap.sl" },
    { WapPushContentTypes::APPLICATION_VND_WAP_SLC,                    "application/vnd.wap.slc" },
    { WapPushContentTypes::TEXT_VND_WAP_CO,                            "text/vnd.wap.co" },
    { WapPushContentTypes::APPLICATION_VND_WAP_COC,                    "application/vnd.wap.coc" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MULTIPART_RELATED,      "application/vnd.wap.multipart.related" },
    { WapPushContentTypes::APPLICATION_VND_WAP_SIA,                    "application/vnd.wap.sia" },
    { WapPushContentTypes::TEXT_VND_WAP_CONNECTIVITY_XML,              "text/vnd.wap.connectivity-xml" },
    { WapPushContentTypes::APPLICATION_VND_WAP_CONNECTIVITY_WBXML,     "application/vnd.wap.connectivity-wbxml" },
    { WapPushContentTypes::APPLICATION_PKCS7_MIME,                     "application/pkcs7-mime" },
    { WapPushContentTypes::APPLICATION_VND_WAP_HASHED_CERTIFICATE,     "application/vnd.wap.hashed-certificate" },
    { WapPushContentTypes::APPLICATION_VND_WAP_SIGNED_CERTIFICATE,     "application/vnd.wap.signed-certificate" },
    { WapPushContentTypes::APPLICATION_VND_WAP_CERT_RESPONSE,          "application/vnd.wap.cert-response" },
    { WapPushContentTypes::APPLICATION_XHTML_XML,                      "application/xhtml+xml" },
    { WapPushContentTypes::APPLICATION_WML_XML,                        "application/wml+xml" },
    { WapPushContentTypes::TEXT_CSS,                                   "text/css" },
    { WapPushContentTypes::APPLICATION_VND_WAP_MMS_MESSAGE,            "application/vnd.wap.mms-message" },
    { WapPushContentTypes::APPLICATION_VND_WAP_ROLLOVER_CERTIFICATE,   "application/vnd.wap.rollover-certificate" },
    { WapPushContentTypes::APPLICATION_VND_WAP_LOCC_WBXML,             "application/vnd.wap.locc+wbxml" },
    { WapPushContentTypes::APPLICATION_VND_WAP_LOC_XML,                "application/vnd.wap.loc+xml" },
    { WapPushContentTypes::APPLICATION_VND_SYNCML_DM_WBXML,            "application/vnd.syncml.dm+wbxml" },
    { WapPushContentTypes::APPLICATION_VND_SYNCML_DM_XML,              "application/vnd.syncml.dm+xml" },
    { WapPushContentTypes::APPLICATION_VND_SYNCML_NOTIFICATION,        "application/vnd.syncml.notification" },
    { WapPushContentTypes::APPLICATION_VND_WAP_XHTML_XML,              "application/vnd.wap.xhtml+xml" },
    { WapPushContentTypes::APPLICATION_VND_WV_CSP_CIR,                 "application/vnd.wv.csp.cir" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DD_XML,                 "application/vnd.oma.dd+xml" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DRM_MESSAGE,            "application/vnd.oma.drm.message" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DRM_CONTENT,            "application/vnd.oma.drm.content" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DRM_RIGHTS_XML,         "application/vnd.oma.drm.rights+xml" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DRM_RIGHTS_WBXML,       "application/vnd.oma.drm.rights+wbxml" },
    { WapPushContentTypes::APPLICATION_VND_WV_CSP_XML,                 "application/vnd.wv.csp+xml" },
    { WapPushContentTypes::APPLICATION_VND_WV_CSP_WBXML,               "application/vnd.wv.csp+wbxml" },
    { WapPushContentTypes::APPLICATION_VND_SYNCML_DS_NOTIFICATION,     "application/vnd.syncml.ds.notification" },
    { WapPushContentTypes::AUDIO_X,                                    "audio/*" },
    { WapPushContentTypes::VIDEO_X,                                    "video/*" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DD2_XML,                "application/vnd.oma.dd2+xml" },
    { WapPushContentTypes::APPLICATION_MIKEY,                          "application/mikey" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DCD,                    "application/vnd.oma.dcd" },
    { WapPushContentTypes::APPLICATION_VND_OMA_DCDC,                   "application/vnd.oma.dcdc" },
};

/**
 * wap-230-wsp-20010705-a   section:Table 38. Well-Known Parameter Assignments
 */
enum class WapContentParam {
    // Encoding Version 1.1
    CT_P_Q = 0x80,                  /* Q */
    CT_P_CHARSET = 0x81,            /* Charset */
    CT_P_LEVEL = 0x82,              /* Level */
    CT_P_TYPE = 0x83,               /* Type */
    CT_P_NAME = 0x85,               /* Name */
    CT_P_FILENAME = 0x86,           /* Filename */
    CT_P_DIFFERENCES = 0x87,        /* Differences */
    CT_P_PADDING = 0x88,            /* Padding */
    // Encoding Version 1.2
    CT_P_TYPE_STRING = 0x89,        /* Type */
    CT_P_START = 0x8A,              /* Start Text-string */
    CT_P_START_INFO = 0x8B,         /* Start-info  Text-string */
    // Encoding Version 1.3
    CT_P_COMMENT = 0x8C,            /* Comment */
    CT_P_DOMAIN = 0x8D,             /* Domain */
    CT_P_MAX_AGE = 0x8E,            /* Max-Age */
    CT_P_PATH = 0x8F,               /* Path */
    CT_P_SECURE = 0x90,             /* Secure */
    // Encoding Version 1.4
    CT_P_SEC = 0x91,                /* SEC */
    CT_P_MAC = 0x92,                /* MAC */
    CT_P_CREATION_DATE = 0x93,      /* Creation-date */
    CT_P_MODIFICATION_DATE = 0x94,  /* Modification-date */
    CT_P_READ_DATE = 0x95,          /* Read-date */
    CT_P_SIZE = 0x96,               /* Size */
    CT_P_NAME_VALUE = 0x97,         /* Name */
    CT_P_FILENAME_VALUE = 0x98,     /* Filename */
    CT_P_START_VALUE = 0x99,        /* Start Text-value */
    CT_P_START_INFO_VALUE = 0x9A,   /* Start-info Text-value */
    CT_P_COMMENT_VALUE = 0x9B,      /* Comment */
    CT_P_DOMAIN_VALUE = 0x9C,       /* Domain */
    CT_P_PATH_VALUE = 0x9D,         /* Path */
};

class SmsWapPushContentType {
public:
    SmsWapPushContentType();
    ~SmsWapPushContentType() = default;
    bool DecodeContentType(SmsWapPushBuffer &decodeBuffer, int32_t &len);
    bool DecodeCTGeneralForm(SmsWapPushBuffer &decodeBuffer, int32_t &contentLength);
    bool DecodeParameter(SmsWapPushBuffer &decodeBuffer, int32_t valueLength);
    bool DecodeTextField(SmsWapPushBuffer &decodeBuffer, uint8_t field, int32_t &valueLength);
    bool DecodeCharsetField(SmsWapPushBuffer &decodeBuffer, int32_t &valueLength);
    bool DecodeTypeField(SmsWapPushBuffer &decodeBuffer, int32_t &valueLength);

public:
    bool GetCharSetIntFromString(uint32_t &charSet, const std::string &strCharSet);
    static std::string GetContentTypeFromInt(uint8_t type);
    std::string GetContentType();
    bool SetContentType(std::string str);

private:
    std::string contentType_ = "";
    uint32_t charset_ = 0;
    std::string type_ = "";
    std::map<uint8_t, std::string> textParameterMap_;
    std::map<std::string, uint32_t> mapCharSet_;
};
} // namespace Telephony
} // namespace OHOS
#endif
