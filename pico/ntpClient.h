#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/util/datetime.h"

#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include <ctime>
#include "errorCodes.h"

#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800        // seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_TIMEOUT_MS (60 * 1000)  // max wait for an NTP response

typedef struct
{
    ip_addr_t ntp_server_address;
    bool ntp_request_sent;
    struct udp_pcb *ntp_pcb;
    time_t ntp_result;
    alarm_id_t ntp_alarm_id;
    int ntp_err;
} ntp_t;

typedef std::pair<int, time_t> ntp_result_t;

class ntpClient
{
private:
    ntp_t nstate;

    int wifiConnected();
    int initInterface();
    int setRequestTimeout();

    static int getHostAddress(ntp_t *state);
    static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);
    static int ntp_request(ntp_t *state);
    static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
    static void ntpComplete(ntp_t *state, int status, time_t result);
    static int64_t timeoutHandler(alarm_id_t id, void *user_data);
    static time_t makeTime_t(const uint8_t seconds[4]);

public:
    explicit ntpClient();
    ntp_result_t getNTPTime(void);

    ~ntpClient();
};
