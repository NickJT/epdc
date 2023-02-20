#include "ntpClient.h"
#include <iostream>

ntpClient::ntpClient()
{
    std::cout << "Created ntpClient" << std::endl;
}

ntp_result_t ntpClient::getNTPTime(void)
{
/* if (auto err{wifiConnected()}; err != PICO_OK)
     {
         std::cout << "Unable to connect to " << WIFI_SSID << " [err " << err << "]" << std::endl;
         return ntp_result_t{err, 0};
     }
 */
    if (auto err{initInterface()}; err != PICO_OK)
    {
        std::cout << "Unable to initialise interface " << std::endl;
        return ntp_result_t{err, 0};
    }
    std::cout << "Initialised interface" << std::endl;

    if (auto err{setRequestTimeout()}; err != PICO_OK)
    {
        std::cout << "Unable set request timeout" << std::endl;
        return ntp_result_t{err, 0};
    }
    std::cout << "Set request timeout on timer id: " << nstate.ntp_alarm_id << std::endl;

    int lookup{getHostAddress(&nstate)};
    if (lookup == ERR_OK) // Then we won't go to ntp_request via the dns callback
    {
        std::cout << "Using cached IP address: " << ip4addr_ntoa(&nstate.ntp_server_address) << std::endl;
        ntp_request(&nstate);
    }
    else if (lookup != ERR_INPROGRESS) // Then we won't go to ntpComplete via the callback
    {
        std::cout << "Unable get NTP host address (err= " << lookup << ")" << std::endl;
        ntpComplete(&nstate, PICO_ERROR_NO_DATA, static_cast<time_t>(0));
    }

    // if the dns request is still in progress we poll until one of the callbacks is called
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    while (nstate.ntp_request_sent)
    {
        sleep_ms(1);
        cyw43_arch_poll();
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    return ntp_result_t{nstate.ntp_err, nstate.ntp_result};
}

int ntpClient::initInterface()
{
    nstate.ntp_pcb = udp_new_ip_type(IPADDR_TYPE_V4);
    if (nstate.ntp_pcb == NULL)
    {
        std::cout << "udp_new_ip_type returns null" << std::endl;
        return PICO_ERROR_NO_DATA;
    }
    udp_recv(nstate.ntp_pcb, ntp_recv, &nstate);
    return PICO_OK;
}

int ntpClient::wifiConnected()
{
    if (auto err{cyw43_arch_init()}; err != pico_error::code::PICO_OK)
    {
        std::cout << "Failed to initialise cyw43 (" << pico_error::toString(err) << ")" << std::endl;
        return err;
    }
    cyw43_arch_enable_sta_mode();

    if (auto err{
            cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)};
        err != pico_error::code::PICO_OK)
    {
        std::cout << "Connect timeout (err=" << err << ")" << std::endl;
        return err;
    }
    else
    {
        std::cout << "Connected to " << WIFI_SSID << "\n\r" << std::endl;
    }
    return PICO_OK;
}

// Set an alarm in case our request gets lost in the post.
// Set ntp_request_sent = true. The timeoutHandler will reset
// the dns_request_pending and the ntp_request_sent flag when it triggers
int ntpClient::setRequestTimeout()
{
    nstate.ntp_alarm_id = add_alarm_in_ms(NTP_TIMEOUT_MS, timeoutHandler, &nstate, true);
    if (nstate.ntp_alarm_id < 1)
    {
        std::cout << "Couldn't set request alarm (response was: " << nstate.ntp_alarm_id << std::endl;
        return PICO_ERROR_NOT_PERMITTED;
    }
    return PICO_OK;
}

/**
 * @brief Gets dns ip address from host name using an lwip function with a callback to
 * ntp_dns_found. The polling loop is cancelled if the overall request timesout (i.e. the alarm triggers).
 *  Exit conditions:
    1) ERR_OK (we have the ipaddress)
    2) request_pending is false (having been cancelled by the timeout alarm)
    3) ERR_ARG :dns client not initialized or invalid hostname
 * @return ERR_OK - if the ip address was already in cache and has been written to nstate
 * @return Other error codes if something went wrong and ntp_dns_found will not be called
 */
int ntpClient::getHostAddress(ntp_t *state)
{
    state->ntp_request_sent = true;
    std::cout << "request sent = " << std::boolalpha << state->ntp_request_sent << std::endl;
    cyw43_arch_lwip_begin();
    int dnsStatus{dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state)};
    cyw43_arch_lwip_end();
    return dnsStatus;
}

