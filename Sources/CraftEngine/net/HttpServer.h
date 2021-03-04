#pragma once

#include "HttpCommon.h"

namespace CraftEngine
{
	namespace net
	{


		namespace http
		{
            /*
             HttpServer

             HttpServer svr;
             if (!svr.is_valid()) return;

             svr.Get("/", [=](const Request & , Response& res) {
                res.set_redirect("/hi");
             });

             svr.Get("/hi", [](const Request&, Response& res) {
                res.set_content("Hello World!\n", "text/plain");
             });

             svr.Get("/slow", [](const Request&, Response& res) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
             res.set_content("Slow...\n", "text/plain");
             });

             svr.Get("/dump", [](const Request& req, Response& res) {
                res.set_content(dump_headers(req.headers), "text/plain");
             });

             svr.Get("/stop", [&](const Request& , Response& ) { svr.stop(); });

             svr.set_error_handler([](const Request& , Response& res) {
                   const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
                   char buf[BUFSIZ];
                   snprintf(buf, sizeof(buf), fmt, res.status);
                   res.set_content(buf, "text/html");
             });

             svr.set_logger([](const Request& req, const Response& res) {
               printf("%s", log(req, res).c_str());
             });

             svr.listen("localhost", 8080);
            */
            class HttpServer {
            public:
                using Handler = std::function<void(const Request&, Response&)>;
                using HandlerWithContentReader = std::function<void(
                    const Request&, Response&, const ContentReader& content_reader)>;
                using Expect100ContinueHandler =
                    std::function<int(const Request&, Response&)>;

                HttpServer();

                virtual ~HttpServer();

                virtual bool is_valid() const;

                HttpServer& Get(const char* pattern, Handler handler);
                template<const char* regExpr, int32_t... paramIndices>
                HttpServer& Get(const char* pattern, Handler handler);
                HttpServer& Post(const char* pattern, Handler handler);
                HttpServer& Post(const char* pattern, HandlerWithContentReader handler);
                HttpServer& Put(const char* pattern, Handler handler);
                HttpServer& Put(const char* pattern, HandlerWithContentReader handler);
                HttpServer& Patch(const char* pattern, Handler handler);
                HttpServer& Patch(const char* pattern, HandlerWithContentReader handler);
                HttpServer& Delete(const char* pattern, Handler handler);
                HttpServer& Delete(const char* pattern, HandlerWithContentReader handler);
                HttpServer& Options(const char* pattern, Handler handler);

                bool set_mount_point(const char* mount_point, const char* dir);
                bool remove_mount_point(const char* mount_point);
                void set_file_extension_and_mimetype_mapping(const char* ext,
                    const char* mime);
                void set_file_request_handler(Handler handler);

                void set_error_handler(Handler handler);
                void set_logger(Logger logger);

                void set_expect_100_continue_handler(Expect100ContinueHandler handler);

                void set_keep_alive_max_count(size_t count);
                void set_read_timeout(time_t sec, time_t usec = 0);
                void set_write_timeout(time_t sec, time_t usec = 0);
                void set_idle_interval(time_t sec, time_t usec = 0);

                void set_payload_max_length(size_t length);

                bool bind_to_port(const char* host, int port, int socket_flags = 0);
                int bind_to_any_port(const char* host, int socket_flags = 0);
                bool listen_after_bind();

                bool listen(const char* host, int port, int socket_flags = 0);

                bool is_running() const;
                void stop();

                std::function<detail::TaskQueue* (void)> new_task_queue;

            protected:
                bool process_request(Stream& strm, bool last_connection,
                    bool& connection_close,
                    const std::function<void(Request&)>& setup_request);

                size_t keep_alive_max_count_ = detail::CRAFT_ENGINE_NET_HTTP_KEEPALIVE_MAX_COUNT;
                time_t read_timeout_sec_ = detail::CRAFT_ENGINE_NET_HTTP_READ_TIMEOUT_SECOND;
                time_t read_timeout_usec_ = detail::CRAFT_ENGINE_NET_HTTP_TIMEOUT_USECOND;
                time_t write_timeout_sec_ = detail::CRAFT_ENGINE_NET_HTTP_WRITE_TIMEOUT_SECOND;
                time_t write_timeout_usec_ = detail::CRAFT_ENGINE_NET_HTTP_TIMEOUT_USECOND;
                time_t idle_interval_sec_ = detail::CRAFT_ENGINE_NET_HTTP_IDLE_INTERVAL_SECOND;
                time_t idle_interval_usec_ = detail::CRAFT_ENGINE_NET_HTTP_IDLE_INTERVAL_USECOND;
                size_t payload_max_length_ = detail::CRAFT_ENGINE_NET_HTTP_PAYLOAD_MAX_LENGTH;

