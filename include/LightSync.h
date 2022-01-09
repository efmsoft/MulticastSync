#pragma once

#include <asio/asio.hpp>
#include <memory>
#include <mutex>

namespace MulticastSync
{
  class LightSync
  {
    bool Server;
    asio::ip::udp::socket Socket;
    asio::ip::udp::endpoint MulticastEndpoint;
    std::vector<char> Buffer;

    std::mutex DataLock;
    bool Playback;
    float Position;

    constexpr static const uint16_t DESTINATION_PORT = 10001;
    constexpr static const uint16_t SERVER_PORT = 10000;
    constexpr static const size_t BUFFER_SIZE = 512;
    constexpr static const char* MULTICAST_GROUP = "224.0.0.0";

  public:
    LightSync(bool server, asio::io_context& context);

    void GetState(bool& playback, float& position);
    void SendState(bool playback, float position);

  private:
    void HandleReceiveFrom(asio::error_code error, std::size_t bytes_recvd);
  };

  typedef std::shared_ptr<LightSync> LightSyncPtr;
}