#include "../include/application.hpp"
#include "../include/http_tools.hpp"
#include <iostream>

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
    return res;
}

    template <class Body, class Allocator>
http::message_generator handle_post_request(
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{
    // Minimal POST request handling: Always return OK
    return send_(req, http::status::ok, R"({"message": "POST request processed"})");
}

    template <class Body, class Allocator>
http::message_generator handle_get_request(
        beast::string_view doc_root,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{

    try {
        std::string path = path_cat(doc_root, req.target());
        if (req.target().back() == '/') {
            path.append("index.html");
        }

        beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::scan, ec);

        if (ec == beast::errc::no_such_file_or_directory) {
            return send_(req, http::status::not_found, "The resource was not found.");
        }

        if (ec) {
            return send_(req, http::status::internal_server_error, "Error: " + ec.message());
        }

        auto const size = body.size();

        if (req.method() == http::verb::head) {
            http::response<http::empty_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }

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
        return send_(req, http::status::internal_server_error, R"({"error": ")" + std::string(e.what()) + "\"}");
    }
}

    template <class Body, class Allocator>
http::message_generator handle_request(
        beast::string_view doc_root,
        http::request<Body, http::basic_fields<Allocator>>&& req,
        std::shared_ptr<Application> app)
{
    /*
    app->get_clock()->wait(10, [app](){
            std::cout << "10 seconds have passed." << std::endl;
            try {
            // Assuming the target and port are defined within the context
            //std::string response = app->get_client()->get("example.com", "80", "/");
            //std::cout << "Response from GET request: " << response << std::endl;
            } catch (const std::exception& e) {
            std::cerr << "GET request failed: " << e.what() << std::endl;
            }
            });
    */
    if (req.method() == http::verb::post && req.target() == "/") {
        return handle_post_request(std::move(req), app);
    } else if (req.method() == http::verb::get || req.method() == http::verb::head) {
        return handle_get_request(doc_root, std::move(req), app);
    } else {
        return send_(req, http::status::bad_request, "Unknown HTTP-method");
    }
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

