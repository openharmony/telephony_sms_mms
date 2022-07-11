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

#ifndef MMS_CONTENT_TYPE_H
#define MMS_CONTENT_TYPE_H

#include "mms_decode_buffer.h"
#include "mms_content_param.h"

namespace OHOS {
namespace Telephony {
enum class ContentTypes {
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

struct KeyValue {
    ContentTypes key;
    const char *value;
};

static const KeyValue mmsContentNames[] = {
    { ContentTypes::X_X,                                        "*/*" },
    { ContentTypes::TEXT_X,                                     "text/*" },
    { ContentTypes::TEXT_HTML,                                  "text/html" },
    { ContentTypes::TEXT_PLAIN,                                 "text/plain" },
    { ContentTypes::TEXT_X_HDML,                                "text/x-hdml" },
    { ContentTypes::TEXT_X_TTML,                                "text/x-ttml" },
    { ContentTypes::TEXT_X_VCALENDAR,                           "text/x-vCalendar" },
    { ContentTypes::TEXT_X_VCARD,                               "text/x-vCard" },
    { ContentTypes::TEXT_VND_WAP_WML,                           "text/vnd.wap.wml" },
    { ContentTypes::TEXT_VND_WAP_WMLSCRIPT,                     "text/vnd.wap.wmlscript" },
    { ContentTypes::TEXT_VND_WAP_WTA_EVENT,                     "text/vnd.wap.wta-event" },
    { ContentTypes::MULTIPART_X,                                "multipart/*" },
    { ContentTypes::MULTIPART_MIXED,                            "multipart/mixed" },
    { ContentTypes::MULTIPART_FORM_DATA,                        "multipart/form-data" },
    { ContentTypes::MULTIPART_BYTERANTES,                       "multipart/byterantes" },
    { ContentTypes::MULTIPART_ALTERNATIVE,                      "multipart/alternative" },
    { ContentTypes::APPLICATION_X,                              "application/*" },
    { ContentTypes::APPLICATION_JAVA_VM,                        "application/java-vm" },
    { ContentTypes::APPLICATION_X_WWW_FORM_URLENCODED,          "application/x-www-form-urlencoded" },
    { ContentTypes::APPLICATION_X_HDMLC,                        "application/x-hdmlc" },
    { ContentTypes::APPLICATION_VND_WAP_WMLC,                   "application/vnd.wap.wmlc" },
    { ContentTypes::APPLICATION_VND_WAP_WMLSCRIPTC,             "application/vnd.wap.wmlscriptc" },
    { ContentTypes::APPLICATION_VND_WAP_WTA_EVENTC,             "application/vnd.wap.wta-eventc" },
    { ContentTypes::APPLICATION_VND_WAP_UAPROF,                 "application/vnd.wap.uaprof" },
    { ContentTypes::APPLICATION_VND_WAP_WTLS_CA_CERTIFICATE,    "application/vnd.wap.wtls-ca-certificate" },
    { ContentTypes::APPLICATION_VND_WAP_WTLS_USER_CERTIFICATE,  "application/vnd.wap.wtls-user-certificate" },
    { ContentTypes::APPLICATION_X_X509_CA_CERT,                 "application/x-x509-ca-cert" },
    { ContentTypes::APPLICATION_X_X509_USER_CERT,               "application/x-x509-user-cert" },
    { ContentTypes::IMAGE_X,                                    "image/*" },
    { ContentTypes::IMAGE_GIF,                                  "image/gif" },
    { ContentTypes::IMAGE_JPEG,                                 "image/jpeg" },
    { ContentTypes::IMAGE_TIFF,                                 "image/tiff" },
    { ContentTypes::IMAGE_PNG,                                  "image/png" },
    { ContentTypes::IMAGE_VND_WAP_WBMP,                         "image/vnd.wap.wbmp" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_X,            "application/vnd.wap.multipart.*" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_MIXED,        "application/vnd.wap.multipart.mixed" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_FORM_DATA,    "application/vnd.wap.multipart.form-data" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_BYTERANGES,   "application/vnd.wap.multipart.byteranges" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE,  "application/vnd.wap.multipart.alternative" },
    { ContentTypes::APPLICATION_XML,                            "application/xml" },
    { ContentTypes::TEXT_XML,                                   "text/xml" },
    { ContentTypes::APPLICATION_VND_WAP_WBXML,                  "application/vnd.wap.wbxml" },
    { ContentTypes::APPLICATION_X_X968_CROSS_CERT,              "application/x-x968-cross-cert" },
    { ContentTypes::APPLICATION_X_X968_CA_CERT,                 "application/x-x968-ca-cert" },
    { ContentTypes::APPLICATION_X_X968_USER_CERT,               "application/x-x968-user-cert" },
    { ContentTypes::TEXT_VND_WAP_SI,                            "text/vnd.wap.si" },
    { ContentTypes::APPLICATION_VND_WAP_SIC,                    "application/vnd.wap.sic" },
    { ContentTypes::TEXT_VND_WAP_SL,                            "text/vnd.wap.sl" },
    { ContentTypes::APPLICATION_VND_WAP_SLC,                    "application/vnd.wap.slc" },
    { ContentTypes::TEXT_VND_WAP_CO,                            "text/vnd.wap.co" },
    { ContentTypes::APPLICATION_VND_WAP_COC,                    "application/vnd.wap.coc" },
    { ContentTypes::APPLICATION_VND_WAP_MULTIPART_RELATED,      "application/vnd.wap.multipart.related" },
    { ContentTypes::APPLICATION_VND_WAP_SIA,                    "application/vnd.wap.sia" },
    { ContentTypes::TEXT_VND_WAP_CONNECTIVITY_XML,              "text/vnd.wap.connectivity-xml" },
    { ContentTypes::APPLICATION_VND_WAP_CONNECTIVITY_WBXML,     "application/vnd.wap.connectivity-wbxml" },
    { ContentTypes::APPLICATION_PKCS7_MIME,                     "application/pkcs7-mime" },
    { ContentTypes::APPLICATION_VND_WAP_HASHED_CERTIFICATE,     "application/vnd.wap.hashed-certificate" },
    { ContentTypes::APPLICATION_VND_WAP_SIGNED_CERTIFICATE,     "application/vnd.wap.signed-certificate" },
    { ContentTypes::APPLICATION_VND_WAP_CERT_RESPONSE,          "application/vnd.wap.cert-response" },
    { ContentTypes::APPLICATION_XHTML_XML,                      "application/xhtml+xml" },
    { ContentTypes::APPLICATION_WML_XML,                        "application/wml+xml" },
    { ContentTypes::TEXT_CSS,                                   "text/css" },
    { ContentTypes::APPLICATION_VND_WAP_MMS_MESSAGE,            "application/vnd.wap.mms-message" },
    { ContentTypes::APPLICATION_VND_WAP_ROLLOVER_CERTIFICATE,   "application/vnd.wap.rollover-certificate" },
    { ContentTypes::APPLICATION_VND_WAP_LOCC_WBXML,             "application/vnd.wap.locc+wbxml" },
    { ContentTypes::APPLICATION_VND_WAP_LOC_XML,                "application/vnd.wap.loc+xml" },
    { ContentTypes::APPLICATION_VND_SYNCML_DM_WBXML,            "application/vnd.syncml.dm+wbxml" },
    { ContentTypes::APPLICATION_VND_SYNCML_DM_XML,              "application/vnd.syncml.dm+xml" },
    { ContentTypes::APPLICATION_VND_SYNCML_NOTIFICATION,        "application/vnd.syncml.notification" },
    { ContentTypes::APPLICATION_VND_WAP_XHTML_XML,              "application/vnd.wap.xhtml+xml" },
    { ContentTypes::APPLICATION_VND_WV_CSP_CIR,                 "application/vnd.wv.csp.cir" },
    { ContentTypes::APPLICATION_VND_OMA_DD_XML,                 "application/vnd.oma.dd+xml" },
    { ContentTypes::APPLICATION_VND_OMA_DRM_MESSAGE,            "application/vnd.oma.drm.message" },
    { ContentTypes::APPLICATION_VND_OMA_DRM_CONTENT,            "application/vnd.oma.drm.content" },
    { ContentTypes::APPLICATION_VND_OMA_DRM_RIGHTS_XML,         "application/vnd.oma.drm.rights+xml" },
    { ContentTypes::APPLICATION_VND_OMA_DRM_RIGHTS_WBXML,       "application/vnd.oma.drm.rights+wbxml" },
    { ContentTypes::APPLICATION_VND_WV_CSP_XML,                 "application/vnd.wv.csp+xml" },
    { ContentTypes::APPLICATION_VND_WV_CSP_WBXML,               "application/vnd.wv.csp+wbxml" },
    { ContentTypes::APPLICATION_VND_SYNCML_DS_NOTIFICATION,     "application/vnd.syncml.ds.notification" },
    { ContentTypes::AUDIO_X,                                    "audio/*" },
    { ContentTypes::VIDEO_X,                                    "video/*" },
    { ContentTypes::APPLICATION_VND_OMA_DD2_XML,                "application/vnd.oma.dd2+xml" },
    { ContentTypes::APPLICATION_MIKEY,                          "application/mikey" },
    { ContentTypes::APPLICATION_VND_OMA_DCD,                    "application/vnd.oma.dcd" },
    { ContentTypes::APPLICATION_VND_OMA_DCDC,                   "application/vnd.oma.dcdc" },
};

class MmsContentType {
public:
    MmsContentType() = default;
    ~MmsContentType() = default;
    MmsContentType(const MmsContentType &srcContentType);
    MmsContentType& operator=(const MmsContentType& srcContentType);
    void DumpMmsContentType();
    bool DecodeMmsContentType(MmsDecodeBuffer &decodeBuffer, int32_t &len);
    bool DecodeMmsCTGeneralForm(MmsDecodeBuffer &decodeBuffer, int32_t &contentLength);
    bool DecodeParameter(MmsDecodeBuffer &decodeBuffer, int32_t valueLength);
    bool DecodeTextField(MmsDecodeBuffer &decodeBuffer, uint8_t field, int32_t &valueLength);
    bool DecodeCharsetField(MmsDecodeBuffer &decodeBuffer, int32_t &valueLength);
    bool DecodeTypeField(MmsDecodeBuffer &decodeBuffer, int32_t &valueLength);

    bool EncodeTextField(MmsEncodeBuffer &encodeBuffer);
    bool EncodeCharsetField(MmsEncodeBuffer &encodeBuffer);
    bool EncodeTypeField(MmsEncodeBuffer &encodeBuffer);
    bool EncodeMmsBodyPartContentType(MmsEncodeBuffer &encodeBuffer);
    bool EncodeMmsBodyPartContentParam(MmsEncodeBuffer &encodeBuffer);
public:
    static int8_t GetContentTypeFromString(std::string str);
    static std::string GetContentTypeFromInt(uint8_t type);
    bool GetContentType(std::string &str);
    bool SetContentType(std::string str);
    bool SetContentParam(MmsContentParam &contentParam);
    MmsContentParam &GetContentParam();
private:
    std::string contentType_;
    MmsContentParam msgContentParm_;
};
} // namespace Telephony
} // namespace OHOS
#endif
