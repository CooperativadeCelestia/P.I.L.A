#ifndef CLIENT_HTTP_HPP
#define CLIENT_HTTP_HPP

#include "utility.hpp"
#include <limits>
#include <mutex>
#include <random>
#include <unordered_set>
#include <vector>

#ifdef USE_STANDALONE_ASIO
#include <asio.hpp>
#include <asio/steady_timer.hpp>
namespace SimpleWeb {
  using error_code = std::error_code;
  using errc = std::errc;
  using system_error = std::system_error;
  namespace make_error_code = std;
  using string_view = const std::string &; // TODO c++17: use std::string_view
} // namespace SimpleWeb
#else
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/utility/string_ref.hpp>
namespace SimpleWeb {
  namespace asio = boost::asio;
  using error_code = boost::system::error_code;
  namespace errc = boost::system::errc;
  using system_error = boost::system::system_error;
  namespace make_error_code = boost::system::errc;
  using string_view = boost::string_ref;
} // namespace SimpleWeb
#endif

namespace SimpleWeb {
  template <class socket_type>
  class Client;

  template <class socket_type>
  class ClientBase {
  public:
    class Content : public std::istream {
      friend class ClientBase<socket_type>;

    public:
      std::size_t size() noexcept {
        return streambuf.size();
      }
      /// Convenience function to return std::string. The stream buffer is consumed.
      std::string string() noexcept {
        try {
          std::stringstream ss;
          ss << rdbuf();
          return ss.str();
        }
        catch(...) {
          return std::string();
        }
      }

    private:
      asio::streambuf &streambuf;
      Content(asio::streambuf &streambuf) noexcept : std::istream(&streambuf), streambuf(streambuf) {}
    };

    class Response {
      friend class ClientBase<socket_type>;
      friend class Client<socket_type>;

      asio::streambuf streambuf;

      Response(std::size_t max_response_streambuf_size) noexcept : streambuf(max_response_streambuf_size), content(streambuf) {}

    public:
      std::string http_version, status_code;

      Content content;

      CaseInsensitiveMultimap header;
    };

    class Config {
      friend class ClientBase<socket_type>;

    private:
      Config() noexcept {}

    public:
      /// Set timeout on requests in seconds. Default value: 0 (no timeout).
      long timeout = 0;
      /// Set connect timeout in seconds. Default value: 0 (Config::timeout is then used instead).
      long timeout_connect = 0;
      /// Maximum size of response stream buffer. Defaults to architecture maximum.
      /// Reaching this limit will result in a message_size error code.
      std::size_t max_response_streambuf_size = std::numeric_limits<std::size_t>::max();
      /// Set proxy server (server:port)
      std::string proxy_server;
    };

  protected:
    class Connection : public std::enable_shared_from_this<Connection> {
    public:
      template <typename... Args>
      Connection(std::shared_ptr<ScopeRunner> handler_runner, long timeout, Args &&... args) noexcept
          : handler_runner(std::move(handler_runner)), timeout(timeout), socket(new socket_type(std::forward<Args>(args)...)) {}

      std::shared_ptr<ScopeRunner> handler_runner;
      long timeout;

      std::unique_ptr<socket_type> socket; // Socket must be unique_ptr since asio::ssl::stream<asio::ip::tcp::.
#ifdef USE_STANDALONE_ASIO
      asio::steady_timer timer;
#else
      asio::deadline_timer timer;
#endif

      std::shared_ptr<asio::ssl::context> ssl_context;

      std::unique_ptr<asio::streambuf> request;
      std::unique_ptr<asio::streambuf> response;

      asio::ip::tcp::endpoint endpoint;

      void close() noexcept {
        error_code ec;
        socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket->lowest_layer().close();
        timer.cancel();
      }
    };

    std::shared_ptr<ScopeRunner> handler_runner;
    Config config;

    std::shared_ptr<asio::ssl::context> create_ssl_context() {
      auto ssl_context = std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12);
      ssl_context->set_default_verify_paths();
      return ssl_context;
    }

