#pragma once

#include "HttpCommon.h"

namespace CraftEngine
{
    namespace net
    {

        namespace http
        {


            class HttpClient {
            public:
                explicit HttpClient(const std::string& host);

                explicit HttpClient(const std::string& host, int port);

                explicit HttpClient(const std::string& host, int port,
                    const std::string& client_cert_path,
                    const std::string& client_key_path);

                virtual ~HttpClient();

                virtual bool is_valid() const;

                std::shared_ptr<Response> Get(const char* path);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers);

                std::shared_ptr<Response> Get(const char* path, Progress progress);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    Progress progress);

                std::shared_ptr<Response> Get(const char* path,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response>
                    Get(const char* path, ContentReceiver content_receiver, Progress progress);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ContentReceiver content_receiver,
                    Progress progress);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver);

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver,
                    Progress progress);

                std::shared_ptr<Response> Head(const char* path);

                std::shared_ptr<Response> Head(const char* path, const Headers& headers);

                std::shared_ptr<Response> Post(const char* path);

                std::shared_ptr<Response> Post(const char* path, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Post(const char* path, const Params& params);

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const Params& params);

                std::shared_ptr<Response> Post(const char* path,
                    const MultipartFormDataItems& items);

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const MultipartFormDataItems& items);

                std::shared_ptr<Response> Put(const char* path);

                std::shared_ptr<Response> Put(const char* path, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Put(const char* path, const Params& params);

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    const Params& params);

                std::shared_ptr<Response> Patch(const char* path, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Patch(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type);

                std::shared_ptr<Response> Delete(const char* path);

                std::shared_ptr<Response> Delete(const char* path, const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Delete(const char* path, const Headers& headers);

                std::shared_ptr<Response> Delete(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type);

                std::shared_ptr<Response> Options(const char* path);

                std::shared_ptr<Response> Options(const char* path, const Headers& headers);

                bool send(const Request& req, Response& res);

                bool send(const std::vector<Request>& requests,
                    std::vector<Response>& responses);

                void stop();

                void set_connection_timeout(time_t sec, time_t usec = 0);
                void set_read_timeout(time_t sec, time_t usec = 0);
                void set_write_timeout(time_t sec, time_t usec = 0);

                void set_keep_alive_max_count(size_t count);

                void set_basic_auth(const char* username, const char* password);
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                void set_digest_auth(const char* username, const char* password);
#endif

                void set_follow_location(bool on);

                void set_compress(bool on);

                void set_decompress(bool on);

                void set_interface(const char* intf);

                void set_proxy(const char* host, int port);
                void set_proxy_basic_auth(const char* username, const char* password);
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                void set_proxy_digest_auth(const char* username, const char* password);
#endif

                void set_logger(Logger logger);

            protected:
                bool process_request(Stream& strm, const Request& req, Response& res,
                    bool last_connection, bool& connection_close);

                std::atomic<socket_t> sock_;

                const std::string host_;
                const int port_;
                const std::string host_and_port_;

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
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                std::string digest_auth_username_;
                std::string digest_auth_password_;
#endif

                bool follow_location_ = false;

                bool compress_ = false;
                bool decompress_ = true;

                std::string interface_;

                std::string proxy_host_;
                int proxy_port_;

                std::string proxy_basic_auth_username_;
                std::string proxy_basic_auth_password_;
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                std::string proxy_digest_auth_username_;
                std::string proxy_digest_auth_password_;
#endif

                Logger logger_;

                void copy_settings(const HttpClient& rhs) {
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
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                    digest_auth_username_ = rhs.digest_auth_username_;
                    digest_auth_password_ = rhs.digest_auth_password_;
#endif
                    follow_location_ = rhs.follow_location_;
                    compress_ = rhs.compress_;
                    decompress_ = rhs.decompress_;
                    interface_ = rhs.interface_;
                    proxy_host_ = rhs.proxy_host_;
                    proxy_port_ = rhs.proxy_port_;
                    proxy_basic_auth_username_ = rhs.proxy_basic_auth_username_;
                    proxy_basic_auth_password_ = rhs.proxy_basic_auth_password_;
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                    proxy_digest_auth_username_ = rhs.proxy_digest_auth_username_;
                    proxy_digest_auth_password_ = rhs.proxy_digest_auth_password_;
#endif
                    logger_ = rhs.logger_;
                }

            private:
                socket_t create_client_socket() const;
                bool read_response_line(Stream& strm, Response& res);
                bool write_request(Stream& strm, const Request& req, bool last_connection);
                bool redirect(const Request& req, Response& res);
                bool handle_request(Stream& strm, const Request& req, Response& res,
                    bool last_connection, bool& connection_close);
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                bool connect(socket_t sock, Response& res, bool& error);
#endif

                std::shared_ptr<Response> send_with_content_provider(
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


            class HttpClient_v2 {
            public:
                explicit HttpClient_v2(const char* scheme_host_port)
                    : HttpClient_v2(scheme_host_port, std::string(), std::string()) {}

                explicit HttpClient_v2(const char* scheme_host_port,
                    const std::string& client_cert_path,
                    const std::string& client_key_path) {
                    const static std::regex re(R"(^(https?)://([^:/?#]+)(?::(\d+))?)");

                    std::cmatch m;
                    if (std::regex_match(scheme_host_port, m, re)) {
                        auto scheme = m[1].str();
                        auto host = m[2].str();
                        auto port_str = m[3].str();

                        auto port = !port_str.empty() ? std::stoi(port_str)
                            : (scheme == "https" ? 443 : 80);

                        if (scheme == "https") {
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                            is_ssl_ = true;
                            cli_ = std::make_shared<SSLClient>(host.c_str(), port, client_cert_path,
                                client_key_path);
#endif
                        }
                        else {
                            cli_ = std::make_shared<HttpClient>(host.c_str(), port, client_cert_path,
                                client_key_path);
                        }
                    }
                }

                ~HttpClient_v2() {}

                bool is_valid() const { return cli_ != nullptr; }

                std::shared_ptr<Response> Get(const char* path) { return cli_->Get(path); }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers) {
                    return cli_->Get(path, headers);
                }

                std::shared_ptr<Response> Get(const char* path, Progress progress) {
                    return cli_->Get(path, progress);
                }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    Progress progress) {
                    return cli_->Get(path, headers, progress);
                }

                std::shared_ptr<Response> Get(const char* path,
                    ContentReceiver content_receiver) {
                    return cli_->Get(path, content_receiver);
                }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ContentReceiver content_receiver) {
                    return cli_->Get(path, headers, content_receiver);
                }

                std::shared_ptr<Response>
                    Get(const char* path, ContentReceiver content_receiver, Progress progress) {
                    return cli_->Get(path, content_receiver, progress);
                }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ContentReceiver content_receiver,
                    Progress progress) {
                    return cli_->Get(path, headers, content_receiver, progress);
                }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver) {
                    return cli_->Get(path, headers, response_handler, content_receiver);
                }

                std::shared_ptr<Response> Get(const char* path, const Headers& headers,
                    ResponseHandler response_handler,
                    ContentReceiver content_receiver,
                    Progress progress) {
                    return cli_->Get(path, headers, response_handler, content_receiver,
                        progress);
                }

                std::shared_ptr<Response> Head(const char* path) { return cli_->Head(path); }

                std::shared_ptr<Response> Head(const char* path, const Headers& headers) {
                    return cli_->Head(path, headers);
                }

                std::shared_ptr<Response> Post(const char* path) { return cli_->Post(path); }

                std::shared_ptr<Response> Post(const char* path, const std::string& body,
                    const char* content_type) {
                    return cli_->Post(path, body, content_type);
                }

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type) {
                    return cli_->Post(path, headers, body, content_type);
                }

                std::shared_ptr<Response> Post(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Post(path, content_length, content_provider, content_type);
                }

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Post(path, headers, content_length, content_provider,
                        content_type);
                }

                std::shared_ptr<Response> Post(const char* path, const Params& params) {
                    return cli_->Post(path, params);
                }

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const Params& params) {
                    return cli_->Post(path, headers, params);
                }

                std::shared_ptr<Response> Post(const char* path,
                    const MultipartFormDataItems& items) {
                    return cli_->Post(path, items);
                }

                std::shared_ptr<Response> Post(const char* path, const Headers& headers,
                    const MultipartFormDataItems& items) {
                    return cli_->Post(path, headers, items);
                }

                std::shared_ptr<Response> Put(const char* path) { return cli_->Put(path); }

                std::shared_ptr<Response> Put(const char* path, const std::string& body,
                    const char* content_type) {
                    return cli_->Put(path, body, content_type);
                }

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type) {
                    return cli_->Put(path, headers, body, content_type);
                }

                std::shared_ptr<Response> Put(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Put(path, content_length, content_provider, content_type);
                }

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Put(path, headers, content_length, content_provider,
                        content_type);
                }

                std::shared_ptr<Response> Put(const char* path, const Params& params) {
                    return cli_->Put(path, params);
                }

                std::shared_ptr<Response> Put(const char* path, const Headers& headers,
                    const Params& params) {
                    return cli_->Put(path, headers, params);
                }

                std::shared_ptr<Response> Patch(const char* path, const std::string& body,
                    const char* content_type) {
                    return cli_->Patch(path, body, content_type);
                }

                std::shared_ptr<Response> Patch(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type) {
                    return cli_->Patch(path, headers, body, content_type);
                }

                std::shared_ptr<Response> Patch(const char* path, size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Patch(path, content_length, content_provider, content_type);
                }

                std::shared_ptr<Response> Patch(const char* path, const Headers& headers,
                    size_t content_length,
                    ContentProvider content_provider,
                    const char* content_type) {
                    return cli_->Patch(path, headers, content_length, content_provider,
                        content_type);
                }

                std::shared_ptr<Response> Delete(const char* path) {
                    return cli_->Delete(path);
                }

                std::shared_ptr<Response> Delete(const char* path, const std::string& body,
                    const char* content_type) {
                    return cli_->Delete(path, body, content_type);
                }

                std::shared_ptr<Response> Delete(const char* path, const Headers& headers) {
                    return cli_->Delete(path, headers);
                }

                std::shared_ptr<Response> Delete(const char* path, const Headers& headers,
                    const std::string& body,
                    const char* content_type) {
                    return cli_->Delete(path, headers, body, content_type);
                }

                std::shared_ptr<Response> Options(const char* path) {
                    return cli_->Options(path);
                }

                std::shared_ptr<Response> Options(const char* path, const Headers& headers) {
                    return cli_->Options(path, headers);
                }

                bool send(const Request& req, Response& res) { return cli_->send(req, res); }

                bool send(const std::vector<Request>& requests,
                    std::vector<Response>& responses) {
                    return cli_->send(requests, responses);
                }

                void stop() { cli_->stop(); }

                HttpClient_v2& set_connection_timeout(time_t sec, time_t usec) {
                    cli_->set_connection_timeout(sec, usec);
                    return *this;
                }

                HttpClient_v2& set_read_timeout(time_t sec, time_t usec) {
                    cli_->set_read_timeout(sec, usec);
                    return *this;
                }

                HttpClient_v2& set_keep_alive_max_count(size_t count) {
                    cli_->set_keep_alive_max_count(count);
                    return *this;
                }

                HttpClient_v2& set_basic_auth(const char* username, const char* password) {
                    cli_->set_basic_auth(username, password);
                    return *this;
                }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                HttpClient_v2& set_digest_auth(const char* username, const char* password) {
                    cli_->set_digest_auth(username, password);
                    return *this;
                }