int ntpClient::ntp_request(ntp_t *state)
{
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    auto lwip_err = udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
    pbuf_free(p);
    cyw43_arch_lwip_end();
    return lwip_err;
}

/**
 * @brief - The last method in the NTP time sequence before returning to the calling client. Can be
 * called by the UDP packet recieved method (ntp_recv) or the ntp timeout handler. Resets the ntp_request
 * pending flag, cancels the timeout timer and de-initialised the wireless subsystem. Then sets the
 * result status code and the time_t result if the status code is PICO_OK
 * @param state  - A pointer to the state we have maintained throughout the call
 * @param status - The oveall closing status of the ntp query (PICO_OK if all went well)
 * @param result - Finally, the time_t from the NTP servers
 */
void ntpClient::ntpComplete(ntp_t *state, int status, time_t result)
{
    state->ntp_request_sent = false;
    std::cout << "request sent = " << std::boolalpha << state->ntp_request_sent << std::endl;
    if (state->ntp_alarm_id > 0) // if the alarm was set then the id > 0
    {
        cancel_alarm(state->ntp_alarm_id);
        state->ntp_alarm_id = 0;
    }
    state->ntp_err = status;
    state->ntp_result = result;
    udp_remove(state->ntp_pcb);
}

/**
 * @brief Callback - Called by lwip when the dns host ip address is resolved, or when we fail to
 * resolve the address, or when the request times out
 * @param hostname - the name of the host we have resolved
 * @param ipaddr - the ip address of hostname
 * @param arg - the nstate struct as it was when we made the dns request
 */
void ntpClient::ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    ntp_t *state = (ntp_t *)arg;
    if (ipaddr)
    {
        state->ntp_server_address = *ipaddr;
        std::cout << "NTP host: " << NTP_SERVER << " at " << ip4addr_ntoa(&state->ntp_server_address) << std::endl;
        if (IP_GET_TYPE(&state->ntp_server_address) != IPADDR_TYPE_V4)
        {
            std::cout << "NTP host: " << ip4addr_ntoa(&state->ntp_server_address) << " is not IPV4" << std::endl;
        }
        ntp_request(state);
    }
    else
    {
        std::cout << "Could not resolve NTP server address " << NTP_SERVER << std::endl;
        ntpComplete(state, PICO_ERROR_NO_DATA, static_cast<time_t>(0));
    }
}

/**
 * @brief Callback - Called by lwip when we receive a UDP packet. Originally setup in the initInterface
 * call with udp_recv(nstate.ntp_pcb, ntp_recv, &nstate)
 * @param arg
 * @param pcb
 * @param p
 * @param addr
 * @param port
 */
void ntpClient::ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    ntp_t *state = (ntp_t *)arg;
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Have we got the right UDP packet?
    if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum != 0)
    {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        time_t result{makeTime_t(seconds_buf)};
        // std::cout << "[ntp_recv]: " << ctime(&result) << std::endl;
        ntpComplete(state, PICO_OK, result);
    }
    else
    {
        ntpComplete(state, PICO_ERROR_INVALID_ARG, static_cast<time_t>(0));
    }
    pbuf_free(p);
}

/**
 * @brief Callback - called if our alarm triggers before being cancelled in ntp_result
 *
 */
int64_t ntpClient::timeoutHandler(alarm_id_t id, void *user_data)
{
    ntp_t *state = (ntp_t *)user_data;
    std::cout << "Timeout" << std::endl;
    ntpComplete(state, PICO_ERROR_TIMEOUT, static_cast<time_t>(0));
    return 0;
}

time_t ntpClient::makeTime_t(const uint8_t seconds[4])
{
    uint32_t seconds_since_1900 = seconds[0] << 24 | seconds[1] << 16 | seconds[2] << 8 | seconds[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
    return static_cast<time_t>(seconds_since_1970);
}

ntpClient::~ntpClient()
{
    std::cout << "ntp_alarm_id: " << nstate.ntp_alarm_id << std::endl;
    std::cout << "ntp_pcb: " << nstate.ntp_pcb << std::endl;
    std::cout << "ntp_request_sent: " << nstate.ntp_request_sent << std::endl;
    std::cout << "ntp_server_address: " << ip4addr_ntoa(&nstate.ntp_server_address) << std::endl;
    std::cout << "ntp_err: " << nstate.ntp_err << std::endl;
    std::cout << "ntp_result: " << ctime(&nstate.ntp_result);

    std::cout << "Destructing ntpClient" << std::endl;
}