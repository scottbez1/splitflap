#pragma once

#include <Arduino.h>

class Jwt {
    public:
        Jwt(const char* audience, const char* service_key_id, const char* email, uint8_t* private_key, size_t private_key_size);

        String get();

        /**
         * Create a JWT token for GCP.
         * For full details, perform a Google search on JWT.  However, in summary, we build two strings.  One that represents the
         * header and one that represents the payload.  Both are JSON and are as described in the GCP and JWT documentation.  Next
         * we base64url encode both strings.  Note that is distinct from normal/simple base64 encoding.  Once we have a string for
         * the base64url encoding of both header and payload, we concatenate both strings together separated by a ".".   This resulting
         * string is then signed using RSASSA which basically produces an SHA256 message digest that is then signed.  The resulting
         * binary is then itself converted into base64url and concatenated with the previously built base64url combined header and
         * payload and that is our resulting JWT token.
         * @param projectId The GCP project.
         * @param privateKey The PEM or DER of the private key.
         * @param privateKeySize The size in bytes of the private key.
         * @returns A JWT token for transmission to GCP.
         */
        static char* createGCPJWT(const char* audience, const char* service_key_id, const char* email, uint8_t* privateKey, size_t privateKeySize, time_t now);
    
    private:
        const char* audience_;
        const char* service_key_id_;
        const char* email_;

        uint8_t* private_key_;
        size_t private_key_size_;
        time_t last_refresh_ = 0;
        String last_jwt_;

        /**
         * Return a string representation of an mbedtls error code
         */
        static char* mbedtlsError(int errnum);
};