#endif

                HttpClient_v2& set_follow_location(bool on) {
                    cli_->set_follow_location(on);
                    return *this;
                }

                HttpClient_v2& set_compress(bool on) {
                    cli_->set_compress(on);
                    return *this;
                }

                HttpClient_v2& set_decompress(bool on) {
                    cli_->set_decompress(on);
                    return *this;
                }

                HttpClient_v2& set_interface(const char* intf) {
                    cli_->set_interface(intf);
                    return *this;
                }

                HttpClient_v2& set_proxy(const char* host, int port) {
                    cli_->set_proxy(host, port);
                    return *this;
                }

                HttpClient_v2& set_proxy_basic_auth(const char* username, const char* password) {
                    cli_->set_proxy_basic_auth(username, password);
                    return *this;
                }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                HttpClient_v2& set_proxy_digest_auth(const char* username, const char* password) {
                    cli_->set_proxy_digest_auth(username, password);
                    return *this;
                }
#endif

                HttpClient_v2& set_logger(Logger logger) {
                    cli_->set_logger(logger);
                    return *this;
                }

                // SSL
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                HttpClient_v2& set_ca_cert_path(const char* ca_cert_file_path,
                    const char* ca_cert_dir_path = nullptr) {
                    if (is_ssl_) {
                        static_cast<SSLClient&>(*cli_).set_ca_cert_path(ca_cert_file_path,
                            ca_cert_dir_path);
                    }
                    return *this;
                }

                HttpClient_v2& set_ca_cert_store(X509_STORE* ca_cert_store) {
                    if (is_ssl_) {
                        static_cast<SSLClient&>(*cli_).set_ca_cert_store(ca_cert_store);
                    }
                    return *this;
                }

                HttpClient_v2& enable_server_certificate_verification(bool enabled) {
                    if (is_ssl_) {
                        static_cast<SSLClient&>(*cli_).enable_server_certificate_verification(
                            enabled);
                    }
                    return *this;
                }

                long get_openssl_verify_result() const {
                    if (is_ssl_) {
                        return static_cast<SSLClient&>(*cli_).get_openssl_verify_result();
                    }
                    return -1; // NOTE: -1 doesn't match any of X509_V_ERR_???
                }

                SSL_CTX* ssl_context() const {
                    if (is_ssl_) { return static_cast<SSLClient&>(*cli_).ssl_context(); }
                    return nullptr;
                }
