#ifndef __BOT_CLIENT__
#define __BOT_CLIENT__

class BotClient {
  private:
    const char *login;
    const char *password;

  public:
    BotClient(const char *login, const char *password);
    /**
     * @brief Sends message to Mqtt2TelegramBot via http
     * 
     * @param topic 
     * @param payload 
     */
    void send_message(const char *topic, const char *payload);

};

#endif