    void connect(const std::shared_ptr<Connection> &connection, const std::shared_ptr<asio::ip::tcp::resolver::iterator> &endpoint_iterator) {
      connection->socket->lowest_layer().async_connect((*endpoint_iterator)->endpoint(), [this, connection, endpoint_iterator](const error_code &ec) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (!ec) {
          if (config.timeout_connect > 0)
            connection->timer.expires_after(std::chrono::seconds(config.timeout_connect));
          else if (config.timeout > 0)
            connection->timer.expires_after(std::chrono::seconds(config.timeout));
          else
            connection->timer.expires_never();
          request_write(connection);
        }
        else if (++(*endpoint_iterator) != asio::ip::tcp::resolver::iterator()) {
          connect(connection, endpoint_iterator);
        }
        else {
          auto response = std::make_shared<Response>(config.max_response_streambuf_size);
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->handler_runner->post([this, connection, response]() {
            connection->callback(*response);
          });
        }
      });
    }

    void handshake(const std::shared_ptr<Connection> &connection) {
      connection->socket->async_handshake(asio::ssl::stream_base::client, [this, connection](const error_code &ec) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (!ec) {
          if (config.timeout > 0)
            connection->timer.expires_after(std::chrono::seconds(config.timeout));
          else
            connection->timer.expires_never();
          request_write(connection);
        }
        else {
          auto response = std::make_shared<Response>(config.max_response_streambuf_size);
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->handler_runner->post([this, connection, response]() {
            connection->callback(*response);
          });
        }
      });
    }

    void write(const std::shared_ptr<Connection> &connection) {
      asio::async_write(*connection->socket, *connection->request, [this, connection](const error_code &ec, std::size_t /*bytes_transferred*/) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (!ec) {
          connection->request.reset();
          response_read(connection);
        }
        else {
          auto response = std::make_shared<Response>(config.max_response_streambuf_size);
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->handler_runner->post([this, connection, response]() {
            connection->callback(*response);
          });
        }
      });
    }

    void read_chunked_transfer_encoded(const std::shared_ptr<Connection> &connection, const std::shared_ptr<Response> &response, const error_code &ec, std::size_t bytes_transferred) {
      if (!ec) {
        auto data = std::make_shared<asio::streambuf>(response->content.size() + bytes_transferred);
        std::ostream stream(data.get());
        stream << response->content.substr(0, response->content.size() - bytes_transferred);
        stream << std::string(std::asio::buffers_begin(response->streambuf.data()), std::asio::buffers_end(response->streambuf.data()));
        response->content.swap(data->data());
        response->streambuf.consume(response->streambuf.size());
        read_chunked_transfer_encoded(connection, response, ec, 0);
      }
      else if (ec != asio::error::eof) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (connection->on_progress)
          connection->on_progress(ec, bytes_transferred, response->content.size());
        response->http_version = "1.1";
        response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
        response->content = "Service unavailable";
        connection->callback(*response);
      }
      else {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (connection->on_progress)
          connection->on_progress(ec, bytes_transferred, response->content.size());
        connection->callback(*response);
      }
    }

    void read_content(const std::shared_ptr<Connection> &connection, const std::shared_ptr<Response> &response, const std::shared_ptr<asio::streambuf> &data, const error_code &ec, std::size_t bytes_transferred) {
      if (!ec) {
        response->content.append(std::asio::buffers_begin(data->data()), std::asio::buffers_begin(data->data()) + bytes_transferred);
        data->consume(bytes_transferred);
        if (connection->on_progress)
          if (!connection->on_progress(ec, bytes_transferred, response->content.size()))
            return;
        if (response->content.size() == response->content.capacity())
          response->content.reserve(response->content.size() + connection->socket->available());
        connection->socket->async_read_some(data->prepare(connection->socket->available()), [this, connection, response, data](const error_code &ec, std::size_t bytes_transferred) {
          read_content(connection, response, data, ec, bytes_transferred);
        });
      }
      else if (ec != asio::error::eof) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (connection->on_progress)
          connection->on_progress(ec, bytes_transferred, response->content.size());
        response->http_version = "1.1";
        response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
        response->content = "Service unavailable";
        connection->callback(*response);
      }
      else if (response->content.size() == response->content.capacity()) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (connection->on_progress)
          connection->on_progress(ec, bytes_transferred, response->content.size());
        response->http_version = "1.1";
        response->status_code = to_string(Response::StatusCode::PayloadTooLarge);
        response->content = "Payload too large";
        connection->callback(*response);
      }
      else {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (connection->on_progress)
          connection->on_progress(ec, bytes_transferred, response->content.size());
        connection->callback(*response);
      }
    }

    void response_read(const std::shared_ptr<Connection> &connection) {
      auto response = std::make_shared<Response>(config.max_response_streambuf_size);
      connection->response = std::make_unique<asio::streambuf>();
      connection->timer.expires_after(std::chrono::seconds(config.timeout));
      asio::async_read_until(*connection->socket, *connection->response, "\r\n\r\n", [this, connection, response](const error_code &ec, std::size_t bytes_transferred) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if ((!ec || ec == asio::error::not_found) && response->http_version_major.empty() && response->http_version_minor == 0 && response->status_code.empty() && response->status_message.empty() && response->header.empty()) {
          std::istream response_stream(connection->response.get());
          parse_response_header(response_stream, *response);
          if (response->content_length > response->content.size()) {
            connection->timer.expires_after(std::chrono::seconds(config.timeout));
            connection->socket->async_read_some(response->content.prepare(response->content.capacity() - response->content.size()), [this, connection, response](const error_code &ec, std::size_t bytes_transferred) {
              read_content(connection, response, response->content, ec, bytes_transferred);
            });
          }
          else if (response->chunked_transfer_encoding) {
            connection->timer.expires_after(std::chrono::seconds(config.timeout));
            asio::async_read_until(*connection->socket, response->streambuf, "\r\n", [this, connection, response](const error_code &ec, std::size_t bytes_transferred) {
              if (!ec) {
                std::istream istream(&response->streambuf);
                std::string line;
                std::getline(istream, line);
                line.pop_back();
                std::size_t length = 0;
                std::stringstream(line) >> std::hex >> length;
                if (length > 0) {
                  connection->timer.expires_after(std::chrono::seconds(config.timeout));
                  asio::async_read(*connection->socket, response->streambuf, asio::transfer_exactly(length + 2), [this, connection, response, length](const error_code &ec, std::size_t bytes_transferred) {
                    read_chunked_transfer_encoded(connection, response, ec, bytes_transferred);
                  });
                }
              }
              else {
                read_chunked_transfer_encoded(connection, response, ec, bytes_transferred);
              }
            });
          }
          else {
            connection->timer.expires_never();
            connection->callback(*response);
          }
        }
        else if ((!ec || ec == asio::error::not_found) && response->content_length > 0) {
          connection->timer.expires_after(std::chrono::seconds(config.timeout));
          read_content(connection, response, connection->response, ec, bytes_transferred);
        }
        else if (!ec) {
          auto lock = connection->handler_runner->continue_lock();
          if (!lock)
            return;
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->callback(*response);
        }
        else {
          auto lock = connection->handler_runner->continue_lock();
          if (!lock)
            return;
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->callback(*response);
        }
      });
    }

    void request_write(const std::shared_ptr<Connection> &connection) {
      asio::async_write(*connection->socket, connection->request->data(), [this, connection](const error_code &ec, std::size_t /*bytes_transferred*/) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock)
          return;
        if (!ec) {
          if (connection->request->size() == 0) {
            connection->timer.expires_never();
            response_read(connection);
          }
          else {
            connection->request->consume(connection->request->size());
            if (config.timeout > 0)
              connection->timer.expires_after(std::chrono::seconds(config.timeout));
            else
              connection->timer.expires_never();
            request_write(connection);
          }
        }
        else {
          auto response = std::make_shared<Response>(config.max_response_streambuf_size);
          response->http_version = "1.1";
          response->status_code = to_string(Response::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->callback(*response);
        }
      });
    }
  };

  class HttpClient {
  public:
    HttpClient(std::size_t thread_pool_size)
        : strand_(ioc_)
        , work_(std::make_unique<asio::io_context::work>(ioc_))
    {
      for (std::size_t i = 0; i < thread_pool_size; ++i) {
        threads_.emplace_back([this] {
          ioc_.run();
        });
      }
    }

    ~HttpClient() {
      work_.reset();
      for (auto &thread : threads_) {
        thread.join();
      }
    }

    template<class RequestBody, class RequestSerializer, class ResponseBody, class ResponseParser>
    std::shared_ptr<Connection> request(const std::string &host,
                                        const std::string &port,
                                        const std::string &target,
                                        const std::shared_ptr<RequestBody> &body,
                                        RequestSerializer &serializer,
                                        const std::shared_ptr<asio::ssl::context> &ssl_context,
                                        ResponseParser &parser,
                                        const std::function<void(const asio::error_code &ec, const std::shared_ptr<Response<ResponseBody>> &response)> &callback,
                                        const std::size_t redirection_count) {
      auto connection = std::make_shared<Connection>(host, port, target, redirection_count);
      connection->handler_runner = std::make_shared<ScopeRunner>();
      connection->ssl_context = ssl_context;
      connection->callback = [this, connection, parser, callback](const Response<ResponseBody> &response) {
        const auto redirect_host = detail::parse_host(response);
        if (redirect_host) {
          const auto redirect_port = detail::parse_port(response);
          const auto redirect_target = detail::parse_target(response);
          if (redirect_port && redirect_target) {
            if (redirect_host.value() == connection->host &&
                redirect_port.value() == connection->port &&
                redirect_target.value() == connection->target) {
              if (callback)
                callback(asio::error::make_error_code(asio::error::permission_denied), nullptr);
              return;
            }
            const auto redirect_redirection_count = connection->redirection_count + 1;
            if (config.max_redirects > 0 && redirect_redirection_count > config.max_redirects) {
              if (callback)
                callback(asio::error::make_error_code(asio::error::permission_denied), nullptr);
              return;
            }
            request(redirect_host.value(), redirect_port.value(), redirect_target.value(), connection->body, serializer, connection->ssl_context, parser, callback, redirect_redirection_count);
            return;
          }
        }
        if (callback) {
          auto response_shared = std::make_shared<Response<ResponseBody>>(response);
          response_shared->content = std::move(response.content);
          response_shared->streambuf = std::move(response.streambuf);
          callback(asio::error_code(), response_shared);
        }
      };
      {
        std::ostream write_stream(connection->request.get());
        serializer(connection->target, write_stream);
      }
      if (connection->ssl_context) {
        connection->socket = std::make_shared<asio::ssl::stream<asio::ip::tcp::socket>>(ioc_, *connection->ssl_context);
      }
      else {
        connection->socket = std::make_shared<asio::ip::tcp::socket>(ioc_);
      }
      connection->socket->async_connect(connection->endpoint, [this, connection](const error_code &ec) {
        if (!ec) {
          if (connection->ssl_context) {
            connection->socket->async_handshake(asio::ssl::stream_base::client, [this, connection](const error_code &ec) {
              if (!ec) {
                asio::async_write(*connection->socket, connection->request->data(), [this, connection](const error_code &ec, std::size_t /*bytes_transferred*/) {
                  if (!ec) {
                    if (connection->request->size() == 0) {
                      response(connection);
                    }
                    else {
                      connection->request->consume(connection->request->size());
                      request(connection);
                    }
                  }
                  else if (connection->callback) {
                    auto response = std::make_shared<Response<ResponseBody>>(Response<ResponseBody>());
                    response->http_version = "1.1";
                    response->status_code = to_string(Response<ResponseBody>::StatusCode::ServiceUnavailable);
                    response->content = "Service unavailable";
                    connection->callback(ec, response);
                  }
                });
              }
              else if (connection->callback) {
                auto response = std::make_shared<Response<ResponseBody>>(Response<ResponseBody>());
                response->http_version = "1.1";
                response->status_code = to_string(Response<ResponseBody>::StatusCode::ServiceUnavailable);
                response->content = "Service unavailable";
                connection->callback(ec, response);
              }
            });
          }
          else {
            asio::async_write(*connection->socket, connection->request->data(), [this, connection](const error_code &ec, std::size_t /*bytes_transferred*/) {
              if (!ec) {
                if (connection->request->size() == 0) {
                  response(connection);
                }
                else {
                  connection->request->consume(connection->request->size());
                  request(connection);
                }
              }
              else if (connection->callback) {
                auto response = std::make_shared<Response<ResponseBody>>(Response<ResponseBody>());
                response->http_version = "1.1";
                response->status_code = to_string(Response<ResponseBody>::StatusCode::ServiceUnavailable);
                response->content = "Service unavailable";
                connection->callback(ec, response);
              }
            });
          }
        }
        else if (connection->callback) {
          auto response = std::make_shared<Response<ResponseBody>>(Response<ResponseBody>());
          response->http_version = "1.1";
          response->status_code = to_string(Response<ResponseBody>::StatusCode::ServiceUnavailable);
          response->content = "Service unavailable";
          connection->callback(ec, response);
        }
      });
      return connection;
    }

    template<class RequestBody, class RequestSerializer, class ResponseBody, class ResponseParser>
    std::shared_ptr<Connection> request(const std::string &host,
                                        const std::string &port,
                                        const std::string &target,
                                        const std::shared_ptr<RequestBody> &body,
                                        RequestSerializer &serializer,
                                        ResponseParser &parser,
                                        const std::function<void(const asio::error_code &ec, const std::shared_ptr<Response<ResponseBody>> &response)> &callback,
                                        const std::size_t redirection_count = 0) {
      return request(host, port, target, body, serializer, nullptr, parser, callback, redirection_count);
    }

    std::shared_ptr<Connection> request(const std::string &host,
                                        const std::string &port,
                                        const std::string &target,
                                        const std::function<void(const asio::error_code &ec, const std::shared_ptr<Response<std::string>> &response)> &callback,
                                        const std::string &body,
                                        const std::string &content_type,
                                        const std::string &method) {
      auto header = std::make_shared<Request::Headers>();
      header->emplace("Host", host);
      header->emplace("Content-Type", content_type);
      header->emplace("Content-Length", std::to_string(body.size()));

      Request request;
      request.method = method;
      request.target = target;
      request.headers = header;
      request.body = std::make_shared<std::string>(body);

      return request(host, port, target, request.body, Request::Serializer(), Response<std::string>::Parser(), callback);
    }

    void stop() {
      ioc_.stop();
    }

    void join() {
      for (auto &thread : threads_) {
        thread.join();
      }
    }

  private:
    asio::io_context ioc_;
    asio::io_context::strand strand_;
    std::unique_ptr<asio::io_context::work> work_;
    std::vector<std::thread> threads_;
  };

} // namespace SimpleWeb

#endif // SIMPLE_WEB_CLIENT_HPP
