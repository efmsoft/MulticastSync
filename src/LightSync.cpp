#include <cassert>
#include <LightSync.h>

#include <jsoncpp/json.h>

using namespace asio;
using namespace asio::ip; 
using namespace MulticastSync;

LightSync::LightSync(bool server, asio::io_context& context)
  : Server(server)
  , Socket(context)
  , MulticastEndpoint(address_v4::broadcast(), DESTINATION_PORT)
  , Buffer(BUFFER_SIZE)
  , Playback(false)
  , Position(0)
{
  Socket.open(udp::v4());

  if (server)
  {
    Socket.bind(udp::endpoint(udp::v4(), SERVER_PORT));
    Socket.set_option(asio::socket_base::broadcast(true));
  }
  else
  {
    Socket.set_option(udp::socket::reuse_address(true));
    Socket.bind(udp::endpoint(udp::v4(), DESTINATION_PORT));

    // Join the multicast group.
    auto addr = ip::address::from_string(MULTICAST_GROUP);
    Socket.set_option(ip::multicast::join_group(addr));

    Socket.async_receive_from(
      asio::buffer(&Buffer[0], BUFFER_SIZE)
      , MulticastEndpoint
      , std::bind(
        &LightSync::HandleReceiveFrom
        , this
        , std::placeholders::_1
        , std::placeholders::_2
      )
    );
  }
}

void LightSync::SendState(bool playback, float position)
{
  assert(Server);

  Json::Value message;
  message["playback"] = playback;
  message["position"] = position;

  Json::FastWriter writer;
  std::string data = writer.write(message);

  udp::endpoint to(address_v4::broadcast(), DESTINATION_PORT);
  Socket.send_to(asio::const_buffer(data.c_str(), data.size()), to);
}

void LightSync::GetState(bool& playback, float& position)
{
  assert(!Server);

  std::lock_guard<std::mutex> guard(DataLock);

  playback = Playback;
  position = Position;
}

void LightSync::HandleReceiveFrom(
  asio::error_code error
  , std::size_t bytes_recvd
)
{
  if (!error && bytes_recvd)
  {
    // Store values sent by server

    Json::Value message;
    Json::Reader reader;

    std::string data(&Buffer[0], bytes_recvd);
    if (reader.parse(data, message))
    {
      std::lock_guard<std::mutex> guard(DataLock);

      Playback = message["playback"].asBool();
      Position = message["position"].asFloat();
    }

    // Queue read request

    Socket.async_receive_from(
      asio::buffer(&Buffer[0], BUFFER_SIZE)
      , MulticastEndpoint
      , std::bind(
        &LightSync::HandleReceiveFrom
        , this
        , std::placeholders::_1
        , std::placeholders::_2
      )
    );
  }
}