            private:
                using Handlers = std::vector<std::pair<std::regex, Handler>>;
                using HandlersForContentReader =
                    std::vector<std::pair<std::regex, HandlerWithContentReader>>;

                socket_t create_server_socket(const char* host, int port,
                    int socket_flags) const;
                int bind_internal(const char* host, int port, int socket_flags);
                bool listen_internal();

                bool routing(Request& req, Response& res, Stream& strm);
                bool handle_file_request(Request& req, Response& res, bool head = false);
                bool dispatch_request(Request& req, Response& res, Handlers& handlers);
                bool dispatch_request_for_content_reader(Request& req, Response& res,
                    ContentReader content_reader,
                    HandlersForContentReader& handlers);

                bool parse_request_line(const char* s, Request& req);
                bool write_response(Stream& strm, bool last_connection, const Request& req,
                    Response& res);
                bool write_content_with_provider(Stream& strm, const Request& req,
                    Response& res, const std::string& boundary,
                    const std::string& content_type);
                bool read_content(Stream& strm, Request& req, Response& res);
                bool
                    read_content_with_content_receiver(Stream& strm, Request& req, Response& res,
                        ContentReceiver receiver,
                        MultipartContentHeader multipart_header,
                        ContentReceiver multipart_receiver);
                bool read_content_core(Stream& strm, Request& req, Response& res,
                    ContentReceiver receiver,
                    MultipartContentHeader mulitpart_header,
                    ContentReceiver multipart_receiver);

                virtual bool process_and_close_socket(socket_t sock);

                std::atomic<bool> is_running_;
                std::atomic<socket_t> svr_sock_;
                std::vector<std::pair<std::string, std::string>> base_dirs_;
                std::map<std::string, std::string> file_extension_and_mimetype_map_;
                Handler file_request_handler_;
                Handlers get_handlers_;
                Handlers post_handlers_;
                HandlersForContentReader post_handlers_for_content_reader_;
                Handlers put_handlers_;
                HandlersForContentReader put_handlers_for_content_reader_;
                Handlers patch_handlers_;
                HandlersForContentReader patch_handlers_for_content_reader_;
                Handlers delete_handlers_;
                HandlersForContentReader delete_handlers_for_content_reader_;
                Handlers options_handlers_;
                Handler error_handler_;
                Logger logger_;
                Expect100ContinueHandler expect_100_continue_handler_;
            };


#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
            class SSLServer : public HttpServer {
            public:
                SSLServer(const char* cert_path, const char* private_key_path,
                    const char* client_ca_cert_file_path = nullptr,
                    const char* client_ca_cert_dir_path = nullptr);

                SSLServer(X509* cert, EVP_PKEY* private_key,
                    X509_STORE* client_ca_cert_store = nullptr);

                ~SSLServer() override;

                bool is_valid() const override;

            private:
                bool process_and_close_socket(socket_t sock) override;

                SSL_CTX* ctx_;
                std::mutex ctx_mutex_;
            };

#endif




            // HTTP server implementation
            inline HttpServer::HttpServer() : is_running_(false), svr_sock_(INVALID_SOCKET) {
#ifndef _WIN32
                signal(SIGPIPE, SIG_IGN);
#endif
                new_task_queue = [] { return new detail::ThreadPool(detail::CRAFT_ENGINE_NET_HTTP_THREAD_POOL_COUNT); };
            }

            inline HttpServer::~HttpServer() {}

