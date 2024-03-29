#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "LoRa_ESP32_WiFi"                         //change this
 
const char WIFI_SSID[] = "VUPlay";               //change this
const char WIFI_PASSWORD[] = "vuplay123";           //change this
const char AWS_IOT_ENDPOINT[] = "a17e3zzs7ea43h-ats.iot.us-east-1.amazonaws.com";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVALZuAVdTheEoC5HGbG7DTK9/IpxsMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzAxMjgxNTIw
MTNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCm6XuJe1LJ3W4oojzL
K1Sx5J+Lu99xpXT5Lbxf7FrTPGEgzdxq8ewxNfBx+SQUCLB4R5cptY+Sck2RD0MO
uzAVjqytH+7sltUQdFzHkhlUz9OEVQFJXsJ2MwmP/AFXEanU0+3WJ4zY/niWwujO
lJCBcbL31PABou3QOZPZPIcy6ut8tR6HJDLXAbH73ZM+92ofzeHsEtEWk5QNhWAh
c3pSqj9plavMw4RgxBNRz8ZDeS+OTBTb6orMDRBWnhbjK+27uGZIVxokMGnoah2K
ErNhA0Dd7sfdfMIG/YyZGcPSUqKF30XWa+eYO7nOc3zU/qFH2gZepYT5cVm+5ZvY
4empAgMBAAGjYDBeMB8GA1UdIwQYMBaAFAF98/TIJtUrYzNlZejlgpXEkRzzMB0G
A1UdDgQWBBTSOU2PZJUKEsr0VC95xirNadWtlzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAeMjI90pdLDY3g09iAqBNFBh5
YvpJ743afX7FVbGPyugFTyzu181pB1RRUT2EwwlcUT6my8dXNTVG/mW7+kdCt4rD
f3N7RCoQ1cOP2vrVDk/OkAijeIm6yR4s4vv0N3eXkQqeK2kGgwFe91a2Z2uVoWxG
K8oLv0X1B6IRej2HBPC64R/MqoJwJSekhEeAk5uViUNiFvolc2rcBHhgrkQr6XUW
aQktuyy3/wMraxr/wkXmljwgAb3jV3rEU63R3/5n70LvaueLMIP1lATZuQsCmxvS
lIIgkqD/UDgJyyra3TLDTz9ibrJecJVlWWulZIn/5FWSsns/ruNwRmx/2pAmJw==
-----END CERTIFICATE-----
 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEApul7iXtSyd1uKKI8yytUseSfi7vfcaV0+S28X+xa0zxhIM3c
avHsMTXwcfkkFAiweEeXKbWPknJNkQ9DDrswFY6srR/u7JbVEHRcx5IZVM/ThFUB
SV7CdjMJj/wBVxGp1NPt1ieM2P54lsLozpSQgXGy99TwAaLt0DmT2TyHMurrfLUe
hyQy1wGx+92TPvdqH83h7BLRFpOUDYVgIXN6Uqo/aZWrzMOEYMQTUc/GQ3kvjkwU
2+qKzA0QVp4W4yvtu7hmSFcaJDBp6GodihKzYQNA3e7H3XzCBv2MmRnD0lKihd9F
1mvnmDu5znN81P6hR9oGXqWE+XFZvuWb2OHpqQIDAQABAoIBAFHxyPvLNu7yLjX2
yJ/6av9XPdkNaHrT/2SiRpxVXpXcS9ceGjvK/+l9EMnzxfmr9m9mvlj00ULVqS0x
b/uYfNfojvAEAHnY50vHxREZsiQIUFDkptgNlK0RC33L/CByBI3/agG8sxKJhEyH
7mIVYAic+H9h5F75TYXv7p01cS7WibvHSbM4Hmm/ieW40Nv48rkwMEEbexMEnZgL
0C6Ihakc8kr6HMBKQbSmemNbl1uN0OBxpt8xPmAYacuEeDjbRaf5vqMVFUSIl5UL
gS4pDxDexyMiYNs81ue6Ujs1cTpU1gaeOceiZZVfVr90NM4rcnef+5hTt/79Sl9C
+Q4AK20CgYEA0mhr5N6dtijNIqC7Dn2D2Jglf39wegaiBzMJk5yu1YPhbxHG0n7T
E7oPnVBZVI2XxzhBZuVL/0qVPAbb43ES6au8BGjbiloP38ZuXHpmwk7CHhgowUW/
Fqx2a7wDAobKDKWy9XnSsbw63K8kFvSVVN2Eyw7rAPy95Tj8KPDwaXcCgYEAyxRL
N++HzYLyPtebwUIsS/MIAIR1PcO/KjQiUYQ6vYoXW2g4EU6VaO8jqyHq14dLO77x
nTVOvKJkylOjpm4luewC6uzDpxLDVBJAtAoDPxXR97ySXUYV/DgG1pn8iPLyC71O
RukXvZ1k9p/fFw6AUcsap35Ln5kz7PrckJV9Dd8CgYASm7GPGDESfyFFQrnIWkli
ynHmiLrFz/hccqMSLvU3ZOZ3EDdpPlYklHv1CbocxDj8IXmHRBgoh8wS9vhTcRif
GdUS/sMLxVNKEx6Hhzu3S4fXaYORND3mH3KJrKMHgiw0WiMtmoyLs33xLnwem5rp
Wn34B2W5PKD+uL90T3qzmQKBgQC83yr14vjS0i6q86wQmZLJMEcI9Ch2QU/R2Jry
ZaCcF4qRoOREdGnRJlhuGSVpPpNiBbyBEcsg9Yf4BMH3UAoWWMJ966YsBO9ViYRT
7dwHGCwn1GHsDfr0kMcLYDvWwt0Kvg3HNZK272OvXBEISJxfdh5cGMIMtVft14K7
lcyw0QKBgQCGaeThvaGVj82ylSg/JMDdXWzT6wsDBKLcioM8OS3wXGRvoDct+ZN7
T0d+OApWg5yOWDhqy0LyN6yJJKqkiGN5TS+oZn0XBcTMsvUMo+9WHUpuwmG3EfaA
xLf7aU+2ZgSwJxZDF1y33ixOIsGcIorhaYN/2Jr4d/4Q3d2pqeHHOQ==
-----END RSA PRIVATE KEY-----
 
 
)KEY";
