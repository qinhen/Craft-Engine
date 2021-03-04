#pragma once

#include "HttpCommon.h"

namespace CraftEngine
{
    namespace net
    {

        namespace http
        {

            /*
             
             
            */
            class HttpRequest {
            public:
                explicit HttpRequest();

                explicit HttpRequest(
                    const std::string& client_cert_path,
                    const std::string& client_key_path
                );

                virtual ~HttpRequest();

                virtual bool is_valid() const;

                std::shared_ptr<Response> Get(const char* url);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers);

                std::shared_ptr<Response> Get(const char* url, Progress progress);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers,
                    Progress progress);

                std::shared_ptr<Response> Get(const char* url,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response>
                    Get(const char* path, ContentReceiver content_receiver, Progress progress);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers,
                    ContentReceiver content_receiver,
                    Progress progress);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response> Get(const char* url, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver,
                    Progress progress);

                std::shared_ptr<Response> Head(const char* url);

                std::shared_ptr<Response> Head(const char* url, const Headers& headers);

                std::shared_ptr<Response> Post(const char* url);

                std::shared_ptr<Response> Post(const char* url, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* url, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* url, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* url, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* url, const Params& params);

                std::shared_ptr<Response> Post(const char* url, const Headers& headers,
                    const Params& params);

                std::shared_ptr<Response> Post(const char* url,
                    const MultipartFormDataItems& items);

                std::shared_ptr<Response> Post(const char* url, const Headers& headers,
                    const MultipartFormDataItems& items);

                std::shared_ptr<Response> Put(const char* url);

                std::shared_ptr<Response> Put(const char* url, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* url, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* url, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* url, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* url, const Params& params);

                std::shared_ptr<Response> Put(const char* url, const Headers& headers,
                    const Params& params);

