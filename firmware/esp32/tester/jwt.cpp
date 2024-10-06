#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

#include "jwt.h"
#include "base64url.h"

Jwt::Jwt(const char* audience, const char* service_key_id, const char* email, uint8_t* private_key, size_t private_key_size) : audience_(audience), service_key_id_(service_key_id), email_(email), private_key_(private_key), private_key_size_(private_key_size) {}

String Jwt::get() {
    time_t now = time(NULL);
    if (now - last_refresh_ > 50 * 60) {
        Serial.println("Regenerating JWT...");
        uint32_t start = millis();
        char* jwt = createGCPJWT(audience_, service_key_id_, email_, private_key_, private_key_size_, now);
        if (jwt == nullptr) {
            return "";
        } else {
            last_refresh_ = now;
            last_jwt_ = String(jwt);
            free(jwt);
            Serial.printf("Finished JWT in %lu millis.\n", millis() - start);
            return last_jwt_;
        }
    } else {
        return last_jwt_;
    }
}

char* Jwt::mbedtlsError(int errnum) {
    static char buffer[200];
    mbedtls_strerror(errnum, buffer, sizeof(buffer));
    return buffer;
} // mbedtlsError


char* Jwt::createGCPJWT(const char* audience, const char* service_key_id, const char* email, uint8_t* privateKey, size_t privateKeySize, time_t now) {
    char base64Header[100];
    char header[100];
    sprintf(header, "{\"alg\":\"RS256\",\"typ\":\"JWT\",\"kid\":\"%s\"}", service_key_id);
    base64url_encode(
        (unsigned char *)header,   // Data to encode.
        strlen(header),            // Length of data to encode.
        base64Header);             // Base64 encoded data.

    uint32_t iat = now;              // Set the time now.
    uint32_t exp = iat + 60*60;      // Set the expiry time.

    char payload[400];
    sprintf(payload, "{\"iat\":%d,\"exp\":%d,\"aud\":\"%s\",\"iss\":\"%s\",\"sub\":\"%s\"}", iat, exp, audience, email, email);

    char base64Payload[400];
    base64url_encode(
        (unsigned char *)payload,  // Data to encode.
        strlen(payload),           // Length of data to encode.
        base64Payload);            // Base64 encoded data.

    uint8_t headerAndPayload[800];
    sprintf((char*)headerAndPayload, "%s.%s", base64Header, base64Payload);

    // At this point we have created the header and payload parts, converted both to base64 and concatenated them
    // together as a single string.  Now we need to sign them using RSASSA

    mbedtls_pk_context pk_context;
    mbedtls_pk_init(&pk_context);
    int rc = mbedtls_pk_parse_key(&pk_context, privateKey, privateKeySize, NULL, 0);
    if (rc != 0) {
        printf("Failed to mbedtls_pk_parse_key: %d (-0x%x): %s\n", rc, -rc, mbedtlsError(rc));
        return nullptr;
    }

    uint8_t oBuf[5000];

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    const char* pers="MyEntropy";
                
    mbedtls_ctr_drbg_seed(
        &ctr_drbg,
        mbedtls_entropy_func,
        &entropy,
        (const unsigned char*)pers,
        strlen(pers));
    

    uint8_t digest[32];
    rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), headerAndPayload, strlen((char*)headerAndPayload), digest);
    if (rc != 0) {
        printf("Failed to mbedtls_md: %d (-0x%x): %s\n", rc, -rc, mbedtlsError(rc));
        return nullptr;        
    }

    size_t retSize;
    rc = mbedtls_pk_sign(&pk_context, MBEDTLS_MD_SHA256, digest, sizeof(digest), oBuf, &retSize, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (rc != 0) {
        printf("Failed to mbedtls_pk_sign: %d (-0x%x): %s\n", rc, -rc, mbedtlsError(rc));
        return nullptr;        
    }


    char base64Signature[600];
    base64url_encode((unsigned char *)oBuf, retSize, base64Signature);

    char* retData = (char*)malloc(strlen((char*)headerAndPayload) + 1 + strlen((char*)base64Signature) + 1);

    sprintf(retData, "%s.%s", headerAndPayload, base64Signature);

    mbedtls_pk_free(&pk_context);
    return retData;
}