#endif

            private:
                bool is_ssl_ = false;
                std::shared_ptr<HttpClient> cli_;
            };





#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT

            class SSLClient : public HttpClient {
            public:
                explicit SSLClient(const std::string& host);

                explicit SSLClient(const std::string& host, int port);

                explicit SSLClient(const std::string& host, int port,
                    const std::string& client_cert_path,
                    const std::string& client_key_path);

                explicit SSLClient(const std::string& host, int port, X509* client_cert,
                    EVP_PKEY* client_key);

                ~SSLClient() override;

                bool is_valid() const override;

                void set_ca_cert_path(const char* ca_cert_file_path,
                    const char* ca_cert_dir_path = nullptr);

                void set_ca_cert_store(X509_STORE* ca_cert_store);

                void enable_server_certificate_verification(bool enabled);

                long get_openssl_verify_result() const;

                SSL_CTX* ssl_context() const;

            private:
                bool process_and_close_socket(
                    socket_t sock, size_t request_count,
                    std::function<bool(Stream& strm, bool last_connection,
                        bool& connection_close)>
                    callback) override;
                bool is_ssl() const override;

                bool verify_host(X509* server_cert) const;
                bool verify_host_with_subject_alt_name(X509* server_cert) const;
                bool verify_host_with_common_name(X509* server_cert) const;
                bool check_host_name(const char* pattern, size_t pattern_len) const;

                SSL_CTX* ctx_;
                std::mutex ctx_mutex_;
                std::vector<std::string> host_components_;

                std::string ca_cert_file_path_;
                std::string ca_cert_dir_path_;
                X509_STORE* ca_cert_store_ = nullptr;
                bool server_certificate_verification_ = false;
                long verify_result_ = 0;
            };