            inline HttpServer& HttpServer::Get(const char* pattern, Handler handler) {
                get_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Post(const char* pattern, Handler handler) {
                post_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Post(const char* pattern,
                HandlerWithContentReader handler) {
                post_handlers_for_content_reader_.push_back(
                    std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Put(const char* pattern, Handler handler) {
                put_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Put(const char* pattern,
                HandlerWithContentReader handler) {
                put_handlers_for_content_reader_.push_back(
                    std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Patch(const char* pattern, Handler handler) {
                patch_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Patch(const char* pattern,
                HandlerWithContentReader handler) {
                patch_handlers_for_content_reader_.push_back(
                    std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Delete(const char* pattern, Handler handler) {
                delete_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Delete(const char* pattern,
                HandlerWithContentReader handler) {
                delete_handlers_for_content_reader_.push_back(
                    std::make_pair(std::regex(pattern), handler));
                return *this;
            }

            inline HttpServer& HttpServer::Options(const char* pattern, Handler handler) {
                options_handlers_.push_back(std::make_pair(std::regex(pattern), handler));
                return *this;
            }


            inline bool HttpServer::set_mount_point(const char* mount_point, const char* dir) {
                if (detail::is_dir(dir)) {
                    std::string mnt = mount_point ? mount_point : "/";
                    if (!mnt.empty() && mnt[0] == '/') {
                        base_dirs_.emplace_back(mnt, dir);
                        return true;
                    }
                }
                return false;
            }

            inline bool HttpServer::remove_mount_point(const char* mount_point) {
                for (auto it = base_dirs_.begin(); it != base_dirs_.end(); ++it) {
                    if (it->first == mount_point) {
                        base_dirs_.erase(it);
                        return true;
                    }
                }
                return false;
            }

            inline void HttpServer::set_file_extension_and_mimetype_mapping(const char* ext,
                const char* mime) {
                file_extension_and_mimetype_map_[ext] = mime;
            }

            inline void HttpServer::set_file_request_handler(Handler handler) {
                file_request_handler_ = std::move(handler);
            }

            inline void HttpServer::set_error_handler(Handler handler) {
                error_handler_ = std::move(handler);
            }

            inline void HttpServer::set_logger(Logger logger) { logger_ = std::move(logger); }

            inline void
                HttpServer::set_expect_100_continue_handler(Expect100ContinueHandler handler) {
                expect_100_continue_handler_ = std::move(handler);
            }

            inline void HttpServer::set_keep_alive_max_count(size_t count) {
                keep_alive_max_count_ = count;
            }

            inline void HttpServer::set_read_timeout(time_t sec, time_t usec) {
                read_timeout_sec_ = sec;
                read_timeout_usec_ = usec;
            }

            inline void HttpServer::set_write_timeout(time_t sec, time_t usec) {
                write_timeout_sec_ = sec;
                write_timeout_usec_ = usec;
            }

            inline void HttpServer::set_idle_interval(time_t sec, time_t usec) {
                idle_interval_sec_ = sec;
                idle_interval_usec_ = usec;
            }

            inline void HttpServer::set_payload_max_length(size_t length) {
                payload_max_length_ = length;
            }

            inline bool HttpServer::bind_to_port(const char* host, int port, int socket_flags) {
                if (bind_internal(host, port, socket_flags) < 0) return false;
                return true;
            }
            inline int HttpServer::bind_to_any_port(const char* host, int socket_flags) {
                return bind_internal(host, 0, socket_flags);
            }

            inline bool HttpServer::listen_after_bind() { return listen_internal(); }

            inline bool HttpServer::listen(const char* host, int port, int socket_flags) {
                return bind_to_port(host, port, socket_flags) && listen_internal();
            }

            inline bool HttpServer::is_running() const { return is_running_; }

            inline void HttpServer::stop() {
                if (is_running_) {
                    assert(svr_sock_ != INVALID_SOCKET);
                    std::atomic<socket_t> sock(svr_sock_.exchange(INVALID_SOCKET));
                    detail::shutdown_socket(sock);
                    detail::close_socket(sock);
                }
            }

            inline bool HttpServer::parse_request_line(const char* s, Request& req) {
                const static std::regex re(
                    "(GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE|PATCH|PRI) "
                    "(([^?]+)(?:\\?(.*?))?) (HTTP/1\\.[01])\r\n");

                std::cmatch m;
                if (std::regex_match(s, m, re)) {
                    req.version = std::string(m[5]);
                    req.method = std::string(m[1]);
                    req.target = std::string(m[2]);
                    req.path = detail::decode_url(m[3], false);

                    // Parse query text
                    auto len = std::distance(m[4].first, m[4].second);
                    if (len > 0) { detail::parse_query_text(m[4], req.params); }

                    return true;
                }

                return false;
            }

            inline bool HttpServer::write_response(Stream& strm, bool last_connection,
                const Request& req, Response& res) {
                assert(res.status != -1);

                if (400 <= res.status && error_handler_) { error_handler_(req, res); }

                detail::BufferStream bstrm;

                // Response line
                if (!bstrm.write_format("HTTP/1.1 %d %s\r\n", res.status,
                    detail::status_message(res.status))) {
                    return false;
                }

                // Headers
                if (last_connection || req.get_header_value("Connection") == "close") {
                    res.set_header("Connection", "close");
                }

                if (!last_connection && req.get_header_value("Connection") == "Keep-Alive") {
                    res.set_header("Connection", "Keep-Alive");
                }

                if (!res.has_header("Content-Type") &&
                    (!res.body.empty() || res.content_length_ > 0)) {
                    res.set_header("Content-Type", "text/plain");
                }

                if (!res.has_header("Accept-Ranges") && req.method == "HEAD") {
                    res.set_header("Accept-Ranges", "bytes");
                }

                std::string content_type;
                std::string boundary;

                if (req.ranges.size() > 1) {
                    boundary = detail::make_multipart_data_boundary();

                    auto it = res.headers.find("Content-Type");
                    if (it != res.headers.end()) {
                        content_type = it->second;
                        res.headers.erase(it);
                    }

                    res.headers.emplace("Content-Type",
                        "multipart/byteranges; boundary=" + boundary);
                }

                if (res.body.empty()) {
                    if (res.content_length_ > 0) {
                        size_t length = 0;
                        if (req.ranges.empty()) {
                            length = res.content_length_;
                        }
                        else if (req.ranges.size() == 1) {
                            auto offsets =
                                detail::get_range_offset_and_length(req, res.content_length_, 0);
                            auto offset = offsets.first;
                            length = offsets.second;
                            auto content_range = detail::make_content_range_header_field(
                                offset, length, res.content_length_);
                            res.set_header("Content-Range", content_range);
                        }
                        else {
                            length = detail::get_multipart_ranges_data_length(req, res, boundary,
                                content_type);
                        }
                        res.set_header("Content-Length", std::to_string(length));
                    }
                    else {
                        if (res.content_provider_) {
                            res.set_header("Transfer-Encoding", "chunked");
                        }
                        else {
                            res.set_header("Content-Length", "0");
                        }
                    }
                }
                else {
                    if (req.ranges.empty()) {
                        ;
                    }
                    else if (req.ranges.size() == 1) {
                        auto offsets =
                            detail::get_range_offset_and_length(req, res.body.size(), 0);
                        auto offset = offsets.first;
                        auto length = offsets.second;
                        auto content_range = detail::make_content_range_header_field(
                            offset, length, res.body.size());
                        res.set_header("Content-Range", content_range);
                        res.body = res.body.substr(offset, length);
                    }
                    else {
                        res.body =
                            detail::make_multipart_ranges_data(req, res, boundary, content_type);
                    }

#ifdef CRAFT_ENGINE_NET_HTTP_ZLIB_SUPPORT
                    // TODO: 'Accept-Encoding' has gzip, not gzip;q=0
                    const auto& encodings = req.get_header_value("Accept-Encoding");
                    if (encodings.find("gzip") != std::string::npos &&
                        detail::can_compress(res.get_header_value("Content-Type"))) {
                        if (detail::compress(res.body)) {
                            res.set_header("Content-Encoding", "gzip");
                        }
                    }
#endif

                    auto length = std::to_string(res.body.size());
                    res.set_header("Content-Length", length);
                }

                if (!detail::write_headers(bstrm, res, Headers())) { return false; }

                // Flush buffer
                auto& data = bstrm.get_buffer();
                strm.write(data.data(), data.size());

                // Body
                if (req.method != "HEAD") {
                    if (!res.body.empty()) {
                        if (!strm.write(res.body)) { return false; }
                    }
                    else if (res.content_provider_) {
                        if (!write_content_with_provider(strm, req, res, boundary,
                            content_type)) {
                            return false;
                        }
                    }
                }

                // Log
                if (logger_) { logger_(req, res); }

                return true;
            }

            inline bool
                HttpServer::write_content_with_provider(Stream& strm, const Request& req,
                    Response& res, const std::string& boundary,
                    const std::string& content_type) {
                if (res.content_length_) {
                    if (req.ranges.empty()) {
                        if (detail::write_content(strm, res.content_provider_, 0,
                            res.content_length_) < 0) {
                            return false;
                        }
                    }
                    else if (req.ranges.size() == 1) {
                        auto offsets =
                            detail::get_range_offset_and_length(req, res.content_length_, 0);
                        auto offset = offsets.first;
                        auto length = offsets.second;
                        if (detail::write_content(strm, res.content_provider_, offset, length) <
                            0) {
                            return false;
                        }
                    }
                    else {
                        if (!detail::write_multipart_ranges_data(strm, req, res, boundary,
                            content_type)) {
                            return false;
                        }
                    }
                }
                else {
                    auto is_shutting_down = [this]() {
                        return this->svr_sock_ == INVALID_SOCKET;
                    };
                    if (detail::write_content_chunked(strm, res.content_provider_,
                        is_shutting_down) < 0) {
                        return false;
                    }
                }
                return true;
            }

            inline bool HttpServer::read_content(Stream& strm, Request& req, Response& res) {
                MultipartFormDataMap::iterator cur;
                if (read_content_core(
                    strm, req, res,
                    // Regular
                    [&](const char* buf, size_t n) {
                    if (req.body.size() + n > req.body.max_size()) { return false; }
                    req.body.append(buf, n);
                    return true;
                },
                    // Multipart
                    [&](const MultipartFormData& file) {
                    cur = req.files.emplace(file.name, file);
                    return true;
                },
                    [&](const char* buf, size_t n) {
                    auto& content = cur->second.content;
                    if (content.size() + n > content.max_size()) { return false; }
                    content.append(buf, n);
                    return true;
                })) {
                    const auto& content_type = req.get_header_value("Content-Type");
                    if (!content_type.find("application/x-www-form-urlencoded")) {
                        detail::parse_query_text(req.body, req.params);
                    }
                    return true;
                }
                return false;
            }

            inline bool HttpServer::read_content_with_content_receiver(
                Stream& strm, Request& req, Response& res, ContentReceiver receiver,
                MultipartContentHeader multipart_header,
                ContentReceiver multipart_receiver) {
                return read_content_core(strm, req, res, receiver, multipart_header,
                    multipart_receiver);
            }

            inline bool HttpServer::read_content_core(Stream& strm, Request& req, Response& res,
                ContentReceiver receiver,
                MultipartContentHeader mulitpart_header,
                ContentReceiver multipart_receiver) {
                detail::MultipartFormDataParser multipart_form_data_parser;
                ContentReceiver out;

                if (req.is_multipart_form_data()) {
                    const auto& content_type = req.get_header_value("Content-Type");
                    std::string boundary;
                    if (!detail::parse_multipart_boundary(content_type, boundary)) {
                        res.status = 400;
                        return false;
                    }

                    multipart_form_data_parser.set_boundary(std::move(boundary));
                    out = [&](const char* buf, size_t n) {
                        return multipart_form_data_parser.parse(buf, n, multipart_receiver,
                            mulitpart_header);
                    };
                }
                else {
                    out = receiver;
                }

                if (!detail::read_content(strm, req, payload_max_length_, res.status,
                    Progress(), out, true)) {
                    return false;
                }

                if (req.is_multipart_form_data()) {
                    if (!multipart_form_data_parser.is_valid()) {
                        res.status = 400;
                        return false;
                    }
                }

                return true;
            }

            inline bool HttpServer::handle_file_request(Request& req, Response& res,
                bool head) {
                for (const auto& kv : base_dirs_) {
                    const auto& mount_point = kv.first;
                    const auto& base_dir = kv.second;

                    // Prefix match
                    if (!req.path.find(mount_point)) {
                        std::string sub_path = "/" + req.path.substr(mount_point.size());
                        if (detail::is_valid_path(sub_path)) {
                            auto path = base_dir + sub_path;
                            if (path.back() == '/') { path += "index.html"; }

                            if (detail::is_file(path)) {
                                detail::read_file(path, res.body);
                                auto type =
                                    detail::find_content_type(path, file_extension_and_mimetype_map_);
                                if (type) { res.set_header("Content-Type", type); }
                                res.status = 200;
                                if (!head && file_request_handler_) {
                                    file_request_handler_(req, res);
                                }
                                return true;
                            }
                        }
                    }
                }
                return false;
            }

            inline socket_t HttpServer::create_server_socket(const char* host, int port,
                int socket_flags) const {
                return detail::create_socket(
                    host, port,
                    [](socket_t sock, struct addrinfo& ai) -> bool {
                    if (::bind(sock, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen))) {
                        return false;
                    }
                    if (::listen(sock, 5)) { // Listen through 5 channels
                        return false;
                    }
                    return true;
                },
                    socket_flags);
            }

            inline int HttpServer::bind_internal(const char* host, int port, int socket_flags) {
                if (!is_valid()) { return -1; }

                svr_sock_ = create_server_socket(host, port, socket_flags);
                if (svr_sock_ == INVALID_SOCKET) { return -1; }

                if (port == 0) {
                    struct sockaddr_storage addr;
                    socklen_t addr_len = sizeof(addr);
                    if (getsockname(svr_sock_, reinterpret_cast<struct sockaddr*>(&addr),
                        &addr_len) == -1) {
                        return -1;
                    }
                    if (addr.ss_family == AF_INET) {
                        return ntohs(reinterpret_cast<struct sockaddr_in*>(&addr)->sin_port);
                    }
                    else if (addr.ss_family == AF_INET6) {
                        return ntohs(reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_port);
                    }
                    else {
                        return -1;
                    }
                }
                else {
                    return port;
                }
            }

            inline bool HttpServer::listen_internal() {
                auto ret = true;
                is_running_ = true;

                {
                    std::unique_ptr<detail::TaskQueue> task_queue(new_task_queue());

                    while (svr_sock_ != INVALID_SOCKET) {
#ifndef _WIN32
                        if (idle_interval_sec_ > 0 || idle_interval_usec_ > 0) {
#endif
                            auto val = detail::select_read(svr_sock_, idle_interval_sec_,
                                idle_interval_usec_);
                            if (val == 0) { // Timeout
                                task_queue->on_idle();
                                continue;
                            }
#ifndef _WIN32
                        }
#endif
                        socket_t sock = accept(svr_sock_, nullptr, nullptr);

                        if (sock == INVALID_SOCKET) {
                            if (errno == EMFILE) {
                                // The per-process limit of open file descriptors has been reached.
                                // Try to accept new connections after a short sleep.
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                                continue;
                            }
                            if (svr_sock_ != INVALID_SOCKET) {
                                detail::close_socket(svr_sock_);
                                ret = false;
                            }
                            else {
                                ; // The server socket was closed by user.
                            }
                            break;
                        }

#if __cplusplus > 201703L
                        task_queue->enqueue([=, this]() { process_and_close_socket(sock); });
#else
                        task_queue->enqueue([=]() { process_and_close_socket(sock); });
#endif
                    }

                    task_queue->shutdown();
                }

                is_running_ = false;
                return ret;
            }

            inline bool HttpServer::routing(Request& req, Response& res, Stream& strm) {
                // File handler
                bool is_head_request = req.method == "HEAD";
                if ((req.method == "GET" || is_head_request) &&
                    handle_file_request(req, res, is_head_request)) {
                    return true;
                }

                if (detail::expect_content(req)) {
                    // Content reader handler
                    {
                        ContentReader reader(
                            [&](ContentReceiver receiver) {
                            return read_content_with_content_receiver(strm, req, res, receiver,
                                nullptr, nullptr);
                        },
                            [&](MultipartContentHeader header, ContentReceiver receiver) {
                            return read_content_with_content_receiver(strm, req, res, nullptr,
                                header, receiver);
                        });

                        if (req.method == "POST") {
                            if (dispatch_request_for_content_reader(
                                req, res, reader, post_handlers_for_content_reader_)) {
                                return true;
                            }
                        }
                        else if (req.method == "PUT") {
                            if (dispatch_request_for_content_reader(
                                req, res, reader, put_handlers_for_content_reader_)) {
                                return true;
                            }
                        }
                        else if (req.method == "PATCH") {
                            if (dispatch_request_for_content_reader(
                                req, res, reader, patch_handlers_for_content_reader_)) {
                                return true;
                            }
                        }
                        else if (req.method == "DELETE") {
                            if (dispatch_request_for_content_reader(
                                req, res, reader, delete_handlers_for_content_reader_)) {
                                return true;
                            }
                        }
                    }

                    // Read content into `req.body`
                    if (!read_content(strm, req, res)) { return false; }
                }

                // Regular handler
                if (req.method == "GET" || req.method == "HEAD") {
                    return dispatch_request(req, res, get_handlers_);
                }
                else if (req.method == "POST") {
                    return dispatch_request(req, res, post_handlers_);
                }
                else if (req.method == "PUT") {
                    return dispatch_request(req, res, put_handlers_);
                }
                else if (req.method == "DELETE") {
                    return dispatch_request(req, res, delete_handlers_);
                }
                else if (req.method == "OPTIONS") {
                    return dispatch_request(req, res, options_handlers_);
                }
                else if (req.method == "PATCH") {
                    return dispatch_request(req, res, patch_handlers_);
                }

                res.status = 400;
                return false;
            }

            inline bool HttpServer::dispatch_request(Request& req, Response& res,
                Handlers& handlers) {

                try {
                    for (const auto& x : handlers) {
                        const auto& pattern = x.first;
                        const auto& handler = x.second;

                        if (std::regex_match(req.path, req.matches, pattern)) {
                            handler(req, res);
                            return true;
                        }
                    }
                }
                catch (const std::exception& ex) {
                    res.status = 500;
                    res.set_header("EXCEPTION_WHAT", ex.what());
                }
                catch (...) {
                    res.status = 500;
                    res.set_header("EXCEPTION_WHAT", "UNKNOWN");
                }
                return false;
            }

            inline bool HttpServer::dispatch_request_for_content_reader(
                Request& req, Response& res, ContentReader content_reader,
                HandlersForContentReader& handlers) {
                for (const auto& x : handlers) {
                    const auto& pattern = x.first;
                    const auto& handler = x.second;

                    if (std::regex_match(req.path, req.matches, pattern)) {
                        handler(req, res, content_reader);
                        return true;
                    }
                }
                return false;
            }

            inline bool
                HttpServer::process_request(Stream& strm, bool last_connection,
                    bool& connection_close,
                    const std::function<void(Request&)>& setup_request) {
                std::array<char, 2048> buf{};

                detail::stream_line_reader line_reader(strm, buf.data(), buf.size());

                // Connection has been closed on client
                if (!line_reader.getline()) { return false; }

                Request req;
                Response res;

                res.version = "HTTP/1.1";

                // Check if the request URI doesn't exceed the limit
                if (line_reader.size() > detail::CRAFT_ENGINE_NET_HTTP_REQUEST_URI_MAX_LENGTH) {
                    Headers dummy;
                    detail::read_headers(strm, dummy);
                    res.status = 414;
                    return write_response(strm, last_connection, req, res);
                }

                // Request line and headers
                if (!parse_request_line(line_reader.ptr(), req) ||
                    !detail::read_headers(strm, req.headers)) {
                    res.status = 400;
                    return write_response(strm, last_connection, req, res);
                }

                if (req.get_header_value("Connection") == "close") {
                    connection_close = true;
                }

                if (req.version == "HTTP/1.0" &&
                    req.get_header_value("Connection") != "Keep-Alive") {
                    connection_close = true;
                }

                strm.get_remote_ip_and_port(req.remote_addr, req.remote_port);
                req.set_header("REMOTE_ADDR", req.remote_addr);
                req.set_header("REMOTE_PORT", std::to_string(req.remote_port));

                if (req.has_header("Range")) {
                    const auto& range_header_value = req.get_header_value("Range");
                    if (!detail::parse_range_header(range_header_value, req.ranges)) {
                        // TODO: error
                    }
                }

                if (setup_request) { setup_request(req); }

                if (req.get_header_value("Expect") == "100-continue") {
                    auto status = 100;
                    if (expect_100_continue_handler_) {
                        status = expect_100_continue_handler_(req, res);
                    }
                    switch (status) {
                    case 100:
                    case 417:
                        strm.write_format("HTTP/1.1 %d %s\r\n\r\n", status,
                            detail::status_message(status));
                        break;
                    default: return write_response(strm, last_connection, req, res);
                    }
                }

                // Rounting
                if (routing(req, res, strm)) {
                    if (res.status == -1) { res.status = req.ranges.empty() ? 200 : 206; }
                }
                else {
                    if (res.status == -1) { res.status = 404; }
                }

                return write_response(strm, last_connection, req, res);
            }

            inline bool HttpServer::is_valid() const { return true; }

            inline bool HttpServer::process_and_close_socket(socket_t sock) {
                return detail::process_and_close_socket(
                    false, sock, keep_alive_max_count_, read_timeout_sec_, read_timeout_usec_,
                    write_timeout_sec_, write_timeout_usec_,
                    [this](Stream& strm, bool last_connection, bool& connection_close) {
                    return process_request(strm, last_connection, connection_close,
                        nullptr);
                });
            }







            /*
             * SSL Implementation
             */
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT

            // SSL HTTP server implementation
            inline SSLServer::SSLServer(const char* cert_path, const char* private_key_path,
                const char* client_ca_cert_file_path,
                const char* client_ca_cert_dir_path) {
                ctx_ = SSL_CTX_new(SSLv23_server_method());

                if (ctx_) {
                    SSL_CTX_set_options(ctx_,
                        SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
                        SSL_OP_NO_COMPRESSION |
                        SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);

                    // auto ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
                    // SSL_CTX_set_tmp_ecdh(ctx_, ecdh);
                    // EC_KEY_free(ecdh);

                    if (SSL_CTX_use_certificate_chain_file(ctx_, cert_path) != 1 ||
                        SSL_CTX_use_PrivateKey_file(ctx_, private_key_path, SSL_FILETYPE_PEM) !=
                        1) {
                        SSL_CTX_free(ctx_);
                        ctx_ = nullptr;
                    }
                    else if (client_ca_cert_file_path || client_ca_cert_dir_path) {
                        // if (client_ca_cert_file_path) {
                        //   auto list = SSL_load_client_CA_file(client_ca_cert_file_path);
                        //   SSL_CTX_set_client_CA_list(ctx_, list);
                        // }

                        SSL_CTX_load_verify_locations(ctx_, client_ca_cert_file_path,
                            client_ca_cert_dir_path);

                        SSL_CTX_set_verify(
                            ctx_,
                            SSL_VERIFY_PEER |
                            SSL_VERIFY_FAIL_IF_NO_PEER_CERT, // SSL_VERIFY_CLIENT_ONCE,
                            nullptr);
                    }
                }
            }

            inline SSLServer::SSLServer(X509* cert, EVP_PKEY* private_key,
                X509_STORE* client_ca_cert_store) {
                ctx_ = SSL_CTX_new(SSLv23_server_method());

                if (ctx_) {
                    SSL_CTX_set_options(ctx_,
                        SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
                        SSL_OP_NO_COMPRESSION |
                        SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);

                    if (SSL_CTX_use_certificate(ctx_, cert) != 1 ||
                        SSL_CTX_use_PrivateKey(ctx_, private_key) != 1) {
                        SSL_CTX_free(ctx_);
                        ctx_ = nullptr;
                    }
                    else if (client_ca_cert_store) {

                        SSL_CTX_set_cert_store(ctx_, client_ca_cert_store);

                        SSL_CTX_set_verify(
                            ctx_,
                            SSL_VERIFY_PEER |
                            SSL_VERIFY_FAIL_IF_NO_PEER_CERT, // SSL_VERIFY_CLIENT_ONCE,
                            nullptr);
                    }
                }
            }

            inline SSLServer::~SSLServer() {
                if (ctx_) { SSL_CTX_free(ctx_); }
            }

            inline bool SSLServer::is_valid() const { return ctx_; }

            inline bool SSLServer::process_and_close_socket(socket_t sock) {
                return detail::process_and_close_socket_ssl(
                    false, sock, keep_alive_max_count_, read_timeout_sec_, read_timeout_usec_,
                    write_timeout_sec_, write_timeout_usec_, ctx_, ctx_mutex_, SSL_accept,
                    [](SSL* /*ssl*/) { return true; },
                    [this](SSL* ssl, Stream& strm, bool last_connection,
                        bool& connection_close) {
                    return process_request(strm, last_connection, connection_close,
                        [&](Request& req) { req.ssl = ssl; });
                });
            }
#endif




		}
	}
}

