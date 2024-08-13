#include "../include/application.hpp"
#include "../include/http_tools.hpp"
#include "../include/services/log.hpp"  // Include the Log service

template <class Body, class Allocator>
http::response<http::string_body> send_(
        http::request<Body, http::basic_fields<Allocator>> const& req,
        http::status status,
        const std::string& body,
        const std::string& content_type = "application/json")
{
    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, content_type);
    res.keep_alive(req.keep_alive());
    res.body() = body;
    res.prepare_payload();
    Log::get().log(Level::INFO, "[handle_request] Sending response: " + std::string(res.reason()));
    return res;
}

template <class Body, class Allocator>
http::message_generator handle_post_request(
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{
    Log::get().log(Level::INFO, "[handle_post_request] Processing POST request");
    return send_(req, http::status::ok, R"({"message": "POST request processed"})");
}

template <class Body, class Allocator>
http::message_generator handle_get_request(
        beast::string_view doc_root,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{
    Log::get().log(Level::INFO, "[handle_get_request] Processing GET request for target: " + std::string(req.target()));

    try {
        std::string path = path_cat(doc_root, req.target());
        if (req.target().back() == '/') {
            path.append("index.html");
        }

        beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::scan, ec);

        if (ec == beast::errc::no_such_file_or_directory) {
            Log::get().log(Level::WARN, "[handle_get_request] Resource not found: " + path);
            return send_(req, http::status::not_found, "The resource was not found.");
        }

        if (ec) {
            Log::get().log(Level::ERROR, "[handle_get_request] Error opening file: " + ec.message());
            return send_(req, http::status::internal_server_error, "Error: " + ec.message());
        }

        auto const size = body.size();

        if (req.method() == http::verb::head) {
            Log::get().log(Level::INFO, "[handle_get_request] HEAD request for: " + path);
            http::response<http::empty_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }

        Log::get().log(Level::INFO, "[handle_get_request] Serving file: " + path + " with size: " + std::to_string(size));
        http::response<http::file_body> res{
            std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(http::status::ok, req.version())
        };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    } catch (const std::exception& e) {
        Log::get().log(Level::ERROR, "[handle_get_request] Exception caught: " + std::string(e.what()));
        return send_(req, http::status::internal_server_error, R"({"error": ")" + std::string(e.what()) + "\"}");
    }
}

template <class Body, class Allocator>
http::message_generator handle_request(
        beast::string_view doc_root,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{
    Log::get().log(Level::INFO, "[handle_request] Received request: " + std::string(req.method_string()) + " " + std::string(req.target()));

    auto response_promise = std::make_shared<std::promise<http::message_generator>>();
    auto response_future = response_promise->get_future();

    app->get_queue()->enqueue([doc_root, req = std::move(req), app, response_promise]() mutable {
        Log::get().log(Level::INFO, "[handle_request] Handling request for target: " + std::string(req.target()));
        if (req.method() == http::verb::post && req.target() == "/") {
            response_promise->set_value(handle_post_request(std::move(req), app));
        } else if (req.method() == http::verb::get || req.method() == http::verb::head) {
            response_promise->set_value(handle_get_request(doc_root, std::move(req), app));
        } else {
            response_promise->set_value(send_(req, http::status::bad_request, "Unknown HTTP-method"));
        }
    });

    return response_future.get();
}


beast::string_view mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

std::string path_cat(beast::string_view base, beast::string_view path)
{
    if(base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}


template http::message_generator handle_request<http::string_body, std::allocator<char>>(
        beast::string_view doc_root,
        http::request<http::string_body, http::basic_fields<std::allocator<char>>>&& req,
        std::shared_ptr<Application> app);