                std::shared_ptr<Response> Patch(const char* url, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* url, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* url, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* url, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Delete(const char* url);

                std::shared_ptr<Response> Delete(const char* url, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Delete(const char* url, const Headers& headers);

                std::shared_ptr<Response> Delete(const char* url, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Options(const char* url);

                std::shared_ptr<Response> Options(const char* url, const Headers& headers);

                bool send(const std::string& host, int port, const Request& req, Response& res);

                bool send(const std::string& host, int port,
                    const std::vector<Request>& requests,
                    std::vector<Response>& responses);

                void stop();

                void set_connection_timeout(time_t sec, time_t usec = 0);
                void set_read_timeout(time_t sec, time_t usec = 0);
                void set_write_timeout(time_t sec, time_t usec = 0);

                void set_keep_alive_max_count(size_t count);

                void set_basic_auth(const char* username, const char* password);

                void set_follow_location(bool on);

                void set_compress(bool on);

                void set_decompress(bool on);

                void set_interface(const char* intf);

                void set_proxy_basic_auth(const char* username, const char* password);

                void set_logger(Logger logger);

            protected:
                bool process_request(const std::string& host, int port, Stream& strm, const Request& req, Response& res,
                    bool last_connection, bool& connection_close);

                std::atomic<socket_t> sock_;


                // Settings
                std::string client_cert_path_;
                std::string client_key_path_;

                time_t connection_timeout_sec_ = detail::CRAFT_ENGINE_NET_HTTP_CONNECTION_TIMEOUT_SECOND;
                time_t connection_timeout_usec_ = detail::CRAFT_ENGINE_NET_HTTP_CONNECTION_TIMEOUT_USECOND;
                time_t read_timeout_sec_ = detail::CRAFT_ENGINE_NET_HTTP_READ_TIMEOUT_SECOND;
                time_t read_timeout_usec_ = detail::CRAFT_ENGINE_NET_HTTP_TIMEOUT_USECOND;
                time_t write_timeout_sec_ = detail::CRAFT_ENGINE_NET_HTTP_WRITE_TIMEOUT_SECOND;
                time_t write_timeout_usec_ = detail::CRAFT_ENGINE_NET_HTTP_TIMEOUT_USECOND;
                size_t keep_alive_max_count_ = detail::CRAFT_ENGINE_NET_HTTP_KEEPALIVE_MAX_COUNT;


                std::string basic_auth_username_;
                std::string basic_auth_password_;

                bool follow_location_ = false;

                bool compress_ = false;
                bool decompress_ = true;

                std::string interface_;

                std::string proxy_basic_auth_username_;
                std::string proxy_basic_auth_password_;

                Logger logger_;

                void copy_settings(const HttpRequest& rhs) {
                    client_cert_path_ = rhs.client_cert_path_;
                    client_key_path_ = rhs.client_key_path_;
                    connection_timeout_sec_ = rhs.connection_timeout_sec_;
                    read_timeout_sec_ = rhs.read_timeout_sec_;
                    read_timeout_usec_ = rhs.read_timeout_usec_;
                    write_timeout_sec_ = rhs.write_timeout_sec_;
                    write_timeout_usec_ = rhs.write_timeout_usec_;
                    keep_alive_max_count_ = rhs.keep_alive_max_count_;
                    basic_auth_username_ = rhs.basic_auth_username_;
                    basic_auth_password_ = rhs.basic_auth_password_;
                    follow_location_ = rhs.follow_location_;
                    compress_ = rhs.compress_;
                    decompress_ = rhs.decompress_;
                    interface_ = rhs.interface_;
                    proxy_basic_auth_username_ = rhs.proxy_basic_auth_username_;
                    proxy_basic_auth_password_ = rhs.proxy_basic_auth_password_;
                    logger_ = rhs.logger_;
                }

            private:

                bool prase_url(const char* url, std::string& host, int& port, std::string& path);
                socket_t create_client_socket(const std::string& host, int port)const;
                bool read_response_line(Stream& strm, Response& res);
                bool write_request(const std::string& host, int port, Stream& strm, const Request& req, bool last_connection);
                bool redirect(const std::string& host, int port, const Request& req, Response& res);
                bool handle_request(const std::string& host, int port, Stream& strm, const Request& req, Response& res,
                    bool last_connection, bool& connection_close);

                std::shared_ptr<Response> send_with_content_provider(
                    const std::string& host, int port,
                    const char* method, const char* path, const Headers& headers,
                    const std::string& body, size_t content_length,
                    ContentProvider content_provider, const char* content_type);

                virtual bool process_and_close_socket(
                    socket_t sock, size_t request_count,
                    std::function<bool(Stream& strm, bool last_connection,
                        bool& connection_close)>
                    callback);

                virtual bool is_ssl() const;
            };



            // HTTP client implementation
            inline HttpRequest::HttpRequest()
                : HttpRequest(std::string(), std::string()) {}

            inline HttpRequest::HttpRequest(
                const std::string& client_cert_path,
                const std::string& client_key_path)
                : sock_(INVALID_SOCKET),
                client_cert_path_(client_cert_path), client_key_path_(client_key_path) {}

            inline HttpRequest::~HttpRequest() {}

            inline bool HttpRequest::is_valid() const { return true; }

            inline bool HttpRequest::prase_url(const char* url, std::string& host, int& port, std::string& path)
            {
                std::string str_url(url);
                std::string str_host_and_port;
                std::string str_path;

                int last_pos = 0;
                auto pos = str_url.find("://", 0);
                if (pos == std::string::npos)
                    last_pos = 0;
                else
                    last_pos = pos + 3;
                pos = str_url.find("/", last_pos);
                if (pos == std::string::npos)
                {
                    str_host_and_port = str_url.substr(last_pos, str_url.size());
                    if (str_host_and_port == "")return false;
                    str_path = "/";
                }
                else
                {
                    str_host_and_port = str_url.substr(last_pos, pos - last_pos);
                    str_path = str_url.substr(pos, str_url.size());
                    if (str_host_and_port == "") str_path = "/";
                }
                pos = str_host_and_port.find(":", 0);
                if (pos == std::string::npos)
                {
                    host = str_host_and_port;
                    port = 80;
                }
                else
                {
                    host = str_host_and_port.substr(0, pos);
                    auto str_port = str_host_and_port.substr(pos + 1, str_host_and_port.size());
                    if (str_port.size())
                        port = std::atoi(str_port.c_str());
                    else
                        port = 80;
                }
                path = str_path;

                return true;
            }

            inline socket_t HttpRequest::create_client_socket(const std::string& host, int port) const {
                return detail::create_client_socket(host.c_str(), port,
                    connection_timeout_sec_,
                    connection_timeout_usec_, interface_);
            }

            inline bool HttpRequest::read_response_line(Stream& strm, Response& res) {
                std::array<char, 2048> buf;

                detail::stream_line_reader line_reader(strm, buf.data(), buf.size());

                if (!line_reader.getline()) { return false; }

                const static std::regex re("(HTTP/1\\.[01]) (\\d+?) .*\r\n");

                std::cmatch m;
                if (std::regex_match(line_reader.ptr(), m, re)) {
                    res.version = std::string(m[1]);
                    res.status = std::stoi(std::string(m[2]));
                }

                return true;
            }

            inline bool HttpRequest::send(const std::string& host, int port, const Request& req, Response& res) {
                sock_ = create_client_socket(host, port);
                if (sock_ == INVALID_SOCKET) { return false; }


                return process_and_close_socket(
                    sock_, 1,
                    [&](Stream& strm, bool last_connection, bool& connection_close) {
                    return handle_request(host, port, strm, req, res, last_connection,
                        connection_close);
                });
            }

            inline bool HttpRequest::send(
                const std::string& host, int port,
                const std::vector<Request>& requests,
                std::vector<Response>& responses) {
                size_t i = 0;
                while (i < requests.size()) {
                    sock_ = create_client_socket(host, port);
                    if (sock_ == INVALID_SOCKET) { return false; }

                    if (!process_and_close_socket(sock_, requests.size() - i,
                        [&](Stream& strm, bool last_connection,
                            bool& connection_close) -> bool {
                        auto& req = requests[i++];
                        auto res = Response();
                        auto ret = handle_request(
                            host, port, 
                            strm, req, res,
                            last_connection,
                            connection_close);
                        if (ret) {
                            responses.emplace_back(std::move(res));
                        }
                        return ret;
                    })) {
                        return false;
                    }
                }

                return true;
            }

            inline bool HttpRequest::handle_request(
                const std::string& host, int port, 
                Stream& strm, const Request& req,
                Response& res, bool last_connection,
                bool& connection_close) {
                if (req.path.empty()) { return false; }

                bool ret;
                ret = process_request(host, port, strm, req, res, last_connection, connection_close);
                if (!ret) { return false; }

                if (300 < res.status && res.status < 400 && follow_location_) {
                    ret = redirect(host, port, req, res);
                }


                return ret;
            }


            inline bool HttpRequest::redirect(const std::string& host, int port, const Request& req, Response& res) {
                if (req.redirect_count == 0) { return false; }

                auto location = res.get_header_value("location");
                if (location.empty()) { return false; }

                const static std::regex re(
                    R"(^(?:(https?):)?(?://([^:/?#]*)(?::(\d+))?)?([^?#]*(?:\?[^#]*)?)(?:#.*)?)");

                std::smatch m;
                if (!std::regex_match(location, m, re)) { return false; }

                auto scheme = is_ssl() ? "https" : "http";

                auto next_scheme = m[1].str();
                auto next_host = m[2].str();
                auto port_str = m[3].str();
                auto next_path = m[4].str();

                auto next_port = port;
                if (!port_str.empty()) {
                    next_port = std::stoi(port_str);
                }
                else if (!next_scheme.empty()) {
                    next_port = next_scheme == "https" ? 443 : 80;
                }

                if (next_scheme.empty()) { next_scheme = scheme; }
                if (next_host.empty()) { next_host = host; }
                if (next_path.empty()) { next_path = "/"; }

                if (next_scheme == scheme && next_host == host && next_port == port) {
                    return detail::redirect(host, port, *this, req, res, next_path);
                }
                else {
                    if (next_scheme == "https") {
                        return false;
                    }
                    else {
                        HttpRequest cli;
                        cli.copy_settings(*this);
                        return detail::redirect(host, port, cli, req, res, next_path);
                    }
                }
            }

            inline bool HttpRequest::write_request(
                const std::string& host, int port,
                Stream& strm, const Request& req,
                bool last_connection) {
                detail::BufferStream bstrm;

                // Request line
                const auto& path = detail::encode_url(req.path);

                bstrm.write_format("%s %s HTTP/1.1\r\n", req.method.c_str(), path.c_str());

                // Additonal headers
                Headers headers;
                if (last_connection) { headers.emplace("Connection", "close"); }

                if (!req.has_header("Host")) {
                    if (is_ssl()) {
                        if (port == 443) {
                            headers.emplace("Host", host);
                        }
                        else {
                            headers.emplace("Host", host + ":" + std::to_string(port));
                        }
                    }
                    else {
                        if (port == 80) {
                            headers.emplace("Host", host);
                        }
                        else {
                            headers.emplace("Host", host + ":" + std::to_string(port));
                        }
                    }
                }

                if (!req.has_header("Accept")) { headers.emplace("Accept", "*/*"); }

                if (!req.has_header("User-Agent")) {
                    headers.emplace("User-Agent", "cfengine-ver8");
                }

                if (req.body.empty()) {
                    if (req.content_provider) {
                        auto length = std::to_string(req.content_length);
                        headers.emplace("Content-Length", length);
                    }
                    else {
                        headers.emplace("Content-Length", "0");
                    }
                }
                else {
                    if (!req.has_header("Content-Type")) {
                        headers.emplace("Content-Type", "text/plain");
                    }

                    if (!req.has_header("Content-Length")) {
                        auto length = std::to_string(req.body.size());
                        headers.emplace("Content-Length", length);
                    }
                }

                if (!basic_auth_username_.empty() && !basic_auth_password_.empty()) {
                    headers.insert(makeBasicAuthenticationHeader(
                        basic_auth_username_, basic_auth_password_, false));
                }

                if (!proxy_basic_auth_username_.empty() &&
                    !proxy_basic_auth_password_.empty()) {
                    headers.insert(makeBasicAuthenticationHeader(
                        proxy_basic_auth_username_, proxy_basic_auth_password_, true));
                }

                detail::write_headers(bstrm, req, headers);

                // Flush buffer
                auto& data = bstrm.get_buffer();
                if (!detail::write_data(strm, data.data(), data.size())) { return false; }

                // Body
                if (req.body.empty()) {
                    if (req.content_provider) {
                        size_t offset = 0;
                        size_t end_offset = req.content_length;

                        bool ok = true;

                        DataSink data_sink;
                        data_sink.write = [&](const char* d, size_t l) {
                            if (ok) {
                                if (detail::write_data(strm, d, l)) {
                                    offset += l;
                                }
                                else {
                                    ok = false;
                                }
                            }
                        };
                        data_sink.is_writable = [&](void) { return ok && strm.is_writable(); };

                        while (offset < end_offset) {
                            if (!req.content_provider(offset, end_offset - offset, data_sink)) {
                                return false;
                            }
                            if (!ok) { return false; }
                        }
                    }
                }
                else {
                    return detail::write_data(strm, req.body.data(), req.body.size());
                }

                return true;
            }

            inline std::shared_ptr<Response> HttpRequest::send_with_content_provider(
                const std::string& host, int port,
                const char* method, const char* path, const Headers& headers,
                const std::string& body, size_t content_length,
                ContentProvider content_provider, const char* content_type) {
                Request req;
                req.method = method;
                req.headers = headers;
                req.path = path;

                if (content_type) { req.headers.emplace("Content-Type", content_type); }

#ifdef CRAFT_ENGINE_NET_HTTP_ZLIB_SUPPORT
                if (compress_) {
                    if (content_provider) {
                        size_t offset = 0;

                        DataSink data_sink;
                        data_sink.write = [&](const char* data, size_t data_len) {
                            req.body.append(data, data_len);
                            offset += data_len;
                        };
                        data_sink.is_writable = [&](void) { return true; };

                        while (offset < content_length) {
                            if (!content_provider(offset, content_length - offset, data_sink)) {
                                return nullptr;
                            }
                        }
                    }
                    else {
                        req.body = body;
                    }

                    if (!detail::compress(req.body)) { return nullptr; }
                    req.headers.emplace("Content-Encoding", "gzip");
                }
                else
#endif
                {
                    if (content_provider) {
                        req.content_length = content_length;
                        req.content_provider = content_provider;
                    }
                    else {
                        req.body = body;
                    }
                }

                auto res = std::make_shared<Response>();

                return send(host, port, req, *res) ? res : nullptr;
            }

            inline bool HttpRequest::process_request(const std::string& host, int port, Stream& strm, const Request& req,
                Response& res, bool last_connection,
                bool& connection_close) {
                // Send request
                if (!write_request(host, port, strm, req, last_connection)) { return false; }

                // Receive response and headers
                if (!read_response_line(strm, res) ||
                    !detail::read_headers(strm, res.headers)) {
                    return false;
                }

                if (res.get_header_value("Connection") == "close" ||
                    res.version == "HTTP/1.0") {
                    connection_close = true;
                }

                if (req.response_handler) {
                    if (!req.response_handler(res)) { return false; }
                }

                // Body
                if (req.method != "HEAD" && req.method != "CONNECT") {
                    auto out =
                        req.content_receiver
                        ? static_cast<ContentReceiver>([&](const char* buf, size_t n) {
                        return req.content_receiver(buf, n);
                    })
                        : static_cast<ContentReceiver>([&](const char* buf, size_t n) {
                        if (res.body.size() + n > res.body.max_size()) { return false; }
                        res.body.append(buf, n);
                        return true;
                    });

                    int dummy_status;
                    if (!detail::read_content(strm, res, (std::numeric_limits<size_t>::max)(),
                        dummy_status, req.progress, out, decompress_)) {
                        return false;
                    }
                }

                // Log
                if (logger_) { logger_(req, res); }

                return true;
            }

            inline bool HttpRequest::process_and_close_socket(
                socket_t sock, size_t request_count,
                std::function<bool(Stream& strm, bool last_connection,
                    bool& connection_close)>
                callback) {
                request_count = (std::min)(request_count, keep_alive_max_count_);
                return detail::process_and_close_socket(
                    true, sock, request_count, read_timeout_sec_, read_timeout_usec_,
                    write_timeout_sec_, write_timeout_usec_, callback);
            }

            inline bool HttpRequest::is_ssl() const { return false; }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url) {
                return Get(url, Headers(), Progress());
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                Progress progress) {
                return Get(url, Headers(), std::move(progress));
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                const Headers& headers) {
                return Get(url, headers, Progress());
            }

            inline std::shared_ptr<Response>
                HttpRequest::Get(const char* url, const Headers& headers, Progress progress) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                Request req;
                req.method = "GET";
                req.path = path;
                req.headers = headers;
                req.progress = std::move(progress);
                auto res = std::make_shared<Response>();
                return send(host, port, req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                ContentReceiver content_receiver) {
                return Get(url, Headers(), nullptr, std::move(content_receiver), Progress());
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                ContentReceiver content_receiver,
                Progress progress) {
                return Get(url, Headers(), nullptr, std::move(content_receiver),
                    std::move(progress));
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                const Headers& headers,
                ContentReceiver content_receiver) {
                return Get(url, headers, nullptr, std::move(content_receiver), Progress());
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                const Headers& headers,
                ContentReceiver content_receiver,
                Progress progress) {
                return Get(url, headers, nullptr, std::move(content_receiver),
                    std::move(progress));
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                const Headers& headers,
                ResponseHandler response_handler,
                ContentReceiver content_receiver) {
                return Get(url, headers, std::move(response_handler), content_receiver,
                    Progress());
            }

            inline std::shared_ptr<Response> HttpRequest::Get(const char* url,
                const Headers& headers,
                ResponseHandler response_handler,
                ContentReceiver content_receiver,
                Progress progress) {

                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                Request req;
                req.method = "GET";
                req.path = path;
                req.headers = headers;
                req.response_handler = std::move(response_handler);
                req.content_receiver = std::move(content_receiver);
                req.progress = std::move(progress);

                auto res = std::make_shared<Response>();
                return send(host, port, req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpRequest::Head(const char* url) {
                return Head(url, Headers());
            }

            inline std::shared_ptr<Response> HttpRequest::Head(const char* url,
                const Headers& headers) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                Request req;
                req.method = "HEAD";
                req.headers = headers;
                req.path = path;
                auto res = std::make_shared<Response>();
                return send(host, port, req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpRequest::Post(const char* url) {
                return Post(url, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpRequest::Post(const char* url,
                const std::string& body,
                const char* content_type) {
                return Post(url, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Post(const char* url,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;

                return send_with_content_provider(host, port, "POST", path.c_str(), headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Post(const char* url,
                const Params& params) {
                return Post(url, Headers(), params);
            }

            inline std::shared_ptr<Response> HttpRequest::Post(const char* url,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Post(url, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Post(const char* url, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                return send_with_content_provider(host, port, "POST", path.c_str(), headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Post(const char* url, const Headers& headers, const Params& params) {
                auto query = detail::params_to_query_str(params);
                return Post(url, headers, query, "application/x-www-form-urlencoded");
            }

            inline std::shared_ptr<Response>
                HttpRequest::Post(const char* url, const MultipartFormDataItems& items) {
                return Post(url, Headers(), items);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Post(const char* url, const Headers& headers,
                    const MultipartFormDataItems& items) {
                auto boundary = detail::make_multipart_data_boundary();

                std::string body;

                for (const auto& item : items) {
                    body += "--" + boundary + "\r\n";
                    body += "Content-Disposition: form-data; name=\"" + item.name + "\"";
                    if (!item.filename.empty()) {
                        body += "; filename=\"" + item.filename + "\"";
                    }
                    body += "\r\n";
                    if (!item.content_type.empty()) {
                        body += "Content-Type: " + item.content_type + "\r\n";
                    }
                    body += "\r\n";
                    body += item.content + "\r\n";
                }

                body += "--" + boundary + "--\r\n";

                std::string content_type = "multipart/form-data; boundary=" + boundary;
                return Post(url, headers, body, content_type.c_str());
            }

            inline std::shared_ptr<Response> HttpRequest::Put(const char* url) {
                return Put(url, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpRequest::Put(const char* url,
                const std::string& body,
                const char* content_type) {
                return Put(url, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Put(const char* url,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                return send_with_content_provider(host, port, "PUT", path.c_str(), headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Put(const char* url,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Put(url, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Put(const char* url, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                return send_with_content_provider(host, port, "PUT", path.c_str(), headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Put(const char* url,
                const Params& params) {
                return Put(url, Headers(), params);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Put(const char* url, const Headers& headers, const Params& params) {
                auto query = detail::params_to_query_str(params);
                return Put(url, headers, query, "application/x-www-form-urlencoded");
            }

            inline std::shared_ptr<Response> HttpRequest::Patch(const char* url,
                const std::string& body,
                const char* content_type) {
                return Patch(url, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Patch(const char* url,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                return send_with_content_provider(host, port, "PATCH", path.c_str(), headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Patch(const char* url,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Patch(url, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpRequest::Patch(const char* url, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                return send_with_content_provider(host, port, "PATCH", path.c_str(), headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Delete(const char* url) {
                return Delete(url, Headers(), std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpRequest::Delete(const char* url,
                const std::string& body,
                const char* content_type) {
                return Delete(url, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpRequest::Delete(const char* url,
                const Headers& headers) {
                return Delete(url, headers, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpRequest::Delete(const char* url,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                Request req;
                req.method = "DELETE";
                req.headers = headers;
                req.path = path;

                if (content_type) { req.headers.emplace("Content-Type", content_type); }
                req.body = body;

                auto res = std::make_shared<Response>();

                return send(host, port, req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpRequest::Options(const char* url) {
                return Options(url, Headers());
            }

            inline std::shared_ptr<Response> HttpRequest::Options(const char* url,
                const Headers& headers) {
                std::string host;
                int port;
                std::string path;
                if (!prase_url(url, host, port, path))
                    return nullptr;
                Request req;
                req.method = "OPTIONS";
                req.path = path;
                req.headers = headers;

                auto res = std::make_shared<Response>();

                return send(host, port, req, *res) ? res : nullptr;
            }

            inline void HttpRequest::stop() {
                if (sock_ != INVALID_SOCKET) {
                    std::atomic<socket_t> sock(sock_.exchange(INVALID_SOCKET));
                    detail::shutdown_socket(sock);
                    detail::close_socket(sock);
                }
            }


            inline void HttpRequest::set_connection_timeout(time_t sec, time_t usec) {
                connection_timeout_sec_ = sec;
                connection_timeout_usec_ = usec;
            }

            inline void HttpRequest::set_read_timeout(time_t sec, time_t usec) {
                read_timeout_sec_ = sec;
                read_timeout_usec_ = usec;
            }

            inline void HttpRequest::set_write_timeout(time_t sec, time_t usec) {
                write_timeout_sec_ = sec;
                write_timeout_usec_ = usec;
            }

            inline void HttpRequest::set_keep_alive_max_count(size_t count) {
                keep_alive_max_count_ = count;
            }

            inline void HttpRequest::set_basic_auth(const char* username, const char* password) {
                basic_auth_username_ = username;
                basic_auth_password_ = password;
            }


            inline void HttpRequest::set_follow_location(bool on) { follow_location_ = on; }

            inline void HttpRequest::set_compress(bool on) { compress_ = on; }

            inline void HttpRequest::set_decompress(bool on) { decompress_ = on; }

            inline void HttpRequest::set_interface(const char* intf) { interface_ = intf; }


            inline void HttpRequest::set_proxy_basic_auth(const char* username,
                const char* password) {
                proxy_basic_auth_username_ = username;
                proxy_basic_auth_password_ = password;
            }

            inline void HttpRequest::set_logger(Logger logger) { logger_ = std::move(logger); }




        }


    }
}