#endif




            // HTTP client implementation
            inline HttpClient::HttpClient(const std::string& host)
                : HttpClient(host, 80, std::string(), std::string()) {}

            inline HttpClient::HttpClient(const std::string& host, int port)
                : HttpClient(host, port, std::string(), std::string()) {}

            inline HttpClient::HttpClient(const std::string& host, int port,
                const std::string& client_cert_path,
                const std::string& client_key_path)
                : sock_(INVALID_SOCKET), host_(host), port_(port),
                host_and_port_(host_ + ":" + std::to_string(port_)),
                client_cert_path_(client_cert_path), client_key_path_(client_key_path) {}

            inline HttpClient::~HttpClient() {}

            inline bool HttpClient::is_valid() const { return true; }

            inline socket_t HttpClient::create_client_socket() const {
                if (!proxy_host_.empty()) {
                    return detail::create_client_socket(proxy_host_.c_str(), proxy_port_,
                        connection_timeout_sec_,
                        connection_timeout_usec_, interface_);
                }
                return detail::create_client_socket(host_.c_str(), port_,
                    connection_timeout_sec_,
                    connection_timeout_usec_, interface_);
            }

            inline bool HttpClient::read_response_line(Stream& strm, Response& res) {
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

            inline bool HttpClient::send(const Request& req, Response& res) {
                sock_ = create_client_socket();
                if (sock_ == INVALID_SOCKET) { return false; }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                if (is_ssl() && !proxy_host_.empty()) {
                    bool error;
                    if (!connect(sock_, res, error)) { return error; }
                }
#endif

                return process_and_close_socket(
                    sock_, 1,
                    [&](Stream& strm, bool last_connection, bool& connection_close) {
                    return handle_request(strm, req, res, last_connection,
                        connection_close);
                });
            }

            inline bool HttpClient::send(const std::vector<Request>& requests,
                std::vector<Response>& responses) {
                size_t i = 0;
                while (i < requests.size()) {
                    sock_ = create_client_socket();
                    if (sock_ == INVALID_SOCKET) { return false; }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                    if (is_ssl() && !proxy_host_.empty()) {
                        Response res;
                        bool error;
                        if (!connect(sock_, res, error)) { return false; }
                    }
#endif

                    if (!process_and_close_socket(sock_, requests.size() - i,
                        [&](Stream& strm, bool last_connection,
                            bool& connection_close) -> bool {
                        auto& req = requests[i++];
                        auto res = Response();
                        auto ret = handle_request(strm, req, res,
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

            inline bool HttpClient::handle_request(Stream& strm, const Request& req,
                Response& res, bool last_connection,
                bool& connection_close) {
                if (req.path.empty()) { return false; }

                bool ret;

                if (!is_ssl() && !proxy_host_.empty()) {
                    auto req2 = req;
                    req2.path = "http://" + host_and_port_ + req.path;
                    ret = process_request(strm, req2, res, last_connection, connection_close);
                }
                else {
                    ret = process_request(strm, req, res, last_connection, connection_close);
                }

                if (!ret) { return false; }

                if (300 < res.status && res.status < 400 && follow_location_) {
                    ret = redirect(req, res);
                }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                if ((res.status == 401 || res.status == 407) &&
                    req.authorization_count_ < 5) {
                    auto is_proxy = res.status == 407;
                    const auto& username =
                        is_proxy ? proxy_digest_auth_username_ : digest_auth_username_;
                    const auto& password =
                        is_proxy ? proxy_digest_auth_password_ : digest_auth_password_;

                    if (!username.empty() && !password.empty()) {
                        std::map<std::string, std::string> auth;
                        if (parseTripleWAuthenticate(res, auth, is_proxy)) {
                            Request new_req = req;
                            new_req.authorization_count_ += 1;
                            auto key = is_proxy ? "Proxy-Authorization" : "Authorization";
                            new_req.headers.erase(key);
                            new_req.headers.insert(make_digest_authentication_header(
                                req, auth, new_req.authorization_count_, makeRandomString(10),
                                username, password, is_proxy));

                            Response new_res;

                            ret = send(new_req, new_res);
                            if (ret) { res = new_res; }
                        }
                    }
                }
#endif

                return ret;
            }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
            inline bool HttpClient::connect(socket_t sock, Response& res, bool& error) {
                error = true;
                Response res2;

                if (!detail::process_socket(
                    true, sock, 1, read_timeout_sec_, read_timeout_usec_,
                    write_timeout_sec_, write_timeout_usec_,
                    [&](Stream& strm, bool /*last_connection*/, bool& connection_close) {
                    Request req2;
                    req2.method = "CONNECT";
                    req2.path = host_and_port_;
                    return process_request(strm, req2, res2, false, connection_close);
                })) {
                    detail::close_socket(sock);
                    error = false;
                    return false;
                }

                if (res2.status == 407) {
                    if (!proxy_digest_auth_username_.empty() &&
                        !proxy_digest_auth_password_.empty()) {
                        std::map<std::string, std::string> auth;
                        if (parseTripleWAuthenticate(res2, auth, true)) {
                            Response res3;
                            if (!detail::process_socket(
                                true, sock, 1, read_timeout_sec_, read_timeout_usec_,
                                write_timeout_sec_, write_timeout_usec_,
                                [&](Stream& strm, bool /*last_connection*/,
                                    bool& connection_close) {
                                Request req3;
                                req3.method = "CONNECT";
                                req3.path = host_and_port_;
                                req3.headers.insert(make_digest_authentication_header(
                                    req3, auth, 1, makeRandomString(10),
                                    proxy_digest_auth_username_, proxy_digest_auth_password_,
                                    true));
                                return process_request(strm, req3, res3, false,
                                    connection_close);
                            })) {
                                detail::close_socket(sock);
                                error = false;
                                return false;
                            }
                        }
                    }
                    else {
                        res = res2;
                        return false;
                    }
                }

                return true;
            }
#endif

            inline bool HttpClient::redirect(const Request& req, Response& res) {
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

                auto next_port = port_;
                if (!port_str.empty()) {
                    next_port = std::stoi(port_str);
                }
                else if (!next_scheme.empty()) {
                    next_port = next_scheme == "https" ? 443 : 80;
                }

                if (next_scheme.empty()) { next_scheme = scheme; }
                if (next_host.empty()) { next_host = host_; }
                if (next_path.empty()) { next_path = "/"; }

                if (next_scheme == scheme && next_host == host_ && next_port == port_) {
                    return detail::redirect(*this, req, res, next_path);
                }
                else {
                    if (next_scheme == "https") {
#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
                        SSLClient cli(next_host.c_str(), next_port);
                        cli.copy_settings(*this);
                        return detail::redirect(cli, req, res, next_path);
#else
                        return false;
#endif
                    }
                    else {
                        HttpClient cli(next_host.c_str(), next_port);
                        cli.copy_settings(*this);
                        return detail::redirect(cli, req, res, next_path);
                    }
                }
            }

            inline bool HttpClient::write_request(Stream& strm, const Request& req,
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
                        if (port_ == 443) {
                            headers.emplace("Host", host_);
                        }
                        else {
                            headers.emplace("Host", host_and_port_);
                        }
                    }
                    else {
                        if (port_ == 80) {
                            headers.emplace("Host", host_);
                        }
                        else {
                            headers.emplace("Host", host_and_port_);
                        }
                    }
                }

                if (!req.has_header("Accept")) { headers.emplace("Accept", "*/*"); }

                if (!req.has_header("User-Agent")) {
                    headers.emplace("User-Agent", "cpp-httplib/0.6");
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

            inline std::shared_ptr<Response> HttpClient::send_with_content_provider(
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

                return send(req, *res) ? res : nullptr;
            }

            inline bool HttpClient::process_request(Stream& strm, const Request& req,
                Response& res, bool last_connection,
                bool& connection_close) {
                // Send request
                if (!write_request(strm, req, last_connection)) { return false; }

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

            inline bool HttpClient::process_and_close_socket(
                socket_t sock, size_t request_count,
                std::function<bool(Stream& strm, bool last_connection,
                    bool& connection_close)>
                callback) {
                request_count = (std::min)(request_count, keep_alive_max_count_);
                return detail::process_and_close_socket(
                    true, sock, request_count, read_timeout_sec_, read_timeout_usec_,
                    write_timeout_sec_, write_timeout_usec_, callback);
            }

            inline bool HttpClient::is_ssl() const { return false; }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path) {
                return Get(path, Headers(), Progress());
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                Progress progress) {
                return Get(path, Headers(), std::move(progress));
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                const Headers& headers) {
                return Get(path, headers, Progress());
            }

            inline std::shared_ptr<Response>
                HttpClient::Get(const char* path, const Headers& headers, Progress progress) {
                Request req;
                req.method = "GET";
                req.path = path;
                req.headers = headers;
                req.progress = std::move(progress);

                auto res = std::make_shared<Response>();
                return send(req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                ContentReceiver content_receiver) {
                return Get(path, Headers(), nullptr, std::move(content_receiver), Progress());
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                ContentReceiver content_receiver,
                Progress progress) {
                return Get(path, Headers(), nullptr, std::move(content_receiver),
                    std::move(progress));
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                const Headers& headers,
                ContentReceiver content_receiver) {
                return Get(path, headers, nullptr, std::move(content_receiver), Progress());
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                const Headers& headers,
                ContentReceiver content_receiver,
                Progress progress) {
                return Get(path, headers, nullptr, std::move(content_receiver),
                    std::move(progress));
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                const Headers& headers,
                ResponseHandler response_handler,
                ContentReceiver content_receiver) {
                return Get(path, headers, std::move(response_handler), content_receiver,
                    Progress());
            }

            inline std::shared_ptr<Response> HttpClient::Get(const char* path,
                const Headers& headers,
                ResponseHandler response_handler,
                ContentReceiver content_receiver,
                Progress progress) {
                Request req;
                req.method = "GET";
                req.path = path;
                req.headers = headers;
                req.response_handler = std::move(response_handler);
                req.content_receiver = std::move(content_receiver);
                req.progress = std::move(progress);

                auto res = std::make_shared<Response>();
                return send(req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpClient::Head(const char* path) {
                return Head(path, Headers());
            }

            inline std::shared_ptr<Response> HttpClient::Head(const char* path,
                const Headers& headers) {
                Request req;
                req.method = "HEAD";
                req.headers = headers;
                req.path = path;

                auto res = std::make_shared<Response>();

                return send(req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpClient::Post(const char* path) {
                return Post(path, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpClient::Post(const char* path,
                const std::string& body,
                const char* content_type) {
                return Post(path, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Post(const char* path,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                return send_with_content_provider("POST", path, headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Post(const char* path,
                const Params& params) {
                return Post(path, Headers(), params);
            }

            inline std::shared_ptr<Response> HttpClient::Post(const char* path,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Post(path, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpClient::Post(const char* path, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                return send_with_content_provider("POST", path, headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response>
                HttpClient::Post(const char* path, const Headers& headers, const Params& params) {
                auto query = detail::params_to_query_str(params);
                return Post(path, headers, query, "application/x-www-form-urlencoded");
            }

            inline std::shared_ptr<Response>
                HttpClient::Post(const char* path, const MultipartFormDataItems& items) {
                return Post(path, Headers(), items);
            }

            inline std::shared_ptr<Response>
                HttpClient::Post(const char* path, const Headers& headers,
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
                return Post(path, headers, body, content_type.c_str());
            }

            inline std::shared_ptr<Response> HttpClient::Put(const char* path) {
                return Put(path, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpClient::Put(const char* path,
                const std::string& body,
                const char* content_type) {
                return Put(path, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Put(const char* path,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                return send_with_content_provider("PUT", path, headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Put(const char* path,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Put(path, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpClient::Put(const char* path, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                return send_with_content_provider("PUT", path, headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Put(const char* path,
                const Params& params) {
                return Put(path, Headers(), params);
            }

            inline std::shared_ptr<Response>
                HttpClient::Put(const char* path, const Headers& headers, const Params& params) {
                auto query = detail::params_to_query_str(params);
                return Put(path, headers, query, "application/x-www-form-urlencoded");
            }

            inline std::shared_ptr<Response> HttpClient::Patch(const char* path,
                const std::string& body,
                const char* content_type) {
                return Patch(path, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Patch(const char* path,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                return send_with_content_provider("PATCH", path, headers, body, 0, nullptr,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Patch(const char* path,
                size_t content_length,
                ContentProvider content_provider,
                const char* content_type) {
                return Patch(path, Headers(), content_length, content_provider, content_type);
            }

            inline std::shared_ptr<Response>
                HttpClient::Patch(const char* path, const Headers& headers, size_t content_length,
                    ContentProvider content_provider, const char* content_type) {
                return send_with_content_provider("PATCH", path, headers, std::string(),
                    content_length, content_provider,
                    content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Delete(const char* path) {
                return Delete(path, Headers(), std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpClient::Delete(const char* path,
                const std::string& body,
                const char* content_type) {
                return Delete(path, Headers(), body, content_type);
            }

            inline std::shared_ptr<Response> HttpClient::Delete(const char* path,
                const Headers& headers) {
                return Delete(path, headers, std::string(), nullptr);
            }

            inline std::shared_ptr<Response> HttpClient::Delete(const char* path,
                const Headers& headers,
                const std::string& body,
                const char* content_type) {
                Request req;
                req.method = "DELETE";
                req.headers = headers;
                req.path = path;

                if (content_type) { req.headers.emplace("Content-Type", content_type); }
                req.body = body;

                auto res = std::make_shared<Response>();

                return send(req, *res) ? res : nullptr;
            }

            inline std::shared_ptr<Response> HttpClient::Options(const char* path) {
                return Options(path, Headers());
            }

            inline std::shared_ptr<Response> HttpClient::Options(const char* path,
                const Headers& headers) {
                Request req;
                req.method = "OPTIONS";
                req.path = path;
                req.headers = headers;

                auto res = std::make_shared<Response>();

                return send(req, *res) ? res : nullptr;
            }

            inline void HttpClient::stop() {
                if (sock_ != INVALID_SOCKET) {
                    std::atomic<socket_t> sock(sock_.exchange(INVALID_SOCKET));
                    detail::shutdown_socket(sock);
                    detail::close_socket(sock);
                }
            }


            inline void HttpClient::set_connection_timeout(time_t sec, time_t usec) {
                connection_timeout_sec_ = sec;
                connection_timeout_usec_ = usec;
            }

            inline void HttpClient::set_read_timeout(time_t sec, time_t usec) {
                read_timeout_sec_ = sec;
                read_timeout_usec_ = usec;
            }

            inline void HttpClient::set_write_timeout(time_t sec, time_t usec) {
                write_timeout_sec_ = sec;
                write_timeout_usec_ = usec;
            }

            inline void HttpClient::set_keep_alive_max_count(size_t count) {
                keep_alive_max_count_ = count;
            }

            inline void HttpClient::set_basic_auth(const char* username, const char* password) {
                basic_auth_username_ = username;
                basic_auth_password_ = password;
            }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
            inline void HttpClient::set_digest_auth(const char* username,
                const char* password) {
                digest_auth_username_ = username;
                digest_auth_password_ = password;
            }
#endif

            inline void HttpClient::set_follow_location(bool on) { follow_location_ = on; }

            inline void HttpClient::set_compress(bool on) { compress_ = on; }

            inline void HttpClient::set_decompress(bool on) { decompress_ = on; }

            inline void HttpClient::set_interface(const char* intf) { interface_ = intf; }

            inline void HttpClient::set_proxy(const char* host, int port) {
                proxy_host_ = host;
                proxy_port_ = port;
            }

            inline void HttpClient::set_proxy_basic_auth(const char* username,
                const char* password) {
                proxy_basic_auth_username_ = username;
                proxy_basic_auth_password_ = password;
            }

#ifdef CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT
            inline void HttpClient::set_proxy_digest_auth(const char* username,
                const char* password) {
                proxy_digest_auth_username_ = username;
                proxy_digest_auth_password_ = password;
            }
#endif

            inline void HttpClient::set_logger(Logger logger) { logger_ = std::move(logger); }



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

            // SSL HTTP client implementation
            inline SSLClient::SSLClient(const std::string& host)
                : SSLClient(host, 443, std::string(), std::string()) {}

            inline SSLClient::SSLClient(const std::string& host, int port)
                : SSLClient(host, port, std::string(), std::string()) {}

            inline SSLClient::SSLClient(const std::string& host, int port,
                const std::string& client_cert_path,
                const std::string& client_key_path)
                : HttpClient(host, port, client_cert_path, client_key_path) {
                ctx_ = SSL_CTX_new(SSLv23_client_method());

                detail::split(&host_[0], &host_[host_.size()], '.',
                    [&](const char* b, const char* e) {
                    host_components_.emplace_back(std::string(b, e));
                });
                if (!client_cert_path.empty() && !client_key_path.empty()) {
                    if (SSL_CTX_use_certificate_file(ctx_, client_cert_path.c_str(),
                        SSL_FILETYPE_PEM) != 1 ||
                        SSL_CTX_use_PrivateKey_file(ctx_, client_key_path.c_str(),
                            SSL_FILETYPE_PEM) != 1) {
                        SSL_CTX_free(ctx_);
                        ctx_ = nullptr;
                    }
                }
            }

            inline SSLClient::SSLClient(const std::string& host, int port,
                X509* client_cert, EVP_PKEY* client_key)
                : HttpClient(host, port) {
                ctx_ = SSL_CTX_new(SSLv23_client_method());

                detail::split(&host_[0], &host_[host_.size()], '.',
                    [&](const char* b, const char* e) {
                    host_components_.emplace_back(std::string(b, e));
                });
                if (client_cert != nullptr && client_key != nullptr) {
                    if (SSL_CTX_use_certificate(ctx_, client_cert) != 1 ||
                        SSL_CTX_use_PrivateKey(ctx_, client_key) != 1) {
                        SSL_CTX_free(ctx_);
                        ctx_ = nullptr;
                    }
                }
            }

            inline SSLClient::~SSLClient() {
                if (ctx_) { SSL_CTX_free(ctx_); }
            }

            inline bool SSLClient::is_valid() const { return ctx_; }

            inline void SSLClient::set_ca_cert_path(const char* ca_cert_file_path,
                const char* ca_cert_dir_path) {
                if (ca_cert_file_path) { ca_cert_file_path_ = ca_cert_file_path; }
                if (ca_cert_dir_path) { ca_cert_dir_path_ = ca_cert_dir_path; }
            }

            inline void SSLClient::set_ca_cert_store(X509_STORE* ca_cert_store) {
                if (ca_cert_store) { ca_cert_store_ = ca_cert_store; }
            }

            inline void SSLClient::enable_server_certificate_verification(bool enabled) {
                server_certificate_verification_ = enabled;
            }

            inline long SSLClient::get_openssl_verify_result() const {
                return verify_result_;
            }

            inline SSL_CTX* SSLClient::ssl_context() const { return ctx_; }

            inline bool SSLClient::process_and_close_socket(
                socket_t sock, size_t request_count,
                std::function<bool(Stream& strm, bool last_connection,
                    bool& connection_close)>
                callback) {

                request_count = std::min(request_count, keep_alive_max_count_);

                return is_valid() &&
                    detail::process_and_close_socket_ssl(
                        true, sock, request_count, read_timeout_sec_, read_timeout_usec_,
                        write_timeout_sec_, write_timeout_usec_, ctx_, ctx_mutex_,
                        [&](SSL* ssl) {
                    if (ca_cert_file_path_.empty() && ca_cert_store_ == nullptr) {
                        SSL_CTX_set_verify(ctx_, SSL_VERIFY_NONE, nullptr);
                    }
                    else if (!ca_cert_file_path_.empty()) {
                        if (!SSL_CTX_load_verify_locations(
                            ctx_, ca_cert_file_path_.c_str(), nullptr)) {
                            return false;
                        }
                        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER, nullptr);
                    }
                    else if (ca_cert_store_ != nullptr) {
                        if (SSL_CTX_get_cert_store(ctx_) != ca_cert_store_) {
                            SSL_CTX_set_cert_store(ctx_, ca_cert_store_);
                        }
                        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER, nullptr);
                    }

                    if (SSL_connect(ssl) != 1) { return false; }

                    if (server_certificate_verification_) {
                        verify_result_ = SSL_get_verify_result(ssl);

                        if (verify_result_ != X509_V_OK) { return false; }

                        auto server_cert = SSL_get_peer_certificate(ssl);

                        if (server_cert == nullptr) { return false; }

                        if (!verify_host(server_cert)) {
                            X509_free(server_cert);
                            return false;
                        }
                        X509_free(server_cert);
                    }

                    return true;
                },
                        [&](SSL* ssl) {
                    SSL_set_tlsext_host_name(ssl, host_.c_str());
                    return true;
                },
                    [&](SSL* /*ssl*/, Stream& strm, bool last_connection,
                        bool& connection_close) {
                    return callback(strm, last_connection, connection_close);
                });
            }

            inline bool SSLClient::is_ssl() const { return true; }

            inline bool SSLClient::verify_host(X509* server_cert) const {
                /* Quote from RFC2818 section 3.1 "Server Identity"

                   If a subjectAltName extension of type dNSName is present, that MUST
                   be used as the identity. Otherwise, the (most specific) Common Name
                   field in the Subject field of the certificate MUST be used. Although
                   the use of the Common Name is existing practice, it is deprecated and
                   Certification Authorities are encouraged to use the dNSName instead.

                   Matching is performed using the matching rules specified by
                   [RFC2459].  If more than one identity of a given type is present in
                   the certificate (e.g., more than one dNSName name, a match in any one
                   of the set is considered acceptable.) Names may contain the wildcard
                   character * which is considered to match any single domain name
                   component or component fragment. E.g., *.a.com matches foo.a.com but
                   not bar.foo.a.com. f*.com matches foo.com but not bar.com.

                   In some cases, the URI is specified as an IP address rather than a
                   hostname. In this case, the iPAddress subjectAltName must be present
                   in the certificate and must exactly match the IP in the URI.

                */
                return verify_host_with_subject_alt_name(server_cert) ||
                    verify_host_with_common_name(server_cert);
            }

            inline bool
                SSLClient::verify_host_with_subject_alt_name(X509* server_cert) const {
                auto ret = false;

                auto type = GEN_DNS;

                struct in6_addr addr6;
                struct in_addr addr;
                size_t addr_len = 0;

#ifndef __MINGW32__
                if (inet_pton(AF_INET6, host_.c_str(), &addr6)) {
                    type = GEN_IPADD;
                    addr_len = sizeof(struct in6_addr);
                }
                else if (inet_pton(AF_INET, host_.c_str(), &addr)) {
                    type = GEN_IPADD;
                    addr_len = sizeof(struct in_addr);
                }
#endif

                auto alt_names = static_cast<const struct stack_st_GENERAL_NAME*>(
                    X509_get_ext_d2i(server_cert, NID_subject_alt_name, nullptr, nullptr));

                if (alt_names) {
                    auto dsn_matched = false;
                    auto ip_mached = false;

                    auto count = sk_GENERAL_NAME_num(alt_names);

                    for (auto i = 0; i < count && !dsn_matched; i++) {
                        auto val = sk_GENERAL_NAME_value(alt_names, i);
                        if (val->type == type) {
                            auto name = (const char*)ASN1_STRING_get0_data(val->d.ia5);
                            auto name_len = (size_t)ASN1_STRING_length(val->d.ia5);

                            if (strlen(name) == name_len) {
                                switch (type) {
                                case GEN_DNS: dsn_matched = check_host_name(name, name_len); break;

                                case GEN_IPADD:
                                    if (!memcmp(&addr6, name, addr_len) ||
                                        !memcmp(&addr, name, addr_len)) {
                                        ip_mached = true;
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    if (dsn_matched || ip_mached) { ret = true; }
                }

                GENERAL_NAMES_free((STACK_OF(GENERAL_NAME)*)alt_names);

                return ret;
            }

            inline bool SSLClient::verify_host_with_common_name(X509* server_cert) const {
                const auto subject_name = X509_get_subject_name(server_cert);

                if (subject_name != nullptr) {
                    char name[BUFSIZ];
                    auto name_len = X509_NAME_get_text_by_NID(subject_name, NID_commonName,
                        name, sizeof(name));

                    if (name_len != -1) {
                        return check_host_name(name, static_cast<size_t>(name_len));
                    }
                }

                return false;
            }

            inline bool SSLClient::check_host_name(const char* pattern,
                size_t pattern_len) const {
                if (host_.size() == pattern_len && host_ == pattern) { return true; }

                // Wildcard match
                // https://bugs.launchpad.net/ubuntu/+source/firefox-3.0/+bug/376484
                std::vector<std::string> pattern_components;
                detail::split(&pattern[0], &pattern[pattern_len], '.',
                    [&](const char* b, const char* e) {
                    pattern_components.emplace_back(std::string(b, e));
                });

                if (host_components_.size() != pattern_components.size()) { return false; }

                auto itr = pattern_components.begin();
                for (const auto& h : host_components_) {
                    auto& p = *itr;
                    if (p != h && p != "*") {
                        auto partial_match = (p.size() > 0 && p[p.size() - 1] == '*' &&
                            !p.compare(0, p.size() - 1, h));
                        if (!partial_match) { return false; }
                    }
                    ++itr;
                }

                return true;
            }
#endif


        }


    }
}
